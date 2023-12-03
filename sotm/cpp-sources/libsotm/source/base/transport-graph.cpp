#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/base/physical-payload.hpp"
#include "sotm/utils/utils.hpp"

#include <tbb/tbb.h>

using namespace sotm;
using namespace tbb;

void GraphRegister::addLink(Link* link)
{
	ASSERT(m_links.find(link) == m_links.end() && m_linksToAdd.find(link) == m_linksToAdd.end(),
			"Link already added to graph register");
	if (m_iteratingNow)
	{
		m_linksToAdd.insert(link);
	} else {
		m_links.insert(link);
		changeStateHash();
	}
}

void GraphRegister::addNode(Node* node)
{
	ASSERT(m_nodes.find(node) == m_nodes.end() && m_nodesToAdd.find(node) == m_nodesToAdd.end(),
			"Node already added to graph register");
	if (m_iteratingNow)
	{
		m_nodesToAdd.insert(node);
	} else {
		m_nodes.insert(node);
		changeStateHash();
	}
}

void GraphRegister::rmLink(Link* link)
{
	ASSERT(m_links.find(link) != m_links.end(), "Link does not exists in graph register");
	if (m_iteratingNow)
	{
		m_linksToDelete.insert(link);
	} else {
		m_links.erase(link);
		changeStateHash();
	}
}

void GraphRegister::rmNode(Node* node)
{
	ASSERT(m_nodes.find(node) != m_nodes.end(), "Node does not exists in graph register");
	if (m_iteratingNow)
	{
		m_nodesToDelete.insert(node);
	} else {
		m_nodes.erase(node);
		changeStateHash();
	}
}

void GraphRegister::applyNodeVisitor(NodeVisitor v)
{
	beginIterating();
	RunOnceOnExit end([this](){ endIterating(); });
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		v(*it);
	}
}

void GraphRegister::applyLinkVisitor(LinkVisitor v)
{
	beginIterating();
	RunOnceOnExit end([this](){ endIterating(); });
	for (auto it = m_links.begin(); it != m_links.end(); ++it)
	{
		v(*it);
	}
}

void GraphRegister::applyNodeVisitorWithoutGraphChganges(NodeVisitor v, bool optimizeToVector)
{
    if (optimizeToVector)
    {
        buildVectorsForIteration();
        for (auto& n : m_nodesVector)
        {
            v(n);
        }
    } else {
        for (auto& n : m_nodes)
        {
            v(n);
        }
    }
}

void GraphRegister::applyLinkVisitorWithoutGraphChganges(LinkVisitor v, bool optimizeToVector)
{
    if (optimizeToVector)
    {
        buildVectorsForIteration();
        for (auto& n : m_linksVector)
        {
            v(n);
        }
    } else {
        for (auto& n : m_links)
        {
            v(n);
        }
    }
}

Node* GraphRegister::getNearestNode(const StaticVector<3>& point, bool searchOverReceintlyAdded)
{
	Node* result = nullptr;
	double minDist = 0.0;

	auto visitor = [&point, &minDist, &result] (Node* node) {
        double d = (node->pos - point).norm();
		if (d < minDist || result == nullptr)
		{
			result = node;
			minDist = d;
		}
	};

    applyNodeVisitorWithoutGraphChganges(visitor, false);
	if (searchOverReceintlyAdded && m_iteratingNow)
	{
		for (auto &it : m_nodesToAdd)
			visitor(it);
	}

	return result;
}

size_t GraphRegister::nodesCount()
{
	return m_nodes.size();
}

size_t GraphRegister::linksCount()
{
	return m_links.size();
}

size_t GraphRegister::stateHash()
{
	return m_stateHash;
}

void GraphRegister::beginIterating()
{
	m_iteratingNow = true;
}

void GraphRegister::endIterating()
{
	m_iteratingNow = false;

	if (!m_nodesToAdd.empty()
		|| !m_nodesToDelete.empty()
		|| !m_linksToAdd.empty()
		|| !m_linksToDelete.empty())
	{
		changeStateHash();
    } else {
		return;
    }

	m_nodes.insert(m_nodesToAdd.begin(), m_nodesToAdd.end());

	m_nodesToAdd.clear();
	m_links.insert(m_linksToAdd.begin(), m_linksToAdd.end());
	m_linksToAdd.clear();
	for (auto it = m_nodesToDelete.begin(); it != m_nodesToDelete.end(); ++it)
	{
		m_nodes.erase(*it);
	}
	m_nodesToDelete.clear();
	for (auto it = m_linksToDelete.begin(); it != m_linksToDelete.end(); ++it)
	{
		m_links.erase(*it);
	}
	m_linksToDelete.clear();
}

