#include "sotm/optimizers/coulomb.hpp"
#include "sotm/utils/const.hpp"

using namespace sotm;

FieldPotential::FieldPotential(double Ex, double Ey, double Ez, double potential) :
    field(Ex, Ey, Ez),
    potential(potential)
{

}

FieldPotential::FieldPotential(const Vector<3>& E, double p) :
    field(E),
    potential(p)
{

}

CoulombOptimizer::CoulombOptimizer(GraphRegister& graph) :
    m_graph(graph)
{

}

FieldPotential CoulombOptimizer::getFP(Vector<3> pos, CoulombNode* exclude)
{
    buildNodesVector();

    double potential = 0.0;
    Vector<3> E;

    for (auto it: m_nodesNotIsolatedVector)
    {
        // Skip this node
        if (it == exclude)
            continue;
        Vector<3> r1 = it->node.pos;
        double dist = (pos - r1).len();
        double charge = it->charge;

        double dp = Const::Si::k * charge / dist;
        double tmp = dp / dist / dist;

        potential += dp;

        E.x[0] += tmp * (pos.x[0]-r1.x[0]);
        E.x[1] += tmp * (pos.x[1]-r1.x[1]);
        E.x[3] += tmp * (pos.x[2]-r1.x[2]);
    };

    return FieldPotential(E, potential);
}

void CoulombOptimizer::addCN(CoulombNode& cn)
{
    m_nodesNotIsolated.insert(&cn);
    m_nodesNotIsolatedVector.push_back(&cn);
}

void CoulombOptimizer::removeCN(CoulombNode& cn)
{
    m_nodesNotIsolated.erase(&cn);
    m_vectorDirty = true;
}

void CoulombOptimizer::buildNodesVector()
{
    if (!m_vectorDirty)
        return;

    m_nodesNotIsolatedVector.clear();

    for (auto it: m_nodesNotIsolated)
        m_nodesNotIsolatedVector.push_back(it);

    m_vectorDirty = false;
}

////////////////////////
// CoulombNode

CoulombNode::CoulombNode(CoulombOptimizer& co, double &charge, Node &thisNode) :
    charge(charge),
    node(thisNode),
    m_co(co)
{
    m_co.addCN(*this);
}

CoulombNode::~CoulombNode()
{
    m_co.removeCN(*this);
}

FieldPotential CoulombNode::getFP()
{
    return m_co.getFP(node.pos, this);
}
