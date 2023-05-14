#include "sotm/math/electromagnetic-emission.hpp"
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


EmissionCounterWithoutLag::EmissionCounterWithoutLag(StaticVector<3> destination) :
    m_destination(destination)
{
}

void EmissionCounterWithoutLag::add_charge(double time, double dt, StaticVector<3> source, double charge, double charge_dot)
{
    update_time(time);
    StaticVector<3> r = m_destination - source;
    double r_length = r.norm();
    StaticVector<3> delta = r / sqr(r_length) * (charge_dot / Const::Si::c + charge / r_length);
    m_field_at_time.back() += delta;
}

void EmissionCounterWithoutLag::add_current(double time, double dt, StaticVector<3> source, StaticVector<3> dl, double current_dot)
{
    update_time(time);
    StaticVector<3> r = m_destination - source;
    double r_length = r.norm();
    StaticVector<3> delta = dl / (Const::Si::c * r_length) * current_dot;
    m_field_at_time.back() += delta;
}


std::vector<StaticVector<3>> EmissionCounterWithoutLag::get_samples(double from, double to, double dt)
{
    std::vector<StaticVector<3>> result(size_t((to-from) / dt));  // init with zeros
    if (m_field_at_time.empty())
        return result;

    size_t internal_i = 0;
    size_t result_i = 0;
    for (double t = from; t <= to; t += dt, result_i++)
    {
        // if t is out of current internal bar, lets increment internal index while t is not on bar
        while (m_time_points.size() > internal_i+1 && m_time_points[internal_i+1] < t)
            internal_i++;

        result.push_back(m_field_at_time[internal_i]);

        if (m_time_points.size() == internal_i+1)
            break;
    }
    return result;
}

void EmissionCounterWithoutLag::update_time(double new_time)
{
    if (!m_time_points.empty())
    {
        if (new_time == m_time_points.back())
        {
            return;
        }
        if (new_time < m_time_points.back())
        {
            throw std::logic_error("EmissionCounterWithoutLag::update_time update with time in the past");
        }
    }
    m_time_points.push_back(new_time);
    m_field_at_time.push_back(StaticVector<3>());
}
