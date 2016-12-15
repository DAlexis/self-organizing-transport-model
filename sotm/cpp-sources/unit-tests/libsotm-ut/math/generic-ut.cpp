#include "sotm/math/generic.hpp"

#include "gtest/gtest.h"

using namespace sotm;

TEST(MonotonicFunctionSolve, rising)
{
	auto func = [](double x) { return x-8.0; };
	double result = MonotonicFunctionSolve(func, -4.0, 0.0, 10.0, 0.01);
	ASSERT_NEAR(result, 4.0, 0.01);
}

TEST(MonotonicFunctionSolve, falling)
{
	auto func = [](double x) { return 8.0-x; };
	double result = MonotonicFunctionSolve(func, 2.0, 0.0, 10.0, 0.01);
	ASSERT_NEAR(result, 6.0, 0.01);
}
