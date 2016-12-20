#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/demo/empty-payloads.hpp"

#include <iostream>

using namespace std;
using namespace sotm;

int main()
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContext()));


	/*
	PtrWrap<Node> n1 = PtrWrap<Node>::make(&c);
	PtrWrap<Node> n2 = PtrWrap<Node>::make(&c);
	PtrWrap<Link> l = PtrWrap<Link>::make(&c);
	l->connect(n1, n2);
	*/

	PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, Vector<3>(0.0, 0.0, 0.0));
	PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, Vector<3>(1.0, 1.0, 1.0));

	PtrWrap<Node> n3 = PtrWrap<Node>::make(&c, Vector<3>(2.0, 1.0, 1.0));
	PtrWrap<Node> n4 = PtrWrap<Node>::make(&c, Vector<3>(1.0, 2.0, 1.0));
	PtrWrap<Node> n5 = PtrWrap<Node>::make(&c, Vector<3>(1.0, 1.0, 2.0));

	PtrWrap<Link> l = PtrWrap<Link>::make(&c);
	PtrWrap<Link> l1 = PtrWrap<Link>::make(&c);
	PtrWrap<Link> l2 = PtrWrap<Link>::make(&c);
	PtrWrap<Link> l3 = PtrWrap<Link>::make(&c);

	l->connect(n1, n2);
	l1->connect(n2, n3);
	l2->connect(n2, n4);
	l3->connect(n2, n5);

	EmptyPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);
	return 0;
}
