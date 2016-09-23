#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_

#include "sotm-gui/gui.hpp"
#include "sotm-gui-internal/frame-maker.hpp"

namespace sotm {

class QtGUI : public IGUIDriver
{
public:
	QtGUI(ModelContext* modelContext, TimeIterator* timeIterator = nullptr);
	int run(int argc, char** argv) override;

	TimeIterator* timeIterator();

private:
	ModelContext* m_modelContext = nullptr;
	TimeIterator* m_timeIterator = nullptr;

	FrameMaker m_frameMaker;
};

}  // namespace sotm






#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_ */
