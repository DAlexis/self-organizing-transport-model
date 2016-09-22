/*
 * gui.hpp
 *
 *  Created on: 22 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_GUI_SOTM_GUI_GUI_HPP_
#define LIBSOTM_GUI_SOTM_GUI_GUI_HPP_

#include "sotm/base/model-context.hpp"

namespace sotm {

void test();

class GUI
{
public:
	GUI(ModelContext* modelContext);
	int run(int argc, char** argv);

private:
	ModelContext* m_modelContext;
};


}  // namespace sotm

#endif /* LIBSOTM_GUI_SOTM_GUI_GUI_HPP_ */
