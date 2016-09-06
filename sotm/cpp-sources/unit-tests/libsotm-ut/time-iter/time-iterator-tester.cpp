/*
 * time-iterator-tester.cpp
 *
 *  Created on: 6 сент. 2016 г.
 *      Author: dalexies
 */

#include "time-iterator-tester.hpp"

TimeIteratorTester::TimeIteratorTester(sotm::ITimeIterable *iterable, sotm::ITimeIterator *iterator) :
	m_iterable(iterable),
	m_iterator(iterator)
{
	m_iterator->setIterable(m_iterable);
}

void TimeIteratorTester::run(double timeStart, double timeFinish, double dt)
{
	m_iterator->setTime(timeStart);
	for (double time = 0; time <= timeFinish; time += dt)
	{
		m_iterator->iterate(dt);
	}
}
