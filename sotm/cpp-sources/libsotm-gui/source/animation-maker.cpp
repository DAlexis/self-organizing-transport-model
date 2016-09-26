#include "sotm-gui-internal/animation-maker.hpp"

using namespace sotm;


AnimationMaker::AnimationMaker(FrameMaker* frameMaker, TimeIterator* timeIterator, vtkRenderer *renderer) :
	m_frameMaker(frameMaker),
	m_timeIterator(timeIterator),
	m_renderer(renderer)
{

}

void AnimationMaker::doIteration()
{
	m_timeIterator->iterate();
	m_frameMaker->addActors(m_renderer);
}

void AnimationMaker::nextFrame()
{
}
