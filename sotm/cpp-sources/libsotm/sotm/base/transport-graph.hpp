#ifndef BRANCH_POINT_HPP_INCLUDED
#define BRANCH_POINT_HPP_INCLUDED

#include "sotm/math/geometry.hpp"
#include "sotm/utils/memory.hpp"
#include "sotm/utils/macros.hpp"
#include <list>
#include <memory>
#include <set>

namespace sotm
{

class Node;
class Link;
class NodePayloadBase;
class LinkPayloadBase;
class ModelContext;

class GraphRegister
{
public:
	void addLink(Link* link);
	void addNode(Node* link);

	void rmLink(Link* link);
	void rmNode(Node* link);

private:
	std::set<Node*> m_nodes;
	std::set<Link*> m_links;
};

class Node : public SelfMemMgr
{
public:
	Node(ModelContext* context);
	~Node();
	void addLink(Link* link);
	void removeLink(Link* link);

	std::unique_ptr<NodePayloadBase> payload;

private:
	std::set<Link*> m_links;
	ModelContext* m_context;
};


class Link : public SelfMemMgr
{
public:
	Link(ModelContext* context);
	~Link();
	void connect(Node* n1, Node* n2);

	std::unique_ptr<LinkPayloadBase> payload;

private:
	PtrWrap<Node> m_n1, m_n2;
	ModelContext* m_context;
};

}

#endif // BRANCH_POINT_HPP_INCLUDED
