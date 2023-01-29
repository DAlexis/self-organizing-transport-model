#ifndef ELECTRICEMISSION_HPP
#define ELECTRICEMISSION_HPP

#include "sotm/math/geometry.hpp"
#include <map>
#include <vector>

namespace sotm {

class EmissionCounter
{
public:
    EmissionCounter(StaticVector<3> destination, double sample_dt);

    void add_charge(double time, double dt, StaticVector<3> source, double charge, double charge_dot);
    void add_current(double time, double dt, StaticVector<3> source, StaticVector<3> dl, double current_dot);

    std::vector<StaticVector<3>> get_samples(double stop_time);

private:
    void add_delta(double t, const StaticVector<3>& delta);

    std::map<double, StaticVector<3>> m_field_changes_at_time;
    StaticVector<3> m_destination;
    double m_sample_dt;
    double m_last_sample_time = 0.0;
    StaticVector<3> m_current_sum;
};

}

#endif // ELECTRICEMISSION_HPP
