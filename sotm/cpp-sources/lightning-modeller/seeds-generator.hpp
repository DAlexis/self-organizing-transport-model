#ifndef SEEDSGENERATOR_HPP
#define SEEDSGENERATOR_HPP

#include "sotm/base/model-context.hpp"
#include "sotm/base/transport-graph.hpp"
#include "cic.hpp"


class SeedsGenerator
{
public:
    SeedsGenerator(cic::ParametersGroup& pg, sotm::ModelContext& c);

    void generate();
private:
    void setNodeParameters(sotm::Node* n);
    void setLinkParameters(sotm::Link* l);

    cic::ParametersGroup& m_pg;
    sotm::ModelContext& m_c;
};

#endif // SEEDSGENERATOR_HPP
