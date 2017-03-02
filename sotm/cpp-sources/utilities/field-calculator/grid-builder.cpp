#include "grid-builder.hpp"

#include <vtk/vtkVersion.h>
#include <vtk/vtkCellArray.h>
#include <vtk/vtkXMLStructuredGridWriter.h>
#include <vtk/vtkStructuredGrid.h>
#include <vtk/vtkCellData.h>

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

				if (ix != m_nx && iy != m_ny && iz != m_nz)
				{
					ValuePoint p;
					p.point = sotm::Vector<3>(x + dx*0.5, y + dy*0.5, z + dz*0.5);
					m_valuePoints.push_back(p);
				}
			}
}

void GridBuilder::writeFile(const std::string& filename)
{
	vtkSmartPointer<vtkStructuredGrid> structuredGrid =
			vtkSmartPointer<vtkStructuredGrid>::New();
/*
	points->InsertNextPoint(0, 0, 0);
	points->InsertNextPoint(1, 0, 0);

	points->InsertNextPoint(0, 1, 0);
	points->InsertNextPoint(1, 1, 0);

	points->InsertNextPoint(0, 2, 0);
	points->InsertNextPoint(1, 2, 0);



	points->InsertNextPoint(0, 0, 3);
	points->InsertNextPoint(1, 0, 3);

	points->InsertNextPoint(0, 1, 3);
	points->InsertNextPoint(1, 1, 3);

	points->InsertNextPoint(0, 2, 3);
	points->InsertNextPoint(1, 2, 3);*/

	for (auto& it : m_valuePoints)
		m_doubleArray->InsertNextTuple1(it.value);

	// Specify the dimensions of the grid
	structuredGrid->SetDimensions(m_nx+1, m_ny+1, m_nz+1);
	structuredGrid->SetPoints(m_points);
	structuredGrid->GetCellData()->SetNumberOfTuples(2);
	structuredGrid->GetCellData()->SetScalars(m_doubleArray);


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
