#include "stdafx.h"
#include "Tests.h"
#include "Utils.h"
#include "PdbReader.h"
#include "Thread.h"

class PdbReaderTests : public CTestCase
{
    BEGIN_TEST_MAP(PdbReaderTests, "CPdbReader class tests")
        REGISTER_TEST(Test_Init);
		REGISTER_TEST(Test_Access_multithreaded);
        REGISTER_TEST(Test_GetStreams);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
	void Test_Access_multithreaded();
    void Test_GetStreams();

	void DoAccessPdb(bool bInWorkerThread);
	CPdbReader* m_pPdbReader;
};


REGISTER_TEST_CASE( PdbReaderTests );

void PdbReaderTests::SetUp()
{
}

void PdbReaderTests::TearDown()
{
}

void PdbReaderTests::Test_Init()
{
    CPdbReader PdbReader;
    BOOL bInit = FALSE;
    BOOL bInit2 = FALSE;
	BOOL bIsInit = FALSE;
	BOOL bAMD64 = FALSE;	

    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

	// Ensure file name is empty
	TEST_ASSERT(PdbReader.GetFileName().length()==0);

    // Init with empty file name - assume failure
    bInit = PdbReader.Init(L"");
    TEST_ASSERT(!bInit);

    // Init - assume success
    bInit2 = PdbReader.Init(sFileName.c_str());
    TEST_ASSERT(bInit2);

	// Ensure initialized
	bIsInit = PdbReader.IsInitialized();
	TEST_ASSERT(bIsInit);

	// Ensure file name is non-empty
	TEST_ASSERT(PdbReader.GetFileName().length()>0);

	// Ensure x86
	bAMD64 = PdbReader.IsAMD64();
	TEST_ASSERT(!bAMD64);

	// Destroy 
	PdbReader.Destroy();

	// Ensure not initialized
	bIsInit = PdbReader.IsInitialized();
	TEST_ASSERT(!bIsInit);

    __TEST_CLEANUP__;
}

class CPdbReaderTestThread : public CThread
{
public:

	long ThreadProc(void* pParam)
	{
		PdbReaderTests* pPdbReaderTests = (PdbReaderTests*)pParam;

		pPdbReaderTests->DoAccessPdb(true);

		return 0;
	}
};

void PdbReaderTests::Test_Access_multithreaded()
{
	wstring sFileName = Utils::GetTestDataFolder();
	sFileName += L"crashtestd.pdb";
#ifndef _WIN32
    std::replace(sFileName.begin(), sFileName.end(), '\\', '/');
#endif
		
	CPdbReaderTestThread WorkerThread;
    CPdbReader PdbReader;

    // Init - assume success
    BOOL bInit = PdbReader.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

	m_pPdbReader = &PdbReader;
	
	// Two trials
	int i;
	for(i=0; i<2; i++)
	{
		// Run worker thread
		WorkerThread.Run(this);

		// Do our work
		DoAccessPdb(false);

		// Wait until thread exists
		WorkerThread.Wait();

		// Release compiland streams
		PdbReader.ReduceMemoryConsumption();
	}
	
	// Destroy PDB
	PdbReader.Destroy();

	__TEST_CLEANUP__;
}

