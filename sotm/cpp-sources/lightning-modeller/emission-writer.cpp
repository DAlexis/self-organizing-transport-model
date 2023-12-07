#include "emission-writer.hpp"
#include <iostream>

using namespace std;
using namespace sotm;

EmissionWriter::EmissionWriter(
    const std::string& filename,
    const sotm::EmissionCounterWithoutLag& emission,
    double dt,
    std::chrono::duration<double> interval) :
    m_filename(filename), m_emission(emission), m_dt(dt), m_interval(interval),
    m_emission_file(filename.c_str(), ios::out | ios::binary)
{
    setPeriod(m_dt*10);
    m_emission_file << "time,Ex,Ey,Ez\n";
}

void EmissionWriter::write(double t, bool force)
{
    auto now = std::chrono::steady_clock::now();
    if (force || (now - m_last_write_ts) >= m_interval)
    {
        std::vector<StaticVector<3>> samples = m_emission.get_samples(m_last_t, t, m_dt);

        cout << "Writing emission samples: " << samples.size() << std::endl;

        for (size_t i = 0; i < samples.size(); i++)
        {
            const auto& e = samples[i];
            m_emission_file << m_last_t + m_dt * i << "," << e[0] << "," << e[1] << "," << e[2] << "\n";
        }

        m_last_write_ts = now;
        m_last_t = t;
        m_emission_file << flush;
    }
}

void EmissionWriter::hook(double realTime, double wantedTime)
{
    write(realTime, true);
}
