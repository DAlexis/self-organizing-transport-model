/*
 * euler-explicit.hpp
 *
 *  Created on: 6 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_TIME_ITER_EULER_EXPLICIT_HPP_
#define LIBSOTM_SOTM_TIME_ITER_EULER_EXPLICIT_HPP_

#include "sotm/base/time-iter.hpp"

namespace sotm {

class EulerExplicitIterator : public ContinuousTimeIteratorBase
{
public:
	void iterate(double dt) override final;
};

}  // namespace sotm







#endif /* LIBSOTM_SOTM_TIME_ITER_EULER_EXPLICIT_HPP_ */
