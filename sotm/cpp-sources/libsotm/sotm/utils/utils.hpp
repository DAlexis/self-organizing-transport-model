/*
 * raii.hpp
 *
 *  Created on: 5 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_UTILS_UTILS_HPP_
#define LIBSOTM_SOTM_UTILS_UTILS_HPP_

#include <functional>

namespace sotm
{

/**
 * Function wrapper that guarantee that function will be called when it will die
 */
class RunOnceOnExit
{
public:
	using Callback = std::function<void(void)>;
	RunOnceOnExit(Callback callback) : m_callback(callback) {}
	~RunOnceOnExit() { if (m_needRunOnDestructor) m_callback(); }

	void run() { m_needRunOnDestructor = false; m_callback(); }

private:
	Callback m_callback = nullptr;
	bool m_needRunOnDestructor = true;
};

}

#endif /* LIBSOTM_SOTM_UTILS_UTILS_HPP_ */
