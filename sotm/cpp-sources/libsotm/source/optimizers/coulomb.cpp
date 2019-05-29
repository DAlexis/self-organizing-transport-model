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
