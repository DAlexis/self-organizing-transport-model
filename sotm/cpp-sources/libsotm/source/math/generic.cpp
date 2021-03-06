/*
 * generic.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "sotm/math/generic.hpp"

#include <cmath>

using namespace sotm;

double sotm::zero(double)
{
	return 0.0;
}

/////////////////////////////////////////////////////
// MonotonicFunctionSolver

MonotonicFunctionSolver::MonotonicFunctionSolver(Function1D func, double from, double to, double precision) :
		m_func(func),
		m_from(from),
		m_to(to),
		m_precision(precision)
{
}

double MonotonicFunctionSolver::operator()(double x)
{
	double left = m_from, right = m_to;
	double fr = m_func(right);
	double fl = m_func(left);
	double center = (right+left)/2.0;
	double prevCenter = center;

	double sig = fr > fl ? 1.0 : -1.0;

	while (right - left > m_precision)
	{
		double f = m_func(center);
		if ((f - x) * sig < 0)
			left = center;
		else
			right = center;

		center = (right+left)/2.0;

		if (prevCenter == center)
		{
			// We have excess of precision: center is numerically equal its old value
			break;
		}

		prevCenter = center;
	}
	return center;
}

bool MonotonicFunctionSolver::inRange(double x)
{
	double lv = m_func(m_from);
	double rv = m_func(m_to);

	return (lv-x)*(rv-x) <= 0;
}

/////////////////////////////////////////////////////
// SmoothedLocalStepFunction

SmoothedLocalStepFunction::SmoothedLocalStepFunction(double stepPoint, double radius) :
		m_stepPoint(stepPoint), m_radius(radius)
{
}

Function1D SmoothedLocalStepFunction::get()
{
	return [this](double x) { return get(x); };
}

double SmoothedLocalStepFunction::get(double arg)
{
	if (arg <= m_stepPoint - m_radius)
		return 0.0;

	if (arg >= m_stepPoint + m_radius)
			return 1.0;

    return 0.5 + sin((arg-m_stepPoint) / m_radius * M_PI / 2) / 2.0;
}

double SmoothedLocalStepFunction::operator()(double arg)
{
	return get(arg);
}
