#ifndef BRANCH_POINT_HPP_INCLUDED
#define BRANCH_POINT_HPP_INCLUDED

#include "stepmod/math/geometry.hpp"
#include "stepmod/utils/memory.hpp"
#include <list>
#include <memory>

namespace Stepmod
{

class Channel;

class BranchPoint : public SelfMemMgr
{
public:
    BranchPoint(Point<3> pos = Point<3>());
    Point<3> position;
    
    void registerChannel(Channel* channel);
    static BranchPoint* create(Point<3> pos = Point<3>());
    
private:
    BranchPoint();
    std::list<Channel*> m_channels;

protected:

};

class Channel : public SelfMemMgr
{
public:
    void connect(BranchPoint& p1, BranchPoint& p2);
    void remove();
    
    static Channel* create(BranchPoint* p1 = nullptr, Point<3> p2pos = Point<3>());
    
private:
    Channel();
    BranchPoint *p1, *p2;
};

}

#endif // BRANCH_POINT_HPP_INCLUDED
