#ifndef COULOMB_HPP
#define COULOMB_HPP

#include "sotm/math/geometry.hpp"
#include "sotm/base/transport-graph.hpp"
#include "octree.hpp"
#include <string>
#include <unordered_set>
#include <atomic>

namespace sotm {

class CoulombNodeBruteForce;

struct FieldPotential
{
    FieldPotential(double Ex = 0, double Ey = 0, double Ez = 0, double potential = 0);
    FieldPotential(const Vector<3>& E, double p);
    Vector<3> field;
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
    CoulombNodeBase(const double& charge)
        : charge(charge)
    { }

    virtual ~CoulombNodeBase() {}
    virtual FieldPotential getFP() = 0;

    const double& charge;
};

class IColoumbCalculator
{
public:
    virtual ~IColoumbCalculator() {}
    virtual FieldPotential getFP(Vector<3> pos, CoulombNodeBase* exclude = nullptr) = 0;
    virtual void rebuildOptimization() = 0;
    virtual void addCN(CoulombNodeBase& cn) = 0;
    virtual void removeCN(CoulombNodeBase& cn) = 0;
    virtual CoulombNodeBase* makeNode(double& charge, Node& thisNode) = 0;
};

//////////////////////////////////////
// Brute force approach

class CoulombBruteForce : public IColoumbCalculator
{
public:
    CoulombBruteForce(GraphRegister& graph);
    FieldPotential getFP(Vector<3> pos, CoulombNodeBase* exclude = nullptr) override;

    /**
     * @brief build positive and negative octree
     */
    void rebuildOptimization() override;

    CoulombNodeBase* makeNode(double& charge, Node& thisNode) override;
private:
    void addCN(CoulombNodeBase& cn) override;
    void removeCN(CoulombNodeBase& cn) override;
    void buildNodesVector();

    GraphRegister& m_graph;
    /// @todo Use unordered_set
    //std::set<CoulombNode*> m_nodesIsolated;
    std::set<CoulombNodeBruteForce*> m_nodesNotIsolated;
    std::vector<CoulombNodeBruteForce*> m_nodesNotIsolatedVector;
    bool m_vectorDirty = true;
};

class CoulombNodeBruteForce : public CoulombNodeBase
{
friend class CoulombBruteForce;
public:
    CoulombNodeBruteForce(IColoumbCalculator& co, double& charge, Node& thisNode);
    ~CoulombNodeBruteForce();

    FieldPotential getFP() override;

    Node& node;
    std::shared_ptr<octree::Element> m_ectreeElement;

private:
    IColoumbCalculator &m_co;
    double m_isolatedPotential = 0;
    Vector<3> m_isolatedField{0.0, 0.0, 0.0};
};

//////////////////////////////////////
// Octree-optimized approach (todo)

class CoulombNodeOctree;

class CoulombOctree: public IColoumbCalculator
{
public:
    CoulombOctree(GraphRegister& graph, std::unique_ptr<const octree::IScalesConfig> scales);
    FieldPotential getFP(Vector<3> pos, CoulombNodeBase* exclude = nullptr) override;
    CoulombNodeBase* makeNode(double& charge, Node& thisNode) override;

    /**
     * @brief build positive and negative octree
     */
    void rebuildOptimization() override;
private:
    void addCN(CoulombNodeBase& cn) override;
    void removeCN(CoulombNodeBase& cn) override;

    GraphRegister& m_graph;
    std::set<CoulombNodeOctree*> m_nodesNotIsolated;

    octree::Octree m_octreePositive;
    octree::Octree m_octreeNegative;

    std::unique_ptr<const octree::IScalesConfig> m_scales;
    octree::Convolution<FieldPotential> m_convolution{*m_scales};
};

class CoulombNodeOctree : public CoulombNodeBase
{
friend class CoulombBruteForce;
public:
    CoulombNodeOctree(IColoumbCalculator& co, double& charge, Node& thisNode);
    ~CoulombNodeOctree();

    FieldPotential getFP() override;


    Node& node;
    std::shared_ptr<octree::Element> m_ectreeElement;

private:
    IColoumbCalculator &m_co;
    double m_isolatedPotential = 0;
    Vector<3> m_isolatedField{0.0, 0.0, 0.0};
};

///////////////////////////
// Special comparator class for benchmark that wrap two other
class CoulombComarator : public IColoumbCalculator
{
public:
    CoulombComarator(std::unique_ptr<IColoumbCalculator> c1, std::unique_ptr<IColoumbCalculator> c2);

    FieldPotential getFP(Vector<3> pos, CoulombNodeBase* exclude = nullptr) override;
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
    Node& m_node;
    IColoumbCalculator &m_co;
    std::unique_ptr<CoulombNodeBase> m_n1;
    std::unique_ptr<CoulombNodeBase> m_n2;
};

}

#endif // COULOMB_HPP
