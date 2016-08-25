/*
 * time-iter.hpp
 *
 *  Created on: 6 июня 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_
#define LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_

namespace sotm
{

class ITimeIterable
{
public:
	virtual ~ITimeIterable() {}
	virtual void makeStep(double dt) = 0;
};

}







#endif /* LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_ */
