/*
 * distrib-gen.hpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_DISTRIB_DISTRIB_GEN_HPP_
#define LIBSOTM_SOTM_DISTRIB_DISTRIB_GEN_HPP_

#include "sotm/math/geometry.hpp"
#include "sotm/math/geometry.hpp"

namespace sotm
{

/**
 * Result of throw with not normed distribution
 */
struct DistributionResult
{
	double value = 0.0;
	bool isHappened = false;
};

}

#endif /* LIBSOTM_SOTM_DISTRIB_DISTRIB_GEN_HPP_ */
