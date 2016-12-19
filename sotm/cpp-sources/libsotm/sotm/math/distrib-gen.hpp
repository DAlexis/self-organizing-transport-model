/*
 * distrib-gen.hpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_DISTRIB_DISTRIB_GEN_HPP_
#define LIBSOTM_SOTM_DISTRIB_DISTRIB_GEN_HPP_

#include "sotm/math/geometry.hpp"
#include "sotm/math/generic.hpp"


namespace sotm
{

/**
 * Result of throw with not normed distribution
 */
template<typename T>
struct DistributionResult
{
	DistributionResult() { }
	DistributionResult(const T& obj) {
		value = obj;
		isHappened = true;
	}
	T value;
	bool isHappened = false;
};

/**
 * Generate direction of discharge from conductive sphere. Normal field on sphere is
 * E_n = E_0*cos(theta) + E_1. Probability of discharge per 1m^2 given by integral
 * distribution function integralDistribution.
 */
DistributionResult<SphericalPoint> generateDischargeDirection(double dt, double r, double E0, double E1, Function1D integralDistribution);

}

#endif /* LIBSOTM_SOTM_DISTRIB_DISTRIB_GEN_HPP_ */