void GraphRegister::changeStateHash()
{
	m_stateHash++;
}

void GraphRegister::buildVectorsForIteration()
{
    if (m_nodesLinksVectorsStateHash != m_stateHash)
    {
        std::unique_lock<std::mutex> m_lock(m_nlvecsMutex);
        // We should double check this condition because two threads may pass first check,
        // but this check is protected by mutex
        if (m_nodesLinksVectorsStateHash == m_stateHash)
            return;
        m_nodesVector = NodeVector(m_nodes.begin(), m_nodes.end());
        m_linksVector = LinkVector(m_linksVector.begin(), m_linksVector.end());
        m_nodesLinksVectorsStateHash = m_stateHash;
    }
}

////////////////////////////
// ModelContextDependent
ModelContextDependent::ModelContextDependent(ModelContext* context) :
	m_context(context)
{ }

ModelContext* ModelContextDependent::context()
{
	return m_context;
}

IPhysicalContext* ModelContextDependent::physicalContext()
{
	return m_context->physicalContext();
}

const IPhysicalContext* ModelContextDependent::physicalContext() const
{
    return m_context->physicalContext();
}


////////////////////////////
// Node
Node::Node(ModelContext* context, StaticVector<3> pos) :
	ModelContextDependent(context),
	pos(pos)
{
	payload.reset(
		m_context->createNodePayload(this)
	);
	m_context->graphRegister.addNode(this);
}

Node::~Node()
{
	m_context->graphRegister.rmNode(this);
}

void Node::addLink(Link* link)
{
    bool wasEmpty = m_links.empty();
	m_links.insert(link);
    if (m_hook && wasEmpty)
        m_hook(this, false);
}

void Node::removeLink(Link* link)
{
	size_t count = m_links.erase(link);
	ASSERT(count != 0, "Removing link that was not connected");
    if (m_hook && m_links.empty())
        m_hook(this, true);
}

void Node::setIsolatedUpdateHook(IsolatedUpdateHook hook)
{
    m_hook = hook;
}

void Node::applyConnectedLinksVisitor(LinkVisitor visitor)
{
	for (auto it = m_links.begin(); it != m_links.end(); it++)
	{
		LinkDirection dir = LinkDirection::in;
		if ((*it)->getNode1() == this)
			dir = LinkDirection::out;
		visitor(*it, dir);
	}
}

bool Node::hasNeighbour(const Node* node) const
{
	for (auto it = m_links.begin(); it != m_links.end(); it++)
	{
		if ((*it)->getNode1() == node || (*it)->getNode2() == node)
			return true;
	}
	return false;
}

Link::Link(ModelContext* context, Node* n1, Node* n2) :
	ModelContextDependent(context)
{
    connect(n1, n2);
	payload.reset(
		m_context->createLinkPayload(this)
	);
	m_context->graphRegister.addLink(this);
}

Link::Link(ModelContext* context, Node* nodeFrom, StaticVector<3> pointTo) :
	ModelContextDependent(context)
{
	ASSERT(nodeFrom != nullptr, "Cannot create link with nullptr node");
	m_context->graphRegister.addLink(this);
	PtrWrap<Node> newNode = PtrWrap<Node>::make(context, pointTo);
	connect(nodeFrom, newNode);
	payload.reset(
		m_context->createLinkPayload(this)
	);

}

Link::~Link()
{
	if (m_n1.assigned())
		m_n1->removeLink(this);
	if (m_n2.assigned())
		m_n2->removeLink(this);
	m_context->graphRegister.rmLink(this);
}

void Link::connect(Node* n1, Node* n2)
{
	m_n1.assign(n1);
	m_n2.assign(n2);
	n1->addLink(this);
	n2->addLink(this);
}

Node* Link::getNode1()
{
	return m_n1.data();
}

Node* Link::getNode2()
{
	return m_n2.data();
}

Node* Link::getNode(unsigned int index)
{
	return index == 0 ? m_n1.data() : m_n2.data();
}

double Link::length()
{
    return (m_n1->pos - m_n2->pos).norm();
}

double Link::lengthCached()
{
	if (m_lengthCached < 0.0) {
		m_lengthCached = length();
	}
	return m_lengthCached;
}

