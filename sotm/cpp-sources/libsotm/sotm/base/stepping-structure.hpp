#ifndef BRANCH_POINT_HPP_INCLUDED
#define BRANCH_POINT_HPP_INCLUDED

#include "sotm/math/geometry.hpp"
#include "sotm/utils/memory.hpp"
#include "sotm/utils/macros.hpp"
#include <list>
#include <memory>
#include <set>

namespace Stepmod
{

class Node;
class Link;

class Node : public SelfMemMgr
{
public:
	void addLink(Link* link);
	void removeLink(Link* link);

private:
	std::set<Link*> m_links;

};


class Link : public SelfMemMgr
{
public:
	void connect(Node* n1, Node* n2);

private:
	PtrWrap<Node> m_n1, m_n2;

};

}

#endif // BRANCH_POINT_HPP_INCLUDED
