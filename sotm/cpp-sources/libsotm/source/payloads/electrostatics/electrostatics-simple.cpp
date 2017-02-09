#include "sotm/payloads/electrostatics/electrostatics-simple.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/utils/const.hpp"
#include "sotm/math/distrib-gen.hpp"

#include <iostream>

#include <ios>
#include <iomanip>
#include <sstream>

using namespace std;

using namespace sotm;

double ElectrostaticNodePayload::chargeMin = 0.0;
double ElectrostaticNodePayload::chargeMax = 0.0;

void ElectrostaticPhysicalContext::destroyGraph()
{
	m_readyToDestroy = true;
}

bool ElectrostaticPhysicalContext::readyToDestroy()
{
	return m_readyToDestroy;
}

void ElectrostaticPhysicalContext::calculateSecondaryValues(double time)
{
}

void ElectrostaticPhysicalContext::calculateRHS(double time)
{
	UNUSED_ARG(time);
}

void ElectrostaticPhysicalContext::addRHSToDelta(double m)
{
	UNUSED_ARG(m);
}

void ElectrostaticPhysicalContext::makeSubIteration(double dt)
{
	UNUSED_ARG(dt);
}

void ElectrostaticPhysicalContext::step()
{

}

void ElectrostaticPhysicalContext::setDischargeFunc(Function1D func)
{
	m_dischargeProb = func;
	m_integralOfProb.reset(new DefinedIntegral(m_dischargeProb, -20e6, 20e6, 10000));
}

void ElectrostaticPhysicalContext::setExternalConstField(Vector<3> field)
{
	m_externalConstField = field;
}

void ElectrostaticPhysicalContext::getElectricField(const Vector<3>& point, Vector<3>& outField, double& outPotential, const Node* exclude)
{
	outPotential = - (point ^ m_externalConstField);
	outField = m_externalConstField;

	GraphRegister::NodeVisitor nodeVisitor = [&point, exclude, &outPotential, &outField](const Node* node) {

		// Skip this node
		if (node == exclude)
			return;
		Vector<3> r1 = node->pos;

		double dist = (point - r1).len();
		double dist3 = dist*dist*dist;

		double charge = static_cast<ElectrostaticNodePayload*>(node->payload.get())->charge.current;

		outPotential += Const::Si::k * charge / dist;

		outField[0] += Const::Si::k * charge * (point[0]-r1[0]) / dist3;
		outField[1] += Const::Si::k * charge * (point[1]-r1[1]) / dist3;
		outField[2] += Const::Si::k * charge * (point[2]-r1[2]) / dist3;
	};

	m_model->graphRegister.applyNodeVisitorWithoutGraphChganges(nodeVisitor);
}

bool ElectrostaticPhysicalContext::testConnection(Node* n1, Node* n2)
{
	double phi1 = static_cast<ElectrostaticNodePayload*>(n1->payload.get())->phi;
	double phi2 = static_cast<ElectrostaticNodePayload*>(n2->payload.get())->phi;

	double U = fabs(phi1 - phi2);
	double l = (n1->pos - n2->pos).len();
	// Filed is enough and we are not already connected. Second check is here for performance reason
	if (U/l > connectionCriticalField && !n1->hasNeighbour(n2))
		return true;
	else
		return false;
}

////////////////////////////////////
////////////////////////////////////
// ElectrostaticNodePayload

ElectrostaticNodePayload::ElectrostaticNodePayload(PhysicalPayloadsRegister* reg, Node* node) :
		NodePayloadBase(reg, node)
{
}

void ElectrostaticNodePayload::calculateSecondaryValues(double time)
{
	calculateExtFieldAndPhi();

	double capacity = context()->nodeRadius / Const::Si::k;
	externalField = externalField * 3;
	phi += charge.current / capacity;
}

void ElectrostaticNodePayload::calculateRHS(double time)
{
	charge.rhs = 0;
	Node::LinkVisitor linkVisitor = [this](Link* link, LinkDirection dir)
	{
		double current = static_cast<ElectrostaticLinkPayload*>(link->payload.get())->getCurrent();
		charge.rhs += current * (dir == LinkDirection::in ? 1.0 : -1.0);
	};
	node->applyConnectedLinksVisitor(linkVisitor);
}

