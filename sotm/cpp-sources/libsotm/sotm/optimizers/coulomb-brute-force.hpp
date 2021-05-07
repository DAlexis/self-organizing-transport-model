#ifndef COULOMB_BRUTE_FORCE_HPP
#define COULOMB_BRUTE_FORCE_HPP

#include "sotm/optimizers/coulomb.hpp"

namespace sotm {

class CoulombBruteForce : public IColoumbCalculator
{
public:
    CoulombBruteForce(GraphRegister& graph);
    FieldPotential getFP(StaticVector<3> pos, CoulombNodeBase* exclude = nullptr) override;

    /**
     * @brief build positive and negative octree
     */
    void rebuildOptimization() override;

    CoulombNodeBase* makeNode(double& charge, Node& thisNode) override;
    void getClose(std::vector<CoulombNodeBase*>& container, const StaticVector<3>& pos, double distace) override;

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

    std::shared_ptr<octree::Element> m_ectreeElement;

private:
    IColoumbCalculator &m_co;
    double m_isolatedPotential = 0;
    StaticVector<3> m_isolatedField{0.0, 0.0, 0.0};
};

}

#endif // COULOMB_BRUTE_FORCE_HPP
