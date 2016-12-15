#include "sotm/math/integration.hpp"

#include "gtest/gtest.h"

using namespace sotm;

TEST(DefinedIntegral, x)
{
	auto func = [](double x) { return x; };
	auto ans = [](double x) { return x*x/2.0; };

	DefinedIntegral i(func, 0, 10, 1000);

	ASSERT_NEAR(i(5)-i(2), ans(5)-ans(2), 0.01);
	ASSERT_NEAR(i(9)-i(1), ans(9)-ans(1), 0.1);
	ASSERT_NEAR(i(2)-i(4), ans(2)-ans(4), 0.01);
}
