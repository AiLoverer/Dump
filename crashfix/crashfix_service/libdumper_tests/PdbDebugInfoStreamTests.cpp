#include "stdafx.h"
#include "Tests.h"
#include "PdbReader.h"
#include "Utils.h"

class PdbDebugInfoStreamTests : public CTestCase
{
    BEGIN_TEST_MAP(PdbDebugInfoStreamTests, "CPdbDebugInfoStream class tests")
        REGISTER_TEST(Test_Init);
        
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
    

};


REGISTER_TEST_CASE( PdbDebugInfoStreamTests );

void PdbDebugInfoStreamTests::SetUp()
{
}

void PdbDebugInfoStreamTests::TearDown()
{
}

void PdbDebugInfoStreamTests::Test_Init()
{
    BOOL bInitialized = FALSE;
    CPdbReader PdbReader;    
    CPdbDebugInfoStream* pDBI = NULL;

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    
    // Init - assume success
    BOOL bInit = PdbReader.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    // Get DBI stream
    pDBI = PdbReader.GetDebugInfoStream();
    TEST_ASSERT(pDBI);

    // Check stream is initialized
    bInitialized = pDBI->IsInitialized();
    TEST_ASSERT(bInitialized);

    TEST_ASSERT(pDBI->GetMachineType()==332);        
    TEST_ASSERT(pDBI->GetModuleCount()==43);
    TEST_ASSERT(pDBI->GetSrcFileNameCount()==291);

    __TEST_CLEANUP__;
}


