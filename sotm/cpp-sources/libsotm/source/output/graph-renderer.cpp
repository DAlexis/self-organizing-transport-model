/*
 * graph-wireframe-drawer.cpp
 *
 *  Created on: 21 сент. 2016 г.
 *      Author: dalexies
 */

#include <sotm/output/graph-renderer.hpp>
#include <vtkCellData.h>
#include <vtkProperty.h>
#include <vtkVectorText.h>
#include <vtkXMLPolyDataWriter.h>

#include <cmath>

using namespace sotm;

SphereDrawer::SphereDrawer()
{
}

SphereDrawer::SphereDrawer(StaticVector<3> pos, double radius, double color[3], const std::string& follower)
{
	create(pos, radius, color, follower);
}

void SphereDrawer::create(StaticVector<3> pos, double radius, double color[3], const std::string& follower)
{
	vtkSmartPointer<vtkSphereSource> m_source = vtkSmartPointer<vtkSphereSource>::New();
	m_source->SetCenter(pos.x);
	m_source->SetRadius(radius);

	vtkSmartPointer<vtkPolyDataMapper> m_mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	m_mapper->SetInputConnection(m_source->GetOutputPort());

	if (!follower.empty())
	{
		vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();
		textSource->SetText(follower.c_str());

		// Create a mapper
		vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		textMapper->SetInputConnection( textSource->GetOutputPort() );

		m_label = vtkSmartPointer<vtkFollower>::New();
		m_label->SetMapper( textMapper );
		m_label->GetProperty()->SetColor( 1, 0, 0 ); // red
		m_label->SetPosition(pos.x);
		m_label->PickableOff();
		m_label->SetScale(0.05);
	}

	m_actor = vtkSmartPointer<vtkActor>::New();
	m_actor->GetProperty()->SetColor(color);
	m_actor->SetMapper(m_mapper);
}

void SphereDrawer::addActors(vtkRenderer* renderer)
{
	renderer->AddActor(m_actor);
	if (m_label)
	{
		m_label->SetCamera(renderer->GetActiveCamera());
		renderer->AddActor(m_label);
	}

}

GraphRenderer::GraphRenderer(sotm::ModelContext* modelContext, RenderPreferences* renderPreferences) :
	m_modelContext(modelContext),
	m_renderPreferences(renderPreferences)
{
}

void GraphRenderer::prepareNextBuffer()
{
	prepareBuffer(m_nextBuffer);
}

void GraphRenderer::prepareCurrentBuffer()
{
	prepareBuffer(m_currentBuffer);
}

void GraphRenderer::prepareBuffer(WireframeBuffer* buffer)
{
	buffer->clear();

	m_modelContext->graphRegister.applyLinkVisitor(
		[this, buffer] (sotm::Link* link)
		{
			linkVisitor(link, buffer);
		}
	);
	if (m_renderPreferences->enableSpheres)
	{
		m_modelContext->graphRegister.applyNodeVisitor(
			[this, buffer] (sotm::Node* node)
			{
				nodeVisitor(node, buffer);
			}
		);
	}
	buffer->prepareWireframeActor();
}

void GraphRenderer::addActorsFromCurrentBuffer(vtkRenderer* renderer)
{
	if (m_renderPreferences->lineWidth)
	{
		for (auto& la : m_currentBuffer->lineActors)
			renderer->AddActor(la);
	} else {
		renderer->AddActor(m_currentBuffer->actor);
	}

	for (auto& sd : m_currentBuffer->sphereDrawers)
			sd.addActors(renderer);

	for (auto& wl : m_currentBuffer->wireLabels)
	{
		wl->SetCamera(renderer->GetActiveCamera());
		renderer->AddActor(wl);
	}
}

void GraphRenderer::writeCurrentBufferToFile(const std::string& filename)
{
	vtkSmartPointer<vtkXMLPolyDataWriter> writer = vtkSmartPointer<vtkXMLPolyDataWriter>::New();
	writer->SetInputData(m_currentBuffer->polyData);
	writer->SetFileName("Line.vtp");
	writer->Write();
}

GraphRenderer::WireframeBuffer::WireframeBuffer(RenderPreferences *renderPreferences) :
		m_renderPreferences(renderPreferences)
{
	clear();
}

