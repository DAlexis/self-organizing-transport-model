/*
 * physical-payload.cpp
 *
 *  Created on: 9 июня 2016 г.
 *      Author: dalexies
 */

#include "sotm/base/physical-payload.hpp"
#include "sotm/utils/assert.hpp"

using namespace sotm;

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

void PhysicalPayloadsRegister::makeStep(double dt)
{
	for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
		(*it)->makeStep(dt);
}

AnyPhysicalPayloadBase::AnyPhysicalPayloadBase(PhysicalPayloadsRegister* reg) :
		m_payloadsRegister(reg)
{
	m_payloadsRegister->add(this);
}

AnyPhysicalPayloadBase::~AnyPhysicalPayloadBase()
{
	m_payloadsRegister->remove(this);
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

NodePayloadBase::NodePayloadBase(PhysicalPayloadsRegister* reg, Node* node) :
	AnyPhysicalPayloadBase(reg),
	node(node)
{
}

void NodePayloadBase::getBranchingParameters(double dt, BranchingParameters& branchingParameters)
{
	branchingParameters.needBranching = false;
}

LinkPayloadBase::LinkPayloadBase(PhysicalPayloadsRegister* reg, Link* link) :
	AnyPhysicalPayloadBase(reg),
	link(link)
{
}
