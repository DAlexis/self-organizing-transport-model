#include "sotm/base/model-context.hpp"
#include <tbb/tbb.h>

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
	m_physicalContext->connectModel(this);
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

void ModelContext::clearSubiteration()
{
	m_physicalContext->clearSubiteration();
	payloadsRegister.clearSubiteration();
}

void ModelContext::calculateSecondaryValues(double time)
{
	m_physicalContext->calculateSecondaryValues(time);
	payloadsRegister.calculateSecondaryValues(time);
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

double ModelContext::getMinimalStepsCount()
{
	return std::min(payloadsRegister.getMinimalStepsCount(), m_physicalContext->getMinimalStepsCount());
}

void ModelContext::prepareBifurcation(double time, double dt)
{
	ASSERT(dt >= 0, "Cannot prepare bifurcations when dt < 0");

	m_physicalContext->prepareBifurcation(time, dt);

	if (parallelSettings.parallelBifurcationIteration.prepareBifurcation)
	{
		rebuildBufurcatableVectorIfNeeded();
		tbb::parallel_for( size_t(0), m_bifurcatableObjects.size(),
			[this, time, dt]( size_t i ) {
				m_bifurcatableObjects[i]->prepareBifurcation(time, dt);
			}
		);
	} else {
		graphRegister.applyLinkVisitor(
			[time, dt](Link* l)
			{
				l->payload->prepareBifurcation(time, dt);
			}
		);

		graphRegister.applyNodeVisitor(
			[time, dt](Node* n)
			{
				n->payload->prepareBifurcation(time, dt);
			}
		);
	}
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

void ModelContext::initAllPhysicalPayloads()
{
	graphRegister.applyNodeVisitor(
		[](Node* n)
		{
			n->payload->init();
		}
	);

	graphRegister.applyLinkVisitor(
		[](Link* l)
		{
			l->payload->init();
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
	Vector<3> newPos = node->pos + bp.direction * bp.length;
	PtrWrap<Link> newLink = PtrWrap<Link>::make(this, node, newPos);
	newLink->getNode2()->payload->init();
	newLink->payload->init();

}

void ModelContext::rebuildBufurcatableVectorIfNeeded()
{
	if (m_lastStateHash != graphRegister.stateHash())
	{
		m_bifurcatableObjects.clear();
		graphRegister.applyNodeVisitorWithoutGraphChganges(
			[this](Node* n)
			{
				m_bifurcatableObjects.push_back(n->payload.get());
            }, false
		);

		graphRegister.applyLinkVisitorWithoutGraphChganges(
			[this](Link* l)
			{
				m_bifurcatableObjects.push_back(l->payload.get());
            }, false
		);
		m_lastStateHash = graphRegister.stateHash();
	}
}
