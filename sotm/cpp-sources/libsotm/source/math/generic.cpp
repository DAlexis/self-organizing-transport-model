/*
 * generic.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "sotm/math/generic.hpp"

using namespace sotm;

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
	if (fr > fl)
	{
		while (right - left > m_precision)
		{
			double f = m_func(center);
			if (f-x < 0.0) left = center;
			if (f-x > 0.0) right = center;
			center = (right+left)/2.0;
		}
	} else {
		while (right - left > m_precision)
		{
			double f = m_func(center);
			if (f-x > 0.0) left = center;
			if (f-x < 0.0) right = center;
			center = (right+left)/2.0;
		}
	}
	return center;
}

bool MonotonicFunctionSolver::inRange(double x)
{
	double lv = m_func(m_from);
	double rv = m_func(m_to);

	return (lv-x)*(rv-x) <= 0;
}
