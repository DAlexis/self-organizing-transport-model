/*
 * graph-wireframe-drawer.cpp
 *
 *  Created on: 21 сент. 2016 г.
 *      Author: dalexies
 */

#include "sotm-gui-internal/graph-wireframe-drawer.hpp"

using namespace sotm;

GraphWireframeDrawer::GraphWireframeDrawer(sotm::ModelContext* modelContext) :
	m_modelContext(modelContext)
{
}

vtkSmartPointer<vtkPolyData> GraphWireframeDrawer::getDataSet()
{
	m_lines.clear();
	m_modelContext->graphRegister.applyLinkVisitor(
		[this] (sotm::Link* link)
		{
			linkVisitor(link);
		}
	);
	m_polyData->SetPoints(m_points);
	m_polyData->SetLines(m_linesCellArray);
	return m_polyData;
}

void GraphWireframeDrawer::linkVisitor(sotm::Link* link)
{
	vtkIdType id1 = m_points->InsertNextPoint( link->getNode1()->pos.x );
	vtkIdType id2 = m_points->InsertNextPoint( link->getNode2()->pos.x );

	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, id1);
	line->GetPointIds()->SetId(1, id2);

	m_lines.push_back(line);

	m_linesCellArray->InsertNextCell(line);
}
