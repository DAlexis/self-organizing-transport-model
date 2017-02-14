#include "sotm/base/time-iter.hpp"
#include "sotm/utils/assert.hpp"

#include <iostream>

using namespace sotm;

const ContiniousIteratorParameters ContinuousTimeIteratorBase::defaultParameters;

void ContinuousTimeIteratorBase::setIterable(IContinuousTimeIterable* target)
{
	m_target = target;
}

void ContinuousTimeIteratorBase::setTime(double time)
{
	m_time = time;
}

double ContinuousTimeIteratorBase::time()
{
	return m_time;
}

void ContinuousTimeIteratorBase::setStepBounds(double min, double max)
{
	m_stepMin = min;
	m_stepMax = max;
}

double ContinuousTimeIteratorBase::getMinStep()
{
	return m_stepMin;
}

double ContinuousTimeIteratorBase::getMaxStep()
{
	return m_stepMax;
}

void ContinuousTimeIteratorBase::setParameters(ContiniousIteratorParameters* parameters)
{
	if (parameters)
		m_parameters = parameters;
	else
		m_parameters = &defaultParameters;
}

const ContiniousIteratorMetrics& ContinuousTimeIteratorBase::metrics()
{
	return m_metrics;
}

/////////////////////////
// TimeIterator

TimeIterator::TimeIterator(
		IContinuousTimeIterable* continiousIterable,
		IContinuousTimeIterator* continiousIterator,
		IBifurcationTimeIterable* bifurcationIterable
	) :
	m_continiousIterable(continiousIterable),
	m_continiousIterator(continiousIterator),
	m_bifurcationIterable(bifurcationIterable)
{
	ASSERT(continiousIterable != nullptr, "continiousIterable cannot be a nullptr");
	ASSERT(continiousIterator != nullptr, "continiousIterator cannot be a nullptr");
	m_continiousIterator->setIterable(m_continiousIterable);
	m_continiousIterator->setParameters(&m_contIteratorParameters);
}

void TimeIterator::setTime(double time)
{
	m_continiousIterator->setTime(time);
	m_lastBifurcationTime = time;
}

void TimeIterator::setBifurcationRunPeriod(double bifurcationPeriod)
{
	m_bifurcationPeriod = bifurcationPeriod;
}

void TimeIterator::setStep(double dt)
{
	m_dt = dt;
	m_continiousIterator->setStepBounds(m_dt, m_dt);
}

void TimeIterator::setStepBounds(double min, double max)
{
	m_continiousIterator->setStepBounds(min, max);
}

void TimeIterator::setStopTime(double stopTime)
{
	m_stopTime = stopTime;
}

void TimeIterator::addHook(ITimeHook* hook)
{
	m_timeHooks.push_back(hook);
	findNextHook();
}

double TimeIterator::getStep()
{
	return m_dt;
}

double TimeIterator::getTimestepMin()
{
	return m_continiousIterator->getMinStep();
}

double TimeIterator::getTimestepMax()
{
	return m_continiousIterator->getMaxStep();
}

double TimeIterator::getStopTime()
{
	return m_stopTime;
}

double TimeIterator::getTime()
{
	return m_continiousIterator->time();
}

bool TimeIterator::isDone()
{
	return m_continiousIterator->time() >= m_stopTime;
}

void TimeIterator::iterate()
{
	double time = m_continiousIterator->time();
	m_dt = m_continiousIterator->iterate(m_dt);

	if (
			(m_bifurcationPeriod == 0.0 || time - m_lastBifurcationTime >= m_bifurcationPeriod) &&
			m_bifurcationIterable != nullptr
		)
	{
		double dt = time - m_lastBifurcationTime;
		m_bifurcationIterable->prepareBifurcation(time, dt);
		m_bifurcationIterable->doBifurcation(time, dt);
		m_lastBifurcationTime = time;
	}
	callHook();
}

void TimeIterator::callHook()
{
	if (m_timeHooks.empty())
		return;
	double time = m_continiousIterator->time();
	if (time >= m_nextHookTime)
	{
		m_timeHooks[m_nextHook]->runHook(time);
		findNextHook();
	}
}

void TimeIterator::findNextHook()
{
	if (m_timeHooks.empty())
		return;
	m_nextHookTime = m_timeHooks[0]->getNextTime();
	m_nextHook = 0;
	for (size_t i=1; i<m_timeHooks.size(); i++)
	{
		double candidatTime = m_timeHooks[i]->getNextTime();
		if (candidatTime < m_nextHookTime) {
			m_nextHook = i;
			m_nextHookTime = candidatTime;
		}
	}
}

void TimeIterator::run()
{
	m_needStop = false;
	findNextHook();
	std::cout << "Next hook: " << m_nextHookTime << std::endl;
	while (!isDone() && !m_needStop)
		iterate();
}

void TimeIterator::stop()
{
	m_needStop = true;
}

ContiniousIteratorParameters& TimeIterator::continiousIterParameters()
{
	return m_contIteratorParameters;
}
