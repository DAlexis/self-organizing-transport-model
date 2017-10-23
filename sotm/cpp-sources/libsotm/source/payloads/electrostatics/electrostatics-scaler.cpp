#include <sotm/payloads/electrostatics/electrostatics-scaler.hpp>

#include <cmath>

using namespace sotm;

ElectrostaticScaler::ElectrostaticScaler(ElectrostaticPhysicalContext* context) :
		m_context(context)
{
}

void ElectrostaticScaler::scaleStep(double factor)
{
	m_context->branchingStep = m_context->branchingStep * factor;
    m_context->nodeRadiusBranchingDefault = m_context->nodeRadiusBranchingDefault * sqrt(factor);
}
