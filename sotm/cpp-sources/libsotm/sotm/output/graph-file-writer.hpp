/*
 * graph-file-writer.hpp
 *
 *  Created on: 14 февр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_OUTPUT_GRAPH_FILE_WRITER_HPP_
#define LIBSOTM_SOTM_OUTPUT_GRAPH_FILE_WRITER_HPP_

#include "sotm/base/model-context.hpp"
#include "sotm/base/time-iter.hpp"
#include "sotm/output/render-preferences.hpp"

#include <vtk/vtkDataSet.h>
#include <vtk/vtkPolyDataMapper.h>
#include <vtk/vtkLineSource.h>
#include <vtk/vtkCellArray.h>
#include <vtk/vtkLine.h>
#include <vtk/vtkSmartPointer.h>
#include <vtk/vtkDoubleArray.h>

namespace sotm {

class FileWriter
{
public:
	FileWriter(ModelContext* modelContext, RenderPreferences* renderPreferences);

	void write(const std::string& filename);

private:

	void buildLinks();

	void linkVisitor(sotm::Link* link);
	void nodeVisitor(sotm::Node* node);

	void clear();

	vtkSmartPointer<vtkPoints>            points{ vtkSmartPointer<vtkPoints>::New() };
	vtkSmartPointer<vtkCellArray>         linesCellArray{ vtkSmartPointer<vtkCellArray>::New() };
	vtkSmartPointer<vtkPolyData>          polyData{ vtkSmartPointer<vtkPolyData>::New() };
	vtkSmartPointer<vtkDoubleArray>       linesDoubleArray{ vtkSmartPointer<vtkDoubleArray>::New() };

	ModelContext* m_modelContext;
	RenderPreferences* m_renderPreferences;
};

class FileWriteHook : public TimeHookPeriodic
{
public:
	FileWriteHook(ModelContext* modelContext, RenderPreferences* renderPreferences, double period = 1.0);

	void setFilenamePrefix(const std::string& prefix);

private:
	void hook(double time) override;
	FileWriter m_writer;
	std::string m_prefix;
};

}

#endif /* LIBSOTM_SOTM_OUTPUT_GRAPH_FILE_WRITER_HPP_ */
