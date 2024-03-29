/*
 * const.hpp
 *
 *  Created on: 13 дек. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_UTILS_CONST_HPP_
#define LIBSOTM_SOTM_UTILS_CONST_HPP_

#include <cmath>

namespace sotm
{
	namespace Const
	{
		constexpr static double pi = M_PI;

		namespace Si
		{
            constexpr static double c = 299792458.0;
			constexpr static double epsilon_0 = 8.85418781762039e-12;
			constexpr static double k = 1 / (4*pi*epsilon_0);

			namespace SpecificHeat
			{
				constexpr static double air = 1005; // J / (kg*K)
			}
		}
	}
}





#endif /* LIBSOTM_SOTM_UTILS_CONST_HPP_ */
