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
	void setScale(Scale scale);
	double scale(double value);
	void fixValue(double value, double result);

	Scale getScale() { return m_scale; }

private:
	void reset();
	double rescale(double value);
	Scale m_scale;
	double m_max = 0;
	double m_min = 0;
	bool m_initialized = false;
	double m_fixedValue = 0.0, m_fixedResult = 0.0;
	bool m_hasFixedValue = false;
};

class ColorMapperBase
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
	static Color yellow;

	virtual ~ColorMapperBase() {}
	virtual Color get(double value) = 0;
	void get(double value, double* rgb);
};

class LinearGradientColorMapper : public ColorMapperBase
{
public:
	LinearGradientColorMapper();
	Color get(double value) override;
	void addColor(double value, const Color& c);

	void setBlueRed();
	void setGreenYellow();
private:
	using ColorPoint = std::pair<double, Color>;
	std::vector<ColorPoint> m_colors;
};

} // namespace sotm


#endif /* LIBSOTM_SOTM_OUTPUT_VARIABLES_HPP_ */
