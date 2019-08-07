#include "stdafx.h"
#include "Tests.h"
#include "MsfFile.h"
#include "Utils.h"

class MsfFileTests : public CTestCase
{
    BEGIN_TEST_MAP(MsfFileTests, "CMsfFile class tests")
        REGISTER_TEST(Test_Init);
        REGISTER_TEST(Test_Destroy);
        REGISTER_TEST(Test_IsInitialized);
        REGISTER_TEST(Test_GetFileName);
        REGISTER_TEST(Test_GetRootStream);
        REGISTER_TEST(Test_GetIndexDirStream);
        REGISTER_TEST(Test_GetStreamCount);
        REGISTER_TEST(Test_GetStream);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
    void Test_Destroy();
    void Test_IsInitialized();
    void Test_GetFileName();
    void Test_GetRootStream();
    void Test_GetIndexDirStream();
    void Test_GetStreamCount();
    void Test_GetStream();
};


REGISTER_TEST_CASE( MsfFileTests );

void MsfFileTests::SetUp()
{
}

void MsfFileTests::TearDown()
{
}

void MsfFileTests::Test_Init()
{
    bool bInit = FALSE;
    bool bInit2 = FALSE;
    CMsfFile MsfFile;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    // Init twice - assume success
    bInit2 = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit2);

    __TEST_CLEANUP__;

}

void MsfFileTests::Test_Destroy()
{
    CMsfFile MsfFile;
    bool bInit = false;
    bool bInit2 = false;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    // Assume not initialized yet
    TEST_ASSERT(!MsfFile.IsInitialized());

    // Call destroy - assume success
    MsfFile.Destroy();

    TEST_ASSERT(!MsfFile.IsInitialized());

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    // Call destroy - assume success
    MsfFile.Destroy();
    TEST_ASSERT(!MsfFile.IsInitialized());

    // Init twice - assume success
    bInit2 = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit2);

    // Call destroy - assume success
    MsfFile.Destroy();
    TEST_ASSERT(!MsfFile.IsInitialized());

    TEST_ASSERT(NULL==MsfFile.GetRootStream());
    TEST_ASSERT(NULL==MsfFile.GetIndexDirStream());
    TEST_ASSERT(MsfFile.GetFileName().empty());

    __TEST_CLEANUP__;

}

void MsfFileTests::Test_IsInitialized()
{
    CMsfFile MsfFile;
    bool bInit = false;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    // Assume not initialized yet
    TEST_ASSERT(!MsfFile.IsInitialized());

    // Call destroy - assume success
    MsfFile.Destroy();

    TEST_ASSERT(!MsfFile.IsInitialized());

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    // Call destroy - assume success
    MsfFile.Destroy();
    TEST_ASSERT(!MsfFile.IsInitialized());

    __TEST_CLEANUP__;
}

void MsfFileTests::Test_GetFileName()
{
    CMsfFile MsfFile;
    bool bInit;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    // File name should be empty
    TEST_ASSERT(MsfFile.GetFileName().empty());

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    // File name should be non-empty
    TEST_ASSERT(!MsfFile.GetFileName().empty());

    // Call destroy - assume success
    MsfFile.Destroy();
    TEST_ASSERT(!MsfFile.IsInitialized());

    // File name should be empty
    TEST_ASSERT(MsfFile.GetFileName().empty());

    __TEST_CLEANUP__;
}

void MsfFileTests::Test_GetRootStream()
{
    CMsfFile MsfFile;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    bool bInit = false;

    TEST_ASSERT(NULL==MsfFile.GetRootStream());

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    TEST_ASSERT(NULL!=MsfFile.GetRootStream());
    TEST_ASSERT(MsfFile.GetRootStream()->GetStreamLen()==20);

    // Call destroy - assume success
    MsfFile.Destroy();
    TEST_ASSERT(!MsfFile.IsInitialized());

    TEST_ASSERT(NULL==MsfFile.GetRootStream());

    __TEST_CLEANUP__;
}

void MsfFileTests::Test_GetIndexDirStream()
{
    CMsfFile MsfFile;
    bool bInit;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    TEST_ASSERT(NULL==MsfFile.GetIndexDirStream());

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    TEST_ASSERT(NULL!=MsfFile.GetIndexDirStream());
    TEST_ASSERT(MsfFile.GetIndexDirStream()->GetStreamLen()==4900);

    // Call destroy - assume success
    MsfFile.Destroy();
    TEST_ASSERT(!MsfFile.IsInitialized());

    TEST_ASSERT(NULL==MsfFile.GetIndexDirStream());

    __TEST_CLEANUP__;
}

void MsfFileTests::Test_GetStreamCount()
{
    bool bInit;
    CMsfFile MsfFile;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    TEST_ASSERT(MsfFile.GetStreamCount()==0);

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    TEST_ASSERT(MsfFile.GetStreamCount()==56);

    // Call destroy - assume success
    MsfFile.Destroy();

    TEST_ASSERT(MsfFile.GetStreamCount()==0);

    __TEST_CLEANUP__;
}

void MsfFileTests::Test_GetStream()
{
    CMsfFile MsfFile;
    bool bInit;
    int nCount = 0;
    CMsfStream* pStream = NULL;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    // Init - assume success
    bInit = MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    nCount = MsfFile.GetStreamCount();
    int i;
    for(i=0; i<nCount; i++)
    {
        CMsfStream* pStream = MsfFile.GetStream(i);
        assert(pStream!=NULL);
    }

    // Invalid stream index - assume failure
    pStream = MsfFile.GetStream(nCount);
    assert(pStream==NULL);

    __TEST_CLEANUP__;
}
