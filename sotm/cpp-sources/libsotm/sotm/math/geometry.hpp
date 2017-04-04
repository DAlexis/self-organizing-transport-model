#ifndef GEOMETRY_HPP_INCLUDED
#define GEOMETRY_HPP_INCLUDED

#include "sotm/utils/memory.hpp"
#include "sotm/utils/assert.hpp"
#include "sotm/math/generic.hpp"

#include <string>
#include <cmath>
#include <initializer_list>

namespace sotm
{

struct SphericalPoint
{
	double theta, phi;
};

template<int dim>
class Vector
{
public:
    
    Vector(std::initializer_list<double> initList)
    {
        ASSERT(dim == initList.size(),
          "Cannot instantiate Point({...}): initList.size == " + std::to_string(initList.size()) + " != dim == " + std::to_string(dim));
        
        double* px = this->x;
        for (const double& coord : initList)
            *(px++) = coord;
    }
    
    Vector(double x, double y, double z)
    {
        ASSERT(dim == 3, "Cannot instantiate Point(x, y, z): 3 != dim == " + std::to_string(dim));
        this->x[0] = x;
        this->x[1] = y;
        this->x[2] = z;
    }
    
    Vector(double x, double y)
    {
        ASSERT(dim == 2, "Cannot instantiate Point(x, y): 2 != dim == " + std::to_string(dim));
        this->x[0] = x;
        this->x[1] = y;
    }
    
    Vector(double x)
    {
        ASSERT(dim == 1, "Cannot instantiate Point(x): 1 != dim == " + std::to_string(dim));
        this->x[0] = x;
    }
    
    
    Vector()
    {
    	memset(x, 0, sizeof(x[0])*dim);
    }
    
    double len()
    {
    	double result = 0;
    	for (int i=0; i<dim; i++)
    	{
    		result += sqr(x[i]);
    	}
    	return sqrt(result);
    }

    Vector<dim> operator=(std::initializer_list<double> initList)
    {
        ASSERT(dim == initList.size(),
            "Cannot call operator= for Point<" + std::to_string(dim) +
            "> and initList with size == " + std::to_string(initList.size())
        );
        
        double* px = this->x;
        for (const double& coord : initList)
            *(px++) = coord;
        return *this;
    }
    
    /// @todo optimize this functions. Remove this cycles for concrete dimensions
    Vector<dim> operator-() const
    {
        Vector<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = -x[i];
        return result;
    }
    
    Vector<dim> operator-(const Vector<dim>& right) const
    {
        Vector<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] - right.x[i];
        return result;
    }
    
    Vector<dim> operator+(const Vector<dim>& right) const
    {
        Vector<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] + right.x[i];
        return result;
    }
    
    Vector<dim> operator+=(const Vector<dim>& right)
    {
        for (int i=0; i<dim; ++i)
            x[i] += right.x[i];
        return *this;
    }
    
    Vector<dim> operator-=(const Vector<dim>& right)
    {
        for (int i=0; i<dim; ++i)
            x[i] -= right.x[i];
        return *this;
    }
    
    Vector<dim> operator*=(double right)
    {
        for (int i=0; i<dim; ++i)
            x[i] *= right;
        return *this;
    }
    
    Vector<dim> operator/=(double right)
    {
        for (int i=0; i<dim; ++i)
            x[i] /= right;
        return *this;
    }
    
    bool operator==(const Vector<dim>& right) const
    {
        for (int i = 0; i<dim; ++i)
            if (x[i] != right.x[i])
                return false;
        return true;
    }
    
    bool operator!=(const Vector<dim>& right) const
    {
        return ! (*this == right);
    }
    
    /// Vector product
    Vector<dim> operator*(double right) const
    {
        Vector<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] * right;
        return result;
    }
    
    /// Scalar product
    double operator^(const Vector<dim>& right) const
    {
    	double result = 0;
    	for (int i=0; i<dim; i++)
    		result += x[i] * right.x[i];

    	return result;
    }

    Vector<dim> operator*(const Vector<dim>& right) const
    {
    	Vector<dim> result;
    	result[0] =   (*this)[1] * right[2] - (*this)[2] * right[1];
		result[1] = - (*this)[0] * right[2] + (*this)[2] * right[0];
		result[2] =   (*this)[0] * right[1] - (*this)[1] * right[0];
		return result;
    }

    Vector<dim> operator/(double right)
    {
        Vector<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] / right;
        return result;
    }
    
    std::string str()
    {
    	return std::string("(") +
    			std::to_string(x[0]) + "; " +
				std::to_string(x[1]) + "; " +
				std::to_string(x[2]) + ")";
    }

    double& operator[](unsigned int i) { return x[i]; }

    const double& operator[](unsigned int i) const { return x[i]; }

    void normalize()
    {
    	double l = len();
    	for (int i=0; i<dim; i++)
    		x[i] /= l;
    }

    double x[dim];
};

using Direction = Vector<3>;

template<int dim>
class SquareMatrix
{
public:
	SquareMatrix() { memset(x, 0, sizeof(double) * dim * dim); }
	double& operator()(size_t i, size_t j) { return x[dim*i +j]; }

	Vector<dim> operator*(const Vector<3>& v)
	{
		Vector<dim> result;
		for (int i=0; i<dim; i++)
		{
			result[i] = 0;
			for (int j=0; j<dim; j++)
			{
				result[i] += (*this) (i, j) * v[j];
			}
		}
		return result;
	}

	double x[dim*dim];

};

template<int dim>
double angle(Vector<dim> v1, Vector<dim> v2)
{
	double c = v1^v2/v1.len()/v2.len();
	if (c > 1.0) c = 1.0;
	if (c <-1.0) c =-1.0;
	return acos(c);
}

void createRotationMatrix(SquareMatrix<3>& matrix, Direction axis, double angle);
void createRotationMatrix(SquareMatrix<3>& matrix, Direction axis, double cos, double sin);

class SphericalVectorPlacer
{
public:
	SphericalVectorPlacer(const Vector<3>& baseVector);
	Vector<3> place(double len, double theta, double phi);
	Vector<3> place(double len, SphericalPoint dir);

private:
	void useRotationToZ(double len, double theta, double phi);
	void useRotationToX(double len, double theta, double phi);

	Vector<3> m_baseVector;
	SquareMatrix<3> m_rotationMatrix;
	Vector<3> m_result;

	bool m_useRotationToZ = true;

	static Vector<3> ortX;
	static Vector<3> ortZ;
};

}

#endif // GEOMETRY_HPP_INCLUDED
