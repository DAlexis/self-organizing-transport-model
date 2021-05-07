#include "grid-builder.hpp"

#include <vtk/vtkVersion.h>
#include <vtk/vtkCellArray.h>
#include <vtk/vtkXMLStructuredGridWriter.h>
#include <vtk/vtkStructuredGrid.h>
#include <vtk/vtkCellData.h>
#include <vtk/vtkPointData.h>

GridBuilder::GridBuilder(
	size_t nx,
	size_t ny,
	size_t nz,
	double x1, double y1, double z1,
	double x2, double y2, double z2) :
		m_nx(nx), m_ny(ny), m_nz(nz),
		m_x1(x1), m_y1(y1), m_z1(z1),
		m_x2(x2), m_y2(y2), m_z2(z2)
{
	m_doubleArray->SetNumberOfComponents(1);

	double dx = (m_x2 - m_x1) / m_nx;
	double dy = (m_y2 - m_y1) / m_ny;
	double dz = (m_z2 - m_z1) / m_nz;

	for (size_t iz=0; iz<=m_nz; iz++)
		for (size_t iy=0; iy<=m_ny; iy++)
			for (size_t ix=0; ix<=m_nx; ix++)
			{
				double x = m_x1 + dx*ix;
				double y = m_y1 + dy*iy;
				double z = m_z1 + dz*iz;

				m_points->InsertNextPoint(x, y, z);

				ValuePoint p;
				p.point = sotm::StaticVector<3>(x, y, z);
				m_valuePoints.push_back(p);
			}
}

void GridBuilder::writeFile(const std::string& filename)
{
	vtkSmartPointer<vtkStructuredGrid> structuredGrid =
			vtkSmartPointer<vtkStructuredGrid>::New();

	for (auto& it : m_valuePoints)
		m_doubleArray->InsertNextTuple1(it.value);

	// Specify the dimensions of the grid
	structuredGrid->SetDimensions(m_nx+1, m_ny+1, m_nz+1);
	structuredGrid->SetPoints(m_points);
	structuredGrid->GetPointData()->SetScalars(m_doubleArray);
	//structuredGrid->GetCellData()->SetScalars(m_doubleArray);


	// Write file
	vtkSmartPointer<vtkXMLStructuredGridWriter> writer =
			vtkSmartPointer<vtkXMLStructuredGridWriter>::New();
	writer->SetFileName((filename+".vts").c_str());
	writer->SetInputData(structuredGrid);
	writer->Write();
}

std::vector<GridBuilder::ValuePoint>& GridBuilder::valuePoints()
{
	return m_valuePoints;
}
