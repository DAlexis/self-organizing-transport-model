#include "sotm-gui/gui.hpp"

#include "sotm-gui-internal/visualizer-ui-window.hpp"
#include "sotm-gui-internal/graph-wireframe-drawer.hpp"

#include <vtkPolyDataMapper.h>

#include <QApplication>

using namespace sotm;

GUI::GUI(ModelContext* modelContext, TimeIterator* timeIterator) :
    m_modelContext(modelContext),
    m_timeIterator(timeIterator)
{

}

int GUI::run(int argc, char** argv)
{
	QApplication app(argc, argv);

	VisualizerUIWindow renderWindowUIMultipleInheritance;
	GraphWireframeDrawer wfd(m_modelContext);

	vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(wfd.getDataSet());

	/*
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
*/

	vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
	actor->SetMapper(mapper);

	renderWindowUIMultipleInheritance.renderer()->AddActor(actor);
	renderWindowUIMultipleInheritance.show();

	return app.exec();
}
