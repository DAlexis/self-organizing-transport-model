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

/**
 * This class represent some lines from system x'=f(time, x)
 * This interface suppose that users implementation contains those vectors (or may be named values):
 * xPrevious
 * xCurrent
 * rhs
 * xDelta
 *
 * All operations over this interface are system structure-safe
 */
class ITimeIterable
{
public:
	virtual ~ITimeIterable() { }

	/*
	/// Do some jobs that may globally change system structure, e. g. remove objects
	virtual void doBifurcation(double dt) = 0;
	 */

	/// Calculate right hand side with falues of f(time, xCurrent, xCurrent of other objects)
	virtual void calculateRHS(double time) = 0;

	/// Adds rhs multiplied by m to xDelta
	virtual void addRHSToDelta(double m) = 0;

	/// Makes xCurrent = xPrevious + rhs*dt
	virtual void makeSubIteration(double dt) = 0;

	/// Makes xCurrent = xPrevious = xPrevious + xDelta; xDelta = 0;
	virtual void step() = 0;
};

}







#endif /* LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_ */
