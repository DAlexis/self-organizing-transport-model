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
}

void EmptyNodePayload::makeStep(double dt)
{
}

EmptyLinkPayload::EmptyLinkPayload(PhysicalPayloadsRegister* reg, Link* link) :
		LinkPayloadBase(reg, link)
{
}

void EmptyLinkPayload::makeStep(double dt)
{
}
