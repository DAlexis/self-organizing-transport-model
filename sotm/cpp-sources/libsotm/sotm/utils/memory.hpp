#ifndef MEMORY_HPP_INCLUDED
#define MEMORY_HPP_INCLUDED

#include "sotm/utils/assert.hpp"
#include <cstring>

namespace sotm
{

class SelfMemMgr
{
public:
    /// @todo Check in destructor ref count and make assertion
    virtual ~SelfMemMgr() {  }
    inline size_t refCount() { return m_refCount; }
    inline void addRef() { ++m_refCount; }
    inline void release()
    {
        if (--m_refCount == 0)
        {
            onDelete(); /// @todo Add possibility to increment refcount in OnDelete
            delete this;
        }
    }

protected:
    virtual void onDelete()
    {
    	// Nothing by default
    }
    
private:
    size_t m_refCount = 1;
};


/// Analog of shared_ptr for SelfMemMgr 
template <typename T>
class PtrWrap
{
public:

    template <typename ... Types>
    static PtrWrap<T> make(Types&& ... args)
    {
        T* pobject = new T(std::forward<Types>(args)...); // pobject has refCount == 1
        PtrWrap<T> w(pobject); // PtrWrap from pobject makes refCount == 2
        pobject->release(); // Now ref count == 1
        return w;
    }
    
    PtrWrap(T* pobject = nullptr) { assign(pobject); }
    
    PtrWrap(PtrWrap<T>& ptr)  { *this = ptr; }
    PtrWrap(PtrWrap<T>&& ptr) { *this = ptr; }
    
    ~PtrWrap()
    {
    	if (m_pobject)
    		m_pobject->release();
    }

    /// Assign object and add its reference. Release previous object
    void assign(T* pobject)
    {
    	clear();
        if (pobject)
        	pobject->addRef();
        m_pobject = pobject;
    }
    
    /// Release object if contained and became a null-pointer
    void clear()
	{
		if (m_pobject != nullptr)
		{
			m_pobject->release();
			m_pobject = nullptr;
		}
	}

    bool assigned() { return m_pobject != nullptr; }

    PtrWrap& operator=(PtrWrap& right)
    {
        assign(right.m_pobject);
        return *this;
    }

    PtrWrap& operator=(PtrWrap&& right)
    {
        m_pobject = right.m_pobject;
        right.m_pobject = nullptr;
        return *this;
    }
    
    operator T*()
    {
    	ASSERT(m_pobject != nullptr, "Invalid usage of operator T*(): pointer is nullptr");
    	return m_pobject;
    }

    operator T&()
    {
    	ASSERT(m_pobject != nullptr, "Invalid usage of operator T&(): pointer is nullptr");
    	return *m_pobject;
    }

    operator const T*()
    {
    	ASSERT(m_pobject != nullptr, "Invalid usage of const operator T*(): pointer is nullptr");
    	return m_pobject;
    }

    operator const T&()
    {
    	ASSERT(m_pobject != nullptr, "Invalid usage of const operator T&(): pointer is nullptr");
    	return *m_pobject;
    }
/*
    PtrWrap& operator=(T* pobject)
    {
        assign(pobject);
        //return *this;
    }   */
    
    bool operator==(const PtrWrap& right)
    {
        return m_pobject == right.m_pobject;
    }
    
    inline T* operator->()
    {
    	ASSERT(m_pobject != nullptr, "Invalid usage of operator->(): pointer is nullptr");
    	return m_pobject;
    }

    inline T* data()
    {
    	ASSERT(m_pobject != nullptr, "Invalid usage of data(): pointer is nullptr");
    	return m_pobject;
    }

    inline T& operator*()
    {
    	return *m_pobject;
    }

private:

    T* m_pobject = nullptr;
};

template <typename T>
void zerify(T& object)
{
    memset(&object, 0, sizeof(T));
}

}

#endif // MEMORY_HPP_INCLUDED
