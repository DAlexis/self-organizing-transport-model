#include "sotm/math/electromagnetic-emission.hpp"

#include "gtest/gtest.h"
#include <cmath>

using namespace sotm;

/**
 * @brief The DipoleSource class - q(t) = Q * cos(omega * t)
 */
class DipoleSource
{
public:
    DipoleSource(double omega, double charge, double distance) :
        m_omega(omega), m_charge(charge), m_distance(distance)
    { }

    double get_charge(double time)
    {
        return m_charge * cos(m_omega * time);
    }

    double get_charge_dot(double time)
    {
        return -m_charge * m_omega * sin(m_omega * time);
    }

    double get_current_dot(double time)
    {
        return -m_charge * m_omega * m_omega * cos(m_omega * time);
    }

    double get_distance()
    {
        return m_distance;
    }

private:
    double m_omega, m_charge, m_distance;
};

/**
 * @brief The TimeStepGenerator class - triangular dt by time generator
 */
class TimeStepGenerator
{
public:
    TimeStepGenerator(double min, double ampl, double period) :
        m_min(min), m_ampl(ampl), m_period(period)
    { }

    double get_dt(double time)
    {
        double in_period_time = time - round(time / m_period) * m_period;
        return m_min + in_period_time / m_period * m_ampl;
    }

private:
    double m_min, m_ampl, m_period;
};

TEST(ElectromagneticEmission, DipoleEmission)
{
/*
    double freq = 10;
    TimeStepGenerator tsg(0.001, 0.01, 0.1234);
    DipoleSource dipole(M_2_PI * freq, 1.0, 1.0);

    StaticVector<3> center(0.0, 0.0, 0.0);

    StaticVector<3> source_top(0.1, 0.0, 0.0);
    StaticVector<3> source_bottom(-0.1, 0.0, 0.0);

    StaticVector<3> dl(0.2, 0.0, 0.0);

    StaticVector<3> receiver1(0.0, 10.0, 0.0);

    EmissionCounterWithoutLag emission_counter(receiver1);

    for (double t = 0; t < 10.0; t += tsg.get_dt(t))
    {
        double dt = tsg.get_dt(t);
        double charge_top = dipole.get_charge(t);
        double charge_top_dot = dipole.get_charge_dot(t);
        double current_dot = dipole.get_current_dot(t);

        emission_counter.add_charge(t, dt, source_top, charge_top, charge_top_dot);
        emission_counter.add_charge(t, dt, source_bottom, -charge_top, -charge_top_dot);

        emission_counter.add_current(t, dt, center, dl, current_dot);
    }

*/
}
