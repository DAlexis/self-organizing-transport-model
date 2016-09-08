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

class PhysicalPayloadsRegister : public IContinuousTimeIterable
{
public:
	PhysicalPayloadsRegister();
	void add(AnyPhysicalPayloadBase* payload);
	void remove(AnyPhysicalPayloadBase* payload);

	void calculateRHS(double time) override final;
	void addRHSToDelta(double m) override final;
	void makeSubIteration(double dt) override final;
	void step() override final;

private:
	std::set<AnyPhysicalPayloadBase*> m_payloads;
};

class AnyPhysicalPayloadBase : public IContinuousTimeIterable, public IBifurcationTimeIterable
{
public:
	AnyPhysicalPayloadBase(PhysicalPayloadsRegister* reg);
	virtual ~AnyPhysicalPayloadBase();
	void doStep() {}

protected:
	/// This function should be called by physical payload
	/// when physics things that object holding this payload should die.
	/// derived classes must call this func BEFORE calling their own version of this func
	virtual void onDeletePayload();

private:
	PhysicalPayloadsRegister *m_payloadsRegister;
};

class IPhysicalContext : public IContinuousTimeIterable
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
	void onDeletePayload() override;

	virtual void getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters);

protected:
	PtrWrap<Node> node;
};

class LinkPayloadBase : public AnyPhysicalPayloadBase
{
public:
	LinkPayloadBase(PhysicalPayloadsRegister* reg, Link* link);
	void onDeletePayload() override;

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

#define SOTM_QUICK_NPF(Classname, FactoryName) \
	class FactoryName : public INodePayloadFactory \
	{ \
	public: \
		Classname* create(PhysicalPayloadsRegister* reg, Node* node) override final \
		{ return new Classname(reg, node); } \
	};

#define SOTM_QUICK_LPF(Classname, FactoryName) \
	class FactoryName : public ILinkPayloadFactory \
	{ \
	public: \
		Classname* create(PhysicalPayloadsRegister* reg, Link* link) override final \
		{ return new Classname(reg, link); } \
	};


}

#endif // PHYSICAL_CONTEXT_HPP_INCLUDE
