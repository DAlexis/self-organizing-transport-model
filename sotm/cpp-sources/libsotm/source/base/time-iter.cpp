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

double TimeIterator::getStep()
{
	return m_dt;
}

double TimeIterator::getStopTime()
{
	return m_stopTime;
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
		m_bifurcationIterable->doBifurcation(time, time - m_lastBifurcationTime);
		m_lastBifurcationTime = time;
	}
}

void TimeIterator::run()
{
	while (m_continiousIterator->time() < m_stopTime)
		iterate();
}
