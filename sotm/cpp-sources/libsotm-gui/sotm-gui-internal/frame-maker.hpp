
#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_FRAME_MAKER_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_FRAME_MAKER_HPP_

#include "sotm/output/graph-drawer.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/output/render-preferences.hpp"

#include <vtkRenderer.h>

namespace sotm {

/**
 * This class builds up the whole frame
 */
class FrameMaker
{
public:
	FrameMaker(ModelContext *modelContext);

	/**
	 * Start next frame creation and swap buffers,
	 * so next call of draw() cause drawing next frame
	 */
	void prepareNextFrameAndSwapBuffers();
	/**
	 * Redraw current frame without swapping buffers
	 * so next call of draw() cause drawing current frame
	 */
	void recreateCurrentFrame();
	void draw(vtkRenderer *renderer);

	RenderPreferences* renderPreferences();

private:
	RenderPreferences m_renderPreferences;
	GraphDrawer m_wireframeDrawer;
};

}  // namespace sotm









#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_FRAME_MAKER_HPP_ */
