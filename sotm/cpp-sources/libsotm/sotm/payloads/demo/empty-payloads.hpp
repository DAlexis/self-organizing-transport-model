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

class EmptyPhysicalContext : public PhysicalContextBase
{
public:
	void destroyGraph();
	bool readyToDestroy();

	virtual void calculateSecondaryValues() override {}
	virtual void calculateRHS(double time) override { UNUSED_ARG(time); }
	virtual void addRHSToDelta(double m) override { UNUSED_ARG(m); }
	virtual void makeSubIteration(double dt) override { UNUSED_ARG(dt); }
	virtual void step() override { }

	void doBifurcation(double time, double dt) { UNUSED_ARG(time); UNUSED_ARG(dt); }

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

	void calculateRHS(double time) override;
	void addRHSToDelta(double m) override;
	void makeSubIteration(double dt) override;
	void step() override;

	void doBifurcation(double time, double dt) override;

};

class EmptyLinkPayload : public LinkPayloadBase
{
public:
	EmptyLinkPayload(PhysicalPayloadsRegister* reg, Link* link);

	void calculateRHS(double time) override;
	void addRHSToDelta(double m) override;
	void makeSubIteration(double dt) override;
	void step() override;

	void doBifurcation(double time, double dt) override;
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
