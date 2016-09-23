#ifndef RenderWindowUIMultipleInheritance_H
#define RenderWindowUIMultipleInheritance_H

#include "ui_RenderWindowUIMultipleInheritance.h"

#include <vtkRenderer.h>
#include <vtkSmartPointer.h>

#include <QMainWindow>

class VisualizerUIWindow : public QMainWindow, private Ui::RenderWindowUIMultipleInheritance
{
  Q_OBJECT
public:

  VisualizerUIWindow();
  vtkSmartPointer<vtkRenderer> renderer();

public slots:

  virtual void slotExit();

private slots:
  void on_pushButtonOneIteration_clicked();

  void on_pushButtonIterateToLimit_clicked();

private:
	vtkSmartPointer<vtkRenderer> m_renderer{ vtkSmartPointer<vtkRenderer>::New() };
};

#endif
