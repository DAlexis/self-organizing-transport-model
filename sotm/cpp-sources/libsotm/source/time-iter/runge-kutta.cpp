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

	for(;;) {
		m_metrics.totalStepCalculations++;
		makeSubiterations(dt);

		// If step adjustment disabled
		if (!m_parameters->autoStepAdjustment)
			break;

		// Now checking step to be optimal
		double iterationsCount = m_target->getMinimalStepsCount();
		if (m_parameters->outputVerboseLevel >= ContiniousIteratorParameters::VerboseLevel::more)
		{
			cout << "[R-K4] Min steps count estimated as " << iterationsCount << endl;
		}

		if (iterationsCount == IContinuousTimeIterable::stepsCountNotMatter)
			break;

		if (iterationsCount < m_parameters->iterationsPerAmplitudeMin)
		{
			// Step is too big
			if (dt == m_stepMin)
				break;

            dt *= 0.6;
            if (m_parameters->outputVerboseLevel >= ContiniousIteratorParameters::VerboseLevel::more)
			{
            	cout << "[R-K4] Deacreasing step to " << dt << endl;
			}
			if (dt < m_stepMin)
				dt = m_stepMin;

			m_target->clearSubiteration();
			continue;
		}
		if (iterationsCount > m_parameters->iterationsPerAmplitudeMax)
		{
			// Step is smaller then enough
			if (dt == m_stepMax)
				break;

            dt *= 1.5;
			if (dt > m_stepMax)
				dt = m_stepMax;
			if (m_parameters->outputVerboseLevel >= ContiniousIteratorParameters::VerboseLevel::more)
			{
				cout << "[R-K4] Increasing step to " << dt << endl;
			}

			// But we should not recompute this iteration, it is done with step precise then enough
			//m_target->clearSubiteration();
			//continue;
		}
		break;
	}

	m_target->step();
	m_metrics.timeIterations++;
	m_time += dt;
	//cout << (int)m_parameters->outputVerboseLevel << endl;
	if (m_parameters->outputVerboseLevel != ContiniousIteratorParameters::VerboseLevel::none)
	{
		cout << "[R-K4] Iteration done. t = " << m_time
				<< ", dt = " << dt
				<< ", efficiency = " << m_metrics.adaptationEfficiency() << endl;
	}
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