void ElectrostaticNodePayload::addRHSToDelta(double m)
{
	charge.addRHSToDelta(m);
}

void ElectrostaticNodePayload::makeSubIteration(double dt)
{
	charge.makeSubIteration(dt);
}

void ElectrostaticNodePayload::step()
{
	charge.step();
}

void ElectrostaticNodePayload::calculateExtFieldAndPhi()
{
	static_cast<ElectrostaticPhysicalContext*>(node->physicalContext())->
		getElectricField(node->pos, externalField, phi, this->node.data());
}

void ElectrostaticNodePayload::findTargetToConnect()
{
	m_connectTo = nullptr;

	GraphRegister::NodeVisitor nodeVisitor = [this](Node* n)
	{
		if (n <= this->node.data()) // To prevent double connections
			return;

		if (context()->testConnection(this->node, n)) // We are not already connected
			m_connectTo = n;
	};

	context()->m_model->graphRegister.applyNodeVisitorWithoutGraphChganges(nodeVisitor);
}

void ElectrostaticNodePayload::connectToTarget()
{
	if (m_connectTo != nullptr)
	{

		PtrWrap<Link> newLink = PtrWrap<Link>::make(context()->m_model);
		newLink->connect(this->node, m_connectTo);
		newLink->payload->init();
		//cout << "Connection!!!" << endl;
	}
}

void ElectrostaticNodePayload::prepareBifurcation(double time, double dt)
{
	findTargetToConnect();
}

void ElectrostaticNodePayload::doBifurcation(double time, double dt)
{
	//findTargetToConnect();
	connectToTarget();

	// Check if physics tells us we can release parent object
	if (context()->readyToDestroy())
	{
		onDeletePayload();
	}
}

void ElectrostaticNodePayload::init()
{
	calculateExtFieldAndPhi();
}

void ElectrostaticNodePayload::getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters)
{
	double radius = context()->nodeRadius;
	double E1 = Const::Si::k*charge.current / sqr(radius);
	DistributionResult<SphericalPoint> res = generateDischargeDirection(
			dt,
			radius,
			externalField.len(),
			E1,
			context()->m_dischargeProb,
			context()->m_integralOfProb->function()
	);
	branchingParameters.needBranching = res.isHappened;
	if (branchingParameters.needBranching)
	{
		SphericalVectorPlacer pl(externalField);
		branchingParameters.direction = pl.place(1.0, res.value);

		Vector<3> branchStartPoint = node->pos + branchingParameters.direction * (radius*1.00);
		double len;
		if (context()->smartBranching)
			len = calculateBranchLen(branchStartPoint, branchingParameters.direction, 0.5, 0.3);
		else
			len = context()->branchingStep;
		//cout << "branch len should be " << len << endl;

		// Testing for collision with nearest nodes
		Vector<3> newPlace = node->pos + branchingParameters.direction * len;
		Node *nearest = context()->m_model->graphRegister.getNearestNode(newPlace);
		double dist = (nearest->pos - newPlace).len();
		if (nearest && dist < radius*2.0)
		{
			//cout << "Branching disabled" << endl;
			branchingParameters.needBranching = false;
		}

		branchingParameters.length = len;
		//cout << "Branching" << endl;
	}
}

void ElectrostaticNodePayload::getColor(double* rgb)
{
	static_cast<ColorMapperBase&>(context()->chargeColorMapper).get(context()->chargeScaler.scale(charge.previous), rgb);
}

double ElectrostaticNodePayload::getSize()
{
	return context()->nodeRadius;
}

std::string ElectrostaticNodePayload::getFollowerText()
{
	std::ostringstream ss;
	ss << "       q = " << std::scientific << std::setprecision(2) << charge.current << endl;
	ss << "       phi = " << phi;
	return ss.str();
}

void ElectrostaticNodePayload::setCharge(double charge)
{
	this->charge.setInitial(charge);
}

