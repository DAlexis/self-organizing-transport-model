#include "sotm/payloads/electrostatics/electrostatics-simple.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/utils/const.hpp"

#include <iostream>

using namespace std;

using namespace sotm;

double ElectrostaticNodePayload::chargeMin = 0.0;
double ElectrostaticNodePayload::chargeMax = 10e-9;

void ElectrostaticPhysicalContext::destroyGraph()
{
	m_readyToDestroy = true;
}

bool ElectrostaticPhysicalContext::readyToDestroy()
{
	return m_readyToDestroy;
}

void ElectrostaticPhysicalContext::calculateSecondaryValues()
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

////////////////////////////////////
////////////////////////////////////
// ElectrostaticNodePayload

ElectrostaticNodePayload::ElectrostaticNodePayload(PhysicalPayloadsRegister* reg, Node* node) :
		NodePayloadBase(reg, node)
{
}

void ElectrostaticNodePayload::calculateSecondaryValues()
{
	double capacity = radius / Const::Si::k;
	phi = charge / capacity;

	externalField[0] = externalField[1] = externalField[2] = 0;

	Point<3> r0 = node->pos;

	auto nodeVisitor = [this, &r0](Node* node) {
		// Skip this node
		if (node == this->node.data())
			return;
		Point<3> r1 = node->pos;

		double dist = (r0 - r1).len();
		double dist3 = dist*dist*dist;

		double charge = static_cast<ElectrostaticNodePayload*>(node->payload.get())->charge;

		phi += Const::Si::k * charge / dist;

		externalField[0] += Const::Si::k * charge * (r0[0]-r1[0]) / dist3;
		externalField[1] += Const::Si::k * charge * (r0[1]-r1[1]) / dist3;
		externalField[2] += Const::Si::k * charge * (r0[2]-r1[2]) / dist3;
	};

	node->context()->graphRegister.applyNodeVisitor(nodeVisitor);
}

void ElectrostaticNodePayload::calculateRHS(double time)
{
	charge_rhs = 0;
	auto linkVisitor = [this](Link* link, LinkDirection dir){
		double current = static_cast<ElectrostaticLinkPayload*>(link->payload.get())->current;
		charge_rhs += current * (dir == LinkDirection::in ? 1.0 : -1.0);
	};
	node->applyConnectedLinksVisitor(linkVisitor);
}

void ElectrostaticNodePayload::addRHSToDelta(double m)
{
	charge_delta += charge_rhs * m;
}

void ElectrostaticNodePayload::makeSubIteration(double dt)
{
	charge = charge_prev + charge_rhs * dt;
}

void ElectrostaticNodePayload::step()
{
	charge = charge_prev = charge_prev + charge_delta;
	charge_delta = 0.0;
}

void ElectrostaticNodePayload::doBifurcation(double time, double dt)
{
	// Check if physics tells us we can release parent object
	ElectrostaticPhysicalContext* context = ElectrostaticPhysicalContext::cast(node->context()->physicalContext());
	if (context->readyToDestroy())
	{
		onDeletePayload();
	}
}

void ElectrostaticNodePayload::getColor(double* rgb)
{
	double v = (charge_prev - chargeMin) / (chargeMax - chargeMin);
	if (v < 0.0) v = 0.0;
	if (v > 1.0) v = 1.0;

//	cout << "v = " << v << endl;
	rgb[0] = v;
	rgb[1] = 0;
	rgb[2] = 1.0-v;
}

void ElectrostaticNodePayload::setCharge(double charge)
{
	charge_prev = this->charge = charge;
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


void ElectrostaticLinkPayload::calculateSecondaryValues()
{
	Node* n1 = link->getNode1();
	ElectrostaticNodePayload* p1 = static_cast<ElectrostaticNodePayload*>(n1->payload.get());

	Node* n2 = link->getNode2();
	ElectrostaticNodePayload* p2 = static_cast<ElectrostaticNodePayload*>(n2->payload.get());

	current = (p1->phi - p2->phi) * conductivity;
}

void ElectrostaticLinkPayload::calculateRHS(double time) { }
void ElectrostaticLinkPayload::addRHSToDelta(double m) { }
void ElectrostaticLinkPayload::makeSubIteration(double dt) { }
void ElectrostaticLinkPayload::step() { }

void ElectrostaticLinkPayload::doBifurcation(double time, double dt)
{
	// Check if physics tells us we can release parent object
	ElectrostaticPhysicalContext* context = ElectrostaticPhysicalContext::cast(link->context()->physicalContext());
	if (context->readyToDestroy())
	{
		onDeletePayload();
	}
}
