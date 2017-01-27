/*
 * variable-mapper.hpp
 *
 *  Created on: 26 янв. 2017 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_OUTPUT_VARIABLES_HPP_
#define LIBSOTM_SOTM_OUTPUT_VARIABLES_HPP_

#include <vector>

namespace sotm {

class Scaler
{
public:
	enum class Scale {
		linear,
		log
	};

	Scaler(Scale scale = Scale::linear);
	double scale(double value);

	Scale getScale() { return m_scale; }

private:
	double rescale(double value);
	Scale m_scale;
	double m_max = 0;
	double m_min = 0;
	bool m_initialized = false;
};

class IColorMapper
{
public:
	struct Color {
		Color(double r = 0.0, double g = 0.0, double b = 0.0) : r(r), g(g), b(b) {}
		double r, g, b;
		bool operator==(const Color& right) { return r == right.r && g == right.g && b == right.b; }
	};
	static Color black;
	static Color red;
	static Color green;
	static Color blue;

	virtual ~IColorMapper() {}
	virtual Color get(double value) = 0;
};

class LinearGradientColorMapper : public IColorMapper
{
public:
	LinearGradientColorMapper();
	Color get(double value) override;
	void addColor(double value, const Color& c);

private:
	using ColorPoint = std::pair<double, Color>;
	std::vector<ColorPoint> m_colors;
};

} // namespace sotm


#endif /* LIBSOTM_SOTM_OUTPUT_VARIABLES_HPP_ */
