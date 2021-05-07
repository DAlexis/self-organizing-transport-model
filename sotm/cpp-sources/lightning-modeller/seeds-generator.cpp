#include "seeds-generator.hpp"

#include "sotm/payloads/electrostatics/electrostatics.hpp"
#include "sotm/math/random.hpp"

#include <iostream>

using namespace sotm;

SeedsGenerator::AddSeedsHook::AddSeedsHook(SeedsGenerator& sg, double period) :
    m_sg(sg)
{
    setPeriod(period);
}

void SeedsGenerator::AddSeedsHook::hook(double realTime, double wantedTime)
{
    if (m_sg.m_dynamicSeedsCount + m_sg.seedsCount <= m_sg.m_currentCount)
        return;

    size_t needCountTillNow = realTime / m_sg.m_addSeedPeriod + m_sg.seedsCount;
    size_t toAdd = needCountTillNow - m_sg.m_currentCount;

    //std::cout << "Need to add " << toAdd << " seeds" << std::endl;
    for (size_t i=0; i<toAdd; i++)
    {
        //std::cout << "Adding seed" << std::endl;
        m_sg.addSeed();
    }
}

SeedsGenerator::SeedsGenerator(cic::ParametersGroup& pg, sotm::ModelContext& c) :
    m_pg(pg), m_c(c)
{

}

void SeedsGenerator::generateInitial()
{
    if (seedsCount == 1)
    {
        PtrWrap<Node> n1 = PtrWrap<Node>::make(&m_c, StaticVector<3>(0, 0, -seedSize/2.0));
        PtrWrap<Node> n2 = PtrWrap<Node>::make(&m_c, StaticVector<3>(0, 0, +seedSize/2.0));
        setNodeParameters(n1);
        setNodeParameters(n2);
        PtrWrap<Link> l = PtrWrap<Link>::make(&m_c);
        setLinkParameters(l);

        l->connect(n1, n2);
    } else {
        for (unsigned int i=0; i<seedsCount; i++)
        {
            addSeed();
        }
    }
}

SeedsGenerator::AddSeedsHook& SeedsGenerator::hook()
{
    if (m_addSeedPeriod == 0.0)
        throw std::runtime_error("Cannot add seeds with period = 0.0");

    if (m_addSeedsHook == nullptr)
        m_addSeedsHook = new AddSeedsHook(*this, m_addSeedPeriod);

    return *m_addSeedsHook;
}

void SeedsGenerator::parseConfig()
{
    seedsCount = m_pg.get<unsigned int>("seeds-number");
    dia = m_pg.get<double>("seeds-zone-dia");
    height = m_pg.get<double>("seeds-zone-height");
    minDist = m_pg.get<double>("seeds-min-dist");
    uniform = m_pg.get<bool>("seeds-z-uniform");
    seedSize = m_pg.get<double>("seed-size");
    m_dynamicSeedsCount = m_pg.get<size_t>("seeds-dynamic-count");
    m_addSeedPeriod = 1 / (m_pg.get<double>("seeds-dynamic-count-per-vol-per-sec") * dia * dia * height);
}

void SeedsGenerator::setNodeParameters(Node* n)
{
    double condRadius = m_pg.get<double>("seed-radius-cond");
    double branchRadius = m_pg.get<double>("seed-radius-branch");

    ElectrostaticNodePayload* p = static_cast<ElectrostaticNodePayload*>(n->payload.get());

    p->nodeRadiusBranching = branchRadius;
    p->nodeRadiusConductivity = condRadius;
}

void SeedsGenerator::setLinkParameters(Link* l)
{
    double beta = m_pg.get<double>("seed-beta");
    double fieldCritical = m_pg.get<double>("seed-field-cond-critical");

    ElectrostaticLinkPayload* p = static_cast<ElectrostaticLinkPayload*>(l->payload.get());
    p->linkBeta = beta;
    p->linkEta = ElectrostaticNodePayload::etaFromCriticalField(fieldCritical, beta);
}

void SeedsGenerator::addSeed()
{
    double d=0.0;
    StaticVector<3> p;
    do
    {
        p[0] = Random::uniform(-dia, dia);
        p[1] = Random::uniform(-dia, dia);
        if (uniform)
            p[2] = -height + 2*height / (seedsCount-1) * m_currentCount;
        else
            p[2] = Random::uniform(-height, height);

        auto nearest = m_c.graphRegister.getNearestNode(p);
        if (nearest == nullptr)
            break;
        d = (nearest->pos-p).norm();
    } while (d < minDist);

    /// Building initial tree
    PtrWrap<Node> n1 = PtrWrap<Node>::make(&m_c, StaticVector<3>(p[0], p[1], p[2]-seedSize/2.0));
    PtrWrap<Node> n2 = PtrWrap<Node>::make(&m_c, StaticVector<3>(p[0], p[1], p[2]+seedSize/2.0));
    PtrWrap<Link> l = PtrWrap<Link>::make(&m_c);
    l->connect(n1, n2);

    n1->payload->init();
    n2->payload->init();
    l->payload->init();

    setNodeParameters(n1);
    setNodeParameters(n2);
    setLinkParameters(l);

    m_currentCount++;
}
