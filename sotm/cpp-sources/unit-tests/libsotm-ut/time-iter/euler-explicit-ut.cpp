#include "exponent-time-iterable.hpp"
#include "sotm/time-iter/euler-explicit.hpp"
#include <cmath>

#include "gtest/gtest.h"

using namespace sotm;

TEST(EulerExplicit, ExponentDiffEq)
{
	Exponent e;
	EulerExplicitIterator euler;
	TimeIterator iter(&e, &euler);

	double timeLimit = 3.0;
	iter.setTime(0.0);
	iter.run(timeLimit, 0.0001);
	ASSERT_NEAR(exp(timeLimit), e.getValue(), 0.01);
}
