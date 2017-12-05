#ifndef COULOMBSELECTOR_HPP
#define COULOMBSELECTOR_HPP

#include "sotm/optimizers/coulomb.hpp"
#include <string>

class CoulombSelector
{
public:
    static void parseScales(octree::DiscreteScales& target, const std::string& source);
};

#endif // COULOMBSELECTOR_HPP
