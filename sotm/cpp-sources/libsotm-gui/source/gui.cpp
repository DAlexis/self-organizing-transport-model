/*
 * gui.cpp
 *
 *  Created on: 22 сент. 2016 г.
 *      Author: dalexies
 */

#include "sotm-gui/gui.hpp"

#include "sotm-gui-internal/visualizer-ui-window.hpp"

#include <QApplication>

using namespace sotm;

void sotm::test()
{
}

GUI::GUI(ModelContext* modelContext) :
		m_modelContext(modelContext)
{

}

int GUI::run(int argc, char** argv)
{
	QApplication app(argc, argv);

	VisualizerUIWindow renderWindowUIMultipleInheritance;
	renderWindowUIMultipleInheritance.show();

	return app.exec();
}
