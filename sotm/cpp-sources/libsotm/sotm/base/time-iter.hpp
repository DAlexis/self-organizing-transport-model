/*
 * time-iter.hpp
 *
 *  Created on: 6 июня 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_
#define LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_

#include "sotm/utils/macros.hpp"
#include "sotm/utils/assert.hpp"

#include <vector>
#include <limits>
#include <cstddef>
#include <cmath>

namespace sotm
{

struct ContiniousIteratorParameters;
struct ContiniousIteratorMetrics;

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
	constexpr static double stepsCountNotMatter = std::numeric_limits<double>::max();
	virtual ~IContinuousTimeIterable() { }


	virtual void clearSubiteration() = 0;

	/** Calculate secondary values thats are trivial function of quantities and does not
	 * contain any differential operators. This function should be called for all objects
	 * before calling calculateRHS
	 */
	virtual void calculateSecondaryValues(double time) = 0;

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

	/**
	 * Function used for precision control.
	 *
	 * Get square of norm of difference between delta and last delta
	 */
	virtual double getDeltaDifferenceNormSqr() { return 0.0; }

	/**
	 * Function used for precision control.
	 *
	 * Get square of norm of difference between delta and last delta
	 */
	virtual double getDeltaNormSqr() { return 0.0; }

	/**
	 * Function used for precision control.
	 *
	 * Get minimal steps count needed to reach zero from maximal value
	 * for any variable
	 */
	virtual double getMinimalStepsCount() { return stepsCountNotMatter; }
};

class IBifurcationTimeIterable
{
public:
	virtual ~IBifurcationTimeIterable() {}

	virtual void prepareBifurcation(double time, double dt) = 0;
	virtual void doBifurcation(double time, double dt) = 0;
};

class IContinuousTimeIterator
{
public:
	virtual ~IContinuousTimeIterator() {}
	virtual void setIterable(IContinuousTimeIterable* target) = 0;
	/**
	 * Make one iteration. If timestep adaptation enabled, timestep may be changed
	 * @return new timestep
	 */
	virtual double iterate(double dt) = 0;
	virtual void setTime(double time) = 0;
	virtual double time() = 0;
	virtual void setStepBounds(double min, double max) = 0;
	virtual double getMinStep() = 0;
	virtual double getMaxStep() = 0;
	virtual void setParameters(ContiniousIteratorParameters* parameters) = 0;
	virtual const ContiniousIteratorMetrics& metrics() = 0;
};

class ITimeHook
{
public:
	virtual ~ITimeHook() {}
	virtual void runHook(double time) = 0;
	virtual double getNextTime() = 0;
};

class Variable
{
public:
	Variable(double initValue = 0.0) :
		previous(initValue), current(initValue)
	{
		updateMaxAbs();
	}

	SOTM_INLINE void clearSubIteration() { current = previous; delta = 0.0;}
	SOTM_INLINE void makeSubIteration(double dt) { current = previous + rhs * dt; }
	SOTM_INLINE void addRHSToDelta(double m) { delta += rhs * m; }
	SOTM_INLINE void step()
	{
		current = previous = previous + delta;
		delta = 0.0;
		updateMaxAbs();
	}
	SOTM_INLINE void setInitial(double value) { previous = current = value; }

	SOTM_INLINE double getCurrentStepsCount()
	{
		if (maxAbs == 0.0 || delta*previous > 0)
			return IContinuousTimeIterable::stepsCountNotMatter;
		return maxAbs / fabs(delta);
	}

	void set(double value)
	{
		previous = current = value;
		delta = 0;
		rhs = 0;
	}

	double previous;
	double current;
	double delta = 0.0;
	double rhs = 0.0;

private:
	double maxAbs = 0.0;

	SOTM_INLINE void updateMaxAbs()
	{
		double abs = fabs(previous);
		if (abs > maxAbs)
			maxAbs = abs;
	}
};

struct ContiniousIteratorParameters
{
	bool autoStepAdjustment = false;
	double iterationsPerAmplitudeMin = 3;
	double iterationsPerAmplitudeMax = 20;
};

struct ContiniousIteratorMetrics
{
	size_t totalStepCalculations = 0;
	size_t timeIterations = 0;

    double adaptationEfficiency() { return double(timeIterations) / totalStepCalculations; }
};

class TimeHookPeriodic : public ITimeHook
{
public:
	void runHook(double time) override final { m_lastRun = time; hook(time); }
	double getNextTime() override final { return m_lastRun + m_period; }

	void setPeriod(double period) { m_period = period; }
	double getPeriod() { return m_period; }
	virtual void hook(double time) = 0;

private:
	double m_lastRun = 0.0;
	double m_period = 1.0;
};

class ContinuousTimeIteratorBase : public IContinuousTimeIterator
{
public:
	void setIterable(IContinuousTimeIterable* target) override;
	void setTime(double time) override;
	double time() override;
	void setStepBounds(double min, double max) override;
	double getMinStep() override;
	double getMaxStep() override;
	void setParameters(ContiniousIteratorParameters* parameters) override;
	const ContiniousIteratorMetrics& metrics() override;

protected:
	double m_time = 0;
	IContinuousTimeIterable* m_target = nullptr;
	double m_stepMin = 0.0, m_stepMax = 0.0;
	static const ContiniousIteratorParameters defaultParameters;
	const ContiniousIteratorParameters *m_parameters = &defaultParameters;
	ContiniousIteratorMetrics m_metrics;
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
	void setStepBounds(double min, double max);
	void setStopTime(double stopTime);
	void addHook(ITimeHook* hook);

	double getStep();
	double getTimestepMin();
	double getTimestepMax();
	double getStopTime();
	double getTime();

	bool isDone();

	void iterate();
	void run();

	/**
	 * Stop after current iteration. May be called from another thread
	 */
	void stop();

	ContiniousIteratorParameters& continiousIterParameters();

private:
	void callHook();
	void findNextHook();

	double m_stopTime = 1.0;
	double m_dt = 0.01;
	double m_bifurcationPeriod = 0;
	double m_lastBifurcationTime = 0;

	double m_nextHookTime = 0;
	size_t m_nextHook = 0;
	bool m_needStop = false;

	ContiniousIteratorParameters m_contIteratorParameters;

	IContinuousTimeIterable* m_continiousIterable;
	IContinuousTimeIterator* m_continiousIterator;
	IBifurcationTimeIterable* m_bifurcationIterable;

	std::vector<ITimeHook*> m_timeHooks;
};

class PeriodicStopHook : public TimeHookPeriodic
{
public:
	PeriodicStopHook(TimeIterator* iterator) : m_iterator(iterator) {}
	void hook(double time) override { UNUSED_ARG(time); m_iterator->stop(); }

private:
	TimeIterator* m_iterator;
};

}

#endif /* LIBSTEPMOD_STEPMOD_BASE_TIME_ITER_HPP_ */
