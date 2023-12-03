#include "../../../unit-tests/libsotm-ut/complex/branching-trivial-physics.hpp"
#include "sotm/time-iter/euler-explicit.hpp"

#include <iostream>

using namespace std;
using namespace sotm;

int EmptyNodePayloadWithBranching::count = 0;
int EmptyLinkPayloadWithBranching::count = 0;

int main()
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadWithBranchingFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadWithBranchingFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContextWithBranching()));

	// Crating graph base: to pounts with link
	{
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, StaticVector<3>({0,0,0}));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, StaticVector<3>({1,0,0}));
        PtrWrap<Link> l = PtrWrap<Link>::make(&c, n1, n2);
	}

	// Iterating without branching
	EulerExplicitIterator euler;
	TimeIterator iter(&c, &euler, &c);
	iter.setTime(0.0);
	iter.setStep(0.1);
	iter.setStopTime(1.0);
	iter.run();

	// Tell physical context to enable branching now
	EmptyPhysicalContextWithBranching::cast(c.physicalContext())->doBranching = true;
	iter.iterate(); // This should do bifurcation and enlarge graph

	// Disable branching
	EmptyPhysicalContextWithBranching::cast(c.physicalContext())->doBranching = false;

	EmptyPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);
}
