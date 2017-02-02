#include "sotm/output/variables.hpp"

#include <cmath>

#include "gtest/gtest.h"

using namespace sotm;

TEST(Scaler, Linear)
{
	Scaler s;
	s.scale(1.0);
	ASSERT_EQ(s.scale(1.1), 1.0);
	ASSERT_EQ(s.scale(1.0), 0.0);

	ASSERT_EQ(s.scale(101.0), 1.0);
	ASSERT_EQ(s.scale(1.0), 0.0);
	ASSERT_EQ(s.scale(51.0), 0.5);
}

TEST(Scaler, FixedValueLinear)
{
	Scaler s;
	s.scale(1.0);
	ASSERT_EQ(s.scale(11.0), 1.0);
	ASSERT_EQ(s.scale(1.0), 0.0);

	s.fixValue(2.0, 0.5);
	ASSERT_EQ(s.scale(2.0), 0.5);
	ASSERT_EQ(s.scale(1.0), 0.0);
	ASSERT_EQ(s.scale(11.0), 1.0);

	ASSERT_NEAR(s.scale(1.5), 0.25, 1e-12);
	ASSERT_NEAR(s.scale(6.5), 0.75, 1e-12);
}

TEST(Scaler, Log)
{
	Scaler s(Scaler::Scale::log);
	s.scale(1.0);
	ASSERT_EQ(s.scale(1.1), 1.0);
	ASSERT_EQ(s.scale(1.0), 0.0);

	ASSERT_EQ(s.scale(101.0), 1.0);
	ASSERT_EQ(s.scale(1.0), 0.0);
}

TEST(LinearGradientColorMapper, LittleColors)
{
	ASSERT_NO_THROW(LinearGradientColorMapper g);
	LinearGradientColorMapper g;
	ColorMapperBase::Color c1, c2;
	ASSERT_NO_THROW(c1 = g.get(48));
	ASSERT_NO_THROW(c2 = g.get(-23143421312));
	ASSERT_TRUE(c1 == c2);
	g.addColor(1.0, ColorMapperBase::red);
	g.addColor(0.0, ColorMapperBase::blue);

	ASSERT_TRUE(g.get(1.0) == ColorMapperBase::red);
	ASSERT_TRUE(g.get(0.0) == ColorMapperBase::blue);
}
