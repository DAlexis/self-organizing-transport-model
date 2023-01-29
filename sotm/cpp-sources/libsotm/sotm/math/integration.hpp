/*
 * integration.hpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_MATH_INTEGRATION_HPP_
#define LIBSOTM_SOTM_MATH_INTEGRATION_HPP_

#include "sotm/math/generic.hpp"

#include <vector>
#include <cstdint>

namespace sotm {

class DefinedIntegral
{
public:
    DefinedIntegral(Function1D target, double from, double to, std::size_t pointsCount);
	double operator()(double arg);

	// Use this to prevent copying to whole object to wrapper
	Function1D function()
	{
		return [this](double arg)
		{
			return (*this)(arg);
		};
	}

private:
	std::vector<double> m_results;
	double m_from, m_to;
};

}  // namespace sotm

#endif /* LIBSOTM_SOTM_MATH_INTEGRATION_HPP_ */
