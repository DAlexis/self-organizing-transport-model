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
	drawNextFrame();
}


void AnimationMaker::iterateToNextFrame()
{
	m_timeIterator->setStopTime(m_timeIterator->getTime() + m_frameDuration);
	m_timeIterator->run();
}

void AnimationMaker::drawNextFrame()
{
	m_frameMaker->draw(m_renderer);
}

double sotm::AnimationMaker::getFrameDuration() const
{
	return m_frameDuration;
}

double sotm::AnimationMaker::getFps() const {
	return m_fps;
}

void sotm::AnimationMaker::setFps(double fps = defaultFPS) {
	m_fps = fps;
}

void sotm::AnimationMaker::setFrameDuration(double frameDuration)
{
	m_frameDuration = frameDuration;
}

