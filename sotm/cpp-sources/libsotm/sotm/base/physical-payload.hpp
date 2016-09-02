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

	void makeStep(double dt) override;

private:
	std::set<AnyPhysicalPayloadBase*> m_payloads;
};

class AnyPhysicalPayloadBase : public ITimeIterable
{
public:
	AnyPhysicalPayloadBase(PhysicalPayloadsRegister* reg);
	virtual ~AnyPhysicalPayloadBase();
	void doStep();
	void initVariables(size_t count);

	virtual void calculateRHS(double time) = 0;

	std::vector<double> x;
	std::vector<double> rhs;
	std::vector<double> xprev;

private:
	PhysicalPayloadsRegister *m_payloadsRegister;
};

class IPhysicalContext
{
public:
	virtual ~IPhysicalContext() {}
};

class NodePayloadBase : public AnyPhysicalPayloadBase
{
public:
	NodePayloadBase(PhysicalPayloadsRegister* reg, Node* node);

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
