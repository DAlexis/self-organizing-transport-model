#include "sotm-gui-internal/frame-maker.hpp"

using namespace sotm;

FrameMaker::FrameMaker(ModelContext *modelContext) :
	m_wireframeDrawer(modelContext)
{
}

void FrameMaker::prepareNextFrame()
{
	m_wireframeDrawer.prepareNextActor();
	m_wireframeDrawer.swapBuffers();
}

void FrameMaker::draw(vtkRenderer *renderer)
{
	renderer->RemoveAllViewProps();
	renderer->AddActor(m_wireframeDrawer.getCurrentActor());
}
