/*
 * grid-writer.hpp
 *
 *  Created on: 2 мар. 2017 г.
 *      Author: dalexies
 */

#ifndef UTILITIES_FIELD_CALCULATOR_GRID_BUILDER_HPP_
#define UTILITIES_FIELD_CALCULATOR_GRID_BUILDER_HPP_

#include "sotm/math/geometry.hpp"

#include <vtk/vtkPoints.h>
#include <vtk/vtkSmartPointer.h>
#include <vtk/vtkDoubleArray.h>

#include <vector>
#include <string>
#include <cstddef>

class GridBuilder
{
public:
	struct ValuePoint {
		sotm::Vector<3> point;
		double value = 0;
	};

	GridBuilder(size_t nx,
			size_t ny,
			size_t nz,
			double x1, double y1, double z1,
			double x2, double y2, double z2
			);

	void writeFile(const std::string& filename);
	std::vector<ValuePoint>& valuePoints();

private:
	size_t m_nx, m_ny, m_nz;
	double m_x1, m_y1, m_z1;
	double m_x2, m_y2, m_z2;

	std::vector<ValuePoint> m_valuePoints;
	vtkSmartPointer<vtkPoints> m_points {vtkSmartPointer<vtkPoints>::New()};
	vtkSmartPointer<vtkDoubleArray> m_doubleArray {vtkSmartPointer<vtkDoubleArray>::New()};

};

#endif /* UTILITIES_FIELD_CALCULATOR_GRID_BUILDER_HPP_ */
