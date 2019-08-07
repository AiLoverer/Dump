#include "stdafx.h"
#include "Tests.h"
#include "PeReader.h"
#include "Utils.h"

class PeReaderTests : public CTestCase
{
    BEGIN_TEST_MAP(PeReaderTests, "CPeReader class tests")
        REGISTER_TEST(Test_Init_x86_DLL);
        REGISTER_TEST(Test_Init_x64_DLL);
        REGISTER_TEST(Test_Destroy);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init_x86_DLL();
    void Test_Init_x64_DLL();
    void Test_Destroy();
};


REGISTER_TEST_CASE( PeReaderTests );

void PeReaderTests::SetUp()
{
}

void PeReaderTests::TearDown()
{
}

void PeReaderTests::Test_Init_x86_DLL()
{
    CPeReader PeReader;
    BOOL bInit = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
#ifdef _WIN32
    sFileName += L"vs2005\\CrashRpt.dll";
#else
    sFileName += L"vs2005/CrashRpt.dll";
#endif
    std::wstring sPdbName;
    std::wstring sPdbGUID;
    int nAge = 1;
    BOOL bGetCvInfo = FALSE;

    // Init with empty filename - assume failure
    bInit = PeReader.Init(L"");
    TEST_ASSERT(!bInit);

    // Init with correct filename - assume success
    bInit = PeReader.Init(sFileName);
    TEST_ASSERT(bInit);

    TEST_ASSERT(TRUE==PeReader.IsDLL());
    TEST_ASSERT(FALSE==PeReader.IsPE32Plus());
    TEST_ASSERT(FALSE==PeReader.IsDebugInfoStripped());
    bGetCvInfo = PeReader.GetCvInfo(sPdbName, sPdbGUID, nAge);
    TEST_ASSERT(bGetCvInfo);
    TEST_ASSERT(sPdbName==L"d:\\Projects\\CrashRpt\\CrashRpt_v.1.2.7_r997\\bin\\CrashRpt.pdb");
    TEST_ASSERT(sPdbGUID==L"9a13c2b4-f74f-4fa4-bdbc-472bdd0dc5e9");
    TEST_ASSERT(nAge==1);

    PeReader.Destroy();

    __TEST_CLEANUP__;
}

void PeReaderTests::Test_Init_x64_DLL()
{
    CPeReader PeReader;
    BOOL bInit = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
#ifdef _WIN32
    sFileName += L"vs2010\\x64\\2\\CrashRpt1300.dll";
#else
    sFileName += L"vs2010/x64/2/CrashRpt1300.dll";
#endif
    std::wstring sPdbName;
    std::wstring sPdbGUID;
    int nAge = 1;
    BOOL bGetCvInfo = FALSE;

    // Init with correct filename - assume success
    bInit = PeReader.Init(sFileName);
    TEST_ASSERT(bInit);

    TEST_ASSERT(TRUE==PeReader.IsDLL());
    TEST_ASSERT(TRUE==PeReader.IsPE32Plus());
    TEST_ASSERT(FALSE==PeReader.IsDebugInfoStripped());
    bGetCvInfo = PeReader.GetCvInfo(sPdbName, sPdbGUID, nAge);
    TEST_ASSERT(bGetCvInfo);
    TEST_ASSERT(sPdbName==L"D:\\Projects\\CrashRpt\\export\\export_1.3.0\\CrashRpt_v.1.3.0_r1291_1\\bin\\x64\\CrashRpt1300.pdb");
    TEST_ASSERT(sPdbGUID==L"c7e5b0ec-4f96-4da1-a657-b3b5cab9eeef");
    TEST_ASSERT(nAge==1);

    PeReader.Destroy();

    __TEST_CLEANUP__;
}

void PeReaderTests::Test_Destroy()
{
    CPeReader PeReader;
    BOOL bInit = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"CrashRptd.dll";

    TEST_ASSERT(PeReader.IsInitialized()==FALSE);

    // Init with empty filename - assume failure
    bInit = PeReader.Init(sFileName);
    TEST_ASSERT(bInit);

    TEST_ASSERT(PeReader.IsInitialized()==TRUE);

    PeReader.Destroy();

    TEST_ASSERT(PeReader.IsInitialized()==FALSE);

    __TEST_CLEANUP__;
}

