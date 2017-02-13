#include "sotm/time-iter/euler-explicit.hpp"

using namespace sotm;

double EulerExplicitIterator::iterate(double dt)
{
	m_target->calculateSecondaryValues(m_time);
	m_target->calculateRHS(m_time);
	m_target->addRHSToDelta(dt);
	m_target->step();
	m_time += dt;
	return dt;
}
