#include <QApplication>

#include "visualizer-ui-window.hpp"

int main(int argc, char** argv)
{
  QApplication app(argc, argv);

  VisualizerUIWindow renderWindowUIMultipleInheritance;
  renderWindowUIMultipleInheritance.show();

  return app.exec();
}
