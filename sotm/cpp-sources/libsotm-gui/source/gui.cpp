#include "sotm/output/graph-drawer.hpp"
#include "sotm-gui/gui.hpp"
#include "sotm-gui-internal/gui-internal.hpp"

#include "sotm-gui-internal/visualizer-ui-window.hpp"
#include <vtkPolyDataMapper.h>

#include <QApplication>

#include <memory>

using namespace sotm;

GUI::GUI(ModelContext* modelContext, TimeIterator* timeIterator) :
    m_modelContext(modelContext),
    m_timeIterator(timeIterator),
	m_guiDriver(new QtGUI(modelContext, timeIterator))
{
}

int GUI::run(int argc, char** argv)
{
	return m_guiDriver->run(argc, argv);
}

void GUI::setFrameOptions(double FramePeriod, double fps)
{
	m_guiDriver->setFrameOptions(FramePeriod, fps);
}

RenderPreferences* GUI::renderPreferences()
{
	return m_guiDriver->renderPreferences();
}

QtGUI::QtGUI(ModelContext* modelContext, TimeIterator* timeIterator) :
    m_modelContext(modelContext),
    m_timeIterator(timeIterator),
    m_drawer(modelContext, &m_renderPreferences),
    m_asynIterationWrapper(timeIterator)
{

}

int QtGUI::run(int argc, char** argv)
{
	QApplication app(argc, argv);

    VisualizerUIWindow renderWindowUIMultipleInheritance(this);

	renderWindowUIMultipleInheritance.prepareUIToRun();
	renderWindowUIMultipleInheritance.show();

	return app.exec();
}

void QtGUI::setFrameOptions(double FramePeriod, double fps)
{
	m_asynIterationWrapper.setPeriod(FramePeriod);
	m_frameOptions.fps = fps;
}

TimeIterator* QtGUI::timeIterator()
{
	return m_timeIterator;
}

bool QtGUI::isStaticGraph()
{
	return m_timeIterator == nullptr;
}

ModelContext* QtGUI::context()
{
	return m_modelContext;
}

AsyncIteratorWrapper* QtGUI::asyncIteratorWrapper()
{
	return &m_asynIterationWrapper;
}

AsyncIteratorRunner* QtGUI::asyncIteratorRunner()
{
	return &m_asyncIterationRunner;
}

RenderPreferences* QtGUI::renderPreferences()
{
	return &m_renderPreferences;
}

GraphDrawer* QtGUI::graphDrawer()
{
	return &m_drawer;
}

FrameOptions* QtGUI::frameOptions()
{
	return &m_frameOptions;
}
