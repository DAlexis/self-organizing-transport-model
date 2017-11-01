#include "seeds-generator.hpp"

#include "sotm/payloads/electrostatics/electrostatics.hpp"
#include "sotm/math/random.hpp"

using namespace sotm;

SeedsGenerator::SeedsGenerator(cic::ParametersGroup& pg, sotm::ModelContext& c) :
    m_pg(pg), m_c(c)
{

}

void SeedsGenerator::generate()
{
    unsigned int seedsCount = m_pg.get<unsigned int>("seeds-number");
    double dia = m_pg.get<double>("seeds-zone-dia");
    double height = m_pg.get<double>("seeds-zone-height");
    double minDist = m_pg.get<double>("seeds-min-dist");
    bool uniform = m_pg.get<bool>("seeds-z-uniform");
    double seedSize = m_pg.get<double>("seed-size");

    if (seedsCount == 1)
    {
        PtrWrap<Node> n1 = PtrWrap<Node>::make(&m_c, Vector<3>(0, 0, -seedSize/2.0));
        PtrWrap<Node> n2 = PtrWrap<Node>::make(&m_c, Vector<3>(0, 0, +seedSize/2.0));
        setNodeParameters(n1);
        setNodeParameters(n2);
        PtrWrap<Link> l = PtrWrap<Link>::make(&m_c);
        setLinkParameters(l);

        l->connect(n1, n2);
    } else {
        for (unsigned int i=0; i<seedsCount; i++)
        {
            double d=0.0;
            Vector<3> p;
            do
            {
                p[0] = Random::uniform(-dia, dia);
                p[1] = Random::uniform(-dia, dia);
                if (uniform)
                    p[2] = -height + 2*height / (seedsCount-1) * i;
                else
                    p[2] = Random::uniform(-height, height);

                auto nearest = m_c.graphRegister.getNearestNode(p);
                if (nearest == nullptr)
                    break;
                d = (nearest->pos-p).len();
            } while (d < minDist);

            /// Building initial tree
            PtrWrap<Node> n1 = PtrWrap<Node>::make(&m_c, Vector<3>(p[0], p[1], p[2]-seedSize/2.0));
            PtrWrap<Node> n2 = PtrWrap<Node>::make(&m_c, Vector<3>(p[0], p[1], p[2]+seedSize/2.0));
            setNodeParameters(n1);
            setNodeParameters(n2);

            PtrWrap<Link> l = PtrWrap<Link>::make(&m_c);
            setLinkParameters(l);

            l->connect(n1, n2);
        }
    }
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
