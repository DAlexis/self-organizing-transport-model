#include "sotm/output/variables.hpp"
#include <algorithm>
#include <cmath>

using namespace sotm;

IColorMapper::Color IColorMapper::black(0.0, 0.0, 0.0);
IColorMapper::Color IColorMapper::red  (1.0, 0.0, 0.0);
IColorMapper::Color IColorMapper::green(0.0, 1.0, 0.0);
IColorMapper::Color IColorMapper::blue (0.0, 0.0, 1.0);


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

	if (m_min == m_max)
		return 0.0;

	return (value - m_min) / (m_max - m_min);
}

double Scaler::rescale(double value)
{
	if (m_scale == Scale::log)
	{
		return log(fabs(value));
	}
	return value;
}


LinearGradientColorMapper::LinearGradientColorMapper()
{

}

IColorMapper::Color LinearGradientColorMapper::get(double value)
{
	if (m_colors.size() < 2)
		return IColorMapper::green;

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
