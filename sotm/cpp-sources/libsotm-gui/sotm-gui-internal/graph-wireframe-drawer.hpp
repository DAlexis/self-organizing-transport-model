/*
 * graph-wireframe-drawer.hpp
 *
 *  Created on: 21 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_
#define LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_

#include "sotm/base/model-context.hpp"
#include "sotm-gui-internal/render-preferences.hpp"


#include <vtkDataSet.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>
#include <vtkFollower.h>

#include <vector>

namespace sotm {

class SphereDrawer
{
public:
	SphereDrawer();
	SphereDrawer(Point<3> pos, double radius, double color[3], const std::string& follower = std::string());
	void create(Point<3> pos, double radius, double color[3], const std::string& follower = std::string());
	void addActors(vtkRenderer* renderer);

private:
	vtkSmartPointer<vtkActor> m_actor = nullptr;
	vtkSmartPointer<vtkFollower> m_label = nullptr;
};

class GraphWireframeDrawer
{
public:
	GraphWireframeDrawer(ModelContext* modelContext, RenderPreferences* renderPreferences);
	void prepareNextActor();
	void addCurrentActors(vtkRenderer* renderer);
	void swapBuffers();

private:
	void linkVisitor(sotm::Link* link);
	void nodeVisitor(sotm::Node* node);

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
	};

	sotm::ModelContext* m_modelContext;
	RenderPreferences* m_renderPreferences;

	WireframeBuffer m_buffer[2];
	WireframeBuffer *m_nextBuffer = &(m_buffer[0]), *m_currentBuffer = &(m_buffer[1]);
};

}

#endif /* LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_ */
