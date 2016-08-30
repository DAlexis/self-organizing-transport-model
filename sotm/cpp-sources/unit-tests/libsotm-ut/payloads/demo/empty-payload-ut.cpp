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
		PtrWrap<Node> n1(new Node(&c));
		PtrWrap<Node> n2(new Node(&c));
		PtrWrap<Link> l(new Link(&c));
	) << "Building simpliest graph";

	// We had no assertion so we can create nodes and link without ASSERT_NO_THROW environment
	PtrWrap<Node> n1(new Node(&c));
	PtrWrap<Node> n2(new Node(&c));
	PtrWrap<Link> l(new Link(&c));
	ASSERT_NO_THROW(l->connect(n1, n2));
}
