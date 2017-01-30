/*
 * async-iteration.hpp
 *
 *  Created on: 27 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_ASYNC_ITERATION_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_ASYNC_ITERATION_HPP_

#include "sotm/base/time-iter.hpp"

#include <QThread>
#include <iostream>
namespace sotm {

class AsyncIteratorWrapper : public QObject
{
    Q_OBJECT
public:
	AsyncIteratorWrapper(TimeIterator *timeIterator);
	void setPeriod(double period);
	double getPeriod();

	static void printThreadId()
	{
		std::string s;
		s += "id = " + std::to_string( (unsigned long int) QThread::currentThreadId());
		std::cout << s << std::endl;
	}
signals:
    void frameDone();

public slots:
    void calculateFrame();

private:
    TimeIterator *m_timeIterator;
    PeriodicStopHook m_stopHook;
};


class AsyncIteratorRunner : public QObject
{
	Q_OBJECT
public:
	AsyncIteratorRunner(AsyncIteratorWrapper& iteratorWrapper);
	void run();
	void stopAndJoin();
private:
	AsyncIteratorWrapper& m_iteratorWrapper;
	QThread *m_thread = nullptr;
};

} // namespace sotm


#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_ASYNC_ITERATION_HPP_ */
