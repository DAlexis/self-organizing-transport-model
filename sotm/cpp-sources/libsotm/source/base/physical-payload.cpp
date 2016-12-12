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

AnyPhysicalPayloadBase::AnyPhysicalPayloadBase(PhysicalPayloadsRegister* reg) :
		m_payloadsRegister(reg)
{
	m_payloadsRegister->add(this);
}

AnyPhysicalPayloadBase::~AnyPhysicalPayloadBase()
{
	if (m_payloadsRegister)
		m_payloadsRegister->remove(this);
}

void AnyPhysicalPayloadBase::getColor(double* rgb)
{
	rgb[0] = defaultColor[0]; rgb[1] = defaultColor[1]; rgb[2] = defaultColor[2];
}

double AnyPhysicalPayloadBase::getSize()
{
	return 1.0;
}

void AnyPhysicalPayloadBase::onDeletePayload()
{
	ASSERT(m_payloadsRegister != nullptr, "AnyPhysicalPayloadBase::onDeletePayload() must be called while m_payloadsRegister != nullptr");
	m_payloadsRegister->remove(this);
	m_payloadsRegister = nullptr;
}

NodePayloadBase::NodePayloadBase(PhysicalPayloadsRegister* reg, Node* node) :
	AnyPhysicalPayloadBase(reg),
	node(node)
{
}

void NodePayloadBase::getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters)
{
	branchingParameters.needBranching = false;
}

void NodePayloadBase::onDeletePayload()
{
	AnyPhysicalPayloadBase::onDeletePayload();
	node.clear();
}

LinkPayloadBase::LinkPayloadBase(PhysicalPayloadsRegister* reg, Link* link) :
	AnyPhysicalPayloadBase(reg),
	link(link)
{
}

void LinkPayloadBase::onDeletePayload()
{
	AnyPhysicalPayloadBase::onDeletePayload();
	link.clear();
}
