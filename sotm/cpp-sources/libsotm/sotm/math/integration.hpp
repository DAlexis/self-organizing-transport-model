/*
 * integration.hpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_MATH_INTEGRATION_HPP_
#define LIBSOTM_SOTM_MATH_INTEGRATION_HPP_

#include <functional>
#include <vector>

namespace sotm {

class DefinedIntegral
{
public:
	using Function = std::function<double(double)>;

	DefinedIntegral(Function target, double from, double to, size_t pointsCount);
	double operator()(double arg);

private:
	std::vector<double> m_results;
	double m_from, m_to;
};

}  // namespace sotm

#endif /* LIBSOTM_SOTM_MATH_INTEGRATION_HPP_ */
