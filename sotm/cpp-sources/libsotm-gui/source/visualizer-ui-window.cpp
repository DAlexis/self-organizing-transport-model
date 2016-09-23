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

using namespace std;

// Constructor
VisualizerUIWindow::VisualizerUIWindow(sotm::QtGUI *gui) :
    m_gui(gui)
{
    this->setupUi(this);

    this->qvtkWidget->GetRenderWindow()->AddRenderer(m_renderer);

    if (m_gui->timeIterator() == nullptr)
    {
        groupBoxIterating->setEnabled(false);
        //groupBoxIterating->setWhatsThis("Iterating not avaliable when GUI initialized without TimeIterator object");
    }

    /// @todo: read values from TimeIterator and put it to UI controls if applicable

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

void VisualizerUIWindow::on_pushButtonOneIteration_clicked()
{
    cout << "One iteration clicked" << endl;
}

void VisualizerUIWindow::on_pushButtonIterateToLimit_clicked()
{
    cout << "Iterate to limit clicked" << endl;
}

void VisualizerUIWindow::on_doubleSpinBoxTimestep_valueChanged(double arg1)
{
	if (m_gui->timeIterator())
		m_gui->timeIterator()->setStep(arg1);
	cout << "Step changed!" << endl;
}

void VisualizerUIWindow::on_doubleSpinBoxIterateTo_valueChanged(double arg1)
{
    if (m_gui->timeIterator())
        m_gui->timeIterator()->setStopTime(arg1);
}
