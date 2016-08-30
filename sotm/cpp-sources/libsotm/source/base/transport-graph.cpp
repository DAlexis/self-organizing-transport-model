#include "sotm/base/transport-graph.hpp"
#include "sotm/base/model-context.hpp"
#include "sotm/base/physical-payload.hpp"

using namespace sotm;

void GraphRegister::addLink(Link* link)
{
	ASSERT(m_links.find(link) == m_links.end(), "Link already added to graph register");
	m_links.insert(link);
}

void GraphRegister::addNode(Node* node)
{
	ASSERT(m_nodes.find(node) == m_nodes.end(), "Link already added to graph register");
	m_nodes.insert(node);
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

Node::Node(ModelContext* context) :
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
	payload.reset(
		m_context->createLinkPayload(this)
	);
	m_context->graphRegister.addLink(this);
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


