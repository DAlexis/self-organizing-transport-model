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

    m_frameTimer = new QTimer(this);
    connect(m_frameTimer, SIGNAL(timeout()), this, SLOT(onFrameTimerTimeout()));

    // Set up action signals and slots

    connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

    connect(this, SIGNAL(calculateNextFrame()), m_gui->asyncIteratorWrapper(), SLOT(calculateFrame()));
    connect(m_gui->asyncIteratorWrapper(), SIGNAL(frameDone()), this, SLOT(onFrameCalculated()));
    m_gui->asyncIteratorRunner()->run();
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
    RenderPreferences* rp = m_gui->renderPreferences();
    horizontalSlider->setValue((rp->gamma - 0.2) / 2.0 * 100);
    checkBoxFollowers->setChecked(rp->enableFollowers);
    checkBoxSpheres->setChecked(rp->enableSpheres);
    if (m_gui->isStaticGraph())
    {
        groupBoxIterating->setEnabled(false);
        //groupBoxIterating->setWhatsThis("Iterating not avaliable when GUI initialized without TimeIterator object");
    } else {
        groupBoxIterating->setEnabled(true);
        doubleSpinBoxTimestep->setValue(m_gui->timeIterator()->getStep());
        doubleSpinBoxTime->setValue(m_gui->timeIterator()->getTime());
        doubleSpinBoxIterateTo->setValue(m_gui->timeIterator()->getStopTime());
        doubleSpinBoxRedrawPeriod->setValue(m_gui->asyncIteratorWrapper()->getPeriod());
        spinBoxFPS->setValue(m_gui->frameOptions()->fps);
    }

}

void VisualizerUIWindow::stopFrameWaiting()
{
    doubleSpinBoxTime->setValue(m_gui->timeIterator()->getTime());
}

bool VisualizerUIWindow::shouldAnimationContinued()
{
    return m_gui->timeIterator()->getTime() < doubleSpinBoxIterateTo->value();
}

void VisualizerUIWindow::prepareNextFrame()
{
    updateModelInfo();
    //m_gui->animationMaker()->iterateToNextFrame();
    //m_gui->animationMaker()->drawNextFrame();
    this->qvtkWidget->repaint();
}

void VisualizerUIWindow::startAnimation()
{
	//startFrameTimer();
    prepareNextFrame();

    pushButtonOneIteration->setEnabled(false);
    pushButtonStartAnimation->setEnabled(false);

    pushButtonPauseAnimation->setEnabled(true);
    //pushButtonIterateToLimit
}

void VisualizerUIWindow::stopAnimation()
{
    //m_animationTimer->stop();

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
	m_gui->asyncIteratorRunner()->stopAndJoin();
    qApp->exit();
}

void VisualizerUIWindow::onFrameCalculated()
{
	/*
	// Todo here
	doubleSpinBoxTime->setValue(m_gui->timeIterator()->getTime());

	if (shouldAnimationContinued())
	{
		//startFrameTimer();
		prepareNextFrame();
	} else {
		//m_gui->animationMaker()->drawNextFrame();
		this->qvtkWidget->repaint();
		stopAnimation();
	}

	switch (m_runningState)
	{
	case RunningSate::needToBeStopped:
	case RunningSate::stopped: // WTF??
	case RunningSate::running:
		break;
	}*/

	renderCurrentFrame();
	unsigned int frameDuration = 1000 / spinBoxFPS->value();
	unsigned int dt = m_frameCalculationElapsed.elapsed();
	m_frameTimer->start(dt < frameDuration ? frameDuration - dt : 0);
}

void VisualizerUIWindow::onFrameTimerTimeout()
{
	m_frameTimer->stop();
	std::cout << "Timer" << std::endl;
	switch (m_runningState)
	{
	case RunningSate::running:
		startNextFrameCalculating();
		break;

	case RunningSate::needToBeStopped:
		buttonsToStopped();
		m_runningState = RunningSate::stopped;
		break;

	case RunningSate::stopped: // WTF??
		break;
	}
}


