#include "sotm/payloads/demo/absolute-random-graph.hpp"
#include "sotm/math/random.hpp"
#include "sotm/math/generic.hpp"

#include <cmath>

using namespace sotm;

RandomGraphNodePayload::RandomGraphNodePayload(PhysicalPayloadsRegister* reg, Node* node) :
	EmptyNodePayload(reg, node)
{
}

void RandomGraphNodePayload::getBranchingParameters(double time, double dt, BranchingParameters& branchingParameters)
{
	double branchProb = RandomGraphPhysicalContext::cast(node->physicalContext())->branchingProbabilityPerSecond;
	branchingParameters.needBranching = false;

	if (Random::uniform(0.0, 1.0) < branchProb*dt)
	{
		branchingParameters.needBranching = true;
		/// @todo generate uniformly by angle
		Point<3> dir;

		dir.x[0] = Random::uniform(-1.0, 1.0);
		dir.x[1] = Random::uniform(-1.0, 1.0);
		dir.x[2] = sqrt(1 - sqr(dir.x[0]) - sqr(dir.x[1]));
		branchingParameters.direction = dir;
		branchingParameters.length = 0.1 + Random::uniform(0.0, 1.0);
	} else {
		branchingParameters.needBranching = false;
		return;
	}
}

RandomGraphLinkPayload::RandomGraphLinkPayload(PhysicalPayloadsRegister* reg, Link* link) :
	EmptyLinkPayload(reg, link)
{
}

void RandomGraphLinkPayload::doBifurcation(double time, double dt)
{

	EmptyLinkPayload::doBifurcation(time, dt);
	if (!link.assigned())
		return;

	double deathProb = RandomGraphPhysicalContext::cast(link->physicalContext())->linkDeathProbabilityPerSecond;
	if (Random::uniform(0.0, 1.0) < deathProb*dt)
	{
		// We can kill this link
		onDeletePayload();
	}
}
