#include "stdafx.h"
#include "Tests.h"
#include "Utils.h"
#include "MiniDumpReader.h"
#include "StackWalker.h"
#include "Thread.h"
#include "FileFinder.h"
#include "CrashReportReader.h"
#include "strconv.h"

class StackWalkerTests : public CTestCase
{
    BEGIN_TEST_MAP(StackWalkerTests, "CStackWalker class tests")		
		REGISTER_TEST(Test_GetCallStack);
        REGISTER_TEST(Test_GetCallStack_x64);				
		REGISTER_TEST(Test_GetCallStack_fpo);
		REGISTER_TEST(Test_GetCallStack_multithreaded);				
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

	void Test_GetCallStack();
	void Test_GetCallStack_multithreaded();
    void Test_GetCallStack_x64();
	void DoStackWalk(bool bInWorkerThread);
	void Test_GetCallStack_fpo();

	std::vector<std::wstring> m_asCrashReports;
	CPdbCache* m_pPdbCache;
};


REGISTER_TEST_CASE( StackWalkerTests );

void StackWalkerTests::SetUp()
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );

	m_pPdbCache = NULL;
}

void StackWalkerTests::TearDown()
{
}

void StackWalkerTests::Test_GetCallStack()
{
    wstring sFileName = Utils::GetTestDataFolder();
#ifdef _WIN32
	sFileName += L"vs2010\\2\\crashdump.dmp";
#else
    sFileName += L"vs2010/2/crashdump.dmp";
#endif

    CMiniDumpReader MdmpReader;
    CStackWalker StackWalker;
    CPdbCache PdbCache;
    BOOL bFrame = FALSE;
    BOOL bInitStackWalker = FALSE;
    BOOL bReadDump = FALSE;
    MiniDumpExceptionInfo* pExcInfo = NULL;
    DWORD dwExcThreadId = 0;
    int nFrame = 0;

	// Set the directory where to look for PDB files - assume success
    bool bAddSearchDir = PdbCache.AddPdbSearchDir(Utils::GetTestDataFolder(), PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir);

    // Open minidump
    bReadDump = MdmpReader.Init(sFileName);
    TEST_ASSERT(bReadDump);

    // Get exception thread id
    pExcInfo = MdmpReader.GetExceptionInfo();
    dwExcThreadId = pExcInfo->m_uThreadId;

    // Init stack walker
    bInitStackWalker = StackWalker.Init(&MdmpReader, &PdbCache, dwExcThreadId);
    TEST_ASSERT(bInitStackWalker);

    bFrame = StackWalker.FirstStackFrame();
    while(bFrame)
    {
        CStackFrame* pStackFrame = StackWalker.GetStackFrame();
        if(!pStackFrame)
            break;

		if(nFrame==0)
		{
			TEST_ASSERT(pStackFrame->m_sUndecoratedSymbolName=="crEmulateCrash()");
			TEST_ASSERT(pStackFrame->m_nSrcLineNumber==828);
			TEST_ASSERT(pStackFrame->m_sModuleName==L"CrashRpt1400.dll");
		}

		if(nFrame==25)
		{
			TEST_ASSERT(pStackFrame->m_sUndecoratedSymbolName=="__tmainCRTStartup()");
			TEST_ASSERT(pStackFrame->m_sSrcFileName==L"f:\\dd\\vctools\\crt_bld\\self_x86\\crt\\src\\crtexe.c");
			TEST_ASSERT(pStackFrame->m_nSrcLineNumber==547);
			TEST_ASSERT(pStackFrame->m_sModuleName==L"WTLDemo.exe");
		}

		nFrame++;
        bFrame = StackWalker.NextStackFrame();
    }

	// Ensure only one frame
	TEST_ASSERT(nFrame==29);

    __TEST_CLEANUP__;
}

