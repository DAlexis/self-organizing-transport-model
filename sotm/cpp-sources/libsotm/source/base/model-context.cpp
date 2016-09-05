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

void ModelContext::doBranchingIteration(double dt)
{
	ASSERT(dt >= 0, "Cannot iterate branching when dt < 0");
	graphRegister.applyNodeVisitor(
		[this, dt](Node* n)
		{
			branchIteration(dt, n);
		}
	);
}
/*
void ModelContext::doBifurcation(double dt)
{
	ASSERT(dt >= 0, "Cannot iterate when dt < 0");
	graphRegister.applyNodeVisitor(
		[dt](Node* n)
		{
			n->payload->doBifurcation(dt);
		}
	);
}*/

void ModelContext::branchIteration(double dt, Node* node)
{
	BranchingParameters bp;
	node->payload->getBranchingParameters(dt, bp);
	if (!bp.needBranching)
		return;

	// Its time to create new node with link
	Point<3> newPos = node->pos + bp.direction * bp.length;
	PtrWrap<Link> newLink = PtrWrap<Link>::make(this, node, newPos);
}
/*
void ModelContext::discreteStep(double dt, Node* node)
{
	node->payload->doBifurcation(dt);
}*/
