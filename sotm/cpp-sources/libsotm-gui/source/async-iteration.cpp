/*
 * async-iteration.cpp
 *
 *  Created on: 27 янв. 2017 г.
 *      Author: dalexies
 */

#include "sotm-gui-internal/async-iteration.hpp"

using namespace sotm;
using namespace std;

AsyncIteratorWrapper::AsyncIteratorWrapper(TimeIterator *timeIterator) :
	m_timeIterator(timeIterator),
	m_stopHook(m_timeIterator)
{
	m_timeIterator->addHook(&m_stopHook);
}

void AsyncIteratorWrapper::setPeriod(double period)
{
	m_stopHook.setPeriod(period);
}

double AsyncIteratorWrapper::getPeriod()
{
	return m_stopHook.getPeriod();
}

void AsyncIteratorWrapper::calculateFrame()
{
	m_timeIterator->run();
	//cout << "End" << endl;
	emit frameDone();
}


AsyncIteratorRunner::AsyncIteratorRunner(AsyncIteratorWrapper& iteratorWrapper) :
		m_iteratorWrapper(iteratorWrapper)
{
}

void AsyncIteratorRunner::run()
{
	if (!m_thread)
	{
		m_thread = new QThread;
		//connect(m_thread, SIGNAL (started()), &m_iteratorWrapper, SLOT (calculateFrame()));
		//connect(&m_iteratorWrapper, SIGNAL (frameDone()), signalReceiver, SLOT (onFrameDone()));
		m_iteratorWrapper.moveToThread(m_thread);
		m_thread->start();
	}
}

void AsyncIteratorRunner::stopAndJoin()
{
	m_thread->quit();
	m_thread->wait();
}

