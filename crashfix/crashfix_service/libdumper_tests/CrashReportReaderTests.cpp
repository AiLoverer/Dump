#include "stdafx.h"
#include "Tests.h"
#include "CrashReportReader.h"
#include "PdbCache.h"
#include "Utils.h"
#include "FileFinder.h"
#include "strconv.h"
#include "StackWalker.h"

class CrashReportReaderTests : public CTestCase
{
    BEGIN_TEST_MAP(CrashReportReaderTests, "CCrashReportReader class tests")
		REGISTER_TEST(Test_Init);
		REGISTER_TEST(Test_Init_no_minidump);
        REGISTER_TEST(Test_ProcessAllCrashReportsInDirectory);        
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
	void Test_Init_no_minidump();
    void Test_ProcessAllCrashReportsInDirectory();

};


REGISTER_TEST_CASE( CrashReportReaderTests );

void CrashReportReaderTests::SetUp()
{
}

void CrashReportReaderTests::TearDown()
{
}

void CrashReportReaderTests::Test_Init()
{
    CCrashReportReader reader;
    BOOL bInit = FALSE;
    BOOL bIsInitialized = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashreport.zip";

    // Check initialized status - assume false
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(!bIsInitialized);

    TEST_ASSERT(reader.GetCrashDescReader()==NULL);
    TEST_ASSERT(reader.GetMiniDumpReader()==NULL);

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

    TEST_ASSERT(reader.GetCrashDescReader()!=NULL);
    TEST_ASSERT(reader.GetMiniDumpReader()!=NULL);

    TEST_ASSERT(reader.GetCrashDescReader()->GetGeneratorVersion()==1210);

    reader.Destroy();

    TEST_ASSERT(reader.GetCrashDescReader()==NULL);
    TEST_ASSERT(reader.GetMiniDumpReader()==NULL);

    // Check initialized status - assume false
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(!bIsInitialized);

	// Init again - assume success
    bInit = reader.Init(sFileName);
    TEST_ASSERT(bInit);

    __TEST_CLEANUP__;
}

void CrashReportReaderTests::Test_Init_no_minidump()
{
	// This test opens a crash report where no minidump file presents.
	// Such crash reports should be treated as valid.

    CCrashReportReader reader;
    BOOL bInit = FALSE;
    BOOL bIsInitialized = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
#ifdef _WIN32
    sFileName += L"crashReports\\WTLDemo\\1.4.0.1\\9bf456a0-7016-4907-a5c3-d914ba8404c4.zip";
#else
	sFileName += L"crashReports/WTLDemo/1.4.0.1/9bf456a0-7016-4907-a5c3-d914ba8404c4.zip";
#endif

    // Init - assume success
    bInit = reader.Init(sFileName);
    TEST_ASSERT(bInit);

    // Check initialized status - assume success
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(bIsInitialized);

    TEST_ASSERT(reader.GetCrashDescReader()!=NULL);
	// Assume minidump is NULL
    TEST_ASSERT(reader.GetMiniDumpReader()==NULL);

    TEST_ASSERT(reader.GetCrashDescReader()->GetGeneratorVersion()==1401);

    reader.Destroy();

    TEST_ASSERT(reader.GetCrashDescReader()==NULL);
    TEST_ASSERT(reader.GetMiniDumpReader()==NULL);

    // Check initialized status - assume false
    bIsInitialized = reader.IsInitialized();
    TEST_ASSERT(!bIsInitialized);

	// Init again - assume success
    bInit = reader.Init(sFileName);
    TEST_ASSERT(bInit);

    __TEST_CLEANUP__;
}

