/*
 * physical-payload.cpp
 *
 *  Created on: 9 июня 2016 г.
 *      Author: dalexies
 */

#include "stepmod/base/physical-payload.hpp"
#include "stepmod/utils/assert.hpp"

using namespace Stepmod;

SINGLETON_IN_CPP(PhysicalPayloadsRegister)

PhysicalPayloadsRegister::PhysicalPayloadsRegister()
{
}

void PhysicalPayloadsRegister::add(AnyPhysicalPayloadBase* payload)
{
	m_payloads.insert(payload);
}

void PhysicalPayloadsRegister::remove(AnyPhysicalPayloadBase* payload)
{
	size_t count = m_payloads.erase(payload);
	ASSERT(count == 1, "Removing payload without adding");
}

AnyPhysicalPayloadBase::AnyPhysicalPayloadBase()
{
	PhysicalPayloadsRegister::instance().add(this);
}

AnyPhysicalPayloadBase::~AnyPhysicalPayloadBase()
{
	PhysicalPayloadsRegister::instance().remove(this);
}


void AnyPhysicalPayloadBase::initVariables(size_t count)
{
	x.resize(count, 0.0);
	rhs.resize(count, 0.0);
	xprev.resize(count, 0.0);
}


void AnyPhysicalPayloadBase::doStep()
{
	ASSERT(xprev.size() == x.size(), "xprev.size() != x.size()");
	for (size_t i=0; i<x.size(); ++i)
	{
		x[i] = xprev[i];
	}
}

NodePayloadBase::NodePayloadBase(Node* node) :
	node(node)
{
}
