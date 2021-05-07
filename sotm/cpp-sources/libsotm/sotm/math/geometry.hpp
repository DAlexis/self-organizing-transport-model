#ifndef GEOMETRY_HPP_INCLUDED
#define GEOMETRY_HPP_INCLUDED


#include <cstring>
#include <string>
#include <cmath>
#include <complex>
#include <initializer_list>

namespace sotm
{

struct SphericalPoint
{
	double theta, phi;
};


template<int dim, typename Type = double>
class StaticVector
{
public:
    using VectorType = StaticVector<dim, Type>;

    StaticVector(const Type* coords)
    {
        memcpy(x, coords, sizeof(Type)*dim);
    }

    StaticVector(std::initializer_list<Type> initList)
    {
        *this = initList;
    }

    StaticVector(const VectorType& right)
    {
        *this = right;
    }

    StaticVector(Type x_, Type y_, Type z_)
    {
        static_assert (dim == 3, "Constructor with 3 args applicable only for dimension = 3");
        x[0] = x_; x[1] = y_; x[2] = z_;
    }

    StaticVector(Type x_, Type y_)
    {
        static_assert (dim == 2, "Constructor with 2 args applicable only for dimension = 2");
        x[0] = x_; x[1] = y_;
    }

    StaticVector(Type x_)
    {
        static_assert (dim == 1, "Constructor with 1 arg applicable only for dimension = 1");
        x[0] = x_;
    }

    StaticVector()
    {
        memset(x, 0, sizeof(x[0])*dim);
    }

    template<typename CastType>
    StaticVector<dim, CastType> cast()
    {
        StaticVector<dim, CastType> result;
        for (int i=0; i<dim; i++)
            result[i] = CastType(x[i]);

        return result;
    }

    double norm() const
    {
        double result = 0;
        for (int i=0; i<dim; i++)
        {
            result += std::norm(x[i]);
        }
        return std::sqrt(result);
    }

    VectorType& operator=(const VectorType& right)
    {
        memcpy(x, right.x, sizeof(Type) * dim);
        return *this;
    }

    VectorType& operator=(const std::initializer_list<Type>& initList)
    {
        Type* px = this->x;
        for (const Type& coord : initList)
            *(px++) = coord;
        return *this;
    }

    VectorType operator-() const
    {
        VectorType result;
        for (int i=0; i<dim; ++i)
            result.x[i] = -x[i];
        return result;
    }

    VectorType operator-(const VectorType& right) const
    {
        VectorType result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] - right.x[i];
        return result;
    }

    VectorType operator+(const VectorType& right) const
    {
        VectorType result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] + right.x[i];
        return result;
    }

    VectorType& operator+=(const VectorType& right)
    {
        for (int i=0; i<dim; ++i)
            x[i] += right.x[i];
        return *this;
    }

    VectorType& operator-=(const VectorType& right)
    {
        for (int i=0; i<dim; ++i)
            x[i] -= right.x[i];
        return *this;
    }

    VectorType& operator*=(Type right)
    {
        for (int i=0; i<dim; ++i)
            x[i] *= right;
        return *this;
    }

    VectorType& operator/=(Type right)
    {
        for (int i=0; i<dim; ++i)
            x[i] /= right;
        return *this;
    }

    bool operator==(const VectorType& right) const
    {
        for (int i = 0; i<dim; ++i)
            if (x[i] != right.x[i])
                return false;
        return true;
    }

    bool operator!=(const VectorType& right) const
    {
        return ! (*this == right);
    }

    VectorType operator*(Type right) const
    {
        VectorType result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] * right;
        return result;
    }

    /**
     * @brief Scalar product
     */
    Type operator*(const VectorType& right) const
    {
        Type result = 0;
        for (int i=0; i<dim; i++)
                result += x[i] * right.x[i];

        return result;
    }

    /**
     * @brief Vector product
     */
    VectorType operator%(const VectorType& right) const
    {
        static_assert(dim == 3, "Vector product works only for dimension = 3!");
        VectorType result;
        result[0] =   (*this)[1] * right[2] - (*this)[2] * right[1];
        result[1] = - (*this)[0] * right[2] - (*this)[2] * right[0];
        result[2] =   (*this)[0] * right[1] - (*this)[1] * right[0];
        return result;
    }

    VectorType operator/(Type right) const
    {
        VectorType result;
        for (int i=0; i<dim; ++i)
            result.x[i] = x[i] / right;
        return result;
    }

    std::string str() const
    {
        std::ostringstream oss;
        oss << "(";
        for (int i=0; i<dim; ++i)
        {
            oss << x[i] << "; ";
        }
        oss << ")";
        return oss.str();
    }

    Type& operator[](unsigned int i) { return x[i]; }

    Type operator[](unsigned int i) const { return x[i]; }

    void normalize()
    {
        double l = norm();
        for (int i=0; i<dim; i++)
            x[i] /= l;
    }

    Type x[dim];
};

template<int dim, typename T1, typename T2>
StaticVector<dim, T2> operator*(const T1& left, const StaticVector<dim, T2>& right)
{
    return right * left;
}
using Direction = StaticVector<3>;

template<int dim>
class SquareMatrix
{
public:
	SquareMatrix() { memset(x, 0, sizeof(double) * dim * dim); }
	double& operator()(size_t i, size_t j) { return x[dim*i +j]; }

    StaticVector<dim> operator*(const StaticVector<3>& v)
	{
        StaticVector<dim> result;
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
double angle(StaticVector<dim> v1, StaticVector<dim> v2)
{
    double c = (v1*v2)/v1.norm()/v2.norm();
	if (c > 1.0) c = 1.0;
	if (c <-1.0) c =-1.0;
	return acos(c);
}

template<int dim>
StaticVector<dim> project(StaticVector<dim> what, StaticVector<dim> where)
{
	return where*(what^where)/where.len();
}


void createRotationMatrix(SquareMatrix<3>& matrix, Direction axis, double angle);
void createRotationMatrix(SquareMatrix<3>& matrix, Direction axis, double cos, double sin);

class SphericalVectorPlacer
{
public:
    SphericalVectorPlacer(const StaticVector<3>& baseVector);
    StaticVector<3> place(double len, double theta, double phi);
    StaticVector<3> place(double len, SphericalPoint dir);

private:
	void useRotationToZ(double len, double theta, double phi);
	void useRotationToX(double len, double theta, double phi);

    StaticVector<3> m_baseVector;
	SquareMatrix<3> m_rotationMatrix;
    StaticVector<3> m_result;

	bool m_useRotationToZ = true;

    static StaticVector<3> ortX;
    static StaticVector<3> ortZ;
};

}

#endif // GEOMETRY_HPP_INCLUDED
