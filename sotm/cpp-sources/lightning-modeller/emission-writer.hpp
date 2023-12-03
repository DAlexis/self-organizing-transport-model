#ifndef EMISSIONWRITER_HPP
#define EMISSIONWRITER_HPP

#include "sotm/math/electromagnetic-emission.hpp"
#include "cic.hpp"

class EmissionWriter
{
public:
    EmissionWriter(cic::Parameters& p);

private:
    sotm::StaticVector<3> m_receiver_position;

    cic::ParametersGroup m_pg{
        "Emission",
        "Emission calculation options",
        cic::Parameter<double>("receiver-x", "Electromagnetic wave receiver x", 500),
        cic::Parameter<double>("receiver-y", "Electromagnetic wave receiver y", 0),
        cic::Parameter<double>("receiver-z", "Electromagnetic wave receiver z", -500)
    };
};

#endif // EMISSIONWRITER_HPP
