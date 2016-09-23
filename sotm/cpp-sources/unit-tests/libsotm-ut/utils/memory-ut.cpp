#include "sotm/utils/memory.hpp"

#include "gtest/gtest.h"

using namespace sotm;

class TestSMM : public SelfMemMgr
{
public:
	constexpr static int initialTestValue = 35;
    TestSMM() { instCount++; }
    ~TestSMM() { instCount--; }
    int testValue = initialTestValue;
    static int instCount;
};

int TestSMM::instCount = 0;

TEST(SelfMemMgrTest, SelfMemMgrInst)
{
    TestSMM::instCount = 0;
    TestSMM* p = new TestSMM();
    ASSERT_EQ(TestSMM::instCount, 1);
    p->release();
    ASSERT_EQ(TestSMM::instCount, 0);
}

TEST(PtrWrapTest, PtrWrapInstEmpty)
{
	PtrWrap<TestSMM> *p = nullptr;
	ASSERT_NO_THROW(p = new PtrWrap<TestSMM>());
	ASSERT_NO_THROW(delete p);
}

TEST(PtrWrapTest, PtrWrapContainment)
{
	PtrWrap<TestSMM> *p = nullptr;
	int initVal = TestSMM::initialTestValue;
	p = new PtrWrap<TestSMM>();
	p->assign(new TestSMM);
	(*p)->testValue++;
	ASSERT_EQ((*p)->testValue,      initVal+1);
	ASSERT_EQ(p->data()->testValue, initVal+1);
	ASSERT_EQ((**p).testValue,      initVal+1);

	delete p;
	p = new PtrWrap<TestSMM>();
	p->assign(new TestSMM);
	ASSERT_EQ((*p)->testValue, initVal);
	delete p;
}

TEST(PtrWrapTest, PtrWrapInst)
{
    TestSMM::instCount = 0;
    TestSMM *tempPointer = new TestSMM(); // Ref count = 1
    {
        PtrWrap<TestSMM> p(tempPointer); // Ref count = 2
        ASSERT_EQ(TestSMM::instCount, 1); // Object is alive
    } // Ref count = 1
    ASSERT_EQ(TestSMM::instCount, 1); // object still alive
    tempPointer->release(); // Ref count = 0
    ASSERT_EQ(TestSMM::instCount, 0); // object deleted

    
    TestSMM::instCount = 0;
    {
        PtrWrap<TestSMM> p = PtrWrap<TestSMM>::make();
        PtrWrap<TestSMM> q = p;
        ASSERT_EQ(TestSMM::instCount, 1);
    }
    ASSERT_EQ(TestSMM::instCount, 0);
}

TEST(PtrWrapTest, AssignmentTest)
{
	TestSMM::instCount = 0;
	{
		PtrWrap<TestSMM> p1;
		p1 = PtrWrap<TestSMM>::make();
		ASSERT_EQ(TestSMM::instCount, 1); // Object is alive
	}
	ASSERT_EQ(TestSMM::instCount, 0); // Object is destroyed

	TestSMM::instCount = 0;
	PtrWrap<TestSMM> p2;
	{
		PtrWrap<TestSMM> p1;
		p1 = PtrWrap<TestSMM>::make();
		ASSERT_EQ(TestSMM::instCount, 1); // Object is alive

		ASSERT_NO_THROW(p2 = p1);

		ASSERT_EQ(TestSMM::instCount, 1); // Object is alive
	}
	ASSERT_EQ(TestSMM::instCount, 1); // Object is alive
	p2.clear();
	ASSERT_EQ(TestSMM::instCount, 0); // Object is destroyed
}
