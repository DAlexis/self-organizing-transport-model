#ifndef RenderWindowUIMultipleInheritance_H
#define RenderWindowUIMultipleInheritance_H

#include "ui_RenderWindowUIMultipleInheritance.h"

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <QTimer>
#include <QMainWindow>
#include <QElapsedTimer>

namespace sotm {
    class QtGUI;
}

enum class RunningSate
{
	stopped = 0,
    running,
	needToBeStopped
};


class VisualizerUIWindow : public QMainWindow, private Ui::RenderWindowUIMultipleInheritance
{
  Q_OBJECT
public:

  VisualizerUIWindow(sotm::QtGUI *gui);
  vtkSmartPointer<vtkRenderer> renderer();

  // this function should be called just before run when all objects that should exist in runtime exist
  void prepareUIToRun();

signals:
  void calculateNextFrame();

public slots:

  virtual void slotExit();
  void onFrameCalculated();
  void onFrameTimerTimeout();

private slots:
  void on_pushButtonOneIteration_clicked();

  void on_doubleSpinBoxTimestep_valueChanged(double arg1);

  void on_doubleSpinBoxIterateTo_valueChanged(double arg1);

  void on_doubleSpinBoxRedrawPeriod_valueChanged(double arg1);

  void on_spinBoxFPS_valueChanged(int arg1);

  void on_pushButtonStartAnimation_clicked();

  void on_pushButtonPauseAnimation_clicked();

  void on_horizontalSlider_valueChanged(int value);

  void on_checkBoxFollowers_clicked();

  void on_checkBoxSpheres_clicked();

private:
    void updateEdiableFields();
    void stopFrameWaiting();
    bool shouldAnimationContinued();
    void prepareNextFrame();
    void startAnimation();
    void stopAnimation();
    void updateModelInfo();
    void renderCurrentFrame();

    void startNextFrameCalculating();

    void buttonsToRunning();
    void buttonsToNeedToBeStopped();
    void buttonsToStopped();

	vtkSmartPointer<vtkRenderer> m_renderer{ vtkSmartPointer<vtkRenderer>::New() };

    sotm::QtGUI *m_gui;

    RunningSate m_runningState = RunningSate::stopped;

    QTimer *m_frameTimer = nullptr;
    QElapsedTimer m_frameCalculationElapsed;
};

#endif