void StackWalkerTests::Test_GetCallStack_x64()
{
    wstring sFileName = Utils::GetTestDataFolder();
#ifdef _WIN32
	sFileName += L"vs2010\\x64\\2\\crashdump.dmp";
#else
    sFileName += L"vs2010/x64/2/crashdump.dmp";
#endif

    CMiniDumpReader MdmpReader;
    CStackWalker StackWalker;
    CPdbCache PdbCache;
    BOOL bFrame = FALSE;
    BOOL bInitStackWalker = FALSE;
    BOOL bReadDump = FALSE;
    MiniDumpExceptionInfo* pExcInfo = NULL;
    DWORD dwExcThreadId = 0;
    int nFrame = 0;

	// Set the directory where to look for PDB files - assume success
    bool bAddSearchDir = PdbCache.AddPdbSearchDir(Utils::GetTestDataFolder(), PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir);

    // Open minidump
    bReadDump = MdmpReader.Init(sFileName);
    TEST_ASSERT(bReadDump);

    // Get exception thread id
    pExcInfo = MdmpReader.GetExceptionInfo();
    dwExcThreadId = pExcInfo->m_uThreadId;

    // Init stack walker
    bInitStackWalker = StackWalker.Init(&MdmpReader, &PdbCache, dwExcThreadId);
    TEST_ASSERT(bInitStackWalker);

    bFrame = StackWalker.FirstStackFrame();
    while(bFrame)
    {
        CStackFrame* pStackFrame = StackWalker.GetStackFrame();
        if(!pStackFrame)
            break;

		if(nFrame==0)
		{
			TEST_ASSERT(pStackFrame->m_sSymbolName=="crEmulateCrash");
		}

		nFrame++;
        bFrame = StackWalker.NextStackFrame();
    }

	// Ensure only one frame
	TEST_ASSERT(nFrame==2);

    __TEST_CLEANUP__;
}

class CStackWalkerTestThread : public CThread
{
public:

	long ThreadProc(void* pParam)
	{
		StackWalkerTests* pStackWalkerTests = (StackWalkerTests*)pParam;

		pStackWalkerTests->DoStackWalk(true);

		return 0;
	}
};

void StackWalkerTests::Test_GetCallStack_multithreaded()
{
	CFileFinder FileFinder;
	CFindFileInfo ffi;
	wstring sFileName = Utils::GetTestDataFolder();
	sFileName += L"crashReports\\WTLDemo\\1.4.0.0\\*.zip";
	bool bFind = false;
	CStackWalkerTestThread WorkerThread;
	CPdbCache PdbCache;
	std::wstring sSearchDir;
	bool bAddSearchDir = false;
	bool bSetMaxCount = false;
	PdbCacheStat Stat;
	bool bGetStat = false;

	// Set cache size
	const int MAX_CACHE_SIZE = 5;
	bSetMaxCount = PdbCache.SetMaxEntryCount(MAX_CACHE_SIZE);
	TEST_ASSERT(bSetMaxCount);

	// Add valid search dir - assume success
	sSearchDir = Utils::GetTestDataFolder();
	sSearchDir += L"debugInfo";
	bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_SYMBOL_STORE, true);
    TEST_ASSERT(bAddSearchDir==true);

	m_pPdbCache = &PdbCache;

	// Get the list of crash reports in the directory
	bFind = FileFinder.FindFirstFile(sFileName, &ffi);
	while(bFind)
	{
		m_asCrashReports.push_back(ffi.m_sFileName);
		bFind = FileFinder.FindNextFile(&ffi);
	}

	// Ensure some files found
	TEST_ASSERT(m_asCrashReports.size()>=10);

	// Run worker thread
	WorkerThread.Run(this);

	// Do our work
	DoStackWalk(false);

	// Wait until thread exists
	WorkerThread.Wait();

	// Ensure all cache entries are unreferenced
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount==Stat.m_nUnrefCount);

	__TEST_CLEANUP__;
}

