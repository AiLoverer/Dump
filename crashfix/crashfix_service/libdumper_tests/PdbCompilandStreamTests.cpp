#include "stdafx.h"
#include "Tests.h"
#include "Utils.h"
#include "PdbReader.h"

class PdbCompilandStreamTests : public CTestCase
{
    BEGIN_TEST_MAP(PdbCompilandStreamTests, "CPdbCompilandStream class tests")
        REGISTER_TEST(Test_Init);
        REGISTER_TEST(Test_GetFileNames);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
    void Test_GetFileNames();
};


REGISTER_TEST_CASE( PdbCompilandStreamTests );

void PdbCompilandStreamTests::SetUp()
{
}

void PdbCompilandStreamTests::TearDown()
{
}

void PdbCompilandStreamTests::Test_Init()
{
    CPdbReader PdbReader;
    int nModuleCount  = 0;
    int i;
    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    //sFileName += L"CrashRptd.pdb";
    CPdbDebugInfoStream* pDBI = NULL;

    // Init - assume success
    BOOL bInit2 = PdbReader.Init(sFileName.c_str());
    TEST_ASSERT(bInit2);

    pDBI = PdbReader.GetDebugInfoStream();
    TEST_ASSERT(pDBI);

    nModuleCount = pDBI->GetModuleCount();
    for(i=0; i<nModuleCount; i++)
    {
        DBI_ModuleInfo* pModuleInfo = pDBI->GetModuleInfo(i);

        CPdbCompilandStream* pStream = PdbReader.GetCompilandStream(pModuleInfo);

        if(pModuleInfo->m_Info.wStreamNumber==65535)
        {
            TEST_ASSERT(!pStream);
            continue;
        }

        TEST_ASSERT(pStream);
        TEST_ASSERT(pStream->IsInitialized());
    }

    __TEST_CLEANUP__;
}

void PdbCompilandStreamTests::Test_GetFileNames()
{
    CPdbReader PdbReader;
    int nModuleCount = 0;
    int i;
    CPdbDebugInfoStream* pDBI = NULL;
    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    //sFileName += L"CrashRptd.pdb";

    // Init - assume success
    BOOL bInit2 = PdbReader.Init(sFileName.c_str());
    TEST_ASSERT(bInit2);

    pDBI = PdbReader.GetDebugInfoStream();
    TEST_ASSERT(pDBI);

    nModuleCount = pDBI->GetModuleCount();
    for(i=0; i<nModuleCount; i++)
    {
        DBI_ModuleInfo* pModuleInfo = pDBI->GetModuleInfo(i);

        CPdbCompilandStream* pStream = PdbReader.GetCompilandStream(pModuleInfo);

        if(pModuleInfo->m_Info.wStreamNumber==65535)
        {
            TEST_ASSERT(!pStream);
            continue;
        }

        TEST_ASSERT(pStream);
        TEST_ASSERT(pStream->IsInitialized());

        int nCount = pStream->GetSymbolLineCount();
        int j;
        for(j=0; j<nCount; j++)
        {
            SymbolLines* pLines = pStream->GetSymbolLines(j);
            int nCheckSumIndex = pStream->GetSrcFileCheckSumIndexByOffs(pLines->m_Header.dwSrcCheckSumOffs);
            SrcChecksum* pChecksum = pStream->GetSrcFileCheckSum(nCheckSumIndex);
            TEST_ASSERT(pChecksum);
            int nFileNameIndex = pDBI->FindSrcFileNameByModule(i, nCheckSumIndex);
            TEST_ASSERT(nFileNameIndex>=0);
        }
    }

    __TEST_CLEANUP__;
}
