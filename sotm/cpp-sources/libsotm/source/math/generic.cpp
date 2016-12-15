/*
 * generic.cpp
 *
 *  Created on: 15 дек. 2016 г.
 *      Author: dalexies
 */

#include "sotm/math/generic.hpp"

using namespace sotm;

double sotm::MonotonicFunctionSolve(Function1D func, double value, double from, double to, double precision)
{
	double left = from, right = to;
	double fr = func(right);
	double fl = func(left);
	double center = (right+left)/2.0;
	if (fr > fl)
	{
		while (right - left > precision)
		{
			double f = func(center);
			if (f-value < 0.0) left = center;
			if (f-value > 0.0) right = center;
			center = (right+left)/2.0;
		}
	} else {
		while (right - left > precision)
		{
			double f = func(center);
			if (f-value > 0.0) left = center;
			if (f-value < 0.0) right = center;
			center = (right+left)/2.0;
		}
	}
	return center;
}
