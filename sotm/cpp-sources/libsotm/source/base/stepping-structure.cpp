#include "sotm/base/stepping-structure.hpp"

using namespace Stepmod;

void Node::addLink(Link* link)
{
	m_links.insert(link);
}

void Node::removeLink(Link* link)
{
	size_t count = m_links.erase(link);
	ASSERT(count != 0, "Removing link that was not connected");
}

void Link::connect(Node* n1, Node* n2)
{
	m_n1.assign(n1);
	m_n2.assign(n2);
}


/*
SINGLETON_IN_CPP(BranchPointsPool)

BranchPointsPool::BranchPointsPool()
{
}

void BranchPointsPool::registerPoint(std::weak_ptr<BranchPoint> point)
{
    m_allPoints.insert(point);
}

void BranchPointsPool::deregisterPoint(std::weak_ptr<BranchPoint> point)
{
    m_allPoints.erase(point);
}

BranchPoint::BranchPoint(Point<3> pos) :
    position(pos)
{
    
}

BranchPoint* BranchPoint::create(Point<3> pos)
{
    BranchPoint* point = new BranchPoint(pos);
    BranchPointsPool::instance().registerPoint(point);
    return point;
}

void BranchPoint::registerChannel(Channel* channel)
{
#ifdef DEBUG
    for (auto pch : m_channels)
    {
        ASSERT (pch != channel, "Double channel connection to point");
    }
#endif
    m_channels.push_back(channel);
}

void BranchPoint::onDelete()
{
    BranchPointsPool::instance().deregisterPoint(this);
}

Channel* Channel::create(BranchPoint* p1, Point<3> p2pos)
{
    Channel* newChannel = new Channel;
    if (p1 != nullptr)
    {
        BranchPoint* p2 = BranchPoint::create(p2pos);
        newChannel->connect(*p1, *p2);
        p2->release(); // Remove this when memory managements will be improved
    }
    return newChannel;
}

void Channel::connect(BranchPoint& p1, BranchPoint& p2)
{
    p1.addRef(); p2.addRef();
    this->p1 = &p1; this->p2 = &p2;
}

void Channel::remove()
{
    p1->release(); p2->release();
}
*/
