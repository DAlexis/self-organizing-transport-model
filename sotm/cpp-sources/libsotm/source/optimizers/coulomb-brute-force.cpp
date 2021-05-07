#include "sotm/optimizers/coulomb-brute-force.hpp"
#include "sotm/utils/const.hpp"

#include <iostream>
#include <sstream>
#include <cmath>

using namespace sotm;

//////////////////////
// CoulombBruteForce
CoulombBruteForce::CoulombBruteForce(GraphRegister& graph) :
    m_graph(graph)
{
    // todo: add scales to m_scales
}

FieldPotential CoulombBruteForce::getFP(StaticVector<3> pos, CoulombNodeBase* exclude)
{
    buildNodesVector();

    double potential = 0.0;
    StaticVector<3> E;

    for (auto it: m_nodesNotIsolatedVector)
    {
        // Skip this node
        if (static_cast<CoulombNodeBase*>(it) == exclude)
            continue;
        StaticVector<3> r1 = it->node.pos;
        double dist = (pos - r1).norm();
        double charge = it->charge;

        double dp = Const::Si::k * charge / dist;
        double tmp = dp / (dist * dist);

        potential += dp;

        E.x[0] += tmp * (pos.x[0]-r1.x[0]);
        E.x[1] += tmp * (pos.x[1]-r1.x[1]);
        E.x[2] += tmp * (pos.x[2]-r1.x[2]);
    };

    return FieldPotential(E, potential);
}

void CoulombBruteForce::rebuildOptimization()
{

}

CoulombNodeBase* CoulombBruteForce::makeNode(double& charge, Node& thisNode)
{
    return new CoulombNodeBruteForce(*this, charge, thisNode);
}

void CoulombBruteForce::getClose(std::vector<CoulombNodeBase*>& container, const StaticVector<3>& pos, double distance)
{
    for (auto it: m_nodesNotIsolatedVector)
    {
        double d = (pos - it->node.pos).norm();
        if (d <= distance)
        {
            container.push_back(it);
        }
    };
}

void CoulombBruteForce::addCN(CoulombNodeBase& cn)
{
    m_nodesNotIsolated.insert(static_cast<CoulombNodeBruteForce*>(&cn));
    m_nodesNotIsolatedVector.push_back(static_cast<CoulombNodeBruteForce*>(&cn));
}

void CoulombBruteForce::removeCN(CoulombNodeBase& cn)
{
    m_nodesNotIsolated.erase(static_cast<CoulombNodeBruteForce*>(&cn));
    m_vectorDirty = true;
}

void CoulombBruteForce::buildNodesVector()
{
    if (!m_vectorDirty)
        return;

    m_nodesNotIsolatedVector.clear();

    for (auto it: m_nodesNotIsolated)
        m_nodesNotIsolatedVector.push_back(it);

    m_vectorDirty = false;
}

////////////////////////
// CoulombNodeTrivial

CoulombNodeBruteForce::CoulombNodeBruteForce(IColoumbCalculator& co, double &charge, Node &thisNode) :
    CoulombNodeBase(charge, thisNode),
    m_co(co)
{
    m_co.addCN(*this);
    m_ectreeElement = std::make_shared<octree::Element>(node.pos.x, charge);
}

CoulombNodeBruteForce::~CoulombNodeBruteForce()
{
    m_co.removeCN(*this);
}

FieldPotential CoulombNodeBruteForce::getFP()
{
    return m_co.getFP(node.pos, this);
}
