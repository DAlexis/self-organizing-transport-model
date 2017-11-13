#ifndef SEEDSGENERATOR_HPP
#define SEEDSGENERATOR_HPP

#include "sotm/base/model-context.hpp"
#include "sotm/base/transport-graph.hpp"
#include "sotm/base/time-iter.hpp"
#include "cic.hpp"

class SeedsGenerator
{
public:
    class AddSeedsHook : public sotm::TimeHookPeriodic
    {
    public:
        AddSeedsHook(SeedsGenerator& sg, double period);

    private:
        void hook(double realTime, double wantedTime);
        SeedsGenerator& m_sg;
    };
    SeedsGenerator(cic::ParametersGroup& pg, sotm::ModelContext& c);

    void parseConfig();
    void generateInitial();
    AddSeedsHook& hook();

private:

    void setNodeParameters(sotm::Node* n);
    void setLinkParameters(sotm::Link* l);
    void addSeed();

    cic::ParametersGroup& m_pg;
    sotm::ModelContext& m_c;

    AddSeedsHook *m_addSeedsHook = nullptr;

    unsigned int seedsCount;
    double dia;
    double height;
    double minDist;
    bool uniform;
    double seedSize;

    double m_addSeedPeriod = 0.0;
    size_t m_dynamicSeedsCount = 0;

    size_t m_currentCount = 0;
};

#endif // SEEDSGENERATOR_HPP
