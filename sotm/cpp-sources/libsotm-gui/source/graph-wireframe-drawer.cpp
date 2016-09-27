/*
 * graph-wireframe-drawer.cpp
 *
 *  Created on: 21 сент. 2016 г.
 *      Author: dalexies
 */

#include "sotm-gui-internal/graph-wireframe-drawer.hpp"

#include <vtkCellData.h>

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
	m_nextBuffer->prepareActor();
}

vtkSmartPointer<vtkActor> GraphWireframeDrawer::getCurrentActor()
{
	return m_currentBuffer->actor;
}

GraphWireframeDrawer::WireframeBuffer::WireframeBuffer()
{
	clear();
}

void GraphWireframeDrawer::WireframeBuffer::clear()
{
	points->Reset();
	linesCellArray->Reset();
	polyData->Reset();
	colors->Reset();
	colors->SetNumberOfComponents(3);
	lines.clear();
}

void GraphWireframeDrawer::WireframeBuffer::prepareActor()
{
	polyData->SetPoints(points);
	polyData->SetLines(linesCellArray);
	polyData->GetCellData()->SetScalars(colors);

	mapper->SetInputData(polyData);
	actor->SetMapper(mapper);
}

void GraphWireframeDrawer::swapBuffers()
{
	std::swap(m_nextBuffer, m_currentBuffer);
}

void GraphWireframeDrawer::linkVisitor(sotm::Link* link)
{
	vtkIdType id1 = m_nextBuffer->points->InsertNextPoint( link->getNode1()->pos.x );
	vtkIdType id2 = m_nextBuffer->points->InsertNextPoint( link->getNode2()->pos.x );

	vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
	line->GetPointIds()->SetId(0, id1);
	line->GetPointIds()->SetId(1, id2);

	m_nextBuffer->lines.push_back(line);

	// Color source stub

	double rgb[3] = {1.0, 1.0, 1.0};
	link->payload->getColor(rgb);
	unsigned char color_uchar[3] = { rgb[0]*255, rgb[1]*255, rgb[2]*255 };
	m_nextBuffer->colors->InsertNextTupleValue(color_uchar);

	m_nextBuffer->linesCellArray->InsertNextCell(line);
}
