#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/payloads/demo/empty-payloads.hpp"


#include "gtest/gtest.h"

using namespace sotm;

TEST(NodeTests, Instanciation)
{
	ModelContext c;
	c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadFactory()));
	c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadFactory()));
	c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContext()));

	PtrWrap<Node> n;
	n = PtrWrap<Node>::make(&c, Vector<3>(1.0, 2.0, 3.0));
	ASSERT_EQ(n->pos.x[0], 1.0);
	ASSERT_EQ(n->pos.x[1], 2.0);
	ASSERT_EQ(n->pos.x[2], 3.0);

}
