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

void GraphRegister::applyNodeVisitorWithoutGraphChganges(NodeVisitor v, bool parallel)
{
	/*if (parallel)
	{
		if (m_nodesVectorDirty) {
			updateNodesVector();
		}
		tbb::parallel_for( size_t(0), m_nodesVector.size(), [this, v]( size_t i ) {
			v(m_nodesVector[i]);
		} );
	} else {*/
		for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
		{
			v(*it);
		}
	//}
}

void GraphRegister::applyLinkVisitorWithoutGraphChganges(LinkVisitor v)
{
	for (auto it = m_links.begin(); it != m_links.end(); ++it)
	{
		v(*it);
	}
}

Node* GraphRegister::getNearestNode(const Vector<3>& point)
{
	Node* result = nullptr;
	double minDist = 0.0;

	auto visitor = [&point, &minDist, &result] (Node* node) {
		double d = (node->pos - point).len();
		if (d < minDist || result == nullptr)
		{
			result = node;
			minDist = d;
		}
	};

	applyNodeVisitorWithoutGraphChganges(visitor);

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

void GraphRegister::beginIterating()
{
	m_iteratingNow = true;
}

void GraphRegister::endIterating()
{
	if (!m_nodesToAdd.empty() || !m_nodesToDelete.empty())
		m_nodesVectorDirty = true;

	m_iteratingNow = false;
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

void GraphRegister::updateNodesVector()
{
	m_nodesVector.clear();
	for (auto &it : m_nodes)
		m_nodesVector.push_back(it);
	m_nodesVectorDirty = false;
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

////////////////////////////
// Node
Node::Node(ModelContext* context, Vector<3> pos) :
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
	m_links.insert(link);
}

void Node::removeLink(Link* link)
{
	size_t count = m_links.erase(link);
	ASSERT(count != 0, "Removing link that was not connected");
}

void Node::applyConnectedLinksVisitor(LinkVisitor visitor)
{
	for (auto it = m_links.begin(); it != m_links.end(); it++)
	{
		LinkDirection dir = LinkDirection::in;
		if ((*it)->getNode1().data() == this)
			dir = LinkDirection::out;
		visitor(*it, dir);
	}
}

Link::Link(ModelContext* context) :
	ModelContextDependent(context)
{
	/// @todo Should we do this before connecting link to nodes?
	payload.reset(
		m_context->createLinkPayload(this)
	);
	m_context->graphRegister.addLink(this);
}

Link::Link(ModelContext* context, Node* nodeFrom, Vector<3> pointTo) :
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

PtrWrap<Node> Link::getNode1()
{
	return m_n1;
}

PtrWrap<Node> Link::getNode2()
{
	return m_n2;
}

PtrWrap<Node> Link::getNode(unsigned int index)
{
	return index == 0 ? m_n1 : m_n2;
}

double Link::length()
{
	return (m_n1->pos - m_n2->pos).len();
}

double Link::lengthCached()
{
	if (m_lengthCached < 0.0) {
		m_lengthCached = length();
	}
	return m_lengthCached;
}

