/*
 * graph-wireframe-drawer.hpp
 *
 *  Created on: 21 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_
#define LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_

#include "sotm/base/model-context.hpp"

#include "vtkDataSet.h"
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSphereSource.h>
#include <vtkLineSource.h>
#include <vtkCellArray.h>
#include <vtkLine.h>
#include <vtkActor.h>
#include <vtkSmartPointer.h>

#include <vector>

class GraphWireframeDrawer
{
public:
	GraphWireframeDrawer(sotm::ModelContext* modelContext);
	void prepareNextActor();
	vtkSmartPointer<vtkActor> getCurrentActor();
	void swapBuffers();

private:
	void linkVisitor(sotm::Link* link);

	sotm::ModelContext* m_modelContext;

	struct WireframeBuffer {
		void clear();
		vtkSmartPointer<vtkPoints> m_points{ vtkSmartPointer<vtkPoints>::New() };
		vtkSmartPointer<vtkCellArray> m_linesCellArray{ vtkSmartPointer<vtkCellArray>::New() };
		vtkSmartPointer<vtkPolyData> m_polyData{ vtkSmartPointer<vtkPolyData>::New() };
		vtkSmartPointer<vtkPolyDataMapper> mapper{ vtkSmartPointer<vtkPolyDataMapper>::New() };
		vtkSmartPointer<vtkActor> actor{ vtkSmartPointer<vtkActor>::New() };
		std::vector< vtkSmartPointer<vtkLine> > m_lines;
	};

	WireframeBuffer m_buffer[2];
	WireframeBuffer *m_nextBuffer = &(m_buffer[0]), *m_currentBuffer = &(m_buffer[1]);
};


#endif /* LIBSOTM_GUI_GRAPH_WIREFRAME_DRAWER_HPP_ */
