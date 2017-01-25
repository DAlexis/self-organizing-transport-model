#include "sotm/math/distrib-gen.hpp"
#include "sotm/math/generic.hpp"
#include "sotm/math/integration.hpp"
#include "sotm/utils/const.hpp"

#include "gtest/gtest.h"

#include <iostream>
#include <fstream>

using namespace std;

using namespace sotm;

TEST(generateDischargeDirection, E1zero)
{
	auto f = [](double E) { return fabs(E) * 0.1; };
	DefinedIntegral intF(f, -10, 10, 10000);

	size_t countFrom0ToPi_2 = 0;
	size_t countFromPi_2ToPi = 0;

	size_t countFrom0To05 = 0;
	size_t countFrom05To10 = 0;

	for (int i=0; i<50000; i++)
	{
		DistributionResult<SphericalPoint> res = generateDischargeDirection(1, 1, 1.0, 0.0, f, intF.function());
		if (res.isHappened) {
			if (res.value.theta < Const::pi / 2.0)
				countFrom0ToPi_2++;
			else
				countFromPi_2ToPi++;

			if (res.value.theta > 0 && res.value.theta < 0.5)
				countFrom0To05++;
			else if (res.value.theta > 0.5 && res.value.theta < 1)
				countFrom05To10++;
		}
	}
	ASSERT_NEAR(((double) countFrom05To10) / countFrom0To05, 2.13, 0.15);
	ASSERT_NEAR(((double) countFrom0ToPi_2) / countFromPi_2ToPi, 1.0, 0.05);

}

TEST(generateDischargeDirection, E1E0eq)
{
	auto f = [](double E) { return fabs(E) * 0.01; };
	DefinedIntegral intF(f, -10, 10, 10000);

	size_t countFrom10to15 = 0;
	size_t countFrom20to25 = 0;

	//ofstream fl("points.txt", fstream::out);
	for (int i=0; i<50000; i++)
	{
		DistributionResult<SphericalPoint> res = generateDischargeDirection(1.0, 1.0, 1.0, 1.0, f, intF.function());
		if (res.isHappened) {
			if (res.value.theta > 1.0 && res.value.theta < 1.5)
				countFrom10to15++;
			if (res.value.theta > 2.0 && res.value.theta < 2.5)
				countFrom20to25++;
			//fl << res.value.theta << " ";
		}
	}
	ASSERT_NEAR((double)countFrom10to15 / countFrom20to25, 4.22, 0.8);
}

TEST(generateDischargeDirection, E1E0eqOther)
{
	auto f = [](double E) { return fabs(E) * 0.1; };
	DefinedIntegral intF(f, -10, 10, 10000);

	size_t countFrom10to15 = 0;
	size_t countFrom20to25 = 0;

	//ofstream fl("points.txt", fstream::out);
	for (int i=0; i<50000; i++)
	{
		DistributionResult<SphericalPoint> res = generateDischargeDirection(0.1, 1.0, 1.0, 1.0, f, intF.function());
		if (res.isHappened) {
			if (res.value.theta > 1.0 && res.value.theta < 1.5)
				countFrom10to15++;
			if (res.value.theta > 2.0 && res.value.theta < 2.5)
				countFrom20to25++;
			//fl << res.value.theta << " ";
		}
	}
	ASSERT_NEAR((double)countFrom10to15 / countFrom20to25, 4.22, 0.8);
}

TEST(generateDischargeDirection, E0zero)
{
	auto f = [](double E) { return fabs(E) * 0.1; };
	DefinedIntegral intF(f, -10, 10, 10000);

	size_t countFrom0ToPi_2 = 0;
	size_t countFromPi_2ToPi = 0;


	//fstream fl("points.txt", fstream::out);
	for (int i=0; i<50000; i++)
	{
		DistributionResult<SphericalPoint> res = generateDischargeDirection(0.1, 1.0, 0.0, 1.0, f, intF.function());
		if (res.isHappened) {
			if (res.value.theta < Const::pi / 2.0)
				countFrom0ToPi_2++;
			else
				countFromPi_2ToPi++;
			//fl << res.value.theta << " ";
		}
	}
	ASSERT_NEAR(((double) countFrom0ToPi_2) / countFromPi_2ToPi, 1.0, 0.05);
}

TEST(generateDischargeDirection, E0moreThanE1)
{
	auto f = [](double E) { return E * E * 0.1; };
	DefinedIntegral intF(f, -10, 10, 10000);

	size_t countFrom00To21 = 0;
	size_t countFrom21ToPi = 0;


	//fstream fl("points.txt", fstream::out);
	for (int i=0; i<50000; i++)
	{
		DistributionResult<SphericalPoint> res = generateDischargeDirection(0.1, 1.0, 2.0, 1.0, f, intF.function());
		if (res.isHappened) {
			if (res.value.theta < 2.1)
				countFrom00To21++;
			else
				countFrom21ToPi++;
			//fl << res.value.theta << " ";
		}
	}
	ASSERT_NEAR((double) countFrom00To21 / countFrom21ToPi, 25.9, 4.0);
}

