#include "../../lightning-modeller/coulomb-selector.hpp"
#include "gtest/gtest.h"

using namespace sotm;

TEST(ScalesParsing, Trivial)
{
    octree::DiscreteScales ds;
    ASSERT_NO_THROW(CoulombSelector::parseScales(ds, ""));
    ASSERT_EQ(ds.findScale(0.0), 0.0);
    ASSERT_EQ(ds.findScale(1.0), 0.0);
    ASSERT_EQ(ds.findScale(10.0), 0.0);
    ASSERT_EQ(ds.findScale(1000.0), 0.0);
}

TEST(ScalesParsing, WellFormatted)
{
    octree::DiscreteScales ds;
    ASSERT_NO_THROW(CoulombSelector::parseScales(ds, "(1.0, 5.0);     (3.0, 6.0); \t(10.0, 7.0); (100.0, 8.0)"));
    EXPECT_EQ(ds.findScale(0.0), 0.0);
    EXPECT_EQ(ds.findScale(1.1), 5.0);
    EXPECT_EQ(ds.findScale(3.1), 6.0);
    EXPECT_EQ(ds.findScale(10.1), 7.0);
    EXPECT_EQ(ds.findScale(1000.1), 8.0);
}
