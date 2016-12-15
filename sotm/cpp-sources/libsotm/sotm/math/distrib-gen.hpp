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
	T value;
	bool isHappened = false;
};

DistributionResult<SphericalPoint> generateDischargeDirection(double E0, double E1, Function1D integralDistribution);

}

#endif /* LIBSOTM_SOTM_DISTRIB_DISTRIB_GEN_HPP_ */
