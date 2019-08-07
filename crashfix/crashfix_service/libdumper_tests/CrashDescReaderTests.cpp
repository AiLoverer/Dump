#include "stdafx.h"
#include "Tests.h"
#include "CrashDescReader.h"
#include "Utils.h"

class CrashDescReaderTests : public CTestCase
{
    BEGIN_TEST_MAP(CrashDescReaderTests, "CCrashDescReader class tests")
        REGISTER_TEST(Test_Init);
        
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
    
};


REGISTER_TEST_CASE( CrashDescReaderTests );

void CrashDescReaderTests::SetUp()
{
}

void CrashDescReaderTests::TearDown()
{
}

void CrashDescReaderTests::Test_Init()
{
    CCrashDescReader reader;
    bool bInit = FALSE;
    bool bIsInitialized = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashrpt.xml";

    // Check initialized status - assume false
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(!bIsInitialized);

    // Init with empty file name - assume failure
    bInit = reader.Init(sFileName);
    TEST_ASSERT(bInit);

    // Check initialized status - assume false
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(bIsInitialized);

    // Init with correct file name - assume success
    bInit = reader.Init(sFileName);
    TEST_ASSERT(bInit);

    // Check initialized status - assume success
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(bIsInitialized);

    TEST_ASSERT(reader.GetGeneratorVersion()==1300);
    TEST_ASSERT(reader.GetAppName()==L"CrashRpt Tests");
    TEST_ASSERT(reader.GetAppVersion()==L"1.3.0.0");

    reader.Destroy();

    // Check initialized status - assume false
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(!bIsInitialized);

    __TEST_CLEANUP__;
}


