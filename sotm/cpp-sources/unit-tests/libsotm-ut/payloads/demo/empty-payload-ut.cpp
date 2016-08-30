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
}
