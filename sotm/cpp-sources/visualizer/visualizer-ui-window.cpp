#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkSmartPointer.h>
#include "visualizer-ui-window.hpp"

// Constructor
VisualizerUIWindow::VisualizerUIWindow()
{
    this->setupUi(this);

	//create two points, P0 and P1
	double P0[3] = {0.0, 0.0, 0.0};
	double P1[3] = {1.0, 0.0, 0.0};
	double P2[3] = {0.0, 1.0, 0.0};
	double P3[3] = {0.0, 0.0, 1.0};

	//add the two poitns to a vtkPoints object
	vtkSmartPointer<vtkPoints> pts = vtkSmartPointer<vtkPoints>::New();
	pts->InsertNextPoint(P0);
	pts->InsertNextPoint(P1);
	pts->InsertNextPoint(P2);
	pts->InsertNextPoint(P3);

	//create a line between the two points
	vtkSmartPointer<vtkLine> line1 = vtkSmartPointer<vtkLine>::New();
	line1->GetPointIds()->SetId(0,0); //the SetId(A,B) call is the following:
	//A = the id of the point relative to the line - this can only be 0 or 1.
	//B = the index into the vtkPoints object of the point that you would like to set the Ath point to.
	line1->GetPointIds()->SetId(1,1);

	vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
	line2->GetPointIds()->SetId(0,0);
	line2->GetPointIds()->SetId(1,2);

	vtkSmartPointer<vtkLine> line3 = vtkSmartPointer<vtkLine>::New();
	line3->GetPointIds()->SetId(0,0);
	line3->GetPointIds()->SetId(1,3);


	//create a cell array to store the line in
	vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();
	lines->InsertNextCell(line1);
	lines->InsertNextCell(line2);
	lines->InsertNextCell(line3);

	//create a polydata to store everything in
	vtkSmartPointer<vtkPolyData> pdata = vtkSmartPointer<vtkPolyData>::New();

	  //add the points to the dataset
	pdata->SetPoints(pts);

	//add the lines to the dataset
	pdata->SetLines(lines);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(pdata);
	/*
    // Create two points, P0 and P1
	double p0[3] = {1.0, 0.0, 0.0};
	double p1[3] = {0.0, 1.0, 0.0};

	vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
	lineSource->SetPoint1(p0);
	lineSource->SetPoint2(p1);
	lineSource->Update();

	// Visualize
	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputConnection(lineSource->GetOutputPort());
*/
	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

    //actor->GetProperty()->SetLineWidth(4);

	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	renderer->AddActor(actor);

	this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);

	/*
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);

	renderer->AddActor(actor);

	renderWindow->Render();
	renderWindowInteractor->Start();
	*/

/*
  // Sphere
  vtkSmartPointer<vtkSphereSource> sphereSource =
    vtkSmartPointer<vtkSphereSource>::New();
  sphereSource->Update();
  vtkSmartPointer<vtkPolyDataMapper> sphereMapper =
    vtkSmartPointer<vtkPolyDataMapper>::New();
  sphereMapper->SetInputConnection(sphereSource->GetOutputPort());
  vtkSmartPointer<vtkActor> sphereActor =
    vtkSmartPointer<vtkActor>::New();
  sphereActor->SetMapper(sphereMapper);

  // VTK Renderer
  vtkSmartPointer<vtkRenderer> renderer =
    vtkSmartPointer<vtkRenderer>::New();
  renderer->AddActor(sphereActor);

  // VTK/Qt wedded
  this->qvtkWidget->GetRenderWindow()->AddRenderer(renderer);
*/

  // Set up action signals and slots
  connect(this->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));

}

void VisualizerUIWindow::slotExit()
{
  qApp->exit();
}
