/*
 * branching-trivial-physics.hpp
 *
 *  Created on: 9 сент. 2016 г.
 *      Author: dalexies
 */

#ifndef UNIT_TESTS_LIBSOTM_UT_COMPLEX_BRANCHING_TRIVIAL_PHYSICS_HPP_
#define UNIT_TESTS_LIBSOTM_UT_COMPLEX_BRANCHING_TRIVIAL_PHYSICS_HPP_

#include "sotm/payloads/demo/empty-payloads.hpp"
#include "sotm/base/model-context.hpp"

namespace sotm
{

class EmptyPhysicalContextWithBranching : public EmptyPhysicalContext
{
public:
	bool doBranching = false;
	static inline EmptyPhysicalContextWithBranching* cast(IPhysicalContext* context)
	{
		return static_cast<EmptyPhysicalContextWithBranching*>(context);
	}

	static inline const EmptyPhysicalContextWithBranching* cast(const IPhysicalContext* context)
	{
		return static_cast<const EmptyPhysicalContextWithBranching*>(context);
	}
};

class EmptyNodePayloadWithBranching : public EmptyNodePayload
{
public:
	EmptyNodePayloadWithBranching(PhysicalPayloadsRegister* reg, Node* node) :
		EmptyNodePayload(reg, node)
	{ count++; }
	~EmptyNodePayloadWithBranching() { count--; }

	void getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters) override
	{
		if (EmptyPhysicalContextWithBranching::cast(node->physicalContext())->doBranching)
		{
			branchingParameters.needBranching = true;
			branchingParameters.direction = StaticVector<3>({1.0, 1.0, 1.0});
			branchingParameters.length = 1.0;
		} else {
			branchingParameters.needBranching = false;
		}
	}

	static int count;
};

class EmptyLinkPayloadWithBranching : public EmptyLinkPayload
{
public:
	EmptyLinkPayloadWithBranching(PhysicalPayloadsRegister* reg, Link* link) :
		EmptyLinkPayload(reg, link)
	{ count++; }
	~EmptyLinkPayloadWithBranching() { count--; }

	static int count;
};

SOTM_QUICK_NPF(EmptyNodePayloadWithBranching, EmptyNodePayloadWithBranchingFactory);
SOTM_QUICK_LPF(EmptyLinkPayloadWithBranching, EmptyLinkPayloadWithBranchingFactory);


} // namespace sotm

#endif /* UNIT_TESTS_LIBSOTM_UT_COMPLEX_BRANCHING_TRIVIAL_PHYSICS_HPP_ */
