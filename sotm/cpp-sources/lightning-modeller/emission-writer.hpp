#ifndef EMISSIONWRITER_HPP
#define EMISSIONWRITER_HPP

#include "sotm/math/electromagnetic-emission.hpp"
#include "sotm/base/time-iter.hpp"
#include <fstream>
#include <chrono>

namespace sotm {

class EmissionWriter : public TimeHookPeriodic
{
public:
    EmissionWriter(
        const std::string& filename,
        const sotm::EmissionCounterWithoutLag& emission,
        double dt,
        std::chrono::duration<double> interval);

    void write(double t, bool force = false);

private:
    void hook(double realTime, double wantedTime) override;

    sotm::StaticVector<3> m_receiver_position;
    const std::string& m_filename;
    const sotm::EmissionCounterWithoutLag& m_emission;
    const double m_dt;
    std::chrono::duration<double> m_interval;

    std::chrono::steady_clock::time_point m_last_write_ts{};
    std::ofstream m_emission_file;
    double m_last_t = 0.0;
};

}

#endif // EMISSIONWRITER_HPP
