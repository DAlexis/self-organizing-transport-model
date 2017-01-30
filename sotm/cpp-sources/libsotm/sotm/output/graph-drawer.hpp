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
	SphereDrawer(Vector<3> pos, double radius, double color[3], const std::string& follower = std::string());
	void create(Vector<3> pos, double radius, double color[3], const std::string& follower = std::string());
	void addActors(vtkRenderer* renderer);

private:
	vtkSmartPointer<vtkActor> m_actor = nullptr;
	vtkSmartPointer<vtkFollower> m_label = nullptr;
};

class GraphDrawer
{
public:
	GraphDrawer(ModelContext* modelContext, RenderPreferences* renderPreferences);
	void prepareNextBuffer();
	void prepareCurrentBuffer();
	void addActorsFromCurrentBuffer(vtkRenderer* renderer);
	void swapBuffers();

private:
	struct WireframeBuffer {
		WireframeBuffer();
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

		std::vector<SphereDrawer> sphereDrawers;

		std::vector< vtkSmartPointer<vtkFollower> > wireLabels;
	};

	void linkVisitor(sotm::Link* link, WireframeBuffer* buffer);
	void nodeVisitor(sotm::Node* node, WireframeBuffer* buffer);

	void prepareBuffer(WireframeBuffer* buffer);

	sotm::ModelContext* m_modelContext;
	RenderPreferences* m_renderPreferences;

	WireframeBuffer m_buffer[2];
	WireframeBuffer *m_nextBuffer = &(m_buffer[0]), *m_currentBuffer = &(m_buffer[1]);
};

}

#endif /* LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_ */
