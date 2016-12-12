#include "sotm-gui-internal/visualizer-ui-window.hpp"
#include "sotm-gui-internal/gui-internal.hpp"

#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkSmartPointer.h>

#include <iostream>
#include <string>
#include <sstream>

using namespace std;

// Constructor
VisualizerUIWindow::VisualizerUIWindow(sotm::QtGUI *gui) :
    m_gui(gui)
{
    this->setupUi(this);

    this->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

    m_animationTimer = new QTimer(this);
    connect(m_animationTimer, SIGNAL(timeout()), this, SLOT(on_animationTimerTimeout()));
    // Set up action signals and slots
    connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

vtkSmartPointer<vtkRenderer> VisualizerUIWindow::renderer()
{
    return m_renderer;
}

void VisualizerUIWindow::prepareUIToRun()
{
    updateEdiableFields();
    updateModelInfo();
}

void VisualizerUIWindow::updateEdiableFields()
{
    horizontalSlider->setValue((m_gui->frameMaker()->renderPreferences()->gamma - 0.2) / 2.0 * 100);
    if (m_gui->isStaticGraph())
    {
        groupBoxIterating->setEnabled(false);
        //groupBoxIterating->setWhatsThis("Iterating not avaliable when GUI initialized without TimeIterator object");
    } else {
        groupBoxIterating->setEnabled(true);
        doubleSpinBoxTimestep->setValue(m_gui->timeIterator()->getStep());
        doubleSpinBoxTime->setValue(m_gui->timeIterator()->getTime());
        //doubleSpinBoxIterateTo->setValue(m_gui->timeIterator()->getStopTime());
        doubleSpinBoxRedrawPeriod->setValue(m_gui->animationMaker()->getFrameDuration());
        spinBoxFPS->setValue(m_gui->animationMaker()->getFps());
    }

}

void VisualizerUIWindow::stopFrameWaiting()
{
    m_animationTimer->stop();
    doubleSpinBoxTime->setValue(m_gui->timeIterator()->getTime());
}

bool VisualizerUIWindow::shouldAnimationContinued()
{
    return m_gui->timeIterator()->getTime() < doubleSpinBoxIterateTo->value();
}

void VisualizerUIWindow::startFrameTimer()
{
	// Starting timer while frame is generating
	m_animationTimer->start(1000 / m_gui->animationMaker()->getFps());
}

void VisualizerUIWindow::prepareNextFrame()
{
    updateModelInfo();
    m_gui->animationMaker()->drawNextFrame();
    m_gui->animationMaker()->iterateToNextFrame();
    this->qvtkWidget->repaint();
}

void VisualizerUIWindow::startAnimation()
{
	startFrameTimer();
    prepareNextFrame();

    pushButtonOneIteration->setEnabled(false);
    pushButtonStartAnimation->setEnabled(false);

    pushButtonPauseAnimation->setEnabled(true);
    //pushButtonIterateToLimit
}

void VisualizerUIWindow::stopAnimation()
{
    m_animationTimer->stop();

    pushButtonOneIteration->setEnabled(true);
    pushButtonStartAnimation->setEnabled(true);

    pushButtonPauseAnimation->setEnabled(false);
}

void VisualizerUIWindow::updateModelInfo()
{
    labelNodesCount->setText(std::to_string(m_gui->context()->graphRegister.nodesCount()).c_str());
    labelLinksCount->setText(std::to_string(m_gui->context()->graphRegister.linksCount()).c_str());
    if (!m_gui->isStaticGraph())
    {
        doubleSpinBoxTime->setValue(m_gui->timeIterator()->getTime());
    }
}

void VisualizerUIWindow::slotExit()
{
    qApp->exit();
}

void VisualizerUIWindow::reRenderCurrentFrame()
{
    updateModelInfo();
    m_gui->frameMaker()->recreateCurrentFrame();
    m_gui->frameMaker()->draw(renderer());
    this->qvtkWidget->repaint();
}

void VisualizerUIWindow::on_animationTimerTimeout()
{
    cout << "Timer event" << endl;
    stopFrameWaiting();
    if (shouldAnimationContinued())
    {
    	startFrameTimer();
        prepareNextFrame();
    } else {
        m_gui->animationMaker()->drawNextFrame();
        this->qvtkWidget->repaint();
        stopAnimation();
    }
}

void VisualizerUIWindow::on_pushButtonOneIteration_clicked()
{
    if (!m_gui->isStaticGraph())
    {
    	m_gui->animationMaker()->doIteration();
    	prepareNextFrame();
        this->qvtkWidget->repaint();
        updateModelInfo();
    }
}

void VisualizerUIWindow::on_doubleSpinBoxTimestep_valueChanged(double arg1)
{
    if (!m_gui->isStaticGraph())
		m_gui->timeIterator()->setStep(arg1);
}

void VisualizerUIWindow::on_doubleSpinBoxIterateTo_valueChanged(double arg1)
{
    /*if (!m_gui->isStaticGraph())
        m_gui->timeIterator()->setStopTime(arg1);*/
}

void VisualizerUIWindow::on_doubleSpinBoxRedrawPeriod_valueChanged(double arg1)
{
    if (!m_gui->isStaticGraph())
    {
        m_gui->animationMaker()->setFrameDuration(arg1);
    }
}

void VisualizerUIWindow::on_spinBoxFPS_valueChanged(int arg1)
{
    if (!m_gui->isStaticGraph())
    {
        m_gui->animationMaker()->setFps(arg1);
    }
}

void VisualizerUIWindow::on_pushButtonStartAnimation_clicked()
{
    if (m_gui->isStaticGraph())
    {
        cout << "Graph is static, iterating impossible" << endl;
        return;
    }
    startAnimation();
}

void VisualizerUIWindow::on_pushButtonPauseAnimation_clicked()
{
    stopAnimation();
}

void VisualizerUIWindow::on_horizontalSlider_valueChanged(int value)
{
    double gammaValue = 0.2 + 2.0 / 100 * value;
    std::stringstream ss;
    ss << fixed << setprecision(2) << gammaValue;
    labelGamma->setText(ss.str().c_str());

    m_gui->frameMaker()->renderPreferences()->gamma = gammaValue;
    reRenderCurrentFrame();
}
