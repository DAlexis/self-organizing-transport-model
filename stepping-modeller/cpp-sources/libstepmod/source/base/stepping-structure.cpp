#include "stepmod/base/stepping-structure.hpp"

using namespace Stepmod;

BranchPoint::BranchPoint(Point<3> pos) :
    position(pos)
{
}

BranchPoint* BranchPoint::create(Point<3> pos)
{
    return new BranchPoint(pos);
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
