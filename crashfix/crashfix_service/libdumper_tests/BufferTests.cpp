#include "stdafx.h"
#include "Tests.h"
#include "Buffer.h"

class BufferTests : public CTestCase
{
    BEGIN_TEST_MAP(BufferTests, "CBuffer class tests")
        REGISTER_TEST(Test_Construct);
        REGISTER_TEST(Test_CopyConstruct);
        REGISTER_TEST(Test_Construct_NonEmpty);
        REGISTER_TEST(Test_Allocate);
        REGISTER_TEST(Test_Reallocate);
        REGISTER_TEST(Test_GetPtr);
        REGISTER_TEST(Test_GetSize);
        REGISTER_TEST(Test_Free);
        REGISTER_TEST(Test_IsEmpty);
        REGISTER_TEST(Test_CopyTo);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Construct();
    void Test_CopyConstruct();
    void Test_Construct_NonEmpty();
    void Test_Allocate();
    void Test_Reallocate();
    void Test_GetPtr();
    void Test_GetSize();
    void Test_Free();
    void Test_IsEmpty();
    void Test_CopyTo();
};


REGISTER_TEST_CASE( BufferTests );

void BufferTests::SetUp()
{
}

void BufferTests::TearDown()
{
}

void BufferTests::Test_Construct()
{
    // Construct empty buffer
    CBuffer buf;
    // Data ptr should be NULL
    TEST_ASSERT(buf.GetPtr()==NULL);
    // Size should be zero
    TEST_ASSERT(buf.GetSize()==0);

    __TEST_CLEANUP__;
}

void BufferTests::Test_Construct_NonEmpty()
{
    // Construct non-empty buffer
    CBuffer buf(256);
    // Data ptr should be non-NULL
    TEST_ASSERT(buf.GetPtr()!=NULL);
    // Size should be 256
    TEST_ASSERT(buf.GetSize()==256);

    __TEST_CLEANUP__;
}

void BufferTests::Test_CopyConstruct()
{
    // Construct buffer
    CBuffer buf1(256);

    // Construct another buffer as a copy of the first buffer
    CBuffer buf2(buf1);

    TEST_ASSERT(buf2.GetPtr()!=NULL);
    TEST_ASSERT(buf2.GetSize()==buf1.GetSize());
    TEST_ASSERT(memcpy(buf2.GetPtr(), buf1.GetPtr(), buf1.GetSize()));

    __TEST_CLEANUP__;
}

void BufferTests::Test_Allocate()
{
    // Construct empty buffer
    CBuffer buf;
    bool bAlloc;
    bool bAlloc2;

    // Allocate zero size
    bAlloc = buf.Allocate(0);
    TEST_ASSERT(bAlloc); // Assume success
    TEST_ASSERT(buf.GetPtr()==NULL); // Data ptr should be NULL
    TEST_ASSERT(buf.GetSize()==0); // Size should be zero

    // Allocate 10 bytes
    bAlloc2 = buf.Allocate(10);
    TEST_ASSERT(bAlloc2); // Assume success
    TEST_ASSERT(buf.GetPtr()!=NULL); // Data ptr should not be NULL
    TEST_ASSERT(buf.GetSize()==10); // Size should be equal to 10

    __TEST_CLEANUP__;
}

void BufferTests::Test_Reallocate()
{
    // Construct buffer
    CBuffer buf(256);
    bool bRealloc;
    bool bRealloc2;
    bool bRealloc3;

    // Fill buffer
    LPBYTE pData = buf.GetPtr();
    int i;
    for(i=0; i<256; i++)
    {
        pData[i] = (BYTE)i;
    }

    // Reallocate buffer to larger size
    bRealloc = buf.Reallocate(512);
    TEST_ASSERT(bRealloc); // Assume success

    // Ensure buffer content is the same
    pData = buf.GetPtr();
    for(i=0; i<256; i++)
    {
        TEST_ASSERT(pData[i]==i);
    }

    // Reallocate buffer to smaller size - assume success
    bRealloc2 = buf.Reallocate(128);
    TEST_ASSERT(bRealloc2);

    // Ensure buffer content is the same
    pData = buf.GetPtr();
    for(i=0; i<128; i++)
    {
        TEST_ASSERT(pData[i]==i);
    }

    // Reallocate to zero size - assume success
    bRealloc3 = buf.Reallocate(0);
    TEST_ASSERT(bRealloc3);
    TEST_ASSERT(buf.GetPtr()==NULL);
    TEST_ASSERT(buf.GetSize()==0);

    __TEST_CLEANUP__;
}

void BufferTests::Test_GetPtr()
{
    // Create empty buf - assume GetPtr() returns NULL
    CBuffer buf;
    // Create non-empty buffer - assume GetPtr() returns non-NULL
    CBuffer buf2(256);

    TEST_ASSERT(buf.GetPtr()==NULL);
    TEST_ASSERT(buf2.GetPtr()!=NULL);

    __TEST_CLEANUP__;
}

void BufferTests::Test_GetSize()
{
    // Create empty buf - assume GetSize() returns 0
    CBuffer buf;
    // Create non-empty buffer - assume GetSize() returns its size
    CBuffer buf2(256);

    TEST_ASSERT(buf.GetSize()==0);

    TEST_ASSERT(buf2.GetSize()==256);

    __TEST_CLEANUP__;
}

void BufferTests::Test_Free()
{
    CBuffer buf(255);
    TEST_ASSERT(buf.GetPtr()!=NULL);
    TEST_ASSERT(buf.GetSize()==255);

    buf.Free();
    TEST_ASSERT(buf.GetPtr()==NULL);
    TEST_ASSERT(buf.GetSize()==0);

    __TEST_CLEANUP__;
}

void BufferTests::Test_IsEmpty()
{
    CBuffer buf;
    CBuffer buf2(1024);

    TEST_ASSERT(buf.IsEmpty());
    TEST_ASSERT(!buf2.IsEmpty());

    __TEST_CLEANUP__;
}

void BufferTests::Test_CopyTo()
{
    CBuffer buf(256);
    CBuffer buf2(256);
    bool bCopy2=false;

    // Fill buffer
    LPBYTE pData = buf.GetPtr();
    int i;
    for(i=0; i<256; i++)
    {
        pData[i] = (BYTE)i;
    }

    bool bCopy = buf.CopyTo(&buf2);
    TEST_ASSERT(bCopy);

    // Ensure buffer content is the same
    pData = buf2.GetPtr();
    for(i=0; i<256; i++)
    {
        TEST_ASSERT(pData[i]==i);
    }

    // Allocate buffer of smaller size
    buf2.Allocate(128);

    // Assume CopyTo fails
    bCopy2 = buf.CopyTo(&buf2);
    TEST_ASSERT(!bCopy2);

    __TEST_CLEANUP__;
}
