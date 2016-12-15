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
class Point
{
public:
    
    Point(std::initializer_list<double> initList)
    {
        ASSERT(dim == initList.size(),
          "Cannot instantiate Point({...}): initList.size == " + std::to_string(initList.size()) + " != dim == " + std::to_string(dim));
        
        double* px = this->x;
        for (const double& coord : initList)
            *(px++) = coord;
    }
    
    Point(double x, double y, double z)
    {
        ASSERT(dim == 3, "Cannot instantiate Point(x, y, z): 3 != dim == " + std::to_string(dim));
        this->x[0] = x;
        this->x[1] = y;
        this->x[2] = z;
    }
    
    Point(double x, double y)
    {
        ASSERT(dim == 2, "Cannot instantiate Point(x, y): 2 != dim == " + std::to_string(dim));
        this->x[0] = x;
        this->x[1] = y;
    }
    
    Point(double x)
    {
        ASSERT(dim == 1, "Cannot instantiate Point(x): 1 != dim == " + std::to_string(dim));
        this->x[0] = x;
    }
    
    
    Point() { }
    
    double len()
    {
    	double result = 0;
    	for (int i=0; i<dim; i++)
    	{
    		result += sqr(x[i]);
    	}
    	return sqrt(result);
    }

    Point<dim> operator=(std::initializer_list<double> initList)
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
    Point<dim> operator-() const
    {
        Point<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = -x[i];
        return result;
    }
    
    Point<dim> operator-(const Point<dim>& right) const
    {
        Point<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] - right.x[i];
        return result;
    }
    
    Point<dim> operator+(const Point<dim>& right) const
    {
        Point<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] + right.x[i];
        return result;
    }
    
    Point<dim> operator+=(const Point<dim>& right)
    {
        for (int i=0; i<dim; ++i)
            x[i] += right.x[i];
        return *this;
    }
    
    Point<dim> operator-=(const Point<dim>& right)
    {
        for (int i=0; i<dim; ++i)
            x[i] -= right.x[i];
        return *this;
    }
    
    Point<dim> operator*=(double right)
    {
        for (int i=0; i<dim; ++i)
            x[i] *= right;
        return *this;
    }
    
    Point<dim> operator/=(double right)
    {
        for (int i=0; i<dim; ++i)
            x[i] /= right;
        return *this;
    }
    
    bool operator==(const Point<dim>& right) const
    {
        for (int i = 0; i<dim; ++i)
            if (x[i] != right.x[i])
                return false;
        return true;
    }
    
    bool operator!=(const Point<dim>& right) const
    {
        return ! (*this == right);
    }
    
    Point<dim> operator*(double right)
    {
        Point<dim> result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] * right;
        return result;
    }
    
    Point<dim> operator/(double right)
    {
        Point<dim> result;
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

    inline double& operator[](unsigned int i) { return x[i]; }

    double x[dim];
};

using Direction = Point<3>;

}

#endif // GEOMETRY_HPP_INCLUDED
