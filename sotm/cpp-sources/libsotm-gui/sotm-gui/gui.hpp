/*
 * gui.hpp
 *
 *  Created on: 22 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_GUI_SOTM_GUI_GUI_HPP_
#define LIBSOTM_GUI_SOTM_GUI_GUI_HPP_

#include "sotm/base/model-context.hpp"
#include "sotm/base/time-iter.hpp"
#include "sotm/output/render-preferences.hpp"

#include <memory>

namespace sotm {

class IGUIDriver
{
public:
	virtual ~IGUIDriver() { }
	virtual int run(int argc, char** argv) = 0;
	virtual void setFrameOptions(double FramePeriod, double fps = 10) = 0;
	virtual RenderPreferences* renderPreferences() = 0;
};

/**
 * This class should not have any explicit QT dependencies on it's header to prevent
 * adding QT include directories to project using it
 */
class GUI
{
public:
    GUI(ModelContext* modelContext, TimeIterator* timeIterator = nullptr);
	int run(int argc, char** argv);
	void setFrameOptions(double FramePeriod, double fps = 10);
	RenderPreferences* renderPreferences();

private:
	ModelContext* m_modelContext;
    TimeIterator* m_timeIterator;

    std::unique_ptr<IGUIDriver> m_guiDriver;
};


}  // namespace sotm

#endif /* LIBSOTM_GUI_SOTM_GUI_GUI_HPP_ */
