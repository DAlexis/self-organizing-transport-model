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

void PhysicalPayloadsRegister::calculateRHS(double time)
{
	for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
		(*it)->calculateRHS(time);
}

void PhysicalPayloadsRegister::addRHSToDelta(double m)
{
	for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
		(*it)->addRHSToDelta(m);
}

void PhysicalPayloadsRegister::makeSubIteration(double dt)
{
	for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
		(*it)->makeSubIteration(dt);
}

void PhysicalPayloadsRegister::step()
{
	for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
		(*it)->step();
}

/*
void PhysicalPayloadsRegister::makeStep(double dt)
{
	for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
		(*it)->makeStep(dt);
}

void PhysicalPayloadsRegister::doBifurcation(double dt)
{
	for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
		(*it)->doBifurcation(dt);
}*/

AnyPhysicalPayloadBase::AnyPhysicalPayloadBase(PhysicalPayloadsRegister* reg) :
		m_payloadsRegister(reg)
{
	m_payloadsRegister->add(this);
}

AnyPhysicalPayloadBase::~AnyPhysicalPayloadBase()
{
	m_payloadsRegister->remove(this);
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
