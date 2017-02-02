#include "sotm/output/variables.hpp"
#include "sotm/utils/assert.hpp"
#include <algorithm>
#include <cmath>

using namespace sotm;

ColorMapperBase::Color ColorMapperBase::black(0.0, 0.0, 0.0);
ColorMapperBase::Color ColorMapperBase::red  (1.0, 0.0, 0.0);
ColorMapperBase::Color ColorMapperBase::green(0.0, 1.0, 0.0);
ColorMapperBase::Color ColorMapperBase::blue (0.0, 0.0, 1.0);


Scaler::Scaler(Scale scale) :
		m_scale(scale)
{
}

double Scaler::scale(double value)
{
	value = rescale(value);

	if (!m_initialized)
	{
		m_max = m_min = value;
		m_initialized = true;
	}

	if (value > m_max)
		m_max = value;
	else if (value < m_min)
		m_min = value;

	if (m_hasFixedValue)
	{
		if (m_min == m_max)
			return m_fixedResult;

		if (value > m_fixedValue)
			return m_fixedResult + (value - m_fixedValue) / (m_max - m_fixedValue) * (1.0 - m_fixedResult);
		else
			return (value - m_min) / (m_fixedValue - m_min) * m_fixedResult;

	} else {
		if (m_min == m_max)
			return 0.0;

		return (value - m_min) / (m_max - m_min);
	}
}

void Scaler::fixValue(double value, double result)
{
	ASSERT(result >= 0.0 && result <= 1.0, "Scaler::fixValue: result must be from [0.0; 1.0]");
	m_fixedValue = rescale(value); m_fixedResult = result;
	m_hasFixedValue = true;
}

double Scaler::rescale(double value)
{
	if (m_scale == Scale::log)
	{
		return log(fabs(value));
	}
	return value;
}


void ColorMapperBase::get(double value, double* rgb)
{
	Color c = get(value);
	rgb[0] = c.r;
	rgb[1] = c.g;
	rgb[2] = c.b;
}

LinearGradientColorMapper::LinearGradientColorMapper()
{

}

ColorMapperBase::Color LinearGradientColorMapper::get(double value)
{
	if (m_colors.size() < 2)
		return ColorMapperBase::green;

	size_t l = 0, r = m_colors.size()-1;
	while (r-l != 1)
	{
		size_t c = (r+l)/2;
		double cv = m_colors[c].first;
		if (cv < value)
			l = c;
		else
			r = c;
	}

	Color result;
	Color &cl = m_colors[l].second, &cr = m_colors[r].second;
	double vl = m_colors[l].first, vr = m_colors[r].first;
	double prop = (value - vl) / (vr - vl);
	result.r = (1.0 - prop) * cl.r + prop * cr.r;
	result.g = (1.0 - prop) * cl.g + prop * cr.g;
	result.b = (1.0 - prop) * cl.b + prop * cr.b;
	return result;
}

void LinearGradientColorMapper::addColor(double value, const Color& c)
{
	m_colors.push_back(ColorPoint(value, c));
	std::sort(
			m_colors.begin(),
			m_colors.end(),
			[](const ColorPoint& left, const ColorPoint& right)
			{
				return left.first < right.first;
			}
	);
}

void LinearGradientColorMapper::setBlueRed()
{
	m_colors.clear();
	addColor(1.0, ColorMapperBase::red);
	addColor(0.0, ColorMapperBase::blue);
}
