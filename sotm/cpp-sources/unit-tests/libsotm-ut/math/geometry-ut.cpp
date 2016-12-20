#include "sotm/math/geometry.hpp"
#include "sotm/utils/const.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace sotm;

TEST(PointTests, PointInstanciation)
{
    EXPECT_NO_THROW(Vector<1>());
    EXPECT_NO_THROW(Vector<2>());
    EXPECT_NO_THROW(Vector<3>());
    
    EXPECT_NO_THROW(Vector<1>(1.0));
    EXPECT_NO_THROW(Vector<2>(1.0, 2.0));
    EXPECT_NO_THROW(Vector<3>(1.0, 2.0, 3.0));

#ifdef DEBUG
    EXPECT_ANY_THROW(Vector<1>(1.0, 2.0, 3.0));
    EXPECT_ANY_THROW(Vector<2>(1.0, 2.0, 3.0));
    
    EXPECT_ANY_THROW(Vector<2>(1.0));

    EXPECT_ANY_THROW(Vector<2>( {1.0, 2.0, 3.0} ));
#endif

    double x = 1.23;
    double y = 2.34;
    double z = 3.45;
    Vector<3> p1(x, y, z);
    Vector<3> p2({x, y, z});
    
    Vector<3> p3({x, y, x});
    EXPECT_EQ(p1, p2);
    EXPECT_NE(p1, p3);
}

TEST(PointTests, BasicAlgebra)
{
    double x = 1.0;
    double y = 2.0;
    double z = 3.0;
    
    Vector<3> p1(x, y, z);
    Vector<3> p2;
    p2 = {x, y, z};
    
    EXPECT_EQ(p1, p2);
    Vector<3> p3 = -p1;
    
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

TEST(angle, SomeCases)
{
	{
		Vector<3> v1{3.0, 0.0, 0.0};
		Vector<3> v2{0.0, 5.0, 0.0};

		ASSERT_NEAR(angle(v1, v2), Const::pi/2, 0.01);
	}
	{
		Vector<3> v1{1.0, 1.0, 0.0};
		Vector<3> v2{1.0, 0.0, 0.0};

		ASSERT_NEAR(angle(v1, v2), Const::pi/4, 0.01);
	}
}

TEST(PlaceVectorRelatively, GeneralCase)
{
	{
		Vector<3> base{1.0, 2.0, -0.01}; // Near plane XY
		double len = 1.0;
		double theta = 0.2;
		double phi = 0.4;
		SphericalVectorPlacer p(base);

		Vector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
		ASSERT_NEAR(a, theta, 0.00000001);
		ASSERT_NEAR(result.len(), len, 0.00000001);

	}
	{
		Vector<3> base{4.0, 0.01, 3.0}; // Near plane XY
		double len = 1.0;
		double theta = 0.1;
		double phi = 0.8;
		SphericalVectorPlacer p(base);

		Vector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
		ASSERT_NEAR(a, theta, 0.00000001);
	}
	{
		Vector<3> base{0.01, 1.8, 4.0}; // Near plane YZ
		double len = 1.0;
		double theta = 0.1;
		double phi = 0.8;
		SphericalVectorPlacer p(base);

		Vector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
		ASSERT_NEAR(a, theta, 0.00000001);
	}
	{
		Vector<3> base{1.0, 0.0, 0.0}; // axis X
		double len = 0.3;
		double theta = 0.2;//2.209;
		double phi = 0.0;//2.27;
		SphericalVectorPlacer p(base);

		Vector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
		ASSERT_NEAR(a, theta, 0.00000001);
	}
	{
		Vector<3> base{0.0, 1.0, 0.0}; // axis Y
		double len = 0.3;
		double theta = 2.209;
		double phi = 2.27;
		SphericalVectorPlacer p(base);

		Vector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
		ASSERT_NEAR(a, theta, 0.00000001);
	}
	{
		Vector<3> base{0.0, 0.0, -1.0}; // - axis Z
		double len = 0.3;
		double theta = 0.2;//2.209;
		double phi = 0.0;//2.27;
		SphericalVectorPlacer p(base);

		Vector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
		ASSERT_NEAR(a, theta, 0.00000001);
	}
}
