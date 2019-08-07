#include "stdafx.h"
#include "Tests.h"
#include "Utils.h"
#include "PdbReader.h"

class PdbHeaderStreamTests : public CTestCase
{
    BEGIN_TEST_MAP(PdbHeaderStreamTests, "CPdbHeaderStream class tests")
        REGISTER_TEST(Test_Init);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
};


REGISTER_TEST_CASE( PdbHeaderStreamTests );

void PdbHeaderStreamTests::SetUp()
{
}

void PdbHeaderStreamTests::TearDown()
{
}

void PdbHeaderStreamTests::Test_Init()
{
    DWORD dwAge = 0;
    BOOL bInitialized = FALSE;
    CPdbReader PdbReader;
    std::wstring sGUID;
    CPdbHeadersStream* pHeaders = NULL;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    //sFileName += L"CrashRptd.pdb";

    // Init - assume success
    BOOL bInit = PdbReader.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    // Get headers stream
    pHeaders = PdbReader.GetHeadersStream();
    TEST_ASSERT(pHeaders);

    // Check stream is initialized
    bInitialized = pHeaders->IsInitialized();
    TEST_ASSERT(bInitialized);

    dwAge = pHeaders->GetAge();
    TEST_ASSERT(dwAge==2);

    sGUID = pHeaders->GetGUID();
    TEST_ASSERT(sGUID.compare(L"94738ec7-c64a-4b15-b4c6-c3d880d75728")==0);

    __TEST_CLEANUP__;
}
