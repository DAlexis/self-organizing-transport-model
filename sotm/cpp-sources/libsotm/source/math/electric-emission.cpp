#include "sotm/math/electric-emission.hpp"
#include "sotm/math/generic.hpp"
#include "sotm/utils/const.hpp"


using namespace sotm;

EmissionCounter::EmissionCounter(StaticVector<3> destination, double sample_dt) :
    m_destination(destination), m_sample_dt(sample_dt)
{
}

void EmissionCounter::add_charge(double time, double dt, StaticVector<3> source, double charge, double charge_dot)
{
    StaticVector<3> r = m_destination - source;
    double r_length = r.norm();
    double time_begin = time + r_length / Const::Si::c;
    double time_end = time_begin + dt;
    StaticVector<3> delta = r / sqr(r_length) * (charge_dot / Const::Si::c + charge / r_length);
    add_delta(time_begin, delta);
    add_delta(time_end, -delta);
}

void EmissionCounter::add_current(double time, double dt, StaticVector<3> source, StaticVector<3> dl, double current_dot)
{
    StaticVector<3> r = m_destination - source;
    double r_length = r.norm();
    double time_begin = time + r_length / Const::Si::c;
    double time_end = time_begin + dt;
    StaticVector<3> delta = dl / (Const::Si::c * r_length) * current_dot;
    add_delta(time_begin, delta);
    add_delta(time_end, -delta);
}


void EmissionCounter::add_delta(double t, const StaticVector<3>& delta)
{
    auto it = m_field_changes_at_time.find(t);
    if (it != m_field_changes_at_time.end())
    {
        it->second += delta;
    } else {
        m_field_changes_at_time.emplace(t, delta);
    }
}

std::vector<StaticVector<3>> EmissionCounter::get_samples(double stop_time)
{
    std::vector<StaticVector<3>> samples;
    samples.push_back(m_current_sum);
    for (auto it = m_field_changes_at_time.upper_bound(m_last_sample_time); it != m_field_changes_at_time.end() && it->first < stop_time; ++it)
    {
        m_current_sum += it->second;
        double t = it->first;
        if (m_last_sample_time + m_sample_dt <= t)
        {
            m_last_sample_time = t;
            samples.push_back(m_current_sum);
        }
    }
    return samples;
}
