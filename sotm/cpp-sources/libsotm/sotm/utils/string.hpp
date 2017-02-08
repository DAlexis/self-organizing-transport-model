/*
 * string.hpp
 *
 *  Created on: 8 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_UTILS_STRING_HPP_
#define LIBSOTM_SOTM_UTILS_STRING_HPP_

#include <string>
#include <sstream>

namespace sotm {

template<typename T>
T parseString(const std::string& str)
{
	std::istringstream stream(str);
	T result;
	stream >> result;
	return result;
}

} // namespace sotm

#endif /* LIBSOTM_SOTM_UTILS_STRING_HPP_ */
