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

void GraphWireframeDrawer::prepareNextActor()
{
	m_nextBuffer->clear();

	m_modelContext->graphRegister.applyLinkVisitor(
		[this] (sotm::Link* link)
		{
			linkVisitor(link);
		}
	);
	m_nextBuffer->m_polyData->SetPoints(m_nextBuffer->m_points);
	m_nextBuffer->m_polyData->SetLines(m_nextBuffer->m_linesCellArray);

	m_nextBuffer->mapper->SetInputData(m_nextBuffer->m_polyData);
	m_nextBuffer->actor->SetMapper(m_nextBuffer->mapper);
}

vtkSmartPointer<vtkActor> GraphWireframeDrawer::getCurrentActor()
{
	return m_currentBuffer->actor;
}

void GraphWireframeDrawer::WireframeBuffer::clear()
{
	m_points->Reset();
	m_linesCellArray->Reset();
	m_polyData->Reset();
	m_lines.clear();
}

void GraphWireframeDrawer::swapBuffers()
{
	std::swap(m_nextBuffer, m_currentBuffer);
}

void GraphWireframeDrawer::linkVisitor(sotm::Link* link)
{
	vtkIdType id1 = m_nextBuffer->m_points->InsertNextPoint( link->getNode1()->pos.x );
	vtkIdType id2 = m_nextBuffer->m_points->InsertNextPoint( link->getNode2()->pos.x );

	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, id1);
	line->GetPointIds()->SetId(1, id2);

	m_nextBuffer->m_lines.push_back(line);

	m_nextBuffer->m_linesCellArray->InsertNextCell(line);
}
