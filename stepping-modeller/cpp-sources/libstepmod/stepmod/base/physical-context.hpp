#ifndef PHYSICAL_CONTEXT_HPP_INCLUDE
#define PHYSICAL_CONTEXT_HPP_INCLUDE

namespace Stepmod
{

class IBranchPayload
{
public:
    virtual ~IBranchPayload() {}
    virtual void onRemove() = 0;
    virtual void onCreate() = 0;
};

class IBranchPayloadFactory
{
};

class IChannelPayload
{
public:

};

class PhysicalContext
{
public:

};

}

#endif // PHYSICAL_CONTEXT_HPP_INCLUDE
