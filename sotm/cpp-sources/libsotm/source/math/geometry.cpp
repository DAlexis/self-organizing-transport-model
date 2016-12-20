#include "sotm/math/geometry.hpp"
#include "sotm/math/generic.hpp"

using namespace sotm;

Vector<3> SphericalVectorPlacer::ortX{1.0, 0.0, 0.0};
Vector<3> SphericalVectorPlacer::ortZ{0.0, 0.0, 1.0};

void sotm::createRotationMatrix(SquareMatrix<3>& matrix, Direction axis, double angle)
{
	createRotationMatrix(matrix, axis, cos(angle), sin(angle));
}

void sotm::createRotationMatrix(SquareMatrix<3>& matrix, Direction axis, double cos, double sin)
{
	axis.normalize();
    const int x = 0, y = 1, z = 2;
    matrix.x[0+0] = cos + (1-cos)*sqr(axis[x]);            matrix.x[0+1] = (1-cos)*axis[x]*axis[y] - sin*axis[z]; matrix.x[0+2] = (1-cos)*axis[x]*axis[z] + sin*axis[y];
    matrix.x[3+0] = (1-cos)*axis[y]*axis[x] + sin*axis[z]; matrix.x[3+1] = cos + (1-cos)*sqr(axis[y]);            matrix.x[3+2] = (1-cos)*axis[y]*axis[z] - sin*axis[x];
    matrix.x[6+0] = (1-cos)*axis[z]*axis[x] - sin*axis[y]; matrix.x[6+1] = (1-cos)*axis[z]*axis[y] + sin*axis[x]; matrix.x[6+2] = cos + (1-cos)*sqr(axis[z]);
}

SphericalVectorPlacer::SphericalVectorPlacer(const Vector<3>& baseVector) :
		m_baseVector(baseVector)
{
	m_baseVector.normalize();
	double mz = m_baseVector^ortZ;
	double mx = m_baseVector^ortX;
	if (fabs(mx) < fabs(mz))
		m_useRotationToZ = false;
}

Vector<3> SphericalVectorPlacer::place(double len, double theta, double phi)
{
	if (m_useRotationToZ)
		useRotationToZ(len, theta, phi);
	else
		useRotationToX(len, theta, phi);
	return m_result;
}

Vector<3> SphericalVectorPlacer::place(double len, SphericalPoint dir)
{
	return place(len, dir.theta, dir.phi);
}

void SphericalVectorPlacer::useRotationToZ(double len, double theta, double phi)
{
	double alpha = angle(m_baseVector, ortZ);
	Vector<3> rotAround = m_baseVector*ortZ;
	rotAround.normalize();
	createRotationMatrix(m_rotationMatrix, rotAround, -alpha);
	m_result[0] = len*cos(phi)*sin(theta);
	m_result[1] = len*sin(phi)*sin(theta);
	m_result[2] = len*cos(theta);
	m_result = m_rotationMatrix*m_result;
}

void SphericalVectorPlacer::useRotationToX(double len, double theta, double phi)
{
	double alpha = angle(m_baseVector, ortX);
	Vector<3> rotAround = m_baseVector*ortX;
	rotAround.normalize();
	createRotationMatrix(m_rotationMatrix, rotAround, -alpha);
	m_result[0] = len*cos(theta);
	m_result[1] = len*cos(phi)*sin(theta);
	m_result[2] = len*sin(phi)*sin(theta);
	m_result = m_rotationMatrix*m_result;
}
