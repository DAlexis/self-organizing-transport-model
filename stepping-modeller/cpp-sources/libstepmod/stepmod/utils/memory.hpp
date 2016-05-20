#ifndef MEMORY_HPP_INCLUDED
#define MEMORY_HPP_INCLUDED

#include "stepmod/utils/assert.hpp"
#include <cstring>

namespace Stepmod
{

class SelfMemMgr
{
public:
    /// @todo Check in destructor ref count and make assertion
    virtual ~SelfMemMgr() {  }
    inline void addRef() { ++m_refCount; }
    inline void release()
    {
        if (--m_refCount == 0)
        {
            onDelete();
            delete this;
        }
    }

protected:
    virtual void onDelete() {}
    
private:
    size_t m_refCount = 1;
};

/*
/// Analog of shared_ptr for SelfMemMgr 
template <typename T>
class PtrWrap
{
public:
    
    PtrWrap(T* pobject) : m_pobject(pobject) { }
    ~PtrWrap() { m_pobject->release(); }
    
    PtrWrap(PtrWrap<T> ptr)
    {
    }
    
    
private:
    T* m_pobject;
};*/

template <typename T>
void zerify(T& object)
{
    memset(&object, 0, sizeof(T));
}

}

#endif // MEMORY_HPP_INCLUDED
