/*
 * empty-payloads.hpp
 *
 *  Created on: 25 авг. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_DEMO_EMPTY_PAYLOADS_HPP_
#define LIBSOTM_SOTM_PAYLOADS_DEMO_EMPTY_PAYLOADS_HPP_


#include "sotm/base/physical-payload.hpp"

namespace sotm
{

class EmptyPhysicalContext : public IPhysicalContext
{
public:
	void destroyGraph();
	bool readyToDestroy();

	static inline EmptyPhysicalContext* cast(IPhysicalContext* context)
	{
		return static_cast<EmptyPhysicalContext*>(context);
	}

	static inline const EmptyPhysicalContext* cast(const IPhysicalContext* context)
	{
		return static_cast<const EmptyPhysicalContext*>(context);
	}

private:
	bool m_readyToDestroy = false;
};

class EmptyNodePayload : public NodePayloadBase
{
public:
	EmptyNodePayload(PhysicalPayloadsRegister* reg, Node* node);


	void calculateRHS(double time) override final;
	void addRHSToDelta(double m) override final;
	void makeSubIteration(double dt) override final;
	void step() override final;

	void doBifurcation(double time, double dt) override final;

};

class EmptyLinkPayload : public LinkPayloadBase
{
public:
	EmptyLinkPayload(PhysicalPayloadsRegister* reg, Link* link);

	void calculateRHS(double time) override final;
	void addRHSToDelta(double m) override final;
	void makeSubIteration(double dt) override final;
	void step() override final;

	void doBifurcation(double time, double dt) override final;
};

class EmptyNodePayloadFactory : public INodePayloadFactory
{
public:
	NodePayloadBase* create(PhysicalPayloadsRegister* reg, Node* node) override final;
};

class EmptyLinkPayloadFactory : public ILinkPayloadFactory
{
public:
	LinkPayloadBase* create(PhysicalPayloadsRegister* reg, Link* link) override final;
};

}

#endif /* LIBSOTM_SOTM_PAYLOADS_DEMO_EMPTY_PAYLOADS_HPP_ */
