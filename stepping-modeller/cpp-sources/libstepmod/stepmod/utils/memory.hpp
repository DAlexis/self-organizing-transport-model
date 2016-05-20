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


/// Analog of shared_ptr for SelfMemMgr 
template <typename T>
class PtrWrap
{
public:

    template <typename ... Types>
    static PtrWrap<T> MakePtrWrap(Types&& ... args)
    {
        T* pobject = new T(std::forward<Types>(args)...);
        PtrWrap<T> w(pobject);
        pobject->release(); // Now ref count = 1
        return w;
    }
    
    PtrWrap(T* pobject) { reset(pobject); }
    
    PtrWrap(PtrWrap<T>& ptr)  { *this = ptr; }
    PtrWrap(PtrWrap<T>&& ptr) { *this = ptr; }
    
    ~PtrWrap() { m_pobject->release(); }
    
    void reset(T* pobject)
    {
        clearPObject();
        pobject->addRef();
        m_pobject = pobject;
    }
    
    PtrWrap& operator=(PtrWrap& right)
    {
        reset(right.m_pobject);
        return *this;
    }
    /*
    PtrWrap& operator=(PtrWrap&& right)
    {
        reset(right.m_pobject);
        return *this;
    }*/
    
    PtrWrap& operator=(T* pobject)
    {
        reset(pobject);
    }   
    
    bool operator==(const PtrWrap& right)
    {
        return m_pobject == right.m_pobject;
    }
    
private:
    void clearPObject()
    {
        if (m_pobject != nullptr)
            m_pobject->release();
    }
    T* m_pobject = nullptr;
};

template <typename T>
void zerify(T& object)
{
    memset(&object, 0, sizeof(T));
}

}

#endif // MEMORY_HPP_INCLUDED
