#ifndef BRANCH_POINT_HPP_INCLUDED
#define BRANCH_POINT_HPP_INCLUDED

#include "sotm/math/geometry.hpp"
#include "sotm/utils/memory.hpp"
#include "sotm/utils/macros.hpp"
#include <list>
#include <memory>
#include <set>
#include <functional>

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

	using NodeVisitor = std::function<void(Node*)>;
	using LinkVisitor = std::function<void(Link*)>;

	void addLink(Link* link);
	void addNode(Node* link);

	void rmLink(Link* link);
	void rmNode(Node* link);

	/// Safely iterate by all nodes. New nodes may be added during iteration
	void applyNodeVisitor(NodeVisitor v);

	/// Safely iterate by all links. New links may be added during iteration
	void applyLinkVisitor(LinkVisitor v);

private:
	using NodeSet = std::set<Node*>;
	using LinkSet = std::set<Link*>;

	/// Switch addLink and addNode to temporary sets when iteration over main set is produced
	void switchToTempContainers();

	/// Switch addLink and addNode to main sets after iteration over main set done
	/// AND move containment of temporary sets to main
	void switchToMainContainers();
	NodeSet m_nodes, m_nodesTemp, *m_nodesToPut = &m_nodes;
	LinkSet m_links, m_linksTemp, *m_linksToPut = &m_links;

};

class Node : public SelfMemMgr
{
public:
	Node(ModelContext* context, Point<3> pos = Point<3>());
	~Node();
	void addLink(Link* link);
	void removeLink(Link* link);

	std::unique_ptr<NodePayloadBase> payload;

	Point<3> pos;

private:
	std::set<Link*> m_links;
	ModelContext* m_context;
};


class Link : public SelfMemMgr
{
public:
	Link(ModelContext* context);
	Link(ModelContext* context, Node* nodeFrom, Point<3> pointTo);
	~Link();
	void connect(Node* n1, Node* n2);

	std::unique_ptr<LinkPayloadBase> payload;

private:
	PtrWrap<Node> m_n1, m_n2;
	ModelContext* m_context;
};

}

#endif // BRANCH_POINT_HPP_INCLUDED
