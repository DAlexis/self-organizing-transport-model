#include "sotm/math/functions.hpp"
#include "sotm/utils/assert.hpp"

using namespace sotm;

#include <cmath>

TrapezoidFunc::TrapezoidFunc(double amplitude, double zScale, double zRecession, double z0) :
	m_z0(z0),
	m_amplitude(amplitude),
	m_zScale(zScale),
	m_zRecession(zRecession),
	m_a(m_amplitude / (2.0 * m_zRecession)),
	m_c(m_amplitude * (m_zScale + m_zRecession) / 2.0)
{
	ASSERT(m_zScale >= 0.0, "TrapezoidFunc: zScale should not be negative");
	ASSERT(m_zRecession >= 0.0, "TrapezoidFunc: zRecession should not be negative");
}

double TrapezoidFunc::operator()(double z)
{
	double dz = z - m_z0;
	if (std::abs(dz) <= m_zScale / 2.0)
		return dz * m_amplitude;

	if (dz >= m_zScale / 2.0 + m_zRecession)
	{
		return m_c;
	}

	if (dz <= -m_zScale / 2.0 - m_zRecession)
	{
		return -m_c;
	}

	if (dz > m_zScale / 2.0)
	{
		double x = (dz - m_zScale / 2.0 - m_zRecession);
		return -m_a * x*x + m_c;
	}

	if (dz < -m_zScale / 2.0)
	{
		double x = (dz + m_zScale / 2.0 + m_zRecession);
		return m_a * x*x - m_c;
	}
	return 0.0;
}

double TrapezoidFunc::maxAbs()
{
	return m_c;
}
