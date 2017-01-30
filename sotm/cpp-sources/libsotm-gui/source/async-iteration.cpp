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

void AsyncIteratorWrapper::calculateFrame()
{
	//m_timeIterator->run();
	unsigned int d = 123;
	cout << "Begin" << endl;
	printThreadId();
	for (size_t i = 0; i<1000000000; i++)
	{
		d = d*63542 + 12345;
		/*if (i % 1000000 == 0)
			emit tick();*/
	}
	cout << "End" << endl;
	emit frameDone();
}


AsyncIteratorRunner::AsyncIteratorRunner(AsyncIteratorWrapper& iteratorWrapper) :
		m_iteratorWrapper(iteratorWrapper)
{
}

void AsyncIteratorRunner::run(QObject* signalReceiver)
{
	if (m_thread)
	{
		m_thread->wait();
		m_thread = nullptr;
	}
	m_thread = new QThread;
	connect(m_thread, SIGNAL (started()), &m_iteratorWrapper, SLOT (calculateFrame()));
	connect(&m_iteratorWrapper, SIGNAL (frameDone()), signalReceiver, SLOT (onFrameDone()));
	m_iteratorWrapper.moveToThread(m_thread);
	m_thread->start();
}

