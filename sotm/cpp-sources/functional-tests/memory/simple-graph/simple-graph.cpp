#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/demo/empty-payloads.hpp"

#include <iostream>

using namespace std;
using namespace sotm;

class Test : public SelfMemMgr
{
public:
	char q[100];
};

int main()
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContext()));

	PtrWrap<Node> n1 = PtrWrap<Node>::make(&c);
	PtrWrap<Node> n2 = PtrWrap<Node>::make(&c);
	PtrWrap<Link> l = PtrWrap<Link>::make(&c);
	l->connect(n1, n2);
	return 0;
}
