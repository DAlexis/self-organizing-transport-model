/*
 * integration.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "sotm/math/integration.hpp"

#include <cmath>

using namespace sotm;

DefinedIntegral::DefinedIntegral(Function1D target, double from, double to, size_t pointsCount) :
		m_from(from),
		m_to(to)
{
	m_results.resize(pointsCount, 0);
	double range = m_to - m_from;
	double dx = range / pointsCount;

	for (size_t i=1; i<pointsCount; i++)
	{
		m_results[i] = m_results[i-1] + target(m_from + dx*(i-0.5))*dx;
	}
}

double DefinedIntegral::operator()(double arg)
{
	if (arg < m_from)
		return 0;

	if (arg > m_to)
		return m_results.back();

	size_t index = round((arg-m_from) * (m_results.size()-1) / (m_to - m_from));
	return m_results[index];
}

