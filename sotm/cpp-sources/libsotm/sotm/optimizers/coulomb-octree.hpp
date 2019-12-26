#ifndef COLOUMB_OCTREE_HPP
#define COLOUMB_OCTREE_HPP

#include "sotm/optimizers/coulomb.hpp"
#include "octree.hpp"

namespace sotm {

class CoulombNodeOctree;

class OctreeElementForColulomb : public octree::Element
{
public:
    OctreeElementForColulomb(const Position& p, double& value, CoulombNodeOctree& cn) :
        octree::Element(p, value), coulombNode(cn)
    { }

    CoulombNodeOctree& coulombNode;
};


class CoulombOctree: public IColoumbCalculator
{
public:
    CoulombOctree(GraphRegister& graph, std::unique_ptr<const octree::IScalesConfig> scales);
    FieldPotential getFP(Vector<3> pos, CoulombNodeBase* exclude = nullptr) override;
    CoulombNodeBase* makeNode(double& charge, Node& thisNode) override;
    void getClose(std::vector<CoulombNodeBase*>& container, const Vector<3>& pos, double distance) override;

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
public:
    CoulombNodeOctree(IColoumbCalculator& co, double& charge, Node& thisNode);
    ~CoulombNodeOctree();

    FieldPotential getFP() override;

    std::shared_ptr<octree::Element> m_ectreeElement;

private:
    IColoumbCalculator &m_co;
    double m_isolatedPotential = 0;
    Vector<3> m_isolatedField{0.0, 0.0, 0.0};
};


}

#endif // COLOUMB_OCTREE_HPP
