/*
 * field.hpp
 *
 *  Created on: 3 апр. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_MATH_FIELD_HPP_
#define LIBSOTM_SOTM_MATH_FIELD_HPP_

#include "sotm/math/geometry.hpp"
#include "sotm/math/generic.hpp"
#include "sotm/utils/macros.hpp"
#include <functional>

namespace sotm {

template<int ValueDim, int SpaceDim>
using FieldFunction = std::function<void (StaticVector<ValueDim>& value, const StaticVector<SpaceDim>& arg)>;

///////////////////////
// Separated scalar and vector interfaces

template<int SpaceDim>
class IFieldScalar
{
public:
	virtual ~IFieldScalar() {}
	virtual double operator()(const StaticVector<SpaceDim>& arg) const = 0;
};

template<int ValueDim, int SpaceDim>
class IFieldVector
{
public:
	virtual ~IFieldVector() {}
	virtual StaticVector<ValueDim> operator()(const StaticVector<SpaceDim>& arg) const = 0;
};

/// Vector field base class
template<int ValueDim, int SpaceDim>
class FieldGeneric : public IFieldVector<ValueDim, SpaceDim>
{
public:
	virtual ~FieldGeneric() { }
	SOTM_INLINE StaticVector<ValueDim> operator()(const StaticVector<SpaceDim>& arg) const
	{
		StaticVector<ValueDim> result;
		getValue(result, arg);
		return result;
	}

	virtual void getValue(StaticVector<ValueDim>& value, const StaticVector<SpaceDim>& arg) const = 0;
};

/////////////////////
// Universal Field class

template<int ValueDim, int SpaceDim>
class Field : public FieldGeneric<ValueDim, SpaceDim> { };

template<int SpaceDim>
class Field<1, SpaceDim> : public IFieldScalar<SpaceDim> { };

////////////////////
// Zero field
template<int SpaceDim>
class FieldScalarZero : public Field<1, SpaceDim>
{
public:
	double operator()(const StaticVector<SpaceDim>& arg) const override
		{ return 0.0; }
};

template<int ValueDim, int SpaceDim>
class FieldGenericZero : public FieldGeneric<ValueDim, SpaceDim>
{
public:
	void getValue(StaticVector<ValueDim>& value, const StaticVector<SpaceDim>& arg) const override
	{
		for (int i=0; i<ValueDim; i++)
			value.x[i] = 0.0;
	}
};

template<int ValueDim, int SpaceDim>
class FieldZero : public FieldGenericZero<ValueDim, SpaceDim> { };

template<int SpaceDim>
class FieldZero<1, SpaceDim> : public FieldScalarZero<SpaceDim> { };

////////////////////
// Linear scalar field
template<int SpaceDim>
class FieldLinearScalar : public Field<1, SpaceDim>
{
public:
	FieldLinearScalar(double amplitude, const StaticVector<SpaceDim>& direction, const StaticVector<SpaceDim>& zeroPoint = StaticVector<SpaceDim>()) :
		m_zeroPoint(zeroPoint),
		m_direction(direction),
		m_amplitude(amplitude)
	{
		m_direction.normalize();
	}

	double operator()(const StaticVector<SpaceDim>& arg) const override
	{
        return ((arg - m_zeroPoint)*m_direction) * m_amplitude;
	}

private:

	const StaticVector<SpaceDim> m_zeroPoint;
	StaticVector<SpaceDim> m_direction;
	double m_amplitude;
};

template<int SpaceDim>
class FieldScalar1D : public Field<1, SpaceDim>
{
public:
	FieldScalar1D(Function1D func, const StaticVector<SpaceDim>& direction, const StaticVector<SpaceDim>& zeroPoint = StaticVector<SpaceDim>()) :
		m_zeroPoint(zeroPoint),
		m_direction(direction),
		m_function(func)
	{
		m_direction.normalize();
	}

	double operator()(const StaticVector<SpaceDim>& arg) const override
	{
        return m_function( (arg - m_zeroPoint)*m_direction );
	}


private:
	const StaticVector<SpaceDim> m_zeroPoint;
	StaticVector<SpaceDim> m_direction;
	Function1D m_function;
};

////////////////////
// Field operating

template<int SpaceDim>
class Gradient : public IFieldVector<SpaceDim, SpaceDim>
{
public:
	Gradient(const IFieldScalar<SpaceDim>& field) : m_field(field) {}
	virtual ~Gradient() {}
	StaticVector<SpaceDim> operator()(const StaticVector<SpaceDim>& arg) const override
	{
		StaticVector<SpaceDim> result;
		getGradient(result, arg);
		return result;
	}

private:
	virtual void getGradient(StaticVector<SpaceDim>& result, const StaticVector<SpaceDim>& arg) const = 0;

protected:
	const IFieldScalar<SpaceDim>& m_field;
};

template<int SpaceDim>
class GradientFixedStep : public Gradient<SpaceDim>
{
public:
	GradientFixedStep(const IFieldScalar<SpaceDim>& field, double step) :
		Gradient<SpaceDim>(field),
		m_step(step)
	{ }

	void setStep(double step) { m_step = step; }

private:
	void getGradient(StaticVector<SpaceDim>& result, const StaticVector<SpaceDim>& arg) const override
	{
		StaticVector<SpaceDim> moved = arg;
		double pv = Gradient<SpaceDim>::m_field(arg);
		for (int i=0; i<SpaceDim; i++)
		{
			moved.x[i] += m_step;
			double mv = Gradient<SpaceDim>::m_field(moved);
			result.x[i] = (mv - pv) / m_step;
			moved.x[i] -= m_step;
		}
	}

	double m_step;
};


/*
template<int ValueDim, int SpaceDim>
class FieldFromFunc : public Field<ValueDim, SpaceDim>
{
public:
	FieldFromFunc(FieldFunction<ValueDim, SpaceDim>& f) :
		m_func(f)
	{ }

	void getValue(Vector<ValueDim>& value, const Vector<SpaceDim>& arg) override
	{
		m_func(value, arg);
	}

private:
	FieldFunction<ValueDim, SpaceDim> m_func;
};*/

}  // namespace sotm


#endif /* LIBSOTM_SOTM_MATH_FIELD_HPP_ */
