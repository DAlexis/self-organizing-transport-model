#include "sotm/payloads/electrostatics/electrostatics.hpp"
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

FieldScalarZero<3> ElectrostaticPhysicalContext::zeroField;

void ElectrostaticPhysicalContext::calculateSecondaryValues(double time)
{
    optimizer->rebuildOptimization();
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

void ElectrostaticPhysicalContext::init()
{
    std::cout << "Here!" << std::endl;
    optimizer->rebuildOptimization();
}

void ElectrostaticPhysicalContext::doBifurcation(double time, double dt)
{
    UNUSED_ARG(time); UNUSED_ARG(dt);
}

void ElectrostaticPhysicalContext::setDischargeFunc(Function1D func)
{
	m_dischargeProb = func;
	m_integralOfProb.reset(new DefinedIntegral(m_dischargeProb, -20e6, 20e6, 10000));
}

bool ElectrostaticPhysicalContext::testConnection(const Node* n1, const Node* n2) const
{
    if (n1 == n2)
    {
        return false;
    }
	double phi1 = static_cast<ElectrostaticNodePayload*>(n1->payload.get())->phi;
	double phi2 = static_cast<ElectrostaticNodePayload*>(n2->payload.get())->phi;

	double U = fabs(phi1 - phi2);
    double l = (n1->pos - n2->pos).norm();
	// Filed is enough and we are not already connected. Second check is here for performance reason
	if (U/l > connectionCriticalField
			&& !n1->hasNeighbour(n2)
			&& (l < connectionMaximalDist || connectionMaximalDist <= 0.0)
			)
		return true;
	else
		return false;
}

////////////////////////////////////
// ElectrostaticNodePayload

ElectrostaticNodePayload::ElectrostaticNodePayload(PhysicalPayloadsRegister* reg, Node* node, double nodeRadiusConductivity, double nodeRadiusBranching) :
        NodePayloadBase(reg, node),
        nodeRadiusBranching(nodeRadiusBranching),
        nodeRadiusConductivity(nodeRadiusConductivity)
{

	// Connecting to node if it is too close
	Node *nearest = context()->m_model->graphRegister.getNearestNode(node->pos);

	if (nearest != nullptr)
	{
        double dist = (nearest->pos - node->pos).norm();
		double r1 = nodeRadiusBranching;
		double r2 = static_cast<ElectrostaticNodePayload*>(nearest->payload.get())->nodeRadiusBranching;
		if (dist < r1 + r2)
		{
			if (!this->node->hasNeighbour(nearest))
				connectToTarget(nearest);
		}
	}
}

void ElectrostaticNodePayload::clearSubiteration()
{
	charge.clearSubIteration();
}

void ElectrostaticNodePayload::calculateSecondaryValues(double time)
{
	calculateExtFieldAndPhi();

    double capacity = nodeRadiusConductivity / Const::Si::k;
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

double ElectrostaticNodePayload::getMinimalStepsCount()
{
	return charge.getCurrentStepsCount();
}

void ElectrostaticNodePayload::calculateExtFieldAndPhi()
{
/*
	static_cast<ElectrostaticPhysicalContext*>(node->physicalContext())->
		getElectricField(node->pos, externalField, phi, this->node.data());

*/

    FieldPotential fp = coulombNode->getFP();
    externalField = fp.field;
    phi = fp.potential;

    externalField += - GradientFixedStep<3>(*context()->externalPotential, 1e-2) (node->pos);
    phi += (*context()->externalPotential) (node->pos);
}

Node* ElectrostaticNodePayload::findTargetToConnectByMeanField() const
{
    std::vector<CoulombNodeBase*> close;
    context()->optimizer->getClose(close, this->node->pos, context()->connectionMaximalDist);
    for (auto it: close)
    {
        Node* n = &(it->node);
        if (context()->testConnection(this->node, n))
        {
            return n;
        }
    }
    return nullptr;
}

void ElectrostaticNodePayload::connectToTarget(Node* connectTo)
{
	if (connectTo != nullptr)
	{
        PtrWrap<Link> newLink = PtrWrap<Link>::make(context()->m_model, this->node, connectTo);
        // newLink->connect(this->node, connectTo);
		newLink->payload->init();
	}
}

void ElectrostaticNodePayload::prepareBifurcation(double time, double dt)
{
    //findTargetToConnectByMeanField();
}

void ElectrostaticNodePayload::doBifurcation(double time, double dt)
{
	//findTargetToConnect();
    context()->emission_counter->add_charge(time, dt, node->pos, charge.current, charge.rhs);
	connectToTarget(findTargetToConnectByMeanField());
}

void ElectrostaticNodePayload::init()
{
	calculateExtFieldAndPhi();
}

void ElectrostaticNodePayload::getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters)
{
    double radius = nodeRadiusBranching;
	double E1 = Const::Si::k*charge.current / sqr(radius);
    double E0 = (externalField*3).norm();
	DistributionResult<SphericalPoint> res = generateDischargeDirection(
		dt,
		radius,
		E0,
		E1,
		context()->m_dischargeProb,
		context()->m_integralOfProb->function()
	);
	branchingParameters.needBranching = res.isHappened;
	if (branchingParameters.needBranching)
	{
		SphericalVectorPlacer pl(externalField);
		branchingParameters.direction = pl.place(1.0, res.value);

		StaticVector<3> branchStartPoint = node->pos + branchingParameters.direction * (radius*1.00);
		double len;
		if (context()->smartBranching)
			len = calculateBranchLen(branchStartPoint, branchingParameters.direction, 0.5, 0.3);
		else
			len = context()->branchingStep;
		//cout << "branch len should be " << len << endl;

		// Testing for collision with nearest nodes
		StaticVector<3> newPlace = node->pos + branchingParameters.direction * len;
		Node *nearest = context()->m_model->graphRegister.getNearestNode(newPlace);
        double dist = (nearest->pos - newPlace).norm();
		//if (nearest && dist < radius*2.0)
		if (nearest && dist < context()->branchingStep*0.3)
		{
			branchingParameters.needBranching = false;
		}

		branchingParameters.length = len;
		//cout << "Branching" << endl;
		/*
        // This code for debugging branching process
		if (branchingParameters.needBranching)
		{
			double pot; Vector<3> field;

			// External field
			context()->getElectricField(branchStartPoint, field, pot, node.data());
			field = project(field, branchingParameters.direction);
			std::cout << "[b] External E_n = " << field.len() << "; E_1 = " << E1 << std::endl;
			std::cout << "[b] Total without 3: " << field.len() + E1 << std::endl;
			std::cout << "[b] Total with 3: " << 3*field.len() + E1 << std::endl;
		}*/
	}
}

void ElectrostaticNodePayload::getColor(double* rgb)
{
	static_cast<ColorMapperBase&>(context()->chargeColorMapper).get(context()->chargeScaler.scale(charge.previous), rgb);
}

double ElectrostaticNodePayload::getSize()
{
    return nodeRadiusConductivity;
}

std::string ElectrostaticNodePayload::getFollowerText()
{
	std::ostringstream ss;
	ss << "       q = " << std::scientific << std::setprecision(2) << charge.current << endl;
	ss << "       phi = " << phi;
	return ss.str();
}

void ElectrostaticNodePayload::getParametersVector(double* parameters)
{
	parameters[0] = charge.current;
}

void ElectrostaticNodePayload::setCharge(double charge)
{
	this->charge.setInitial(charge);
}

double ElectrostaticNodePayload::calculateBranchLen(const StaticVector<3>& startPoint, const StaticVector<3>& direction, double eDiffMax, double lenMax)
{
    /// @todo: Fix this function or remove it away
    /*
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

    return len;*/
    return 0.15;
}

void ElectrostaticNodePayload::setChargeColorLimits(double chargeMin, double chargeMax)
{
	ElectrostaticNodePayload::chargeMin = chargeMin;
	ElectrostaticNodePayload::chargeMax = chargeMax;
}

double ElectrostaticNodePayload::etaFromCriticalField(double criticalFeild, double beta)
{
    return beta / sotm::sqr(criticalFeild);
}

////////////////////////////////////
// ElectrostaticLinkPayload

ElectrostaticLinkPayload::ElectrostaticLinkPayload(PhysicalPayloadsRegister* reg, Link* link, double linkEta, double linkBeta) :
        LinkPayloadBase(reg, link),
        linkEta(linkEta),
        linkBeta(linkBeta)
{
    Node* n1 = link->getNode1();
    Node* n2 = link->getNode2();
    center = (n1->pos + n2->pos) / 2;
    direction = n2->pos - n1->pos; // TODO: check sign
}


void ElectrostaticLinkPayload::clearSubiteration()
{
	conductivity.clearSubIteration();
	temperature.clearSubIteration();
}

void ElectrostaticLinkPayload::calculateSecondaryValues(double time)
{
}

void ElectrostaticLinkPayload::calculateRHS(double time)
{
    double current = getCurrent();
	double U = getVoltage();
    conductivity.rhs = (linkEta * sqr(U / link->length()) - linkBeta) * conductivity.current;
    temperature.rhs = current * U / getHeatCapacity();
}

void ElectrostaticLinkPayload::addRHSToDelta(double m)
{
	conductivity.addRHSToDelta(m);
	temperature.addRHSToDelta(m);
}

void ElectrostaticLinkPayload::makeSubIteration(double dt)
{
	conductivity.makeSubIteration(dt);
	temperature.makeSubIteration(dt);
}

void ElectrostaticLinkPayload::step()
{
	conductivity.step();
	temperature.step();
}

double ElectrostaticLinkPayload::getMinimalStepsCount()
{
	return std::min(conductivity.getCurrentStepsCount(), temperature.getCurrentStepsCount());
}

void ElectrostaticLinkPayload::doBifurcation(double time, double dt)
{
	double current = getCurrent();
    double currentDerivative = (current - lastCurrent) / dt;
    if (dt == 0.0)
        currentDerivative = 0.0;
    lastCurrent = current;
    context()->emission_counter->add_current(time, dt, center, direction, currentDerivative);
	if (current != 0.0 && getIOIEffectiveCondictivity() < context()->minimalConductivity)
	{
		onDeletePayload();
		return;
	}
}

void ElectrostaticLinkPayload::init()
{
	setTemperature(context()->airTemperature);
    conductivity.set(context()->initialConductivity);
    /*
	// Calculating middle field got set conductivity correctly
	Vector<3> c = (link->getNode1()->pos + link->getNode2()->pos) * 0.5;

	Vector<3> field;
	double phi;
	context()->getElectricField(c, field, phi);

	double absE = field.len();
    UNUSED_ARG(absE);*/
	
	// todo: add conductivity from temperature function
	//conductivity.set(context()->initialConductivity * (1+absE / 0.2e5));
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
	ss << "  G = " << std::scientific << std::setprecision(2) << getIOIEffectiveCondictivity() << endl;
	//ss << "  I = " << std::scientific << std::setprecision(2) << getCurrent() << endl;
	ss << "  T = " << std::scientific << std::setprecision(2) << temperature.current << endl;
	return ss.str();
}

void ElectrostaticLinkPayload::getParametersVector(double* parameters)
{
	parameters[0] = getIOIEffectiveCondictivity();
	parameters[1] = temperature.current;
	parameters[2] = 0.0;
}

double ElectrostaticLinkPayload::getTotalConductivity()
{
	return getIOIEffectiveCondictivity() * Const::pi * context()->linkRadius * context()->linkRadius / link->length();
}

double ElectrostaticLinkPayload::getIOIEffectiveCondictivity()
{
	double factor = context()->ionizationOverheatingInstFunc(temperature.current);
	return (1.0-factor)*conductivity.current + factor*context()->conductivityLimit;
}

double ElectrostaticLinkPayload::getCurrent()
{
	return getVoltage() * getTotalConductivity();
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
	temperature.set(temp);
}

double ElectrostaticLinkPayload::getTemperature()
{
	//return heatness.current / heatCapacity();
	return 0.0;
}

double ElectrostaticLinkPayload::getHeatCapacity()
{
	double l = link->lengthCached();
	double r = context()->linkRadius;
	double volume = Const::pi * sqr(r) * l;
	double heatCapacity = volume * Const::Si::SpecificHeat::air;
	return heatCapacity;
}

/////////////////////////////
// ElectrostaticNodePayloadFactory

ElectrostaticNodePayloadFactory::ElectrostaticNodePayloadFactory(ElectrostaticPhysicalContext& context) :
    m_context(context)
{

}

NodePayloadBase* ElectrostaticNodePayloadFactory::create(PhysicalPayloadsRegister* reg, Node* node)
{
    return new ElectrostaticNodePayload(reg, node, m_context.nodeRadiusConductivityDefault, m_context.nodeRadiusBranchingDefault);
}

/////////////////////////////
// ElectrostaticLinkPayloadFactory
ElectrostaticLinkPayloadFactory::ElectrostaticLinkPayloadFactory(ElectrostaticPhysicalContext& context) :
    m_context(context)
{

}

LinkPayloadBase* ElectrostaticLinkPayloadFactory::create(PhysicalPayloadsRegister* reg, Link* link)
{
    return new ElectrostaticLinkPayload(reg, link, m_context.linkEtaDefault, m_context.linkBetaDefault);
}
