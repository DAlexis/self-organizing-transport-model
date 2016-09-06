#include "sotm/time-iter/euler-explicit.hpp"

using namespace sotm;

void EulerExplicitIterator::iterate(double dt)
{
	m_target->calculateRHS(m_time);
	m_target->addRHSToDelta(dt);
	m_target->step();
}
