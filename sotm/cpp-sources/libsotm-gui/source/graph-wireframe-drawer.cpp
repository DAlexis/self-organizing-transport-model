/*
 * graph-wireframe-drawer.cpp
 *
 *  Created on: 21 сент. 2016 г.
 *      Author: dalexies
 */

#include "sotm-gui-internal/graph-wireframe-drawer.hpp"

#include <vtkCellData.h>
#include <vtkProperty.h>

#include <cmath>

using namespace sotm;

SphereDrawer::SphereDrawer()
{
}

SphereDrawer::SphereDrawer(Point<3> pos, double radius, double color[3])
{
	create(pos, radius, color);
}

void SphereDrawer::create(Point<3> pos, double radius, double color[3])
{
	m_source = vtkSmartPointer<vtkSphereSource>::New();
	m_source->SetCenter(pos.x);
	m_source->SetRadius(radius);

	m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_mapper->SetInputConnection(m_source->GetOutputPort());

	m_actor = vtkSmartPointer<vtkActor>::New();
	m_actor->GetProperty()->SetColor(color);
	m_actor->SetMapper(m_mapper);
}

vtkSmartPointer<vtkActor> SphereDrawer::actor()
{
	return m_actor;
}

GraphWireframeDrawer::GraphWireframeDrawer(sotm::ModelContext* modelContext, RenderPreferences* renderPreferences) :
	m_modelContext(modelContext),
	m_renderPreferences(renderPreferences)
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

	m_modelContext->graphRegister.applyNodeVisitor(
		[this] (sotm::Node* node)
		{
			nodeVisitor(node);
		}
	);
	m_nextBuffer->prepareWireframeActor();
}

void GraphWireframeDrawer::addCurrentActors(vtkRenderer* renderer)
{
	renderer->AddActor(m_currentBuffer->actor);

	for (auto& sd : m_currentBuffer->sphereDrawers)
		renderer->AddActor(sd.actor());
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
	sphereDrawers.clear();
}

void GraphWireframeDrawer::WireframeBuffer::prepareWireframeActor()
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
	unsigned char color_uchar[3] =
		{
			(unsigned char) ( pow(rgb[0], m_renderPreferences->gamma)*255 ),
			(unsigned char) ( pow(rgb[1], m_renderPreferences->gamma)*255 ),
			(unsigned char) ( pow(rgb[2], m_renderPreferences->gamma)*255 )
		};
	m_nextBuffer->colors->InsertNextTupleValue(color_uchar);

	m_nextBuffer->linesCellArray->InsertNextCell(line);
}

void GraphWireframeDrawer::nodeVisitor(sotm::Node* node)
{
	double rgb[3];
	node->payload->getColor(rgb);
	m_nextBuffer->sphereDrawers.push_back(SphereDrawer(node->pos, node->payload->getSize() * 0.1, rgb));
}
