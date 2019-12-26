#include "sotm/optimizers/coulomb-octree.hpp"
#include "sotm/utils/const.hpp"

#include <iostream>
#include <sstream>
#include <cmath>

using namespace sotm;

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
