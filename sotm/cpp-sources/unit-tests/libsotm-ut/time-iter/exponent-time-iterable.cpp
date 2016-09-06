/*
 * exponent-time-iterable.cpp
 *
 *  Created on: 6 сент. 2016 г.
 *      Author: dalexies
 */

#include "exponent-time-iterable.hpp"
#include "sotm/utils/macros.hpp"

void Exponent::calculateRHS(double time)
{
	UNUSED_ARG(time);
	rhs = xCurrent;
}

void Exponent::addRHSToDelta(double m)
{
	delta += rhs*m;
}

void Exponent::makeSubIteration(double dt)
{
	xCurrent = xPrevious + rhs*dt;
}

void Exponent::step()
{
	xCurrent = xPrevious = xPrevious + delta;
	delta = 0;
}

double Exponent::getValue()
{
	return xPrevious;
}
