#include "sotm/optimizers/coulomb.hpp"
#include "sotm/utils/const.hpp"

#include <iostream>
#include <sstream>
#include <cmath>

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

FieldPotential FieldPotential::convolutionVisitor(const Position& target, const Position& object, double mass)
{
    double dist = target.distTo(object);
    if (dist == 0.0)
        return FieldPotential();
    double p = Const::Si::k * mass / dist;
    double tmp = p / (dist * dist);

    FieldPotential fp;
    const double *p1 = target.x;
    const double *p2 = object.x;
    double *pres = fp.field.x;
    fp.potential = p;
    for (int i=0; i<3; i++)
        pres[i] = tmp * (p1[i]-p2[i]);

    return fp;
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

void CoulombBruteForce::getClose(std::vector<CoulombNodeBase*>& container, const Vector<3>& pos, double distance)
{
    for (auto it: m_nodesNotIsolatedVector)
    {
        double d = (pos - it->node.pos).len();
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

////////////////////////
// CoulombOctree

CoulombOctree::CoulombOctree(GraphRegister& graph, std::unique_ptr<const octree::IScalesConfig> scales) :
    m_graph(graph),
    m_scales(std::move(scales))
{

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

    octreeResult  = m_convolution.convolute(m_octreePositive, pos.x, FieldPotential::convolutionVisitor);
    octreeResult += m_convolution.convolute(m_octreeNegative, pos.x, FieldPotential::convolutionVisitor);

    return octreeResult;
}

CoulombNodeBase* CoulombOctree::makeNode(double& charge, Node& thisNode)
{
    return new CoulombNodeOctree(*this, charge, thisNode);
}

void CoulombOctree::getClose(std::vector<CoulombNodeBase*>& container, const Vector<3>& pos, double distace)
{
    std::vector<octree::Element*> octreeElements;
    m_octreeNegative.getClose(octreeElements, pos.x, distace);
    container.reserve(octreeElements.size());
    for (auto it: octreeElements)
    {
        OctreeElementForColulomb* e = static_cast<OctreeElementForColulomb*>(it);
        container.push_back(&e->coulombNode);
    }
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

////////////////////////
// CoulombNodeOctree

CoulombNodeOctree::CoulombNodeOctree(IColoumbCalculator& co, double &charge, Node &thisNode) :
    CoulombNodeBase(charge, thisNode),
    m_co(co)
{
    m_co.addCN(*this);
    m_ectreeElement = std::make_shared<OctreeElementForColulomb>(node.pos.x, charge, *this);
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
    FieldPotential r1 = m_c1->getFP(pos, static_cast<CoulombComaratorNode*>(exclude)->m_n1.get());
    FieldPotential r2 = m_c2->getFP(pos, static_cast<CoulombComaratorNode*>(exclude)->m_n2.get());
    std::cout << diffStr(r1, r2) << std::endl;
    return r1;
}

void CoulombComarator::getClose(std::vector<CoulombNodeBase*>& container, const Vector<3>& pos, double distance)
{
    m_c1->getClose(container, pos, distance);
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

std::string CoulombComarator::diffStr(const FieldPotential& r1, const FieldPotential& r2)
{
    std::ostringstream oss;
    m_counter++;

    if (std::isnan(r2.potential))
        oss << header() << "[ERROR] r2.potential == nan" << std::endl;

    for (int i=0; i<3; i++)
        if (std::isnan(r2.field[i]))
            oss << header() << "[ERROR] r2.field[" << i << "]== nan" << std::endl;

    oss << header() << " eps(potential) = " << err(r1.potential, r2.potential) << std::endl;

    for (int i=0; i<3; i++)
    {
        oss << header() << " eps(E[" << i << "]) = " << err(r1.field[i], r2.field[i]) << std::endl;
    }

    oss << header() << " max error = " << m_maxDiff;
    return oss.str();
}

double CoulombComarator::err(double v1, double v2)
{
    double err = 0.0;
    v1 = fabs(v1); v2 = fabs(v2);
    if (v1 != 0.0 || v2 != 0.0)
        err = fabs(v1-v2) / std::max(v1, v2);
    if (err > m_maxDiff)
        m_maxDiff = err;
    return err;
}

std::string CoulombComarator::header()
{
    return std::string("[Compare ") + std::to_string(m_counter) + "] ";
}

////////////////////////
// CoulombComaratorNode

CoulombComaratorNode::CoulombComaratorNode(IColoumbCalculator &co, Node& thisNode, std::unique_ptr<CoulombNodeBase> n1, std::unique_ptr<CoulombNodeBase> n2) :
    CoulombNodeBase(n1->charge, thisNode),
    m_co(co),
    m_n1(std::move(n1)), m_n2(std::move(n2))
{
}

FieldPotential CoulombComaratorNode::getFP()
{
    return m_co.getFP(node.pos, this);
}