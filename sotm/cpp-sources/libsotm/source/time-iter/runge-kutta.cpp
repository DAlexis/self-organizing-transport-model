/*
 * runge-kutta.cpp
 *
 *  Created on: 12 янв. 2017 г.
 *      Author: dalexies
 */

#include "sotm/time-iter/runge-kutta.hpp"

using namespace sotm;

void RungeKuttaIterator::iterate(double dt)
{
	// Sub-iteration 1

	// k1 = f(tn, xn)
	m_target->calculateSecondaryValues(m_time);
	m_target->calculateRHS(m_time);
	m_target->addRHSToDelta(dt / 6.0);

	// k2 = f(tn + dt/2, xn + dt/2*k1)
	m_target->makeSubIteration(dt / 2.0);
	m_target->calculateSecondaryValues(m_time + dt / 2.0);
	m_target->calculateRHS(m_time + dt / 2.0);
	m_target->addRHSToDelta(dt / 3.0);

	// k3 = f(tn + dt/2, xn + dt/2*k2)
	m_target->makeSubIteration(dt / 2.0);
	m_target->calculateSecondaryValues(m_time + dt / 2.0);
	m_target->calculateRHS(m_time + dt / 2.0);
	m_target->addRHSToDelta(dt / 3.0);

	// k3 = f(tn + dt, xn + dt*k3)
	m_target->makeSubIteration(dt);
	m_target->calculateSecondaryValues(m_time + dt);
	m_target->calculateRHS(m_time + dt);
	m_target->addRHSToDelta(dt / 6.0);

	m_target->step();
	m_time += dt;
}