void CrashReportReaderTests::Test_ProcessAllCrashReportsInDirectory()
{
	CFileFinder FileFinder;
	CPdbCache PdbCache;
	CFindFileInfo ffi;
	bool bAddDir = false;
	int nFilesProcessed = 0;
	bool bFound = false;

	// Format crash report file search pattern
	std::wstring sFileSearchPattern = Utils::GetTestDataFolder();
    sFileSearchPattern += L"ErrorReports/ErrorReports/*.zip";

	// Format symbol search pattern
	std::wstring sSymSearchDir = Utils::GetTestDataFolder();
    sSymSearchDir += L"ErrorReports/Symbols";

	// Init PDB cache
	bAddDir = PdbCache.AddPdbSearchDir(sSymSearchDir, PDB_USUAL_DIR, true);
	TEST_ASSERT_MSG(bAddDir, "Error setting symbol search directory %s", strconv::w2a(sSymSearchDir).c_str());

	// Walk through crash report files
	bFound = FileFinder.FindFirstFile(sFileSearchPattern, &ffi);
	while(bFound)
	{
		// Increment counter
		nFilesProcessed++;
		printf(".");
		fflush(stdout);

		CCrashReportReader reader;
		BOOL bRead = reader.Init(ffi.m_sFileName);
		//TEST_EXPECT_MSG(bRead, "Could not read crash report %s; error message = %s",
		//	strconv::w2a(ffi.m_sFileName).c_str(),
		//	strconv::w2a(reader.GetErrorMsg()).c_str());

		if(bRead)
		{
			//bool bSymbolsLoaded = false;

			// Walk through modules
			int i;
			/*for(i=0; i<reader.GetMiniDumpReader()->GetModuleCount(); i++)
			{
				MiniDumpModuleInfo* pmi = reader.GetMiniDumpReader()->GetModuleInfo(i);

				// Try to load PDB file for this module
				CPdbReader* pPdbReader = NULL;
				CPeReader* pPeReader = NULL;
				PdbCache.FindPdb(pmi->GUIDnAge(), pmi->m_sPdbFileName, pmi->m_sModuleName, &pPdbReader, &pPeReader);

				// Check if symbols loaded for this module
				if(pPdbReader)
					bSymbolsLoaded = true;
			}

			// Ensure that symbols were loaded for at least one module
			TEST_EXPECT_MSG(bSymbolsLoaded, "No symbols loaded for any stack frame for crash report %s",
				strconv::w2a(ffi.m_sFileName).c_str());*/

			// Walk through threads
			for(i=0; i<reader.GetMiniDumpReader()->GetThreadCount(); i++)
			{
				MiniDumpThreadInfo* pThread = reader.GetMiniDumpReader()->GetThreadInfo(i);
				TEST_ASSERT_MSG(pThread!=NULL, "Error getting thread information for thread 0x%x", pThread->m_uThreadId);

				// Walk the call stack
				CStackWalker StackWalker;
				bool bInitStack = StackWalker.Init(reader.GetMiniDumpReader(), &PdbCache, pThread->m_uThreadId);
				TEST_EXPECT_MSG(bInitStack, "Error reading stack trace for thread 0x%x in crash report %s",
					pThread->m_uThreadId, strconv::w2a(ffi.m_sFileName).c_str());
				if(bInitStack)
				{
					// Get first stack frame
					BOOL bGetFrame = StackWalker.FirstStackFrame();
					while(bGetFrame)
					{
						// Get current stack frame
						CStackFrame* pStackFrame = StackWalker.GetStackFrame();
						TEST_ASSERT(pStackFrame);

						if(!pStackFrame->m_sSymbolName.empty())
						{
							// Ensure undecorated name presents
							TEST_EXPECT_MSG(!pStackFrame->m_sUndecoratedSymbolName.empty(), "Undecorated symbol name is empty for symbol %s",
								pStackFrame->m_sSymbolName.c_str());
						}

						// Get next stack frame
						bGetFrame = StackWalker.NextStackFrame();
					}
				}
			}
		}

		// Look for next file
		bFound = FileFinder.FindNextFile(&ffi);
	}

	// Ensure we have processed at least 10 files
	TEST_ASSERT(nFilesProcessed>=10);

	__TEST_CLEANUP__;
}


