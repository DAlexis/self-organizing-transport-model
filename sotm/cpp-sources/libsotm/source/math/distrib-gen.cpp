/*
 * distrib-gen.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "sotm/math/distrib-gen.hpp"
#include "sotm/math/generic.hpp"
#include "sotm/math/random.hpp"
#include "sotm/utils/const.hpp"

using namespace sotm;

DistributionResult<SphericalPoint> sotm::generateDischargeDirection(double dt, double r, double E0, double E1, Function1D integralDistribution)
{
	/**
	 * rnd = 1/E_0 (F(E_0+E_1) - F(E_0*cos(theta) + E_1))
	 */
	// @todo what if E0 == 0?
	double val = Random::uniform(0.0, 1.0) * E0 / (2*Const::pi * r * r * dt);
	double tmp = integralDistribution(E0+E1) - val;
	if (tmp < 0)
	{
		return DistributionResult<SphericalPoint>();
	}
	SphericalPoint result;
	result.phi = Random::uniform(0.0, 2*Const::pi);

	MonotonicFunctionSolver solver(integralDistribution, E1-1.5*E0, E1+1.5*E0, Const::pi / 1000.0);

	if (!solver.inRange(tmp))
		return DistributionResult<SphericalPoint>();

	double arg = solver(tmp); // == E_1 + E_0 * cos(theta)
	double cosTheta = (arg-E1)/E0;
	if (cosTheta > 1.0 || cosTheta < -1.0)
		return DistributionResult<SphericalPoint>();
	result.theta = acos(cosTheta);

	return DistributionResult<SphericalPoint>(result);
}
