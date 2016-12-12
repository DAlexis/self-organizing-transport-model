#include "sotm-gui-internal/frame-maker.hpp"

using namespace sotm;

FrameMaker::FrameMaker(ModelContext *modelContext) :
	m_wireframeDrawer(modelContext, &m_renderPreferences)
{
}

void FrameMaker::prepareNextFrameAndSwapBuffers()
{
	m_wireframeDrawer.prepareNextActor();
	m_wireframeDrawer.swapBuffers();
}

void FrameMaker::recreateCurrentFrame()
{
	m_wireframeDrawer.swapBuffers();
	m_wireframeDrawer.prepareNextActor();
	m_wireframeDrawer.swapBuffers();
}

void FrameMaker::draw(vtkRenderer *renderer)
{
	renderer->RemoveAllViewProps();
	m_wireframeDrawer.addCurrentActors(renderer);
}

RenderPreferences* FrameMaker::renderPreferences()
{
	return &m_renderPreferences;
}
