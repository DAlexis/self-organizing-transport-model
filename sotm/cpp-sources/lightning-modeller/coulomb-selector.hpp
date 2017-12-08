#ifndef COULOMBSELECTOR_HPP
#define COULOMBSELECTOR_HPP

#include "sotm/optimizers/coulomb.hpp"
#include "sotm/payloads/electrostatics/electrostatics.hpp"
#include "sotm/optimizers/coulomb.hpp"

#include "cic.hpp"
#include <string>

class CoulombSelector
{
public:
    CoulombSelector(cic::Parameters& p);

    void addCoulombCalculator(sotm::ElectrostaticPhysicalContext& c);

    static void parseScales(octree::DiscreteScales& target, const std::string& source);

private:

    std::unique_ptr<const octree::IScalesConfig> generateScales();

    std::unique_ptr<sotm::IColoumbCalculator> produce(sotm::ElectrostaticPhysicalContext& c, const std::string& method);

    cic::ParametersGroup m_pg{
        "Coulomb",
        "Coulomb calculation optimization options",
        cic::Parameter<std::string>("method",        "Method used by default: bruteforce, octree", "bruteforce"),
        cic::Parameter<std::string>("compare-with",  "Method used to be compared with default: none, bruteforce, octree", "none"),
        cic::Parameter<std::string>("octree-scales", "Scales for octree method. Format: \"(1.0, 1.0); (3.0, 4.0); (100.0, 200.0)\"", "")
    };
};

#endif // COULOMBSELECTOR_HPP
