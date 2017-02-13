/*
 * runge-kutta.cpp
 *
 *  Created on: 12 янв. 2017 г.
 *      Author: dalexies
 */

#include "sotm/time-iter/runge-kutta.hpp"
#include <iostream>

using namespace sotm;
using namespace std;


double RungeKuttaIterator::iterate(double dt)
{
	if (m_parameters->autoStepAdjustment)
	{
		if (dt < m_stepMin)
			dt = m_stepMin;

		if (dt > m_stepMax)
			dt = m_stepMax;
	}

	cout << "Selecting time step..." << endl;
	for(;;) {
		makeSubiterations(dt);

		// If step adjustment disabled
		if (!m_parameters->autoStepAdjustment)
			break;


		double iterationsCount = m_target->getMinimalStepsCount();

		cout << "MinStepsCount = " << iterationsCount << endl;

		if (iterationsCount == IContinuousTimeIterable::stepsCountNotMatter)
			break;

		if (iterationsCount < m_parameters->iterationsPerAmplitudeMin)
		{
			if (dt == m_stepMin)
				break;

			dt *= 0.8;
			cout << "Deacreasing step to " << dt << endl;
			if (dt < m_stepMin)
				dt = m_stepMin;

			m_target->clearSubiteration();
			continue;
		}
		if (iterationsCount > m_parameters->iterationsPerAmplitudeMax)
		{
			if (dt == m_stepMax)
				break;

			dt *= 1.2;
			cout << "Increasing step to " << dt << endl;
			if (dt > m_stepMax)
				dt = m_stepMax;

			m_target->clearSubiteration();
			continue;
		}
		break;
	}
	m_target->step();
	m_time += dt;
	return dt;
}

void RungeKuttaIterator::makeSubiterations(double dt)
{
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
}
