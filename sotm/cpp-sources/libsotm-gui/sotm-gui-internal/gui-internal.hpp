#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_

#include "sotm/output/render-preferences.hpp"
#include "sotm/output/graph-drawer.hpp"

#include "sotm-gui/gui.hpp"
#include "sotm-gui-internal/async-iteration.hpp"

#include <memory>

namespace sotm {

class QtGUI : public IGUIDriver
{
public:
	QtGUI(ModelContext* modelContext, TimeIterator* timeIterator = nullptr);
	int run(int argc, char** argv) override;
	void setFrameOptions(double FramePeriod, double fps = 10) override;

	TimeIterator* timeIterator();
	bool isStaticGraph();
	ModelContext* context();
	AsyncIteratorWrapper* asyncIteratorWrapper();
	AsyncIteratorRunner* asyncIteratorRunner();
	RenderPreferences* renderPreferences() override;
	GraphDrawer* graphDrawer();
	FrameOptions* frameOptions();

private:
	ModelContext* m_modelContext;
	TimeIterator* m_timeIterator;

	GraphDrawer m_drawer;

    AsyncIteratorWrapper m_asynIterationWrapper;
    AsyncIteratorRunner m_asyncIterationRunner{m_asynIterationWrapper};

    RenderPreferences m_renderPreferences;

    FrameOptions m_frameOptions;
};

}  // namespace sotm






#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_ */
