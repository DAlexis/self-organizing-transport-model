#ifndef LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_
#define LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_

#include "sotm-gui/gui.hpp"
#include "sotm-gui-internal/frame-maker.hpp"
#include "sotm-gui-internal/animation-maker.hpp"
#include "sotm-gui-internal/async-iteration.hpp"

#include <memory>

namespace sotm {

class QtGUI : public IGUIDriver
{
public:
	QtGUI(ModelContext* modelContext, TimeIterator* timeIterator = nullptr);
	int run(int argc, char** argv) override;

	TimeIterator* timeIterator();
	AnimationMaker* animationMaker();
    FrameMaker* frameMaker();
	bool isStaticGraph();
	ModelContext* context();
	AsyncIteratorRunner* asyncIteratorRunner();

private:
	ModelContext* m_modelContext;
	TimeIterator* m_timeIterator;

    FrameMaker m_frameMaker;
    std::unique_ptr<AnimationMaker> m_animationMaker;

    AsyncIteratorWrapper m_asynIterationWrapper;
    AsyncIteratorRunner m_asyncIterationRunner{m_asynIterationWrapper};
};

}  // namespace sotm






#endif /* LIBSOTM_GUI_SOTM_GUI_INTERNAL_GUI_INTERNAL_HPP_ */
