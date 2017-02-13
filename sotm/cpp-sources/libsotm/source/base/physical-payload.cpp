/*
 * physical-payload.cpp
 *
 *  Created on: 9 июня 2016 г.
 *      Author: dalexies
 */

#include "sotm/base/physical-payload.hpp"
#include "sotm/utils/assert.hpp"

#include <tbb/tbb.h>

using namespace sotm;

PhysicalPayloadsRegister::PhysicalPayloadsRegister(const ParallelSettings* parallelSettings) :
		m_parallelSettings(parallelSettings)
{
}

void PhysicalPayloadsRegister::add(AnyPhysicalPayloadBase* payload)
{
	m_payloads.insert(payload);
	m_payloadsVector.push_back(payload);
}

void PhysicalPayloadsRegister::remove(AnyPhysicalPayloadBase* payload)
{
	size_t count = m_payloads.erase(payload);
	m_payloadsVectorDirty = true;
	ASSERT(count == 1, "Removing payload without adding");
}

void PhysicalPayloadsRegister::rebuildPayloadsVectorIfNeeded()
{
	if (!m_payloadsVectorDirty)
		return;

	m_payloadsVector.clear();
	for (auto &it : m_payloads)
		m_payloadsVector.push_back(it);
	m_payloadsVectorDirty = false;
}

void PhysicalPayloadsRegister::clearSubiteration()
{
	for (auto &it :  m_payloads)
		it->clearSubiteration();
}

void PhysicalPayloadsRegister::calculateSecondaryValues(double time)
{
	if (m_parallelSettings->parallelContiniousIteration.calculateSecondaryValues)
	{
		rebuildPayloadsVectorIfNeeded();
		tbb::parallel_for( size_t(0), m_payloadsVector.size(),
			[this, time]( size_t i ) {
				m_payloadsVector[i]->calculateSecondaryValues(time);
			}
		);
	} else {
		for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
			(*it)->calculateSecondaryValues(time);
	}
}

void PhysicalPayloadsRegister::calculateRHS(double time)
{
	if (m_parallelSettings->parallelContiniousIteration.calculateRHS)
	{
		rebuildPayloadsVectorIfNeeded();
		tbb::parallel_for( size_t(0), m_payloadsVector.size(),
			[this, time]( size_t i ) {
				m_payloadsVector[i]->calculateRHS(time);
			}
		);
	} else {
		for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
			(*it)->calculateRHS(time);
	}
}

void PhysicalPayloadsRegister::addRHSToDelta(double m)
{
	if (m_parallelSettings->parallelContiniousIteration.addRHSToDelta)
	{
		rebuildPayloadsVectorIfNeeded();
		tbb::parallel_for( size_t(0), m_payloadsVector.size(),
			[this, m]( size_t i ) {
				m_payloadsVector[i]->addRHSToDelta(m);
			}
		);
	} else {
		for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
			(*it)->addRHSToDelta(m);
	}
}

void PhysicalPayloadsRegister::makeSubIteration(double dt)
{
	if (m_parallelSettings->parallelContiniousIteration.addRHSToDelta)
	{
		rebuildPayloadsVectorIfNeeded();
		tbb::parallel_for( size_t(0), m_payloadsVector.size(),
			[this, dt]( size_t i ) {
				m_payloadsVector[i]->makeSubIteration(dt);
			}
		);
	} else {
		for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
			(*it)->makeSubIteration(dt);
	}
}

void PhysicalPayloadsRegister::step()
{
	if (m_parallelSettings->parallelContiniousIteration.step)
	{
		rebuildPayloadsVectorIfNeeded();
		tbb::parallel_for( size_t(0), m_payloadsVector.size(),
			[this]( size_t i ) {
				m_payloadsVector[i]->step();
			}
		);
	} else {
		for (auto it = m_payloads.begin(); it != m_payloads.end(); ++it)
			(*it)->step();
	}
}

double PhysicalPayloadsRegister::getMinimalStepsCount()
{
	double minStepsCount = IContinuousTimeIterable::stepsCountNotMatter;
	for (auto &it : m_payloads)
	{
		double msc = it->getMinimalStepsCount();
		if (msc < minStepsCount)
			minStepsCount = msc;
	}
	return minStepsCount;
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

std::string AnyPhysicalPayloadBase::getFollowerText()
{
	return std::string();
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
