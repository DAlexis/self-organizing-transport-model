/*
 * empty-payloads.cpp
 *
 *  Created on: 25 авг. 2016 г.
 *      Author: dalexies
 */

#include "sotm/payloads/demo/empty-payloads.hpp"
#include "sotm/base/model-context.hpp"

using namespace sotm;

void EmptyPhysicalContext::destroyGraph()
{
	m_readyToDestroy = true;
}

bool EmptyPhysicalContext::readyToDestroy()
{
	return m_readyToDestroy;
}

EmptyNodePayload::EmptyNodePayload(PhysicalPayloadsRegister* reg, Node* node) :
		NodePayloadBase(reg, node)
{
}

void EmptyNodePayload::calculateRHS(double time) { }
void EmptyNodePayload::addRHSToDelta(double m) { }
void EmptyNodePayload::makeSubIteration(double dt) { }
void EmptyNodePayload::step() { }

void EmptyNodePayload::doBifurcation(double dt)
{
	// Check if physics tells us we can release parent object
	EmptyPhysicalContext* context = EmptyPhysicalContext::cast(node->context()->physicalContext());
	if (context->readyToDestroy())
		this->node.clear();
}

EmptyLinkPayload::EmptyLinkPayload(PhysicalPayloadsRegister* reg, Link* link) :
		LinkPayloadBase(reg, link)
{
	// Empty payload does not hold link and it may be deleted at any time
	this->link.clear();
}

void EmptyLinkPayload::calculateRHS(double time) { }
void EmptyLinkPayload::addRHSToDelta(double m) { }
void EmptyLinkPayload::makeSubIteration(double dt) { }
void EmptyLinkPayload::step() { }

void EmptyLinkPayload::doBifurcation(double dt)
{
	// Check if physics tells us we can release parent object
	EmptyPhysicalContext* context = EmptyPhysicalContext::cast(link->context()->physicalContext());
	if (context->readyToDestroy())
		this->link.clear();
}

NodePayloadBase* EmptyNodePayloadFactory::create(PhysicalPayloadsRegister* reg, Node* node)
{
	return new EmptyNodePayload(reg, node);
}


LinkPayloadBase* EmptyLinkPayloadFactory::create(PhysicalPayloadsRegister* reg, Link* link)
{
	return new EmptyLinkPayload(reg, link);
}