void PdbReaderTests::DoAccessPdb(bool bInWorkerThread)
{
	const int TRIAL_COUNT = 200;
	std::wstring sGUID;
	DWORD dwAge = 0;
	std::wstring sGUIDnAge;
	bool bGetTypeName = false;
	std::string sTypeName;
	BOOL bFind = FALSE;

	int i;
	for(i=0; i<TRIAL_COUNT; i++)
	{
		if(bInWorkerThread)
			printf(",");
		else
			printf(".");
		fflush(stdout);
	
		// Get headers stream - assume success
		CPdbHeadersStream* pHeaders = m_pPdbReader->GetHeadersStream();
		TEST_ASSERT(pHeaders);

		sGUID = pHeaders->GetGUID();
		TEST_ASSERT(sGUID==L"94738ec7-c64a-4b15-b4c6-c3d880d75728");

		dwAge = pHeaders->GetAge();
		TEST_ASSERT(dwAge==2);

		sGUIDnAge = pHeaders->GetGUIDnAge();
		TEST_ASSERT(sGUIDnAge==L"94738ec7-c64a-4b15-b4c6-c3d880d757282");

		CPdbTypeInfoStream* pTPI = m_pPdbReader->GetTypeInfoStream();
		TEST_ASSERT(pTPI);

		bGetTypeName = pTPI->GetTypeName(4096, sTypeName);
		TEST_ASSERT(bGetTypeName);
		TEST_ASSERT(sTypeName=="");
				
		CPdbDebugInfoStream* pDBI = m_pPdbReader->GetDebugInfoStream();
		TEST_ASSERT(pDBI);

		bFind = pDBI->FindSrcFileNameByModule(0, 1);
		TEST_ASSERT(!bFind)
					
		CPdbLinkInfoStream* pLNK = m_pPdbReader->GetLinkInfoStream();
		TEST_ASSERT(pLNK);

		CPdbFPOStream* pFPO = m_pPdbReader->GetFPOStream();
		TEST_ASSERT(pFPO);

		FPO_DATA_RECORD* pRec = pFPO->GetFPORecord(0);
		TEST_ASSERT(pRec!=NULL && pRec->fpo.fHasSEH==1);

		CPdbFPOStream* pNewFPO = m_pPdbReader->GetNewFPOStream();
		TEST_ASSERT(pNewFPO);

		FPO_DATA_RECORD* pRec2 = pNewFPO->GetFPORecord(0);
		TEST_ASSERT(pRec2!=NULL && pRec2->fpo2.cbProlog==61);

		CPdbGSIStream* pGSI = m_pPdbReader->GetGSIStream();
		TEST_ASSERT(pGSI);

		CPdbPSGSIStream* pPSI = m_pPdbReader->GetPSIStream();
		TEST_ASSERT(pPSI);

		
		CPdbSymbolStream* pSymbols = m_pPdbReader->GetSymbolStream();
		TEST_ASSERT(pSymbols);

		int i;
		for(i=0; i<200; i++)
		{
			int nSymbol = rand() % pSymbols->GetSymbolCount();		
			CPdbSymbol* pSymbol = pSymbols->GetSymbol(nSymbol);
			TEST_ASSERT_MSG(pSymbol, "Cannot get symbol %d in symbol stream.", nSymbol);

			pSymbol->GetName();
			pSymbol->GetUndecoratedName();
			
			pSymbol->GetLength();			
		}		
		
		CPdbSectionMapStream* pSecMap = m_pPdbReader->GetSectionMapStream();
		TEST_ASSERT(pSecMap);
		
		for(i=0; i<500; i++)
		{
			int nModule = rand() % pDBI->GetModuleCount();
			DBI_ModuleInfo* pModuleInfo = pDBI->GetModuleInfo(nModule);
			TEST_ASSERT(pModuleInfo!=NULL);

			CPdbCompilandStream* pStream = m_pPdbReader->GetCompilandStream(pModuleInfo);
			if(pModuleInfo->m_Info.wStreamNumber==65535)
			{
				TEST_ASSERT(!pStream);
				continue;
			}

			TEST_ASSERT(pStream);
			TEST_ASSERT(pStream->IsInitialized());
			
			int j;
			for(j=0; j<100; j++)
			{
				int nSymbol = rand() % pStream->GetSymbolCount();		

				CPdbSymbol* pSymbol = pStream->GetSymbol(nSymbol);
				TEST_ASSERT_MSG(pSymbol, "Cannot get symbol %d in compiland stream for module %d.", nSymbol, nModule);

				std::string sUndName = pSymbol->GetUndecoratedName();				

				/*DWORD dwSymLen = */pSymbol->GetLength();
				
			}			
		}
	}

	__TEST_CLEANUP__;
}

void PdbReaderTests::Test_GetStreams()
{
    std::vector<std::wstring> asFileNames;
    asFileNames.push_back(L"crashtestd.pdb");
    asFileNames.push_back(L"CrashRptTest.pdb");
    asFileNames.push_back(L"CrashRptd.pdb");
    asFileNames.push_back(L"CrashRpt.pdb");
    asFileNames.push_back(L"x64\\CrashRptd.pdb");
	asFileNames.push_back(L"vs2010\\CrashRptd.pdb");
	asFileNames.push_back(L"vs2010\\x64\\CrashRpt1300d.pdb");
	asFileNames.push_back(L"vs2010\\x64\\CrashRptTestd.pdb");

    UINT i;
    for(i=0; i<asFileNames.size(); i++)
    {
		printf(".");
		fflush(stdout);

        wstring sFileName = Utils::GetTestDataFolder();
        sFileName += asFileNames[i];
#ifndef _WIN32
        std::replace(sFileName.begin(), sFileName.end(), '\\', '/');
#endif

        CPdbReader PdbReader;

        // Init - assume success
        BOOL bInit = PdbReader.Init(sFileName.c_str());
        TEST_ASSERT(bInit);

        // Get headers stream - assume success
        CPdbHeadersStream* pHeaders = PdbReader.GetHeadersStream();
        TEST_ASSERT(pHeaders);

        CPdbDebugInfoStream* pDBI = PdbReader.GetDebugInfoStream();
        TEST_ASSERT(pDBI);

        CPdbSymbolStream* pSymbols = PdbReader.GetSymbolStream();
        TEST_ASSERT(pSymbols);

        int nModuleCount = pDBI->GetModuleCount();
        int i;
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
    }

    __TEST_CLEANUP__;
}
