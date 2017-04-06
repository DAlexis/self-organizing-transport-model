
#include "sotm/math/functions.hpp"

#include "gtest/gtest.h"
#include <cmath>


using namespace sotm;

TEST(FunctionsTest, Trapezoid)
{
	double ampl = 20;
	double z0 = -84;
	double scale = 10;
	double rec = 3;
	double linEnd = ampl*scale/2;
	ASSERT_NO_THROW(TrapezoidFunc(10, 10, 3, z0));
	TrapezoidFunc t(ampl, scale, rec, z0);
	ASSERT_EQ(t(-1000), -t.maxAbs());
	ASSERT_EQ(t(1000), t.maxAbs());
	ASSERT_EQ(t(z0), 0.0);

	ASSERT_EQ(t(z0+scale/2+rec), t.maxAbs());
	ASSERT_EQ(t(z0-scale/2-rec), -t.maxAbs());

	ASSERT_EQ(t(z0+scale/2), linEnd);
	ASSERT_EQ(t(z0-scale/2), -linEnd);

	ASSERT_LT(t(z0+scale/2+rec/2), t.maxAbs());
	ASSERT_GT(t(z0+scale/2+rec/2), linEnd);

	ASSERT_GT(t(z0-scale/2-rec/2), -t.maxAbs());
	ASSERT_LT(t(z0-scale/2-rec/2), -linEnd);
}
