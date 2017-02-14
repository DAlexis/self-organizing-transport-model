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

struct FrameOptions
{
	double fps = 10;
};

class AsyncIteratorWrapper : public QObject
{
    Q_OBJECT
public:
	AsyncIteratorWrapper(TimeIterator *timeIterator);
	void setPeriod(double period);
	double getPeriod();

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
