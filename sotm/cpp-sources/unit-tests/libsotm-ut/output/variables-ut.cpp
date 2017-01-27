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
	IColorMapper::Color c1, c2;
	ASSERT_NO_THROW(c1 = g.get(48));
	ASSERT_NO_THROW(c2 = g.get(-23143421312));
	ASSERT_TRUE(c1 == c2);
	g.addColor(1.0, IColorMapper::red);
	g.addColor(0.0, IColorMapper::blue);

	ASSERT_TRUE(g.get(1.0) == IColorMapper::red);
	ASSERT_TRUE(g.get(0.0) == IColorMapper::blue);
}
