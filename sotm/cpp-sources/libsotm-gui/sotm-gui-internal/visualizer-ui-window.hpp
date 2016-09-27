#ifndef RenderWindowUIMultipleInheritance_H
#define RenderWindowUIMultipleInheritance_H

#include "ui_RenderWindowUIMultipleInheritance.h"

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <QTimer>
#include <QMainWindow>

namespace sotm {
    class QtGUI;
}

class VisualizerUIWindow : public QMainWindow, private Ui::RenderWindowUIMultipleInheritance
{
  Q_OBJECT
public:

  VisualizerUIWindow(sotm::QtGUI *gui);
  vtkSmartPointer<vtkRenderer> renderer();

  // this function should be called just before run when all objects that should exist in runtime exist
  void prepareUIToRun();

public slots:

  virtual void slotExit();

private slots:
  void on_pushButtonOneIteration_clicked();

  void on_pushButtonIterateToLimit_clicked();

  void on_doubleSpinBoxTimestep_valueChanged(double arg1);

  void on_doubleSpinBoxIterateTo_valueChanged(double arg1);

  void on_doubleSpinBoxRedrawPeriod_valueChanged(double arg1);

  void on_spinBoxFPS_valueChanged(int arg1);

  void on_animationTimerTimeout();

  void on_pushButtonStartAnimation_clicked();

  void on_pushButtonPauseAnimation_clicked();

private:
    void updateEdiableFields();
    void stopFrameWaiting();
    bool shouldAnimationContinued();
    void prepareAndWaitNextFrame();
    void startAnimation();
    void stopAnimation();
    void updateModelInfo();

	vtkSmartPointer<vtkRenderer> m_renderer{ vtkSmartPointer<vtkRenderer>::New() };

    sotm::QtGUI *m_gui;
    QTimer *m_animationTimer;
};

#endif
