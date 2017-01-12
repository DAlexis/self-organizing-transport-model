/*
 * time-iter.hpp
 *
 *  Created on: 6 июня 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_
#define LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_

#include "sotm/utils/macros.hpp"

namespace sotm
{

struct Variable {
	double previous = 0;
	double current = 0;
	double delta = 0;
	double rhs = 0;

	SOTM_INLINE void makeSubIteration(double dt) { current = previous + rhs * dt; }
	SOTM_INLINE void addRHSToDelta(double m) { delta += rhs * m; }
	SOTM_INLINE void step()
	{
		current = previous = previous + delta;
		delta = 0.0;
	}
	SOTM_INLINE void setInitial(double value) { previous = current = value; }
};

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
class IContinuousTimeIterable
{
public:
	virtual ~IContinuousTimeIterable() { }

	/** Calculate secondary values thats are trivial function of quantities and does not
	 * contain any differential operators. This function should be called for all objects
	 * before calling calculateRHS
	 */
	virtual void calculateSecondaryValues() = 0;

	/** Calculate right hand side with values of
	 * f(time, xCurrent, xCurrent of other objects, secondaryValues of other objects)
	 */
	virtual void calculateRHS(double time) = 0;

	/**
	 * Adds rhs multiplied by m to xDelta
	 */
	virtual void addRHSToDelta(double m) = 0;

	/**
	 * Makes xCurrent = xPrevious + rhs*dt
	 */
	virtual void makeSubIteration(double dt) = 0;

	/*
	 * Makes xCurrent = xPrevious = xPrevious + xDelta; xDelta = 0;
	 */
	virtual void step() = 0;
};

class IBifurcationTimeIterable
{
public:
	virtual ~IBifurcationTimeIterable() {}

	virtual void doBifurcation(double time, double dt) = 0;
};

class IContinuousTimeIterator
{
public:
	virtual ~IContinuousTimeIterator() {}
	virtual void setIterable(IContinuousTimeIterable* target) = 0;
	virtual void iterate(double dt) = 0;
	virtual void setTime(double time) = 0;
	virtual double time() = 0;
};

class ContinuousTimeIteratorBase : public IContinuousTimeIterator
{
public:
	void setIterable(IContinuousTimeIterable* target) override;
	void setTime(double time) override;
	double time() override;

protected:
	double m_time = 0;
	IContinuousTimeIterable* m_target = nullptr;
};

class TimeIterator
{
public:
	TimeIterator(
		IContinuousTimeIterable* continiousIterable,
		IContinuousTimeIterator* continiousIterator,
		IBifurcationTimeIterable* bifurcationIterable = nullptr
	);

	void setTime(double time);
	void setBifurcationRunPeriod(double bifurcationPeriod);
	void setStep(double dt);
	void setStopTime(double stopTime);

	double getStep();
	double getStopTime();
	double getTime();

	void iterate();
	void run();

private:
	double m_stopTime = 1.0;
	double m_dt = 0.01;
	double m_bifurcationPeriod = 0;
	double m_lastBifurcationTime = 0;
	IContinuousTimeIterable* m_continiousIterable;
	IContinuousTimeIterator* m_continiousIterator;
	IBifurcationTimeIterable* m_bifurcationIterable;
};

}

#endif /* LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_ */