double ElectrostaticNodePayload::calculateBranchLen(const Vector<3>& startPoint, const Vector<3>& direction, double eDiffMax, double lenMax)
{
	Vector<3> step = direction;
	step.normalize();
	step = step * branchProbeStep;

	Vector<3> currentPoint = startPoint;
	double currentPhi = 0;
	Vector<3> startField;
	Vector<3> currentField;

	Vector<3> deltaField;

	// Getting start field
	context()->getElectricField(currentPoint, startField, currentPhi);

	double len = 0;

	do {
		context()->getElectricField(currentPoint, currentField, currentPhi);
		currentPoint += step;
		len = (currentPoint - startPoint).len();
		deltaField = currentField - startField;

	} while (deltaField.len() / startField.len() < eDiffMax && len < lenMax);

	return len;
}

void ElectrostaticNodePayload::setChargeColorLimits(double chargeMin, double chargeMax)
{
	ElectrostaticNodePayload::chargeMin = chargeMin;
	ElectrostaticNodePayload::chargeMax = chargeMax;
}

////////////////////////////////////
////////////////////////////////////
// ElectrostaticLinkPayload

ElectrostaticLinkPayload::ElectrostaticLinkPayload(PhysicalPayloadsRegister* reg, Link* link) :
		LinkPayloadBase(reg, link)
{
}


void ElectrostaticLinkPayload::calculateSecondaryValues(double time)
{
}

void ElectrostaticLinkPayload::calculateRHS(double time)
{
	conductivity.rhs = (context()->linkEta * sqr(getVoltage()) - context()->linkBeta) * conductivity.current;
}

void ElectrostaticLinkPayload::addRHSToDelta(double m)
{
	conductivity.addRHSToDelta(m);
}

void ElectrostaticLinkPayload::makeSubIteration(double dt)
{
	conductivity.makeSubIteration(dt);
}

void ElectrostaticLinkPayload::step()
{
	conductivity.step();
}

void ElectrostaticLinkPayload::doBifurcation(double time, double dt)
{
	double current = getCurrent();
	if (current != 0.0 && conductivity.current < context()->minimalConductivity)
	{
		onDeletePayload();
		return;
	}

	// Check if physics tells us we can release parent object
	ElectrostaticPhysicalContext* context = ElectrostaticPhysicalContext::cast(link->context()->physicalContext());
	if (context->readyToDestroy())
	{
		onDeletePayload();
	}
}

void ElectrostaticLinkPayload::init()
{
	setTemperature(context()->airTemperature);
	conductivity.set(context()->initialConductivity);
}

void ElectrostaticLinkPayload::getColor(double* rgb)
{
	//rgb[0] = defaultColor[0]; rgb[1] = defaultColor[1]; rgb[2] = defaultColor[2];
	static_cast<ColorMapperBase&>(context()->conductivityColorMapper).get(context()->conductivityScaler.scale(conductivity.previous), rgb);
}

double ElectrostaticLinkPayload::getSize()
{
	return 1.0;
}

std::string ElectrostaticLinkPayload::getFollowerText()
{
	std::ostringstream ss;
	ss << "  G = " << std::scientific << std::setprecision(2) << conductivity.current << endl;
	ss << "  I = " << std::scientific << std::setprecision(2) << getCurrent() << endl;
	return ss.str();
}

double ElectrostaticLinkPayload::getCurrent()
{
	return getVoltage() * conductivity.current;
}

double ElectrostaticLinkPayload::getVoltage()
{
	Node* n1 = link->getNode1();
	ElectrostaticNodePayload* p1 = static_cast<ElectrostaticNodePayload*>(n1->payload.get());

	Node* n2 = link->getNode2();
	ElectrostaticNodePayload* p2 = static_cast<ElectrostaticNodePayload*>(n2->payload.get());

	return (p1->phi - p2->phi);
}

void ElectrostaticLinkPayload::setTemperature(double temp)
{
	//heatness.setInitial(temp*heatCapacity());
}

double ElectrostaticLinkPayload::getTemperature()
{
	//return heatness.current / heatCapacity();
	return 0.0;
}

double ElectrostaticLinkPayload::heatCapacity()
{
	double l = link->lengthCached();
	double volume = Const::pi/4.0*radius*radius*l;
	double heatCapacity = volume * Const::Si::SpecificHeat::air;
	return heatCapacity;
}
