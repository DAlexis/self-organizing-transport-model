/*
 * distrib-gen.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "sotm/math/distrib-gen.hpp"
#include "sotm/math/random.hpp"
#include "sotm/utils/const.hpp"

using namespace sotm;

DistributionResult<SphericalPoint> generateDischargeDirection(double E0, double E1, Function1D integralDistribution)
{
	double val = Random::uniform(0.0, 1.0) * E0;
	double tmp = integralDistribution(E0+E1) - val;
	if (tmp < 0)
	{
		return DistributionResult<SphericalPoint>();
	}
	SphericalPoint result;
	result.phi = Random::uniform(0.0, 2*Const::pi);

}
