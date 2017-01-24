#include "sotm/base/time-iter.hpp"
#include "sotm/utils/assert.hpp"

using namespace sotm;

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
}

void TimeIterator::setStopTime(double stopTime)
{
	m_stopTime = stopTime;
}

void TimeIterator::addHook(ITimeHook* hook)
{
	m_timeHooks.push_back(hook);
	if (m_timeHooks.size() == 1)
	{
		m_nextHookTime = hook->getNextTime();
		m_nextHook = 0;
	}
}

double TimeIterator::getStep()
{
	return m_dt;
}

double TimeIterator::getStopTime()
{
	return m_stopTime;
}

double TimeIterator::getTime()
{
	return m_continiousIterator->time();
}

void TimeIterator::iterate()
{
	double time = m_continiousIterator->time();
	m_continiousIterator->iterate(m_dt);

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
	m_nextHookTime = m_timeHooks[0]->getNextTime();
	m_nextHook = 0;
	for (size_t i=1; i<m_timeHooks.size(); i++)
	{
		double candidatTime = m_timeHooks[i]->getNextTime();
		if (candidatTime < m_nextHook) {
			m_nextHook = i;
			m_nextHookTime = candidatTime;
		}
	}
}

void TimeIterator::run()
{
	while (m_continiousIterator->time() < m_stopTime)
		iterate();
}
