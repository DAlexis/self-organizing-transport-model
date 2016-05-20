#include "stepmod/utils/memory.hpp"

#include "gtest/gtest.h"

using namespace Stepmod;

class TestSMM : public SelfMemMgr
{
public:
    TestSMM() { instCount++; }
    ~TestSMM() { instCount--; }
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

TEST(PtrWrapTest, PtrWrapInst)
{
    TestSMM::instCount = 0;
    TestSMM *tempPointer = new TestSMM(); // Ref count = 1
    {
        PtrWrap<TestSMM> p(tempPointer); // Ref count = 2
        ASSERT_EQ(TestSMM::instCount, 1);
    } // Ref count = 1
    ASSERT_EQ(TestSMM::instCount, 1);
    tempPointer->release(); // Ref count = 0
    ASSERT_EQ(TestSMM::instCount, 0);
    
    TestSMM::instCount = 0;
    {
        PtrWrap<TestSMM> p = PtrWrap<TestSMM>::MakePtrWrap();
        PtrWrap<TestSMM> q = p;
        ASSERT_EQ(TestSMM::instCount, 1);
    }
    ASSERT_EQ(TestSMM::instCount, 0);
}
