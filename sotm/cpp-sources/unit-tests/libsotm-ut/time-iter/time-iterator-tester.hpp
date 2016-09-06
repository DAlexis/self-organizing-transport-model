/*
 * time-iterator-tester.hpp
 *
 *  Created on: 6 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIT_TESTS_LIBSOTM_UT_TIME_ITER_TIME_ITERATOR_TESTER_HPP_
#define UNIT_TESTS_LIBSOTM_UT_TIME_ITER_TIME_ITERATOR_TESTER_HPP_

#include "sotm/base/time-iter.hpp"

class TimeIteratorTester
{
public:
	TimeIteratorTester(sotm::ITimeIterable *iterable, sotm::ITimeIterator *iterator);
	void run(double timeStart, double timeFinish, double dt);

private:
	sotm::ITimeIterable *m_iterable;
	sotm::ITimeIterator *m_iterator;
};

#endif /* UNIT_TESTS_LIBSOTM_UT_TIME_ITER_TIME_ITERATOR_TESTER_HPP_ */
