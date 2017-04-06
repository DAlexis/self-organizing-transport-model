/*
 * functions.hpp
 *
 *  Created on: 6 апр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_MATH_FUNCTIONS_HPP_
#define LIBSOTM_SOTM_MATH_FUNCTIONS_HPP_

namespace sotm {

class TrapezoidFunc
{
public:
	TrapezoidFunc(double amplitude, double zScale, double zRecession, double z0 = 0);

	double operator()(double z);
	double maxAbs();
private:
	double m_z0;
	double m_amplitude;
	double m_zScale, m_zRecession;
	double m_a, m_c;
};

}


#endif /* LIBSOTM_SOTM_MATH_FUNCTIONS_HPP_ */
