#include "sotm/math/geometry.hpp"
#include "sotm/utils/const.hpp"

#include "gtest/gtest.h"

#include <iostream>

using namespace sotm;

TEST(PointTests, PointInstanciation)
{
    EXPECT_NO_THROW(StaticVector<1>());
    EXPECT_NO_THROW(StaticVector<2>());
    EXPECT_NO_THROW(StaticVector<3>());
    
    EXPECT_NO_THROW(StaticVector<1>(1.0));
    EXPECT_NO_THROW(StaticVector<2>(1.0, 2.0));
    EXPECT_NO_THROW(StaticVector<3>(1.0, 2.0, 3.0));

    double x = 1.23;
    double y = 2.34;
    double z = 3.45;
    StaticVector<3> p1(x, y, z);
    StaticVector<3> p2({x, y, z});
    
    StaticVector<3> p3({x, y, x});
    EXPECT_EQ(p1, p2);
    EXPECT_NE(p1, p3);
}

TEST(PointTests, BasicAlgebra)
{
    double x = 1.0;
    double y = 2.0;
    double z = 3.0;
    
    StaticVector<3> p1(x, y, z);
    StaticVector<3> p2;
    p2 = {x, y, z};
    
    EXPECT_EQ(p1, p2);
    StaticVector<3> p3 = -p1;
    
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
		StaticVector<3> v1{3.0, 0.0, 0.0};
		StaticVector<3> v2{0.0, 5.0, 0.0};

		ASSERT_NEAR(angle(v1, v2), Const::pi/2, 0.01);
	}
	{
		StaticVector<3> v1{1.0, 1.0, 0.0};
		StaticVector<3> v2{1.0, 0.0, 0.0};

		ASSERT_NEAR(angle(v1, v2), Const::pi/4, 0.01);
	}
}

TEST(VectorProduct, SomeCases)
{
    {
        StaticVector<3> v1{0.002279797838314907, 0.4103636108966833, 0.9119191353259629};
        StaticVector<3> v2{1.0, 0.0, 0.0};

        StaticVector<3> product = v1 % v2;
        double ang1 = angle(product, v1);
        double ang2 = angle(product, v2);

        EXPECT_NEAR(ang1, M_PI_2, 0.00001);
        EXPECT_NEAR(ang2, M_PI_2, 0.00001);
    }

}

TEST(PlaceVectorRelatively, GeneralCase)
{
	{
        StaticVector<3> base{1.0, 2.0, -0.01}; // low Z normal component
		double len = 1.0;
		double theta = 0.2;
		double phi = 0.4;
		SphericalVectorPlacer p(base);

		StaticVector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
        EXPECT_NEAR(a, theta, 0.00000001);
        EXPECT_NEAR(result.norm(), len, 0.00000001);

	}
	{
        StaticVector<3> base{4.0, 0.01, 3.0}; // low Y normal component
		double len = 1.0;
		double theta = 0.1;
		double phi = 0.8;
		SphericalVectorPlacer p(base);

		StaticVector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
        EXPECT_NEAR(a, theta, 0.00000001);
	}
	{
        StaticVector<3> base{0.01, 1.8, 4.0}; // low X normal component
		double len = 1.0;
		double theta = 0.1;
		double phi = 0.8;
		SphericalVectorPlacer p(base);

		StaticVector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
        EXPECT_NEAR(a, theta, 0.00000001);
	}
	{
		StaticVector<3> base{1.0, 0.0, 0.0}; // axis X
		double len = 0.3;
		double theta = 0.2;//2.209;
		double phi = 0.0;//2.27;
		SphericalVectorPlacer p(base);

		StaticVector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
        EXPECT_NEAR(a, theta, 0.00000001);
	}
	{
		StaticVector<3> base{0.0, 1.0, 0.0}; // axis Y
		double len = 0.3;
		double theta = 2.209;
		double phi = 2.27;
		SphericalVectorPlacer p(base);

		StaticVector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
        EXPECT_NEAR(a, theta, 0.00000001);
	}
	{
		StaticVector<3> base{0.0, 0.0, -1.0}; // - axis Z
		double len = 0.3;
		double theta = 0.2;//2.209;
		double phi = 0.0;//2.27;
		SphericalVectorPlacer p(base);

		StaticVector<3> result = p.place(len, theta, phi);
		double a = angle(result, base);
        EXPECT_NEAR(a, theta, 0.00000001);
	}
}
