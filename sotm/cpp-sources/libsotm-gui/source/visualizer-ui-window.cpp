#include "sotm-gui-internal/visualizer-ui-window.hpp"

#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkSmartPointer.h>

// Constructor
VisualizerUIWindow::VisualizerUIWindow()
{
    this->setupUi(this);

    this->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

    // Set up action signals and slots
    connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));
}

vtkSmartPointer<vtkRenderer> VisualizerUIWindow::renderer()
{
    return m_renderer;
}

void VisualizerUIWindow::slotExit()
{
    qApp->exit();
}