void GraphRenderer::WireframeBuffer::clear()
{
	points->Reset();
	linesCellArray->Reset();
	polyData->Reset();
	colors->Reset();
	colors->SetNumberOfComponents(3);
	lines.clear();
	lineActors.clear();
	sphereDrawers.clear();
	wireLabels.clear();
}

void GraphRenderer::WireframeBuffer::prepareWireframeActor()
{
	if (!m_renderPreferences->lineWidth)
	{
		polyData->SetPoints(points);
		polyData->SetLines(linesCellArray);
		polyData->GetCellData()->SetScalars(colors);

		mapper->SetInputData(polyData);
		actor->SetMapper(mapper);
		//actor->GetProperty()->SetLineWidth(5);
	}
}

void GraphRenderer::swapBuffers()
{
	std::swap(m_nextBuffer, m_currentBuffer);
}

void GraphRenderer::linkVisitor(sotm::Link* link, WireframeBuffer* buffer)
{
	auto& p1 = link->getNode1()->pos;
	auto& p2 = link->getNode2()->pos;
	StaticVector<3> center = (p1 + p2) / 2.0;
	if (m_renderPreferences->lineWidth)
	{
		// Line-by-line rendering
		vtkSmartPointer<vtkLineSource> lineSource = vtkSmartPointer<vtkLineSource>::New();
		lineSource->SetPoint1(p1.x);
		lineSource->SetPoint2(p2.x);
		lineSource->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(lineSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->GetProperty()->SetLineWidth(link->payload->getSize());

		{
			double rgb[3] = {1.0, 1.0, 1.0};
			link->payload->getColor(rgb);
			actor->GetProperty()->SetColor(rgb);
		}

		buffer->lineActors.push_back(actor);
		// Line-by-line rendering end
	} else {
		vtkIdType id1 = buffer->points->InsertNextPoint( p1.x );
		vtkIdType id2 = buffer->points->InsertNextPoint( p2.x );

		vtkSmartPointer<vtkLine> line = vtkSmartPointer<vtkLine>::New();
		line->GetPointIds()->SetId(0, id1);
		line->GetPointIds()->SetId(1, id2);

		buffer->lines.push_back(line);
		// Color source stub

		double rgb[3] = {1.0, 1.0, 1.0};
		link->payload->getColor(rgb);
		unsigned char color_uchar[] =
			{
				(unsigned char) ( pow(rgb[0], m_renderPreferences->gamma)*255 ),
				(unsigned char) ( pow(rgb[1], m_renderPreferences->gamma)*255 ),
				(unsigned char) ( pow(rgb[2], m_renderPreferences->gamma)*255 ),
			};
		buffer->colors->InsertNextTypedTuple(color_uchar);
		//buffer->colors->InsertNe

		buffer->linesCellArray->InsertNextCell(line);
	}

	std::string linkFollowerText = link->payload->getFollowerText();

	if (m_renderPreferences->enableFollowers && !linkFollowerText.empty())
	{

		vtkSmartPointer<vtkVectorText> textSource = vtkSmartPointer<vtkVectorText>::New();
		textSource->SetText(linkFollowerText.c_str());

		// Create a mapper
		vtkSmartPointer<vtkPolyDataMapper> textMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
		textMapper->SetInputConnection( textSource->GetOutputPort() );

		vtkSmartPointer<vtkFollower> label = vtkSmartPointer<vtkFollower>::New();

		label = vtkSmartPointer<vtkFollower>::New();
		label->SetMapper( textMapper );
		label->GetProperty()->SetColor( 0.2, 0.8, 0.1 );
		label->SetPosition(center.x);
		label->PickableOff();
		label->SetScale(0.05);
		buffer->wireLabels.push_back(label);
	}
}

void GraphRenderer::nodeVisitor(sotm::Node* node, WireframeBuffer* buffer)
{
	double rgb[3];
	node->payload->getColor(rgb);
	if (m_renderPreferences->enableFollowers)
		buffer->sphereDrawers.push_back(SphereDrawer(node->pos, node->payload->getSize(), rgb, node->payload->getFollowerText()));
	else
		buffer->sphereDrawers.push_back(SphereDrawer(node->pos, node->payload->getSize(), rgb, ""));
}
