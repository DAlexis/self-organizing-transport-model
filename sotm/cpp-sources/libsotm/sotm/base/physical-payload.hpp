#ifndef PHYSICAL_CONTEXT_HPP_INCLUDE
#define PHYSICAL_CONTEXT_HPP_INCLUDE

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/time-iter.hpp"
#include "sotm/utils/memory.hpp"
#include <vector>
#include <set>
#include <string>

namespace sotm
{

class AnyPhysicalPayloadBase;
class ModelContext;

class PhysicalPayloadsRegister : public IContinuousTimeIterable
{
public:
	PhysicalPayloadsRegister();
	void add(AnyPhysicalPayloadBase* payload);
	void remove(AnyPhysicalPayloadBase* payload);

	void calculateSecondaryValues() override final;
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

	virtual void calculateSecondaryValues() { }

	/// Get object color for visualization purpose. Color is RGB array. Each color is from interval [0.0; 1.0]
	virtual void getColor(double* rgb);

	/// Get object size for visualization purpose. Normal size if 1.0
	virtual double getSize();

	virtual std::string getFollowerText();


protected:
	constexpr static double defaultColor[3] = {1.0, 0.8, 0.3};
	/// This function should be called by physical payload
	/// when physics things that object holding this payload should die.
	/// derived classes must call this func BEFORE calling their own version of this func
	virtual void onDeletePayload();

private:
	PhysicalPayloadsRegister *m_payloadsRegister;
};

class IPhysicalContext : public IContinuousTimeIterable, public IBifurcationTimeIterable
{
public:
	virtual ~IPhysicalContext() {}
	virtual void connectModel(ModelContext* m) = 0;
};

class PhysicalContextBase : public IPhysicalContext
{
public:
	void connectModel(ModelContext* m) override { m_model = m; }

protected:
	ModelContext* m_model = nullptr;
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

// Quickly define typical factory for node payload
#define SOTM_QUICK_NPF(Classname, FactoryName) \
	class FactoryName : public INodePayloadFactory \
	{ \
	public: \
		Classname* create(PhysicalPayloadsRegister* reg, Node* node) override final \
		{ return new Classname(reg, node); } \
	};

// Quickly define typical factory for link payload
#define SOTM_QUICK_LPF(Classname, FactoryName) \
	class FactoryName : public ILinkPayloadFactory \
	{ \
	public: \
		Classname* create(PhysicalPayloadsRegister* reg, Link* link) override final \
		{ return new Classname(reg, link); } \
	};

// Quickly add cast() method for physical context. There is no real need in this method, but it simplify casting
#define SOTM_QUICK_PHYS_CONT_CASTS(Classname) \
	static inline Classname* cast(IPhysicalContext* context) \
	{ \
		return static_cast<Classname*>(context); \
	} \
	 \
	static inline const Classname* cast(const IPhysicalContext* context) \
	{ \
		return static_cast<const Classname*>(context); \
	} \

}

#endif // PHYSICAL_CONTEXT_HPP_INCLUDE
