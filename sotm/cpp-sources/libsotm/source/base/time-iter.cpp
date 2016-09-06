#include "sotm/base/time-iter.hpp"

using namespace sotm;

void TimeIteratorBase::setIterable(ITimeIterable* target)
{
	m_target = target;
}

void TimeIteratorBase::setTime(double time)
{
	m_time = time;
}
