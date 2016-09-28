#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_ANIMATION_MAKER_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_ANIMATION_MAKER_HPP_

#include "sotm-gui-internal/frame-maker.hpp"

#include <memory>
#include <thread>

namespace sotm {

class AnimationMaker
{
public:
	constexpr static double defaultFPS = 10;
	constexpr static double defaultFrameDuration = 0.1;
	constexpr static double defaultGamma = 1;

    AnimationMaker(FrameMaker *frameMaker, TimeIterator *timeIterator, vtkRenderer *renderer);

	void doIteration();
	/**
	 * Prepare next frame to draw. Async operation
	 */
	void iterateToNextFrame();
	/**
	 * Draw prepared frame. Waints while async iterateToNextFrame() done
	 */
	void drawNextFrame();
	void redrawCurrentFrame();

	double getFrameDuration() const;
	void setFrameDuration(double frameDuration);

	double getFps() const;
	void setFps(double fps);

	double getGamma() const;
	void setGamma(double gamma);

	RenderPreferences* renderPreferences();

private:
	double m_gamma = defaultGamma;
	double m_fps = defaultFPS;
	double m_frameDuration = defaultFrameDuration;

	FrameMaker *m_frameMaker;
	TimeIterator *m_timeIterator;
	vtkRenderer *m_renderer;

	std::unique_ptr<std::thread> m_thread;
};


}  // namespace sotm








#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_ANIMATION_MAKER_HPP_ */
