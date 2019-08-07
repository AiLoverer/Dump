#include "stdafx.h"
#include "Tests.h"
#include "Utils.h"
#include "PdbReader.h"

class PdbSymbolStreamTests : public CTestCase
{
    BEGIN_TEST_MAP(PdbSymbolStreamTests, "CPdbSymbolStream class tests")
        REGISTER_TEST(Test_Init);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
};


REGISTER_TEST_CASE( PdbSymbolStreamTests );

void PdbSymbolStreamTests::SetUp()
{
}

void PdbSymbolStreamTests::TearDown()
{
}

void PdbSymbolStreamTests::Test_Init()
{
    CPdbReader PdbReader;
    CPdbCompilandStream* pCompiland = NULL;
    DBI_ModuleInfo* pModuleInfo = NULL;
    CPdbDebugInfoStream* pDBI = NULL;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    //sFileName += L"CrashRptd.pdb";

    // Init - assume success
    BOOL bInit = PdbReader.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    pDBI = PdbReader.GetDebugInfoStream();
    TEST_ASSERT(pDBI);

    pModuleInfo = pDBI->GetModuleInfo(2);
    TEST_ASSERT(pModuleInfo);

    pCompiland = PdbReader.GetCompilandStream(pModuleInfo);
    TEST_ASSERT(pCompiland);

    __TEST_CLEANUP__;
}