void VisualizerUIWindow::renderCurrentFrame()
{
	std::cout << "Rendering" << std::endl;
    updateModelInfo();
    m_renderer->RemoveAllViewProps();
	m_gui->graphDrawer()->prepareCurrentBuffer();
	m_gui->graphDrawer()->addActorsFromCurrentBuffer(renderer());
	this->qvtkWidget->repaint();
	std::cout << "Rendering done" << std::endl;

	//m_gui->frameMaker()->recreateCurrentFrame();
    //m_gui->frameMaker()->draw(renderer());
    //this->qvtkWidget->repaint();
}

void VisualizerUIWindow::startNextFrameCalculating()
{
	m_frameCalculationElapsed.start();
	emit calculateNextFrame();
}

void VisualizerUIWindow::buttonsToRunning()
{
	pushButtonStartAnimation->setEnabled(false);
	pushButtonPauseAnimation->setEnabled(true);
}

void VisualizerUIWindow::buttonsToNeedToBeStopped()
{
	pushButtonStartAnimation->setEnabled(false);
	pushButtonPauseAnimation->setEnabled(false);
}

void VisualizerUIWindow::buttonsToStopped()
{
	pushButtonStartAnimation->setEnabled(true);
	pushButtonPauseAnimation->setEnabled(false);
}


void VisualizerUIWindow::on_animationTimerTimeout()
{
    cout << "Timer event" << endl;
    stopFrameWaiting();
    if (shouldAnimationContinued())
    {
    	//startFrameTimer();
        prepareNextFrame();
    } else {
        //m_gui->animationMaker()->drawNextFrame();
        this->qvtkWidget->repaint();
        stopAnimation();
    }
}

void VisualizerUIWindow::on_pushButtonOneIteration_clicked()
{
    if (!m_gui->isStaticGraph())
    {
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
    if (!m_gui->isStaticGraph())
        m_gui->timeIterator()->setStopTime(arg1);
}

void VisualizerUIWindow::on_doubleSpinBoxRedrawPeriod_valueChanged(double arg1)
{
    if (!m_gui->isStaticGraph())
    {
    	m_gui->asyncIteratorWrapper()->setPeriod(arg1);
    }
}

void VisualizerUIWindow::on_spinBoxFPS_valueChanged(int arg1)
{
    if (!m_gui->isStaticGraph())
    {
        //m_gui->animationMaker()->setFps(arg1);
    }
}

void VisualizerUIWindow::on_pushButtonStartAnimation_clicked()
{
	/*
    if (m_gui->isStaticGraph())
    {
        cout << "Graph is static, iterating impossible" << endl;
        return;
    }
    startAnimation();*/
	//m_gui->asyncIteratorRunner()->run(this);

	buttonsToRunning();
	m_runningState = RunningSate::running;
	startNextFrameCalculating();
}

void VisualizerUIWindow::on_pushButtonPauseAnimation_clicked()
{
	m_runningState = RunningSate::needToBeStopped;
	buttonsToNeedToBeStopped();
}

void VisualizerUIWindow::on_horizontalSlider_valueChanged(int value)
{
    double gammaValue = 0.2 + 2.0 / 100 * value;
    std::stringstream ss;
    ss << fixed << setprecision(2) << gammaValue;
    labelGamma->setText(ss.str().c_str());

    //m_gui->frameMaker()->renderPreferences()->gamma = gammaValue;
    renderCurrentFrame();
}

void VisualizerUIWindow::on_checkBoxFollowers_clicked()
{
	m_gui->renderPreferences()->enableFollowers = checkBoxFollowers->isChecked();
	renderCurrentFrame();
}

void VisualizerUIWindow::on_checkBoxSpheres_clicked()
{
	m_gui->renderPreferences()->enableSpheres = checkBoxSpheres->isChecked();
	renderCurrentFrame();
}
