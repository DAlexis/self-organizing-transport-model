#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_ANIMATION_MAKER_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_ANIMATION_MAKER_HPP_

#include "sotm-gui-internal/frame-maker.hpp"

namespace sotm {

class AnimationMaker
{
public:
    AnimationMaker(FrameMaker *frameMaker, TimeIterator *timeIterator, vtkRenderer *renderer);

	void doIteration();
	void nextFrame();

private:
	FrameMaker* m_frameMaker;
	TimeIterator* m_timeIterator;
	vtkRenderer *m_renderer;
};


}  // namespace sotm








#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_ANIMATION_MAKER_HPP_ */
