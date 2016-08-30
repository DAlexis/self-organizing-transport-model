#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/demo/empty-payloads.hpp"

using namespace sotm;

int main()
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadFactory()));

	PtrWrap<Node> n1(new Node(&c));
/*	PtrWrap<Node> n2(new Node(&c));
	PtrWrap<Link> l(new Link(&c));
	l->connect(n1, n2);*/
	return 0;
}
