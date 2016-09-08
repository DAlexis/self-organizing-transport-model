#include "sotm/payloads/demo/empty-payloads.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include <cmath>

#include "gtest/gtest.h"

using namespace sotm;

class EmptyPhysicalContextWithBranching : public EmptyPhysicalContext
{
public:
	bool doBranching = false;
	static inline EmptyPhysicalContextWithBranching* cast(IPhysicalContext* context)
	{
		return static_cast<EmptyPhysicalContextWithBranching*>(context);
	}

	static inline const EmptyPhysicalContextWithBranching* cast(const IPhysicalContext* context)
	{
		return static_cast<const EmptyPhysicalContextWithBranching*>(context);
	}
};

class EmptyNodePayloadWithBranching : public EmptyNodePayload
{
public:
	EmptyNodePayloadWithBranching(PhysicalPayloadsRegister* reg, Node* node) :
		EmptyNodePayload(reg, node)
	{ count++; }
	~EmptyNodePayloadWithBranching() { count--; }

	void getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters) override
	{
		if (EmptyPhysicalContextWithBranching::cast(node->physicalContext())->doBranching)
		{
			branchingParameters.needBranching = true;
			branchingParameters.direction = Point<3>({1.0, 1.0, 1.0});
			branchingParameters.length = 1.0;
		} else {
			branchingParameters.needBranching = false;
		}
	}

	static int count;
};

class EmptyLinkPayloadWithBranching : public EmptyLinkPayload
{
public:
	EmptyLinkPayloadWithBranching(PhysicalPayloadsRegister* reg, Link* link) :
		EmptyLinkPayload(reg, link)
	{ count++; }
	~EmptyLinkPayloadWithBranching() { count--; }

	static int count;
};

int EmptyNodePayloadWithBranching::count = 0;
int EmptyLinkPayloadWithBranching::count = 0;

SOTM_QUICK_NPF(EmptyNodePayloadWithBranching, EmptyNodePayloadWithBranchingFactory);
SOTM_QUICK_LPF(EmptyLinkPayloadWithBranching, EmptyLinkPayloadWithBranchingFactory);

TEST(ComplexBranchingGraph, GraphCreationAndDestruction)
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadWithBranchingFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadWithBranchingFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContextWithBranching()));

	{
		ASSERT_EQ(EmptyNodePayloadWithBranching::count, 0);
		ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 0);
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Point<3>({0.0, 0.0, 0.0}));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Point<3>({1.0, 0.0, 0.0}));
		PtrWrap<Link> l = PtrWrap<Link>::make(&c);
		l->connect(n1, n2);
		ASSERT_EQ(EmptyNodePayloadWithBranching::count, 2);
		ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 1);

	}

	ASSERT_EQ(EmptyNodePayloadWithBranching::count, 2);
	ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 1);
	EmptyPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);
	ASSERT_EQ(EmptyNodePayloadWithBranching::count, 0);
	ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 0);
}


TEST(ComplexBranchingGraph, Branching)
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadWithBranchingFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadWithBranchingFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContextWithBranching()));

	// Crating graph base: to pounts with link
	{
		ASSERT_EQ(EmptyNodePayloadWithBranching::count, 0);
		ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 0);
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Point<3>({0,0,0}));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Point<3>({1,0,0}));
		PtrWrap<Link> l = PtrWrap<Link>::make(&c);
		l->connect(n1, n2);
		ASSERT_EQ(EmptyNodePayloadWithBranching::count, 2);
		ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 1);
	}
	// Graph is still absolutely alive
	ASSERT_EQ(EmptyNodePayloadWithBranching::count, 2);
	ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 1);

	// Iterating without branching
	EulerExplicitIterator euler;
	TimeIterator iter(&c, &euler, &c);
	iter.setTime(0.0);
	iter.run(1.0, 0.1);
	// Now nothing should be changed
	ASSERT_EQ(EmptyNodePayloadWithBranching::count, 2);
	ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 1);

	// Tell physical context to enable branching now
	EmptyPhysicalContextWithBranching::cast(c.physicalContext())->doBranching = true;
	iter.iterate(0.1); // This should do bifurcation and enlarge graph

	ASSERT_EQ(EmptyNodePayloadWithBranching::count, 4);
	ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 3);

	// Disable branching
	EmptyPhysicalContextWithBranching::cast(c.physicalContext())->doBranching = false;

	EmptyPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);
	ASSERT_EQ(EmptyNodePayloadWithBranching::count, 0);
	ASSERT_EQ(EmptyLinkPayloadWithBranching::count, 0);


}

