/*
 * distrib-gen.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "sotm/math/distrib-gen.hpp"
#include "sotm/math/generic.hpp"
#include "sotm/math/random.hpp"

#include <cmath>

using namespace sotm;

DistributionResult<SphericalPoint> sotm::generateDischargeDirection(
		double dt,
		double r,
		double E0,
		double E1,
		Function1D distribution,
		Function1D integralDistribution
)
{
	/**
	 * rnd = 1/E_0 (F(E_0+E_1) - F(E_0*cos(theta) + E_1))
	 */
	// @todo what if E0 == 0?

	constexpr double externalMin = 0.001;

	SphericalPoint result;

	if (fabs(E0 / E1) < externalMin) {
		// We have small external field so we can generate uniform distribution by Omega
        double val = Random::uniform(0.0, 1.0) / (2*M_PI * r * r * dt * distribution(E1));
		if (val <= 2.0)
		{
			double cosTheta = 1.0-val;
            result.phi = Random::uniform(0.0, 2*M_PI);
			result.theta = acos(cosTheta);
			return DistributionResult<SphericalPoint>(result);
		} else {
			return DistributionResult<SphericalPoint>();
		}
	}

    double val = Random::uniform(0.0, 1.0) * E0 / (2*M_PI * r * r * dt);
	double tmp = integralDistribution(E0+E1) - val;
	if (tmp < 0)
	{
		return DistributionResult<SphericalPoint>();
	}

    result.phi = Random::uniform(0.0, 2*M_PI);

    MonotonicFunctionSolver solver(integralDistribution, E1-1.5*E0, E1+1.5*E0, M_PI / 1000.0);

	if (!solver.inRange(tmp))
		return DistributionResult<SphericalPoint>();

	double arg = solver(tmp); // == E_1 + E_0 * cos(theta)
	double cosTheta = (arg-E1)/E0;
	if (cosTheta > 1.0 || cosTheta < -1.0)
		return DistributionResult<SphericalPoint>();
	result.theta = acos(cosTheta);

	return DistributionResult<SphericalPoint>(result);
}
