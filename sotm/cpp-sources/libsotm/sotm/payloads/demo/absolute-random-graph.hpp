/*
 * absolute-random-graph.hpp
 *
 *  Created on: 26 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef LIBSOTM_SOTM_PAYLOADS_DEMO_ABSOLUTE_RANDOM_GRAPH_HPP_
#define LIBSOTM_SOTM_PAYLOADS_DEMO_ABSOLUTE_RANDOM_GRAPH_HPP_

#include "sotm/payloads/demo/empty-payloads.hpp"
#include "sotm/base/model-context.hpp"

namespace sotm
{

class RandomGraphPhysicalContext : public EmptyPhysicalContext
{
public:
	double branchingProbabilityPerSecond = 0.2;
	double linkDeathProbabilityPerSecond = 0.1;

	SOTM_QUICK_PHYS_CONT_CASTS(RandomGraphPhysicalContext)
};

class RandomGraphNodePayload : public EmptyNodePayload
{
public:
	RandomGraphNodePayload(PhysicalPayloadsRegister* reg, Node* node);

	void getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters) override;
};

class RandomGraphLinkPayload : public EmptyLinkPayload
{
public:
	RandomGraphLinkPayload(PhysicalPayloadsRegister* reg, Link* link);

	void doBifurcation(double time, double dt) override;
};

SOTM_QUICK_NPF(RandomGraphNodePayload, RandomGraphNodePayloadFactory);
SOTM_QUICK_LPF(RandomGraphLinkPayload, RandomGraphLinkPayloadFactory);

} // namespace sotm

#endif /* LIBSOTM_SOTM_PAYLOADS_DEMO_ABSOLUTE_RANDOM_GRAPH_HPP_ */
