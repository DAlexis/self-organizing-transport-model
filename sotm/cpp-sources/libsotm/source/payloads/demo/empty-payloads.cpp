/*
 * empty-payloads.cpp
 *
 *  Created on: 25 авг. 2016 г.
 *      Author: dalexies
 */

#include "sotm/payloads/demo/empty-payloads.hpp"

using namespace sotm;

EmptyNodePayload::EmptyNodePayload(PhysicalPayloadsRegister* reg, Node* node) :
		NodePayloadBase(reg, node)
{
	// Empty payload does not hold node and it may be deleted at any time
	this->node.clear();
}

void EmptyNodePayload::makeStep(double dt)
{
}

void EmptyNodePayload::calculateRHS(double time)
{
}

EmptyLinkPayload::EmptyLinkPayload(PhysicalPayloadsRegister* reg, Link* link) :
		LinkPayloadBase(reg, link)
{
	// Empty payload does not hold link and it may be deleted at any time
	this->link.clear();
}

void EmptyLinkPayload::makeStep(double dt)
{
}

void EmptyLinkPayload::calculateRHS(double time)
{
}

NodePayloadBase* EmptyNodePayloadFactory::create(PhysicalPayloadsRegister* reg, Node* node)
{
	return new EmptyNodePayload(reg, node);
}


LinkPayloadBase* EmptyLinkPayloadFactory::create(PhysicalPayloadsRegister* reg, Link* link)
{
	return new EmptyLinkPayload(reg, link);
}
