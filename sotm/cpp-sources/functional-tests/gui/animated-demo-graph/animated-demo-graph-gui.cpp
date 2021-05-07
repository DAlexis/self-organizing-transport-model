#include <iostream>

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/demo/absolute-random-graph.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include "sotm/math/random.hpp"

#include "sotm-gui/gui.hpp"

using namespace std;
using namespace sotm;


int main(int argc, char** argv)
{
	Random::randomize(0);
	ModelContext c;

	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new RandomGraphNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new RandomGraphLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new RandomGraphPhysicalContext()));

	{ // Scope to remove pointers

		/// Building initial tree
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, StaticVector<3>(0.0, 0.0, 0.0));
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, StaticVector<3>(1.0, 1.0, 1.0));

		PtrWrap<Node> n3 = PtrWrap<Node>::make(&c, StaticVector<3>(2.0, 1.0, 1.0));
		PtrWrap<Node> n4 = PtrWrap<Node>::make(&c, StaticVector<3>(1.0, 2.0, 1.0));
		PtrWrap<Node> n5 = PtrWrap<Node>::make(&c, StaticVector<3>(1.0, 1.0, 2.0));

		PtrWrap<Link> l = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l1 = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l2 = PtrWrap<Link>::make(&c);
		PtrWrap<Link> l3 = PtrWrap<Link>::make(&c);

		l->connect(n1, n2);
		l1->connect(n2, n3);
		l2->connect(n2, n4);
		l3->connect(n2, n5);
	}

	// Time iteration
	EulerExplicitIterator euler;
	TimeIterator iter(&c, &euler, &c);
	iter.setTime(0.0);
	iter.setStep(0.1);
	iter.setStopTime(1.0);

	// Running GUI
	GUI gui(&c, &iter);
	gui.run(argc, argv);

	cout << "Destroying graph" << endl;
	RandomGraphPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);

	cout << "Exiting" << endl;
	return 0;
}
