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
#include <mutex>

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
    void applyNodeVisitorWithoutGraphChganges(NodeVisitor v, bool optimizeToVector = true);

	/// Iterate by all links. New nodes should not be added during iteration
    void applyLinkVisitorWithoutGraphChganges(LinkVisitor v, bool optimizeToVector = true);

	Node* getNearestNode(const StaticVector<3>& point, bool searchOverReceintlyAdded = true);

	size_t nodesCount();
	size_t linksCount();

	size_t stateHash();

private:
    using NodeSet = std::set<Node*>;
    using LinkSet = std::set<Link*>;

    using NodeVector = std::vector<Node*>;
    using LinkVector = std::vector<Link*>;

	/// Make iterating over links on nodes safe for add/remove link/node operations
	void beginIterating();

	/// Apply cached add/remove operations with links/nodes
	void endIterating();

	void changeStateHash();

    /**
     * @brief This function builds from scratch m_nodesVector and m_linksVector.
     * It uses mutex to protect this objects. In normal case it should not be a problem,
     * because only one building of links and nodes vector is needed for a long time
     */
    void buildVectorsForIteration();

	/// If true we cannot add/remove directly to/from m_nodes and m_links because we are iterating by them
	bool m_iteratingNow = false;

	NodeSet m_nodes, m_nodesToAdd, m_nodesToDelete;
	LinkSet m_links, m_linksToAdd, m_linksToDelete;

    NodeVector m_nodesVector;
    LinkVector m_linksVector;
    std::mutex m_nlvecsMutex;

    size_t m_stateHash = 1;
    size_t m_nodesLinksVectorsStateHash = 0;
};

class ModelContextDependent
{
public:
	ModelContextDependent(ModelContext* context);
	ModelContext* context();
    IPhysicalContext* physicalContext();
    const IPhysicalContext* physicalContext() const;


protected:
	ModelContext* m_context;
};

class Node : public ModelContextDependent, public SelfMemMgr
{
public:
	using LinkVisitor = std::function<void(Link*, LinkDirection)>;
    using IsolatedUpdateHook = std::function<void(Node*, bool isIsolated)>;

	Node(ModelContext* context, StaticVector<3> pos = StaticVector<3>());
	~Node();
	void addLink(Link* link);
	void removeLink(Link* link);
    void setIsolatedUpdateHook(IsolatedUpdateHook hook);
	void applyConnectedLinksVisitor(LinkVisitor visitor);
    bool hasNeighbour(const Node* node) const;

	std::unique_ptr<NodePayloadBase> payload;

	StaticVector<3> pos;

private:
	std::set<Link*> m_links;
    IsolatedUpdateHook m_hook = nullptr;
};


class Link : public ModelContextDependent, public SelfMemMgr
{
public:
    Link(ModelContext* context, Node* n1, Node* n2);
    Link(ModelContext* context, Node* nodeFrom, StaticVector<3> pointTo);
	~Link();

	Node* getNode1();
	Node* getNode2();
	Node* getNode(unsigned int index);
	double length();
	double lengthCached();

	std::unique_ptr<LinkPayloadBase> payload;

private:
    void connect(Node* n1, Node* n2);

	PtrWrap<Node> m_n1, m_n2;
	double m_lengthCached = -1.0;
};

}

#endif // BRANCH_POINT_HPP_INCLUDED
