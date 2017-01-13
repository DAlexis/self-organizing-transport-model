#include "sotm/payloads/demo/empty-payloads.hpp"
#include "sotm/base/model-context.hpp"

#include "gtest/gtest.h"

using namespace sotm;

TEST(EmptyPayload, ModelContextInitialization)
{
	ModelContext c;
	ASSERT_NO_THROW(
		c.setNodePayloadFactory(std::unique_ptr<INodePayloadFactory>(new EmptyNodePayloadFactory()))
	);
	ASSERT_NO_THROW(
		c.setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory>(new EmptyLinkPayloadFactory()))
	);
	ASSERT_NO_THROW(
		c.setPhysicalContext(std::unique_ptr<IPhysicalContext>(new EmptyPhysicalContext()));
	);
	ASSERT_NO_THROW(
		PtrWrap<Node> n1 = PtrWrap<Node>::make(&c);
		PtrWrap<Node> n2 = PtrWrap<Node>::make(&c);
		PtrWrap<Link> l = PtrWrap<Link>::make(&c);
	) << "Building simplest graph";

	// We had no assertion so we can create nodes and link without ASSERT_NO_THROW environment
	PtrWrap<Node> n1 = PtrWrap<Node>::make(&c);
	PtrWrap<Node> n2 = PtrWrap<Node>::make(&c);
	PtrWrap<Link> l = PtrWrap<Link>::make(&c);
	ASSERT_NO_THROW(l->connect(n1, n2));
	ASSERT_NO_THROW(EmptyPhysicalContext::cast(c.physicalContext())->destroyGraph());
	ASSERT_NO_THROW(c.doBifurcation(0.0, 1.0));

}

