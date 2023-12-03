/*
 * main.cpp
 *
 *  Created on: 22 сент. 2016 г.
 *      Author: dalexies
 */

#include <iostream>

#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/demo/empty-payloads.hpp"

#include "sotm-gui/gui.hpp"

using namespace std;
using namespace sotm;


int main(int argc, char** argv)
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContext()));

	PtrWrap<Node> n1 = PtrWrap<Node>::make(&c, StaticVector<3>(0.0, 0.0, 0.0));
	PtrWrap<Node> n2 = PtrWrap<Node>::make(&c, StaticVector<3>(1.0, 1.0, 1.0));

	PtrWrap<Node> n3 = PtrWrap<Node>::make(&c, StaticVector<3>(2.0, 1.0, 1.0));
	PtrWrap<Node> n4 = PtrWrap<Node>::make(&c, StaticVector<3>(1.0, 2.0, 1.0));
	PtrWrap<Node> n5 = PtrWrap<Node>::make(&c, StaticVector<3>(1.0, 1.0, 2.0));

    PtrWrap<Link> l = PtrWrap<Link>::make(&c, n1, n2);
    PtrWrap<Link> l1 = PtrWrap<Link>::make(&c, n2, n3);
    PtrWrap<Link> l2 = PtrWrap<Link>::make(&c, n2, n4);
    PtrWrap<Link> l3 = PtrWrap<Link>::make(&c, n2, n5);

	GUI gui(&c);
	gui.run(argc, argv);

	cout << "Destroying graph" << endl;
	EmptyPhysicalContext::cast(c.physicalContext())->destroyGraph();
	c.doBifurcation(0.0, 1.0);

	cout << "Exiting" << endl;
	return 0;
}
