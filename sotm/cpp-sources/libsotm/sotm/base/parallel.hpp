/*
 * parallel.hpp
 *
 *  Created on: 16 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_BASE_PARALLEL_HPP_
#define LIBSOTM_SOTM_BASE_PARALLEL_HPP_

struct ParallelSettings
{
	struct ContiniousIteration {
		bool calculateSecondaryValues = false;
		bool calculateRHS = false;
		bool addRHSToDelta = false;
		bool makeSubIteration = false;
		bool step = false;
	};

	ContiniousIteration parallelContiniousIteration;

	static ParallelSettings parallelDisabled;
};

#endif /* LIBSOTM_SOTM_BASE_PARALLEL_HPP_ */
