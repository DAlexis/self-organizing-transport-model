
#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_FRAME_MAKER_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_FRAME_MAKER_HPP_

#include "sotm-gui-internal/graph-wireframe-drawer.hpp"

#include "sotm/base/model-context.hpp"
#include <vtkRenderer.h>


namespace sotm {

/**
 * This class builds up the whole frame
 */
class FrameMaker
{
public:
	FrameMaker(ModelContext *modelContext);

	void draw(vtkRenderer *renderer);

private:
	GraphWireframeDrawer m_wireframeDrawer;
};

}  // namespace sotm









#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_FRAME_MAKER_HPP_ */
