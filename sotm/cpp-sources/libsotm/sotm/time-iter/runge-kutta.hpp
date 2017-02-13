/*
 * runge-kutta.hpp
 *
 *  Created on: 12 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_TIME_ITER_RUNGE_KUTTA_HPP_
#define LIBSOTM_SOTM_TIME_ITER_RUNGE_KUTTA_HPP_

#include "sotm/base/time-iter.hpp"

namespace sotm {

class RungeKuttaIterator : public ContinuousTimeIteratorBase
{
public:
	double iterate(double dt) override final;

private:
	void makeSubiterations(double dt);
};

}  // namespace sotm









#endif /* LIBSOTM_SOTM_TIME_ITER_RUNGE_KUTTA_HPP_ */
