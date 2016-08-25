#include "sotm/math/geometry.hpp"

#include "gtest/gtest.h"

using namespace Stepmod;

TEST(PointTests, PointInstanciation)
{
    EXPECT_NO_THROW(Point<1>());
    EXPECT_NO_THROW(Point<2>());
    EXPECT_NO_THROW(Point<3>());
    
    EXPECT_NO_THROW(Point<1>(1.0));
    EXPECT_NO_THROW(Point<2>(1.0, 2.0));
    EXPECT_NO_THROW(Point<3>(1.0, 2.0, 3.0));

#ifdef DEBUG
    EXPECT_ANY_THROW(Point<1>(1.0, 2.0, 3.0));
    EXPECT_ANY_THROW(Point<2>(1.0, 2.0, 3.0));
    
    EXPECT_ANY_THROW(Point<2>(1.0));

    EXPECT_ANY_THROW(Point<2>( {1.0, 2.0, 3.0} ));
#endif

    double x = 1.23;
    double y = 2.34;
    double z = 3.45;
    Point<3> p1(x, y, z);
    Point<3> p2({x, y, z});
    
    Point<3> p3({x, y, x});
    EXPECT_EQ(p1, p2);
    EXPECT_NE(p1, p3);
}

TEST(PointTests, BasicAlgebra)
{
    double x = 1.0;
    double y = 2.0;
    double z = 3.0;
    
    Point<3> p1(x, y, z);
    Point<3> p2;
    p2 = {x, y, z};
    
    EXPECT_EQ(p1, p2);
    Point<3> p3 = -p1;
    
    EXPECT_EQ(p3[0], -x);
    EXPECT_EQ(p3[1], -y);
    EXPECT_EQ(p3[2], -z);
    
    p3 = -p3;
    EXPECT_EQ(p3, p1);
    
    p1 += p2;
    EXPECT_EQ(p1[0], x * 2);
    EXPECT_EQ(p1[1], y * 2);
    EXPECT_EQ(p1[2], z * 2);
    
    p1 = p2;
    p1 -= p2;
    EXPECT_EQ(p1[0], 0.0);
    EXPECT_EQ(p1[1], 0.0);
    EXPECT_EQ(p1[2], 0.0);
    
    double k = -2.345;
    p1 = p2;
    p1 *= k;
    EXPECT_EQ(p1[0], x * k);
    EXPECT_EQ(p1[1], y * k);
    EXPECT_EQ(p1[2], z * k);
    
    p1 = p2;
    p1 /= k;
    EXPECT_EQ(p1[0], x / k);
    EXPECT_EQ(p1[1], y / k);
    EXPECT_EQ(p1[2], z / k);
    
    p1 = p2 + p2*k;
    EXPECT_EQ(p1[0], p2[0]  + p2[0] * k);
    EXPECT_EQ(p1[1], p2[1]  + p2[1] * k);
    EXPECT_EQ(p1[2], p2[2]  + p2[2] * k);
    
    p1 = p2 - p2/k;
    EXPECT_EQ(p1[0], p2[0] - p2[0] / k);
    EXPECT_EQ(p1[1], p2[1] - p2[1] / k);
    EXPECT_EQ(p1[2], p2[2] - p2[2] / k);
}
