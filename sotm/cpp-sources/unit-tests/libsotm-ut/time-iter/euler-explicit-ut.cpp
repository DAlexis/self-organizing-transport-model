#include "exponent-time-iterable.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include "time-iterator-tester.hpp"
#include <cmath>

#include "gtest/gtest.h"

using namespace sotm;

TEST(EulerExplicit, ExponentDiffEq)
{
	Exponent e;
	EulerExplicitIterator iter;
	TimeIteratorTester tester(&e, &iter);
	double timeLimit = 3.0;
	tester.run(0.0, timeLimit, 0.0001);
	ASSERT_NEAR(exp(timeLimit), e.getValue(), 0.01);
}
