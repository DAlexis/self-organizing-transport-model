#ifndef BRANCH_POINT_HPP_INCLUDED
#define BRANCH_POINT_HPP_INCLUDED

#include "sotm/math/geometry.hpp"
#include "sotm/utils/memory.hpp"
#include "sotm/utils/macros.hpp"

#include <list>
#include <memory>
#include <set>
#include <vector>
#include <functional>

namespace sotm
{

class Node;
class Link;
class NodePayloadBase;
class LinkPayloadBase;
class ModelContext;
class IPhysicalContext;

enum class LinkDirection
{
	in = 1,
	out = -1
};

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

	/// Iterate by all nodes. New nodes should not be added during iteration
	void applyNodeVisitorWithoutGraphChganges(NodeVisitor v, bool parallel = false);

	/// Iterate by all links. New nodes should not be added during iteration
	void applyLinkVisitorWithoutGraphChganges(LinkVisitor v);

	Node* getNearestNode(const Vector<3>& point);

	size_t nodesCount();
	size_t linksCount();

	size_t stateHash();

private:
	using NodeSet = std::set<Node*>;
	using LinkSet = std::set<Link*>;

	/// Make iterating over links on nodes safe for add/remove link/node operations
	void beginIterating();

	/// Apply cached add/remove operations with links/nodes
	void endIterating();

	void changeStateHash();

	/// If true we cannot add/remove directly to/from m_nodes and m_links because we are iterating by them
	bool m_iteratingNow = false;

	NodeSet m_nodes, m_nodesToAdd, m_nodesToDelete;
	LinkSet m_links, m_linksToAdd, m_linksToDelete;

	size_t m_stateHash = 0;
};

class ModelContextDependent
{
public:
	ModelContextDependent(ModelContext* context);
	ModelContext* context();
	IPhysicalContext* physicalContext();


protected:
	ModelContext* m_context;
};

class Node : public ModelContextDependent, public SelfMemMgr
{
public:
	using LinkVisitor = std::function<void(Link*, LinkDirection)>;

	Node(ModelContext* context, Vector<3> pos = Vector<3>());
	~Node();
	void addLink(Link* link);
	void removeLink(Link* link);
	void applyConnectedLinksVisitor(LinkVisitor visitor);
	bool hasNeighbour(Node* node);

	std::unique_ptr<NodePayloadBase> payload;

	Vector<3> pos;

private:
	std::set<Link*> m_links;
};


class Link : public ModelContextDependent, public SelfMemMgr
{
public:
	Link(ModelContext* context);
	Link(ModelContext* context, Node* nodeFrom, Vector<3> pointTo);
	~Link();
	void connect(Node* n1, Node* n2);

	PtrWrap<Node> getNode1();
	PtrWrap<Node> getNode2();
	PtrWrap<Node> getNode(unsigned int index);
	double length();
	double lengthCached();

	std::unique_ptr<LinkPayloadBase> payload;
private:
	PtrWrap<Node> m_n1, m_n2;
	double m_lengthCached = -1.0;
};

}

#endif // BRANCH_POINT_HPP_INCLUDED
