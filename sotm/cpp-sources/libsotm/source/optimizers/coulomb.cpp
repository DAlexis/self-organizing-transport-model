#include "sotm/optimizers/coulomb.hpp"
#include "sotm/utils/const.hpp"

#include <iostream>

using namespace sotm;

//////////////////////
// FieldPotential
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

FieldPotential& FieldPotential::operator+=(const FieldPotential& right)
{
    field += right.field;
    potential += right.potential;
    return *this;
}

//////////////////////
// CoulombBruteForce
CoulombBruteForce::CoulombBruteForce(GraphRegister& graph) :
    m_graph(graph)
{
    // todo: add scales to m_scales
}

FieldPotential CoulombBruteForce::getFP(Vector<3> pos, CoulombNodeBase* exclude)
{
    buildNodesVector();

    double potential = 0.0;
    Vector<3> E;

    for (auto it: m_nodesNotIsolatedVector)
    {
        // Skip this node
        if (static_cast<CoulombNodeBase*>(it) == exclude)
            continue;
        Vector<3> r1 = it->node.pos;
        double dist = (pos - r1).len();
        double charge = it->charge;

        double dp = Const::Si::k * charge / dist;
        double tmp = dp / dist / dist;

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

FieldPotential CoulombBruteForce::convolutionVisitor(const Position& target, const Position& object, double mass)
{
    double dist = (target - object).len();
    double charge = mass;

    double p = Const::Si::k * charge / dist;
    double tmp = p / dist / dist;

    FieldPotential fp;
    fp.potential = p;
    fp.field.x[0] = tmp * (target.x[0]-object.x[0]);
    fp.field.x[1] = tmp * (target.x[1]-object.x[1]);
    fp.field.x[2] = tmp * (target.x[2]-object.x[2]);
    return fp;
}

////////////////////////
// CoulombNodeTrivial

CoulombNodeBruteForce::CoulombNodeBruteForce(IColoumbCalculator& co, double &charge, Node &thisNode) :
    CoulombNodeBase(charge),
    node(thisNode),
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

////////////////////////
// CoulombOctree

CoulombOctree::CoulombOctree(GraphRegister& graph) :
    m_graph(graph)
{
    // todo: add scales to m_scales
}

FieldPotential CoulombOctree::getFP(Vector<3> pos, CoulombNodeBase* exclude)
{
    // No-octree variant
    if (m_octreeNegative.empty() && m_octreePositive.empty())
    {
        rebuildOptimization();
    }

    // Octree-based variant

    FieldPotential octreeResult;
    if (exclude != nullptr)
    {
        octreeResult  = m_convolution.convoluteExcludingElement(m_octreePositive, *static_cast<CoulombNodeOctree*>(exclude)->m_ectreeElement.get(), convolutionVisitor);
        octreeResult += m_convolution.convoluteExcludingElement(m_octreeNegative, *static_cast<CoulombNodeOctree*>(exclude)->m_ectreeElement.get(), convolutionVisitor);
    } else {
        octreeResult  = m_convolution.convolute(m_octreePositive, pos.x, convolutionVisitor);
        octreeResult += m_convolution.convolute(m_octreeNegative, pos.x, convolutionVisitor);

    }

    return octreeResult;
}

CoulombNodeBase* CoulombOctree::makeNode(double& charge, Node& thisNode)
{
    return new CoulombNodeOctree(*this, charge, thisNode);
}

void CoulombOctree::rebuildOptimization()
{
    m_octreeNegative.clear();
    m_octreePositive.clear();

    octree::CenterMassUpdatingMute cmumn(m_octreeNegative);
    octree::CenterMassUpdatingMute cmump(m_octreePositive);

    for (auto &it: m_nodesNotIsolated)
    {
        if (it->charge >= 0)
            m_octreePositive.add(it->m_ectreeElement);
        else
            m_octreeNegative.add(it->m_ectreeElement);
    }
}

void CoulombOctree::addCN(CoulombNodeBase& cn)
{
    m_nodesNotIsolated.insert(static_cast<CoulombNodeOctree*>(&cn));
}

void CoulombOctree::removeCN(CoulombNodeBase& cn)
{
    m_nodesNotIsolated.erase(static_cast<CoulombNodeOctree*>(&cn));
}

FieldPotential CoulombOctree::convolutionVisitor(const Position& target, const Position& object, double mass)
{
    double dist = (target - object).len();
    double charge = mass;

    double p = Const::Si::k * charge / dist;
    double tmp = p / dist / dist;

    FieldPotential fp;
    fp.potential = p;
    fp.field.x[0] = tmp * (target.x[0]-object.x[0]);
    fp.field.x[1] = tmp * (target.x[1]-object.x[1]);
    fp.field.x[2] = tmp * (target.x[2]-object.x[2]);
    return fp;
}

////////////////////////
// CoulombNodeOctree

CoulombNodeOctree::CoulombNodeOctree(IColoumbCalculator& co, double &charge, Node &thisNode) :
    CoulombNodeBase(charge),
    node(thisNode),
    m_co(co)
{
    m_co.addCN(*this);
    m_ectreeElement = std::make_shared<octree::Element>(node.pos.x, charge);
}

CoulombNodeOctree::~CoulombNodeOctree()
{
    m_co.removeCN(*this);
}

FieldPotential CoulombNodeOctree::getFP()
{
    return m_co.getFP(node.pos, this);
}

////////////////////////
// CoulombComarator
CoulombComarator::CoulombComarator(std::unique_ptr<IColoumbCalculator> c1, std::unique_ptr<IColoumbCalculator> c2) :
    m_c1(std::move(c1)), m_c2(std::move(c2))
{

}

FieldPotential CoulombComarator::getFP(Vector<3> pos, CoulombNodeBase* exclude)
{
    return m_c1->getFP(pos, static_cast<CoulombComaratorNode*>(exclude)->m_n1.get());
}

void CoulombComarator::rebuildOptimization()
{
    m_c1->rebuildOptimization();
    m_c2->rebuildOptimization();
}

void CoulombComarator::addCN(CoulombNodeBase& cn)
{
    m_c1->addCN( *(static_cast<CoulombComaratorNode&>(cn).m_n1) );
    m_c2->addCN( *(static_cast<CoulombComaratorNode&>(cn).m_n2) );
}

void CoulombComarator::removeCN(CoulombNodeBase& cn)
{
    m_c1->removeCN( *(static_cast<CoulombComaratorNode&>(cn).m_n1) );
    m_c2->removeCN( *(static_cast<CoulombComaratorNode&>(cn).m_n2) );
}

CoulombNodeBase* CoulombComarator::makeNode(double& charge, Node& thisNode)
{
    return new CoulombComaratorNode(
        *this, thisNode,
        std::unique_ptr<CoulombNodeBase>(m_c1->makeNode(charge, thisNode)),
        std::unique_ptr<CoulombNodeBase>(m_c2->makeNode(charge, thisNode))
    );
}

////////////////////////
// CoulombComaratorNode

CoulombComaratorNode::CoulombComaratorNode(IColoumbCalculator &co, Node& thisNode, std::unique_ptr<CoulombNodeBase> n1, std::unique_ptr<CoulombNodeBase> n2) :
    CoulombNodeBase(n1->charge),
    m_node(thisNode),
    m_co(co),
    m_n1(std::move(n1)), m_n2(std::move(n2))
{
}

FieldPotential CoulombComaratorNode::getFP()
{
    return m_co.getFP(m_node.pos, this);
}
