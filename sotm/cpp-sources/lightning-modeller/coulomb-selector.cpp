#include "coulomb-selector.hpp"
#include <boost/algorithm/string.hpp>
#include <string>
#include <algorithm>
#include <locale>
#include <cctype>

using namespace sotm;
using namespace std;

CoulombSelector::CoulombSelector(cic::Parameters& p)
{
    p.addGroup(m_pg);
}

void CoulombSelector::addCoulombCalculator(ElectrostaticPhysicalContext& c)
{
    if (m_pg.get<std::string>("method") != "bruteforce" && m_pg.get<std::string>("method") != "octree")
        throw std::runtime_error(std::string("Unknown coulomb field calculation method \"") + m_pg.get<std::string>("method") + "\" in option method");

    if (m_pg.get<std::string>("compare-with") != "bruteforce"
            && m_pg.get<std::string>("compare-with") != "octree"
            && m_pg.get<std::string>("compare-with") != "none"
            && m_pg.get<std::string>("compare-with") != "")
        throw std::runtime_error(std::string("Unknown coulomb field calculation method \"") + m_pg.get<std::string>("method") + "\" in option compare-with");

    std::unique_ptr<IColoumbCalculator> first = produce(c, m_pg.get<std::string>("method")),
        second = produce(c, m_pg.get<std::string>("compare-with"));

    if (second == nullptr)
    {
        c.optimizer = std::move(first);
    } else {
        c.optimizer.reset(
            new CoulombComarator(
                std::move(first),
                std::move(second)
            )
        );
    }
}

void CoulombSelector::parseScales(octree::DiscreteScales& target, const std::string& source)
{
    std::string processed = source;
    processed.erase(std::remove_if(processed.begin(), processed.end(), ::isspace), processed.end());
    if (processed.size() == 0)
        return;
    std::vector<std::string> scales;
    boost::split(scales, processed, boost::is_any_of(";"));
    for (auto &it : scales)
    {
        if (it.size() < 5 || it.front() != '(' || it.back() != ')')
            throw std::runtime_error(std::string("Invalid scales string") + source + " in " + it);

        std::string valsTogether(it.begin()+1, it.end()-2);
        std::vector<std::string> vals;
        boost::split(vals, valsTogether, boost::is_any_of(","));
        if (vals.size() != 2)
            throw std::runtime_error(std::string("Invalid scales string") + source + " in " + it);

        target.addScale(std::stof(vals[0]), std::stof(vals[1]));
    }
}

std::unique_ptr<const octree::IScalesConfig> CoulombSelector::generateScales()
{
    std::unique_ptr<octree::IScalesConfig> result;

    std::string scalesConfig = m_pg.get<std::string>("octree-scales");
    scalesConfig.erase(std::remove_if(scalesConfig.begin(), scalesConfig.end(), ::isspace), scalesConfig.end());
    std::vector<std::string> substrs;
    boost::split(substrs, scalesConfig, boost::is_any_of(":"));
    if (substrs.size() == 1)
    {
        std::cout << "Crating discrete" << std::endl;
        result.reset(new octree::DiscreteScales());
        parseScales(static_cast<octree::DiscreteScales&>(*result), substrs[0]);
    } else if (substrs.size() == 2 && substrs[0] == "discrete")
    {
        std::cout << "Crating discrete" << std::endl;
        result.reset(new octree::DiscreteScales());
        parseScales(static_cast<octree::DiscreteScales&>(*result), substrs[1]);
    } else if (substrs.size() == 2 && substrs[0] == "linear")
    {
        std::cout << "Crating linear" << std::endl;
        result.reset(new octree::LinearScales(stod(substrs[1])));
    } else {
        throw std::runtime_error(std::string("Scales string format os bad: ") + m_pg.get<std::string>("octree-scales"));
    }
    return std::unique_ptr<const octree::IScalesConfig>(std::move(result)); // Making const unique_ptr
}

std::unique_ptr<IColoumbCalculator> CoulombSelector::produce(ElectrostaticPhysicalContext& c, const std::string& method)
{
    std::unique_ptr<IColoumbCalculator> result;

    if (method == "bruteforce")
    {
        cout << "Creating brute force coulomb field calculator" << endl;
        result.reset(new CoulombBruteForce(c.model().graphRegister));
    } else if (method == "octree")
    {
        cout << "Creating octree-based optimization for coulomb field calculator" << endl;

        result.reset(new CoulombOctree(c.model().graphRegister, generateScales()));
    }
    return result;
}
