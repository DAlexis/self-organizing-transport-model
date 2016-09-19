#ifndef RenderWindowUIMultipleInheritance_H
#define RenderWindowUIMultipleInheritance_H

#include "ui_RenderWindowUIMultipleInheritance.h"

#include <QMainWindow>

class VisualizerUIWindow : public QMainWindow, private Ui::RenderWindowUIMultipleInheritance
{
  Q_OBJECT
public:

  VisualizerUIWindow();

public slots:

  virtual void slotExit();

};

#endif
