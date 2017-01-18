#include "sotm/output/graph-drawer.hpp"
#include "sotm-gui/gui.hpp"
#include "sotm-gui-internal/gui-internal.hpp"

#include "sotm-gui-internal/visualizer-ui-window.hpp"
#include <vtkPolyDataMapper.h>

#include <QApplication>

#include <memory>

using namespace sotm;

GUI::GUI(ModelContext* modelContext, TimeIterator* timeIterator) :
    m_modelContext(modelContext),
    m_timeIterator(timeIterator),
	m_guiDriver(new QtGUI(modelContext, timeIterator))
{
}

int GUI::run(int argc, char** argv)
{
	return m_guiDriver->run(argc, argv);
}

QtGUI::QtGUI(ModelContext* modelContext, TimeIterator* timeIterator) :
    m_modelContext(modelContext),
    m_timeIterator(timeIterator),
    m_frameMaker(modelContext)
{

}

int QtGUI::run(int argc, char** argv)
{
	QApplication app(argc, argv);

    VisualizerUIWindow renderWindowUIMultipleInheritance(this);
	//GraphWireframeDrawer wfd(m_modelContext);

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
    if (m_timeIterator != nullptr)
    {
        m_animationMaker.reset(new AnimationMaker(&m_frameMaker, m_timeIterator, renderWindowUIMultipleInheritance.renderer()));
    }

    // Drawing first frame
    m_frameMaker.prepareNextFrameAndSwapBuffers();
	m_frameMaker.draw(renderWindowUIMultipleInheritance.renderer());

	renderWindowUIMultipleInheritance.prepareUIToRun();
	renderWindowUIMultipleInheritance.show();

	return app.exec();
}

TimeIterator* QtGUI::timeIterator()
{
	return m_timeIterator;
}

AnimationMaker* QtGUI::animationMaker()
{
	return &(*m_animationMaker);
}

FrameMaker* QtGUI::frameMaker()
{
    return &m_frameMaker;
}

bool QtGUI::isStaticGraph()
{
	return m_timeIterator == nullptr;
}

ModelContext* QtGUI::context()
{
	return m_modelContext;
}
