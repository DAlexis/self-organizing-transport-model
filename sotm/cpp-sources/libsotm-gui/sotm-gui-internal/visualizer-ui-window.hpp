#ifndef RenderWindowUIMultipleInheritance_H
#define RenderWindowUIMultipleInheritance_H

#include "ui_RenderWindowUIMultipleInheritance.h"

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

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

public slots:

  virtual void slotExit();

private slots:
  void on_pushButtonOneIteration_clicked();

  void on_pushButtonIterateToLimit_clicked();

  void on_doubleSpinBoxTimestep_valueChanged(double arg1);

  void on_doubleSpinBoxIterateTo_valueChanged(double arg1);

private:
	vtkSmartPointer<vtkRenderer> m_renderer{ vtkSmartPointer<vtkRenderer>::New() };

    sotm::QtGUI *m_gui;
};

#endif
