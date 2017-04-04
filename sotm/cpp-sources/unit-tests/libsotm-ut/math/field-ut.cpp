/*
 * field-ut.cpp
 *
 *  Created on: 3 апр. 2017 г.
 *      Author: dalexies
 */

#include "sotm/math/field.hpp"

#include "gtest/gtest.h"
#include <cmath>

using namespace sotm;

TEST(FieldClasses, Instantiation)
{
	using fz11 = FieldZero<1, 1>;
	using fz12 = FieldZero<1, 2>;
	using fz19 = FieldZero<1, 9>;
	ASSERT_NO_THROW(fz11());
	ASSERT_NO_THROW(fz12());
	ASSERT_NO_THROW(fz19());

	using fz25 = FieldZero<2, 5>;
	ASSERT_NO_THROW(fz25());

	ASSERT_NO_THROW(FieldLinearScalar<3>(10, {1.0, 0.0, 0.0}));
}

TEST(FieldClasses, LinearField)
{
	FieldLinearScalar<3> lf(10.0, {1.0, 0.0, 0.0});
	ASSERT_EQ(lf({1.0, 0.0, 0.0}), 10.0);
}

TEST(FieldClasses, FieldScalar1D)
{
	FieldScalar1D<3> fs1d([](double x) { return sin(x); }, {1.0, 1.0, 0.0});
	ASSERT_NEAR(
			fs1d({2.0, 2.0, 0.0}),
			sin(sqrt(2.0)*2.0),
			1e-10
	);
}

TEST(FieldGradients, Instantiation)
{
	FieldScalar1D<3> fs1d([](double x) { return sin(x); }, {1.0, 1.0, 0.0});
	ASSERT_NO_THROW(GradientFixedStep<3>(fs1d, 1e-5));
}

TEST(FieldGradients, GradientFixedStep)
{
	FieldScalar1D<3> fs1d([](double x) { return sin(x); }, {1.0, 1.0, 0.0});
	GradientFixedStep<3> g(fs1d, 1e-5);
	ASSERT_NEAR(
			g({2.0, 2.0, 0.0})[0],
			cos(sqrt(2.0)*2.0) * sqrt(2.0)/2.0,
			1e-3
	);
}
