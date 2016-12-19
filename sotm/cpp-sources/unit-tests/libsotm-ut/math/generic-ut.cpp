#include "sotm/math/generic.hpp"

#include "gtest/gtest.h"

using namespace sotm;

TEST(MonotonicFunctionSolve, rising)
{
	auto func = [](double x) { return x-8.0; };
	MonotonicFunctionSolver solver(func, 0.0, 10.0, 0.01);
	ASSERT_TRUE(solver.inRange(-4));
	ASSERT_NEAR(solver(-4.0), 4.0, 0.01);
}

TEST(MonotonicFunctionSolve, falling)
{
	auto func = [](double x) { return 8.0-x; };
	MonotonicFunctionSolver solver(func, 0.0, 10.0, 0.01);
	ASSERT_TRUE(solver.inRange(2.0));
	ASSERT_NEAR(solver(2.0), 6.0, 0.01);
}
