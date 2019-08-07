#include "stdafx.h"
#include "Tests.h"
#include "MiniDumpReader.h"
#include "Utils.h"

class MiniDumpReaderTests : public CTestCase
{
    BEGIN_TEST_MAP(MiniDumpReaderTests, "CMiniDumpReader class tests")
        REGISTER_TEST(Test_Init);		
		REGISTER_TEST(Test_Init_full_memory_dump);
        REGISTER_TEST(Test_GetStreams);        

    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
    void Test_GetStreams();
    void Test_Init_full_memory_dump();
};


REGISTER_TEST_CASE( MiniDumpReaderTests );

void MiniDumpReaderTests::SetUp()
{
}

void MiniDumpReaderTests::TearDown()
{
}

void MiniDumpReaderTests::Test_Init()
{
    CMiniDumpReader dmp;
    BOOL bInit = FALSE;
    BOOL bInitialized = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.dmp";

    // Check initialized flag - assume false
    bInitialized = dmp.IsInitialized();
    TEST_ASSERT(!bInitialized);

    // Init with empty file name - assume failure
    bInit = dmp.Init(L"");
    TEST_ASSERT(!bInit);

    // Check initialized flag - assume false
    bInitialized = dmp.IsInitialized();
    TEST_ASSERT(!bInitialized);

    // Init with correct file name - assume success
    bInit = dmp.Init(sFileName);
    TEST_ASSERT(bInit);

    // Check initialized flag - assume true
    bInitialized = dmp.IsInitialized();
    TEST_ASSERT(bInitialized);

    // Destroy
    dmp.Destroy();

    // Check initialized flag - assume false
    bInitialized = dmp.IsInitialized();
    TEST_ASSERT(!bInitialized);

    __TEST_CLEANUP__;
}

void MiniDumpReaderTests::Test_Init_full_memory_dump()
{
    CMiniDumpReader dmp;
    BOOL bInit = FALSE;
    BOOL bInitialized = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashdump_full_mem.dmp";
	    
    // Init with correct file name - assume success
    bInit = dmp.Init(sFileName);
    TEST_ASSERT(bInit);

    // Check initialized flag - assume true
    bInitialized = dmp.IsInitialized();
    TEST_ASSERT(bInitialized);

    // Destroy
    dmp.Destroy();

    // Check initialized flag - assume false
    bInitialized = dmp.IsInitialized();
    TEST_ASSERT(!bInitialized);

    __TEST_CLEANUP__;
}

void MiniDumpReaderTests::Test_GetStreams()
{
    CMiniDumpReader dmp;
    BOOL bInit = FALSE;    
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.dmp";
    MiniDumpHeaderInfo* pHeader = NULL;
    MiniDumpSystemInfo* pSysInfo = NULL;
    MiniDumpMiscInfo* pMiscInfo = NULL;
    MiniDumpExceptionInfo* pExcInfo = NULL;

    // Get streams of uninitialized minidump - assume NULL

    pHeader = dmp.GetHeaderInfo();
    TEST_ASSERT(!pHeader);
    
    pSysInfo = dmp.GetSystemInfo();
    TEST_ASSERT(!pSysInfo);
    
    pMiscInfo = dmp.GetMiscInfo();
    TEST_ASSERT(!pMiscInfo);

    pExcInfo = dmp.GetExceptionInfo();
    TEST_ASSERT(!pExcInfo);

    // Init with correct file name - assume success
    bInit = dmp.Init(sFileName);
    TEST_ASSERT(bInit);

    // Get streams of initialized minidump - assume non-NULL

    pHeader = dmp.GetHeaderInfo();
    TEST_ASSERT(pHeader);
    
    TEST_ASSERT(pHeader->m_uChecksum==0);
    TEST_ASSERT(pHeader->m_uMinidumpType==0);
    TEST_ASSERT(pHeader->m_uStreamCount==8);

    pSysInfo = dmp.GetSystemInfo();
    TEST_ASSERT(pSysInfo);
    TEST_ASSERT(pSysInfo->m_uProcessorArchitecture==0);
    TEST_ASSERT(pSysInfo->m_ulVerMajor==5);

    pMiscInfo = dmp.GetMiscInfo();
    TEST_ASSERT(pMiscInfo);
    TEST_ASSERT(pMiscInfo->m_uProcessId==3580);

    pExcInfo = dmp.GetExceptionInfo();
    TEST_ASSERT(pExcInfo);
    TEST_ASSERT(pExcInfo->m_uExceptionCode==0xC0000005);

    
    __TEST_CLEANUP__;
}

