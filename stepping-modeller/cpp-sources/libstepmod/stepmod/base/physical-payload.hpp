#ifndef PHYSICAL_CONTEXT_HPP_INCLUDE
#define PHYSICAL_CONTEXT_HPP_INCLUDE

#include "time-iter.hpp"
#include "stepmod/utils/memory.hpp"
#include "stepmod/base/stepping-structure.hpp"

#include <vector>
#include <set>

namespace Stepmod
{

class AnyPhysicalPayloadBase;

class PhysicalPayloadsRegister
{
public:
	void add(AnyPhysicalPayloadBase* payload);
	void remove(AnyPhysicalPayloadBase* payload);

	SINGLETON_IN_CLASS(PhysicalPayloadsRegister);

private:
	PhysicalPayloadsRegister();
	std::set<AnyPhysicalPayloadBase*> m_payloads;
};

class AnyPhysicalPayloadBase
{
public:
	AnyPhysicalPayloadBase();
	virtual ~AnyPhysicalPayloadBase();
	void doStep();
	void initVariables(size_t count);

	virtual void calculateRHS(double time) = 0;

	std::vector<double> x;
	std::vector<double> rhs;
	std::vector<double> xprev;
};

class NodePayloadBase : public AnyPhysicalPayloadBase
{
public:
	NodePayloadBase(Node* node);

protected:
	PtrWrap<Node> node;
};

/*
class INodePhysPayload : public ITimeIterable
{
public:
	virtual ~INodePhysPayload() {}

};

class ILinkPhysPayload : public ITimeIterable
{
public:
	virtual ~ILinkPhysPayload() {}
};

class IPhysicalPayloadFactory
{
public:
	virtual ~IPhysicalPayloadFactory() {}
	INodePhysPayload* createNodePayload(Node* node);
	ILinkPhysPayload* createLinkPayload(Link* node);
};
*/

}

#endif // PHYSICAL_CONTEXT_HPP_INCLUDE