void StackWalkerTests::DoStackWalk(bool bInWorkerThread)
{
	int nAttempt;
	for(nAttempt=0; nAttempt<100; nAttempt++)
	{
		CCrashReportReader CrashRptReader;
		CMiniDumpReader* pMiniDump = NULL;
		CStackWalker StackWalker;
		BOOL bSymbolsFound = FALSE;
		BOOL bInit = FALSE;

		// Choose report randomly
		int n = rand()%m_asCrashReports.size();
		std::wstring sFileName = m_asCrashReports[n];

		if(bInWorkerThread)
			printf(",%d", n);
		else
			printf(".%d", n);

		// Init report
		bInit = CrashRptReader.Init(sFileName);
		TEST_ASSERT_MSG(bInit, "Error opening crash report %s: %s",
			strconv::w2a(sFileName).c_str(),
			strconv::w2a(CrashRptReader.GetErrorMsg()).c_str());

		pMiniDump = CrashRptReader.GetMiniDumpReader();
		TEST_ASSERT(pMiniDump);

		// Walk through threads
		int i;
		for(i=0; i<pMiniDump->GetThreadCount(); i++)
		{
			MiniDumpThreadInfo* pThread = pMiniDump->GetThreadInfo(i);

			// Read stack trace for this thread
			bool bInit = StackWalker.Init(pMiniDump, m_pPdbCache, pThread->m_uThreadId);
			if(bInit)
			{
				BOOL bGetFrame = StackWalker.FirstStackFrame();
				while(bGetFrame)
				{
					CStackFrame* pStackFrame = StackWalker.GetStackFrame();
					if(!pStackFrame)
						break;

					if(pStackFrame->m_sSymbolName.length()!=0)
						bSymbolsFound = TRUE;

					bGetFrame = StackWalker.NextStackFrame();
				}
			}
		}

		// Ensure symbols found
		TEST_ASSERT(bSymbolsFound);


		// Destroy report
		CrashRptReader.Destroy();
	}

	__TEST_CLEANUP__;
}

void StackWalkerTests::Test_GetCallStack_fpo()
{
	std::wstring sDir = Utils::GetTestDataFolder();
    wstring sFileName = sDir;
#ifdef _WIN32
	sFileName += L"fpo_test\\57442638-726a-4b66-9327-981cb10ecd96.zip";
#else
    sFileName += L"fpo_test/57442638-726a-4b66-9327-981cb10ecd96.zip";
#endif

    CCrashReportReader CrashReportReader;
    CStackWalker StackWalker;
    CPdbCache PdbCache;
    BOOL bFrame = FALSE;
    BOOL bInitStackWalker = FALSE;
    BOOL bRead = FALSE;
    MiniDumpExceptionInfo* pExcInfo = NULL;
    DWORD dwExcThreadId = 0;
    int nFrame = 0;
	CMiniDumpReader* pMdmpReader = NULL;
	
	// Set the directory where to look for PDB files - assume success
    bool bAddSearchDir = PdbCache.AddPdbSearchDir(sDir, PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir);

    // Open crash report
    bRead = CrashReportReader.Init(sFileName);
    TEST_ASSERT(bRead);

	// Get minidump
	pMdmpReader = CrashReportReader.GetMiniDumpReader();
	TEST_ASSERT(pMdmpReader);

    // Get exception thread id
    pExcInfo = pMdmpReader->GetExceptionInfo();
	TEST_ASSERT(pExcInfo);
    dwExcThreadId = pExcInfo->m_uThreadId;

    // Init stack walker
    bInitStackWalker = StackWalker.Init(pMdmpReader, &PdbCache, dwExcThreadId);
    TEST_ASSERT(bInitStackWalker);

    bFrame = StackWalker.FirstStackFrame();
    while(bFrame)
    {
        CStackFrame* pStackFrame = StackWalker.GetStackFrame();
        if(!pStackFrame)
            break;

		if(nFrame==0)
		{
			TEST_ASSERT(pStackFrame->m_sUndecoratedSymbolName=="function_2()");
			TEST_ASSERT(pStackFrame->m_nSrcLineNumber==12);
			TEST_ASSERT(pStackFrame->m_sModuleName==L"fpo_test.exe");
		}

		if(nFrame==2)
		{
			TEST_ASSERT(pStackFrame->m_sUndecoratedSymbolName=="wmain()");
			TEST_ASSERT(pStackFrame->m_sSrcFileName==L"d:\\projects\\fpo_test\\fpo_test\\fpo_test.cpp");
			TEST_ASSERT(pStackFrame->m_nSrcLineNumber==58);
			TEST_ASSERT(pStackFrame->m_sModuleName==L"fpo_test.exe");
		}

		nFrame++;
        bFrame = StackWalker.NextStackFrame();
    }

	// Ensure frame count is correct
	TEST_ASSERT(nFrame==8);

    __TEST_CLEANUP__;
}

