#ifndef BRANCH_POINT_HPP_INCLUDED
#define BRANCH_POINT_HPP_INCLUDED

#include <list>

class ChannelBase;

class BranchPointBase
{
public:
    std::list<ChannelBase*> channels;
    
protected:
};

#endif // BRANCH_POINT_HPP_INCLUDED
