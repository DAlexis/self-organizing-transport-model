#include "sotm/base/model-context.hpp"

using namespace sotm;

void ModelContext::setNodePayloadFactory(std::unique_ptr<INodePayloadFactory> factory)
{
	m_nodePayloadFactory = std::move(factory);
}

void ModelContext::setLinkPayloadFactory(std::unique_ptr<ILinkPayloadFactory> factory)
{
	m_linkPayloadFactory = std::move(factory);
}

void ModelContext::setPhysicalContext(std::unique_ptr<IPhysicalContext> physicalContext)
{
	m_physicalContext = std::move(physicalContext);
}

NodePayloadBase* ModelContext::createNodePayload(Node* node)
{
	ASSERT(m_nodePayloadFactory != nullptr, "Node physical payload factory is not set!");
	return m_nodePayloadFactory->create(&payloadsRegister, node);
}

LinkPayloadBase* ModelContext::createLinkPayload(Link* link)
{
	ASSERT(m_linkPayloadFactory != nullptr, "Link physical payload factory is not set!");
	return m_linkPayloadFactory->create(&payloadsRegister, link);
}

void ModelContext::calculateSecondaryValues()
{
	m_physicalContext->calculateSecondaryValues();
	payloadsRegister.calculateSecondaryValues();
}

void ModelContext::calculateRHS(double time)
{
	m_physicalContext->calculateRHS(time);
	payloadsRegister.calculateRHS(time);
}

void ModelContext::addRHSToDelta(double m)
{
	m_physicalContext->addRHSToDelta(m);
	payloadsRegister.addRHSToDelta(m);
}

void ModelContext::makeSubIteration(double dt)
{
	m_physicalContext->makeSubIteration(dt);
	payloadsRegister.makeSubIteration(dt);
}

void ModelContext::step()
{
	m_physicalContext->step();
	payloadsRegister.step();
}

void ModelContext::doBifurcation(double time, double dt)
{
	ASSERT(dt >= 0, "Cannot iterate bifurcations when dt < 0");

	m_physicalContext->doBifurcation(time, dt);

	graphRegister.applyLinkVisitor(
		[time, dt](Link* l)
		{
			l->payload->doBifurcation(time, dt);
		}
	);

	graphRegister.applyNodeVisitor(
		[time, dt](Node* n)
		{
			n->payload->doBifurcation(time, dt);
		}
	);

	graphRegister.applyNodeVisitor(
		[this, time, dt](Node* n)
		{
			branchIteration(time, dt, n);
		}
	);
}

void ModelContext::branchIteration(double time, double dt, Node* node)
{
	BranchingParameters bp;
	node->payload->getBranchingParameters(time, dt, bp);
	if (!bp.needBranching)
		return;

	// Its time to create new node with link
	Point<3> newPos = node->pos + bp.direction * bp.length;
	PtrWrap<Link> newLink = PtrWrap<Link>::make(this, node, newPos);
}
