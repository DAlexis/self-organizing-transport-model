#ifndef COULOMB_HPP
#define COULOMB_HPP

#include "sotm/math/geometry.hpp"
#include "sotm/base/transport-graph.hpp"
#include <unordered_set>

namespace sotm {

class CoulombNode;

struct FieldPotential
{
    FieldPotential(double Ex = 0, double Ey = 0, double Ez = 0, double potential = 0);
    FieldPotential(const Vector<3>& E, double p);
    Vector<3> field;
    double potential = 0.0;
};

class CoulombOptimizer
{
friend class CoulombNode;
public:
    CoulombOptimizer(GraphRegister& graph);
    FieldPotential getFP(Vector<3> pos, CoulombNode* exclude = nullptr);

private:
    void addCN(CoulombNode& cn);
    void removeCN(CoulombNode& cn);
    void buildNodesVector();

    GraphRegister& m_graph;
    /// @todo Use unordered_set
    //std::set<CoulombNode*> m_nodesIsolated;
    std::set<CoulombNode*> m_nodesNotIsolated;
    std::vector<CoulombNode*> m_nodesNotIsolatedVector;
    bool m_vectorDirty = true;
};

class CoulombNode
{
public:
    CoulombNode(CoulombOptimizer& co, double& charge, Node& thisNode);
    ~CoulombNode();

    FieldPotential getFP();

    const double& charge;
    Node& node;

private:
    CoulombOptimizer &m_co;
    double m_isolatedPotential = 0;
    Vector<3> m_isolatedField{0.0, 0.0, 0.0};
};

}

#endif // COULOMB_HPP
