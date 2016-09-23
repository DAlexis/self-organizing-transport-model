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

/**
 * This class should not have any explicit QT dependencies on it's header to prevent
 * adding QT include directories to project using it
 */
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
