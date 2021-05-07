#ifndef COULOMB_HPP
#define COULOMB_HPP

#include "sotm/math/geometry.hpp"
#include "sotm/base/transport-graph.hpp"
#include "octree.hpp"
#include <string>
#include <atomic>

namespace sotm {

class CoulombNodeBruteForce;

struct FieldPotential
{
    FieldPotential(double Ex = 0, double Ey = 0, double Ez = 0, double potential = 0);
    FieldPotential(const StaticVector<3>& E, double p);
    StaticVector<3> field;
    double potential = 0.0;

    FieldPotential& operator+=(const FieldPotential& right);

    static FieldPotential convolutionVisitor(const Position& target, const Position& object, double mass);
};

class ICoulombNode
{
public:
    virtual ~ICoulombNode() {}
    virtual FieldPotential getFP() = 0;
};

class CoulombNodeBase : public ICoulombNode
{
public:
    CoulombNodeBase(const double& charge, Node& node)
        : charge(charge), node(node)
    { }

    virtual ~CoulombNodeBase() {}
    virtual FieldPotential getFP() = 0;

    const double& charge;
    Node& node;
};

class IColoumbCalculator
{
public:
    virtual ~IColoumbCalculator() {}
    virtual FieldPotential getFP(StaticVector<3> pos, CoulombNodeBase* exclude = nullptr) = 0;
    virtual void rebuildOptimization() = 0;
    virtual void addCN(CoulombNodeBase& cn) = 0;
    virtual void removeCN(CoulombNodeBase& cn) = 0;
    virtual CoulombNodeBase* makeNode(double& charge, Node& thisNode) = 0;
    virtual void getClose(std::vector<CoulombNodeBase*>& container, const StaticVector<3>& pos, double distace) = 0;
};


///////////////////////////
// Special comparator class for benchmark that wrap two other
class CoulombComarator : public IColoumbCalculator
{
public:
    CoulombComarator(std::unique_ptr<IColoumbCalculator> c1, std::unique_ptr<IColoumbCalculator> c2);

    FieldPotential getFP(StaticVector<3> pos, CoulombNodeBase* exclude = nullptr) override;
    void getClose(std::vector<CoulombNodeBase*>& container, const StaticVector<3>& pos, double distance) override;

    void rebuildOptimization() override;
    void addCN(CoulombNodeBase& cn) override;
    void removeCN(CoulombNodeBase& cn) override;
    CoulombNodeBase* makeNode(double& charge, Node& thisNode) override;

private:
    std::string diffStr(const FieldPotential& r1, const FieldPotential& r2);
    double err(double v1, double v2);
    std::string header();
    std::unique_ptr<IColoumbCalculator> m_c1;
    std::unique_ptr<IColoumbCalculator> m_c2;
    std::atomic<size_t> m_counter{0};

    double m_maxDiff = 0.0;
};

class CoulombComaratorNode : public CoulombNodeBase
{
friend class CoulombComarator;
public:
    CoulombComaratorNode(IColoumbCalculator &co, Node& thisNode, std::unique_ptr<CoulombNodeBase> n1, std::unique_ptr<CoulombNodeBase> n2);

    FieldPotential getFP() override;

private:
    IColoumbCalculator &m_co;
    std::unique_ptr<CoulombNodeBase> m_n1;
    std::unique_ptr<CoulombNodeBase> m_n2;
};

}

#endif // COULOMB_HPP
