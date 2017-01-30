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

QtGUI::QtGUI(ModelContext* modelContext, TimeIterator* timeIterator) :
    m_modelContext(modelContext),
    m_timeIterator(timeIterator),
    m_frameMaker(modelContext),
	m_asynIterationWrapper(timeIterator)
{

}

int QtGUI::run(int argc, char** argv)
{
	QApplication app(argc, argv);

    VisualizerUIWindow renderWindowUIMultipleInheritance(this);

    if (m_timeIterator != nullptr)
    {
        m_animationMaker.reset(new AnimationMaker(&m_frameMaker, m_timeIterator, renderWindowUIMultipleInheritance.renderer()));
    }

    // Drawing first frame
    m_frameMaker.prepareNextFrameAndSwapBuffers();
	m_frameMaker.draw(renderWindowUIMultipleInheritance.renderer());

	renderWindowUIMultipleInheritance.prepareUIToRun();
	renderWindowUIMultipleInheritance.show();

	return app.exec();
}

TimeIterator* QtGUI::timeIterator()
{
	return m_timeIterator;
}

AnimationMaker* QtGUI::animationMaker()
{
	return &(*m_animationMaker);
}

FrameMaker* QtGUI::frameMaker()
{
    return &m_frameMaker;
}

bool QtGUI::isStaticGraph()
{
	return m_timeIterator == nullptr;
}

ModelContext* QtGUI::context()
{
	return m_modelContext;
}

AsyncIteratorRunner* QtGUI::asyncIteratorRunner()
{
	return &m_asyncIterationRunner;
}
