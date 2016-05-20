#ifndef BRANCH_POINT_HPP_INCLUDED
#define BRANCH_POINT_HPP_INCLUDED

#include "stepmod/math/geometry.hpp"
#include "stepmod/utils/memory.hpp"
#include "stepmod/utils/macros.hpp"
#include <list>
#include <memory>
#include <set>

namespace Stepmod
{

class Channel;
class BranchPoint;

/**
 * This class collects all BranchPoint s in the system and it is entry point to access
 * any BranchPoint by coords
 */
class BranchPointsPool
{
public:
    SINGLETON_IN_CLASS(BranchPointsPool)
    
    void registerPoint(BranchPoint* point);
    void deregisterPoint(BranchPoint* point);
    
private:
    BranchPointsPool();
    std::set<BranchPoint*> m_allPoints;
    /// @todo Add here @deleted@ objects caching
};

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
    
    void onDelete() override final;
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
