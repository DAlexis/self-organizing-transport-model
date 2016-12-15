/*
 * generic.hpp
 *
 *  Created on: 26 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_MATH_GENERIC_HPP_
#define LIBSOTM_SOTM_MATH_GENERIC_HPP_

#include <functional>

namespace sotm {

using Function1D = std::function<double(double)>;

template<typename T>
T sqr(const T& x) { return x*x; }

double MonotonicFunctionSolve(Function1D func, double value, double from, double to, double precision);

}  // namespace sotm




#endif /* LIBSOTM_SOTM_MATH_GENERIC_HPP_ */
