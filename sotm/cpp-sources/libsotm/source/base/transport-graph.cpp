#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/base/physical-payload.hpp"

using namespace sotm;

void GraphRegister::addLink(Link* link)
{
	ASSERT(m_links.find(link) == m_links.end() && m_linksTemp.find(link) == m_linksTemp.end(),
			"Link already added to graph register");
	m_linksToPut->insert(link);
}

void GraphRegister::addNode(Node* node)
{
	ASSERT(m_nodes.find(node) == m_nodes.end() && m_nodesTemp.find(node) == m_nodesTemp.end(),
			"Link already added to graph register");
	m_nodesToPut->insert(node);
}

void GraphRegister::rmLink(Link* link)
{
	ASSERT(m_links.find(link) != m_links.end(), "Link does not exists in graph register");
	m_links.erase(link);
}

void GraphRegister::rmNode(Node* node)
{
	ASSERT(m_nodes.find(node) != m_nodes.end(), "Link does not exists in graph register");
	m_nodes.erase(node);
}

void GraphRegister::applyNodeVisitor(NodeVisitor v)
{
	switchToTempContainers();
	for (auto it = m_nodes.begin(); it != m_nodes.end(); ++it)
	{
		v(*it);
	}
	switchToMainContainers();
}

void GraphRegister::applyLinkVisitor(LinkVisitor v)
{
	switchToTempContainers();
	for (auto it = m_links.begin(); it != m_links.end(); ++it)
	{
		v(*it);
	}
	switchToMainContainers();
}

void GraphRegister::switchToTempContainers()
{
	m_nodesToPut = &m_nodesTemp;
	m_linksToPut = &m_linksTemp;
}

void GraphRegister::switchToMainContainers()
{
	m_nodes.insert(m_nodesTemp.begin(), m_nodesTemp.end());
	m_nodesTemp.clear();
	m_links.insert(m_linksTemp.begin(), m_linksTemp.end());
	m_linksTemp.clear();
	m_nodesToPut = &m_nodes;
	m_linksToPut = &m_links;
}

////////////////////////////
// Node
Node::Node(ModelContext* context, Point<3> pos) :
	pos(pos),
	m_context(context)
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

Link::Link(ModelContext* context) :
	m_context(context)
{
	/// @todo Should we do this before connecting link to nodes?
	payload.reset(
		m_context->createLinkPayload(this)
	);
	m_context->graphRegister.addLink(this);
}

Link::Link(ModelContext* context, Node* nodeFrom, Point<3> pointTo) :
	m_context(context)
{
	ASSERT(nodeFrom != nullptr, "Cannot create link with nullptr node");
	m_context->graphRegister.addLink(this);
	PtrWrap<Node> newNode = PtrWrap<Node>::make(context, pointTo);
	payload.reset(
		m_context->createLinkPayload(this)
	);
	connect(nodeFrom, newNode);
}

Link::~Link()
{
	m_context->graphRegister.rmLink(this);
}

void Link::connect(Node* n1, Node* n2)
{
	m_n1.assign(n1);
	m_n2.assign(n2);
}


