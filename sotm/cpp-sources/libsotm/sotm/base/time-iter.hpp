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
class IContinuousTimeIterable
{
public:
	virtual ~IContinuousTimeIterable() { }

	/// Calculate right hand side with falues of f(time, xCurrent, xCurrent of other objects)
	virtual void calculateRHS(double time) = 0;

	/// Adds rhs multiplied by m to xDelta
	virtual void addRHSToDelta(double m) = 0;

	/// Makes xCurrent = xPrevious + rhs*dt
	virtual void makeSubIteration(double dt) = 0;

	/// Makes xCurrent = xPrevious = xPrevious + xDelta; xDelta = 0;
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

	void iterate(double dt);
	void run(double stopTime, double dt);

private:
	double m_bifurcationPeriod = 0;
	double m_lastBifurcationTime = 0;
	IContinuousTimeIterable* m_continiousIterable;
	IContinuousTimeIterator* m_continiousIterator;
	IBifurcationTimeIterable* m_bifurcationIterable;
};

}

#endif /* LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_ */
