#ifndef PHYSICAL_CONTEXT_HPP_INCLUDE
#define PHYSICAL_CONTEXT_HPP_INCLUDE

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/time-iter.hpp"
#include "sotm/utils/memory.hpp"
#include <vector>
#include <set>

namespace sotm
{

class AnyPhysicalPayloadBase;

class PhysicalPayloadsRegister : public ITimeIterable
{
public:
	PhysicalPayloadsRegister();
	void add(AnyPhysicalPayloadBase* payload);
	void remove(AnyPhysicalPayloadBase* payload);

	void calculateRHS(double time) override final;
	void addRHSToDelta(double m) override final;
	void makeSubIteration(double dt) override final;
	void step() override final;

	/*
	void makeStep(double dt) {}
	/// This function may delete/create new nodes in graph and should be called carefully
	void doBifurcation(double dt) {}
*/

private:
	std::set<AnyPhysicalPayloadBase*> m_payloads;
};

class AnyPhysicalPayloadBase : public ITimeIterable
{
public:
	AnyPhysicalPayloadBase(PhysicalPayloadsRegister* reg);
	virtual ~AnyPhysicalPayloadBase();
	void doStep() {}

private:
	PhysicalPayloadsRegister *m_payloadsRegister;
};

class IPhysicalContext
{
public:
	virtual ~IPhysicalContext() {}
};

struct BranchingParameters
{
	bool needBranching = false;
	Direction direction;
	double length = 0.0;
};

class NodePayloadBase : public AnyPhysicalPayloadBase
{
public:
	NodePayloadBase(PhysicalPayloadsRegister* reg, Node* node);

	virtual void getBranchingParameters(double dt, BranchingParameters& branchingParameters);

protected:
	PtrWrap<Node> node;
};

class LinkPayloadBase : public AnyPhysicalPayloadBase
{
public:
	LinkPayloadBase(PhysicalPayloadsRegister* reg, Link* link);

protected:
	PtrWrap<Link> link;
};

class INodePayloadFactory
{
public:
	virtual ~INodePayloadFactory() {}
	virtual NodePayloadBase* create(PhysicalPayloadsRegister* reg, Node* node) = 0;
};

class ILinkPayloadFactory
{
public:
	virtual ~ILinkPayloadFactory() {}
	virtual LinkPayloadBase* create(PhysicalPayloadsRegister* reg, Link* link) = 0;
};

}

#endif // PHYSICAL_CONTEXT_HPP_INCLUDE
