/*
 * random.hpp
 *
 *  Created on: 26 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_MATH_RANDOM_HPP_
#define LIBSOTM_SOTM_MATH_RANDOM_HPP_

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_real.hpp>
#include <boost/random/normal_distribution.hpp>
#include <boost/random/variate_generator.hpp>

namespace sotm {

class Random
{
public:
    static void randomizeUsingTime();
    static void randomize(unsigned int parameter);
    static double gaussian(double center, double dispersion);
    static double uniform(double from, double to);
private:
    static boost::mt19937 randomGenerator;
};

}  // namespace sotm

#endif /* LIBSOTM_SOTM_MATH_RANDOM_HPP_ */
