/*
 * graph-wireframe-drawer.hpp
 *
 *  Created on: 21 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_
#define LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_

#include "sotm/base/model-context.hpp"
#include "sotm/output/render-preferences.hpp"

#include <vtk/vtkDataSet.h>
#include <vtk/vtkPolyDataMapper.h>
#include <vtk/vtkRenderer.h>
#include <vtk/vtkRenderWindow.h>
#include <vtk/vtkSphereSource.h>
#include <vtk/vtkLineSource.h>
#include <vtk/vtkCellArray.h>
#include <vtk/vtkLine.h>
#include <vtk/vtkActor.h>
#include <vtk/vtkSmartPointer.h>
#include <vtk/vtkFollower.h>

#include <vector>

namespace sotm {

class SphereDrawer
{
public:
	SphereDrawer();
	SphereDrawer(StaticVector<3> pos, double radius, double color[3], const std::string& follower = std::string());
	void create(StaticVector<3> pos, double radius, double color[3], const std::string& follower = std::string());
	void addActors(vtkRenderer* renderer);

private:
	vtkSmartPointer<vtkActor> m_actor = nullptr;
	vtkSmartPointer<vtkFollower> m_label = nullptr;
};

class GraphRenderer
{
public:
	GraphRenderer(ModelContext* modelContext, RenderPreferences* renderPreferences);
	void prepareNextBuffer();
	void prepareCurrentBuffer();
	void addActorsFromCurrentBuffer(vtkRenderer* renderer);
	void writeCurrentBufferToFile(const std::string& filename);
	void swapBuffers();

private:
	struct WireframeBuffer {
		WireframeBuffer(RenderPreferences *renderPreferences);
		void clear();
		void prepareWireframeActor();

		// @todo Remove most of this objects from class: smart pointers do its job successfully without this
		vtkSmartPointer<vtkPoints>            points{ vtkSmartPointer<vtkPoints>::New() };
		vtkSmartPointer<vtkCellArray>         linesCellArray{ vtkSmartPointer<vtkCellArray>::New() };
		vtkSmartPointer<vtkPolyData>          polyData{ vtkSmartPointer<vtkPolyData>::New() };
		vtkSmartPointer<vtkPolyDataMapper>    mapper{ vtkSmartPointer<vtkPolyDataMapper>::New() };
		vtkSmartPointer<vtkActor>             actor{ vtkSmartPointer<vtkActor>::New() };
		vtkSmartPointer<vtkUnsignedCharArray> colors{ vtkSmartPointer<vtkUnsignedCharArray>::New() };

		std::vector< vtkSmartPointer<vtkLine> > lines;

		std::vector< vtkSmartPointer<vtkActor> > lineActors;

		std::vector<SphereDrawer> sphereDrawers;

		std::vector< vtkSmartPointer<vtkFollower> > wireLabels;

		RenderPreferences* m_renderPreferences;
	};

	void linkVisitor(sotm::Link* link, WireframeBuffer* buffer);
	void nodeVisitor(sotm::Node* node, WireframeBuffer* buffer);

	void prepareBuffer(WireframeBuffer* buffer);

	sotm::ModelContext* m_modelContext;
	RenderPreferences* m_renderPreferences;

	WireframeBuffer m_buffer1{m_renderPreferences}, m_buffer2{m_renderPreferences};
	WireframeBuffer *m_nextBuffer = &(m_buffer1), *m_currentBuffer = &(m_buffer2);
};

}

#endif /* LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_ */
