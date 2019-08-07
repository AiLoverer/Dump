#include "stdafx.h"
#include "Tests.h"
#include "PdbCache.h"
#include "Utils.h"
#include "Thread.h"
#include "strconv.h"
#include "Misc.h"

class PdbCacheTests : public CTestCase
{
    BEGIN_TEST_MAP(PdbCacheTests, "CPdbCache class tests")
		REGISTER_TEST(Test_FindPdb_not_exact_age);
		REGISTER_TEST(Test_DeleteCachedFile_multithreaded);
        REGISTER_TEST(Test_SetMaxEntryCount);
		REGISTER_TEST(Test_AddSearchDir);
		REGISTER_TEST(Test_Clear);
		REGISTER_TEST(Test_FindPdb);
		REGISTER_TEST(Test_FindPdb_in_sym_store);
		REGISTER_TEST(Test_FindPdb_in_sym_store_no_file);
		REGISTER_TEST(Test_FindPdb_multithreaded_normal);
		REGISTER_TEST(Test_FindPdb_multithreaded_low);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

	void Test_SetMaxEntryCount();
	void Test_AddSearchDir();
    void Test_FindPdb();
	void Test_FindPdb_multithreaded_low();
	void Test_FindPdb_multithreaded_normal();
	void Test_FindPdb_in_sym_store();
	void Test_FindPdb_in_sym_store_no_file();
	void Test_FindPdb_not_exact_age();
    void Test_Clear();
	void Test_DeleteCachedFile_multithreaded();

	void DoFindPdb(bool bInWorkerThread);
	void DoDeleteCachedFile();

	CPdbCache* m_pPdbCache;
	int m_nThread;
	bool m_bStopThread;
};


REGISTER_TEST_CASE( PdbCacheTests );

void PdbCacheTests::SetUp()
{
	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );

	m_pPdbCache = NULL;
	m_nThread = 0;
	m_bStopThread = false;
}

void PdbCacheTests::TearDown()
{
}

void PdbCacheTests::Test_SetMaxEntryCount()
{
	bool bSet = false;
	int nEntryCount = 0;
	CPdbCache PdbCache;

	// Default entry count is 100
	nEntryCount = PdbCache.GetMaxEntryCount();
	TEST_ASSERT(nEntryCount==100);

	// Set negative - assume failure
	bSet = PdbCache.SetMaxEntryCount(-1);
	TEST_ASSERT(bSet==false);

	// Set 0 - assume failure
	bSet = PdbCache.SetMaxEntryCount(0);
	TEST_ASSERT(bSet==false);

	// Set 10 - assume success
	bSet = PdbCache.SetMaxEntryCount(5);
	TEST_ASSERT(bSet==true);

	// Set 2000 - assume failure
	bSet = PdbCache.SetMaxEntryCount(2000);
	TEST_ASSERT(bSet==false);

	// Ensure entry count is still 5
	nEntryCount = PdbCache.GetMaxEntryCount();
	TEST_ASSERT(nEntryCount==5);


	__TEST_CLEANUP__;
}

void PdbCacheTests::Test_AddSearchDir()
{
	bool bAddSearchDir = false;
	bool bAddSearchDir2 = false;
	CPdbCache PdbCache;
	std::wstring sSearchDir;

	// Add not existing search dir - assume failure
	sSearchDir = L"abcdefg";
	bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir==false);

	// Add valid search dir - assume success
	sSearchDir = Utils::GetTestDataFolder();
	bAddSearchDir2 = PdbCache.AddPdbSearchDir(sSearchDir, PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir2==true);

	__TEST_CLEANUP__;
}

void PdbCacheTests::Test_FindPdb()
{
    CPdbCache PdbCache;
	PdbCacheStat Stat;
    bool bAddSearchDir = false;
    std::wstring sSearchDir = Utils::GetTestDataFolder();
    CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
    bool bFind = false;
	bool bRelease = false;
    bool bRelease2 = false;
	bool bGetStat = false;

    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir);

    // Call with empty GUID - assume failure
    bFind = PdbCache.FindPdb(L"", L"crashtestd.pdb", L"crashtestd.exe", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(!bFind && !pPdbReader && nEntry==-1);

    // Call with empty file name - assume failure
    bFind = PdbCache.FindPdb(L"", L"crashtestd.pdb", L"crashtestd.exe", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(!bFind && !pPdbReader && nEntry==-1);

    // Call with correct GUID and file name - assume success
    bFind = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(bFind && pPdbReader && nEntry>=0);

	// Check count of entries in cache
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT_MSG(Stat.m_nEntryCount!=0, "check 1");
	TEST_ASSERT(Stat.m_nUnrefCount==Stat.m_nEntryCount-1);

    // Release the entry
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease);

	// Check count of entries in cache
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT_MSG(Stat.m_nEntryCount!=0, "check 2");
	TEST_ASSERT(Stat.m_nUnrefCount==Stat.m_nEntryCount);

	// Release the entry twice - should fail
	bRelease2 = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease2==false);

	// Call again with the same GUID and file name - assume success
    bFind = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(bFind && pPdbReader && nEntry>=0);

	// Release the entry
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease);

	// Check count of entries in cache
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT_MSG(Stat.m_nEntryCount!=0, "check 3");
	TEST_ASSERT(Stat.m_nUnrefCount==Stat.m_nEntryCount);

    __TEST_CLEANUP__;
}

class CPdbCacheTestThread : public CThread
{
public:

	long ThreadProc(void* pParam)
	{
		PdbCacheTests* pPdbCacheTests = (PdbCacheTests*)pParam;

		if(pPdbCacheTests->m_nThread==0)
			pPdbCacheTests->DoFindPdb(true);
		else
			pPdbCacheTests->DoDeleteCachedFile();

		return 0;
	}
};

void PdbCacheTests::Test_FindPdb_multithreaded_low()
{
	CPdbCache PdbCache;
	PdbCacheStat Stat;
    bool bAddSearchDir = false;
    std::wstring sSearchDir = Utils::GetTestDataFolder();
	bool bSetMaxCount = false;
	bool bSetMaxMem = false;
	CPdbCacheTestThread WorkerThread;
	bool bGetStat = false;

	// Set low cache size
	const int MAX_CACHE_SIZE = 3;
	bSetMaxCount = PdbCache.SetMaxEntryCount(MAX_CACHE_SIZE);
	TEST_ASSERT(bSetMaxCount);

	// Set low mem usage
	bSetMaxMem = PdbCache.SetMaxMemUsage(1*1024/*10 MB */);
	TEST_ASSERT(bSetMaxMem);

	// Add search dir
    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir);

	m_pPdbCache = &PdbCache;

	// Run worker thread
	m_nThread = 0;
	WorkerThread.Run(this);

	// Do our work
	DoFindPdb(false);

	// Wait until thread exists
	WorkerThread.Wait();

	// Check that all entries are unreferenced
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount<=MAX_CACHE_SIZE);

	__TEST_CLEANUP__;
}

void PdbCacheTests::DoFindPdb(bool bInWorkerThread)
{
	bool bFind = false;
	CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
	bool bRelease = false;
	CPdbHeadersStream* pHeaders = NULL;
	std::wstring sGUIDnAge;

	struct _GUIDnName
	{
		const wchar_t* szGUID;
		const wchar_t* szPdbName;
		const wchar_t* szPeName;
	};

	const int GUID_COUNT = 4;
	_GUIDnName aGUIDs[GUID_COUNT] =
	{
		{L"4e57fa72-8ce7-40bb-bfd3-74f2a5ba67392", L"crashtestd.pdb", L"crashtestd.exe"}, // not existing
		{L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll"},
		{L"8845b7ae-9aca-413d-8342-99eda7b0132c1", L"CrashRptd.pdb", L"CrashRptd.dll"},
		{L"708b7f83-b587-4d05-870a-28de8dec883b2", L"CrashRptTest.pdb", L"CrashRptTest.exe"}

	};

	const int TRIAL_COUNT = 100;
	int i;
	for(i=0; i<TRIAL_COUNT; i++)
	{
		// Choose GUID randomly
		int n = rand() % GUID_COUNT;

		if(bInWorkerThread)
			printf(",%d", n);
		else
			printf(".%d", n);
        fflush(stdout);

		// Call with correct GUID and file name - assume success
		bFind = m_pPdbCache->FindPdb(aGUIDs[n].szGUID, aGUIDs[n].szPdbName, aGUIDs[n].szPeName, &pPdbReader, &pPeReader, &nEntry);
		if(n==0)
		{
			TEST_ASSERT(!bFind && !pPdbReader && nEntry==-1);
		}
		else
		{
			TEST_ASSERT_MSG(bFind && pPdbReader && nEntry>=0, "Error find cache entry %s %s",
				strconv::w2a(aGUIDs[n].szGUID).c_str(), strconv::w2a(aGUIDs[n].szPdbName).c_str());

			// Do something with it
			pHeaders = pPdbReader->GetHeadersStream();
			TEST_ASSERT(pHeaders);
			sGUIDnAge = pHeaders->GetGUIDnAge();
			TEST_ASSERT(sGUIDnAge==aGUIDs[n].szGUID);

			// Release the entry
			bRelease = m_pPdbCache->ReleaseCacheEntry(nEntry);
			TEST_ASSERT(bRelease);
		}
	}

	__TEST_CLEANUP__;
}

void PdbCacheTests::Test_FindPdb_multithreaded_normal()
{
	CPdbCache PdbCache;
	PdbCacheStat Stat;
    bool bAddSearchDir = false;
    std::wstring sSearchDir = Utils::GetTestDataFolder();
	bool bSetMaxCount = false;
	CPdbCacheTestThread WorkerThread;
	bool bGetStat = false;

	// Set normal cache size
	const int MAX_CACHE_SIZE = 30;
	bSetMaxCount = PdbCache.SetMaxEntryCount(MAX_CACHE_SIZE);
	TEST_ASSERT(bSetMaxCount);

	// Add search dir
    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir);

	m_pPdbCache = &PdbCache;

	// Run worker thread
	m_nThread = 0;
	WorkerThread.Run(this);

	// Do our work
	DoFindPdb(false);

	// Wait until thread exists
	WorkerThread.Wait();

	// Check that all entries are unreferenced
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount<=MAX_CACHE_SIZE);

	__TEST_CLEANUP__;
}

void PdbCacheTests::Test_Clear()
{
    CPdbCache PdbCache;
	PdbCacheStat Stat;
    bool bAddSearchDir = false;
    std::wstring sSearchDir = Utils::GetTestDataFolder();
    CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
	CPdbReader* pPdbReader2 = NULL;
    CPeReader* pPeReader2 = NULL;
	int nEntry2 = 0;
    bool bFind = false;
	bool bGetStat = false;
	bool bRelease = false;
	bool bFind2 = false;
	bool bSetMaxCount = false;

    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_USUAL_DIR, true);
    TEST_ASSERT(bAddSearchDir);

	// Set low cache size
	bSetMaxCount = PdbCache.SetMaxEntryCount(1);
	TEST_ASSERT(bSetMaxCount);

    // Find PDB by GUID+Age - assume success
    bFind = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(bFind && pPdbReader && nEntry>=0);

	// Find another PDB by GUID+Age - assume success
    bFind2 = PdbCache.FindPdb(L"708b7f83-b587-4d05-870a-28de8dec883b2", L"CrashRptTest.pdb", L"CrashRptTest.exe", &pPdbReader2, &pPeReader2, &nEntry2);
    TEST_ASSERT(bFind2 && pPdbReader2 && nEntry2>=0);

	// Get entry count - assume 2
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount==2);
	TEST_ASSERT(Stat.m_nUnrefCount==0);

	// Release the entry
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease);

	// Get entry count
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount==1);
	TEST_ASSERT(Stat.m_nUnrefCount==0);

	// Clear cache
    PdbCache.Clear();

	// Get entry count - assume 0
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount==0);
	TEST_ASSERT(Stat.m_nUnrefCount==0);

	// Try to release the entry - should fail
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(!bRelease);

    __TEST_CLEANUP__;
}

void PdbCacheTests::Test_FindPdb_in_sym_store()
{
	CPdbCache PdbCache;
	bool bAddSearchDir = false;
    std::wstring sSearchDir;
    CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
	CPdbReader* pPdbReader2 = NULL;
    CPeReader* pPeReader2 = NULL;
	int nEntry2 = 0;
    bool bFind = false;
	bool bRelease = false;
	bool bFind2 = false;
	PdbCacheStat Stat;
	bool bGetStat = false;

	sSearchDir = Utils::GetTestDataFolder();
#ifdef WIN32
    sSearchDir += L"\\debugInfo";
#else
    sSearchDir += L"/debugInfo";
#endif

	// Add symbol store dir - assume success
    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_SYMBOL_STORE, false);
    TEST_ASSERT(bAddSearchDir);

    // Find PDB by GUID+Age - assume success
    bFind = PdbCache.FindPdb(L"563d79cb-74dd-4193-9b88-70200b7baf513", L"crashcon.pdb", L"crashcon.exe", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(bFind && pPdbReader && nEntry>=0);

	// Find another PDB by GUID+Age - assume success
    bFind2 = PdbCache.FindPdb(L"6dbf5b63-6c53-4cc1-a9c4-5a97a7c66cad1", L"CrashRptd.pdb", L"CrashRptd.dll", &pPdbReader2, &pPeReader2, &nEntry2);
    TEST_ASSERT(bFind2 && pPdbReader2 && nEntry2>=0);

	// Release the entry
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease);

	// Get entry count
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount==2);
	TEST_ASSERT(Stat.m_nUnrefCount==1);

	__TEST_CLEANUP__;
}

void PdbCacheTests::Test_FindPdb_in_sym_store_no_file()
{
	CPdbCache PdbCache;
	bool bAddSearchDir = false;
    std::wstring sSearchDir;
    CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
    bool bFind = false;
	bool bFind2 = false;
	std::wstring sSrcFile;
	std::wstring sDstFile;
	int nCreateDir = -1;
	bool bCopy = false;

	sSearchDir = Utils::GetTestDataFolder();
#ifdef WIN32
    sSearchDir += L"\\debugInfo";
#else
    sSearchDir += L"/debugInfo";
#endif

	// Add symbol store dir - assume success
    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_SYMBOL_STORE, false);
    TEST_ASSERT(bAddSearchDir);

    // Try to find PDB by GUID+Age when such a file is not in symbol store - assume failure
    bFind = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(!bFind && pPdbReader==NULL && nEntry==-1);

	// Copy the file into symbol store
#ifdef _WIN32
#define SLASHW L"\\"
#else
#define SLASHW L"/"
#endif

	sSrcFile += Utils::GetTestDataFolder() + SLASHW L"CrashRpt.pdb";

	sDstFile += sSearchDir + SLASHW L"CrashRpt.pdb";
	nCreateDir = CreateDir(sDstFile.c_str());
	TEST_ASSERT(nCreateDir==0 || errno==EEXIST);

	sDstFile += SLASHW L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1";
	nCreateDir = CreateDir(sDstFile.c_str());
	TEST_ASSERT(nCreateDir==0 || errno==EEXIST);

	sDstFile += SLASHW L"CrashRpt.pdb";
	bCopy = copy_file(sSrcFile, sDstFile, true);
	TEST_ASSERT(bCopy);

	// Try to find PDB by GUID+Age - assume success
    bFind2 = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll",
		&pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(bFind2 && pPdbReader && nEntry>=0);

	__TEST_CLEANUP__;

	// This will unlock the file
	PdbCache.Clear();

	// Delete file
	RemoveFile(sDstFile);
}

void PdbCacheTests::Test_DeleteCachedFile_multithreaded()
{
	CPdbCache PdbCache;
	PdbCacheStat Stat;
    bool bAddSearchDir = false;
    std::wstring sSearchDir;
	bool bSetMaxCount = false;
	CPdbCacheTestThread WorkerThread;
	bool bGetStat = false;
	bool bFind = false;
	bool bFind2 = false;
	std::wstring sSrcFile;
	std::wstring sDstFile;
	std::wstring sDstDir;
	CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
	int nCreateDir = -1;
	bool bCopy = false;
	bool bRelease = false;
	CPdbHeadersStream* pHeaders = NULL;
	std::wstring sGUIDnAge;
	bool bDelete = false;
	const int TRIAL_COUNT = 50;
	int i = 0;
	int n = 0;
	int nAttempt = 0;
	std::string sErrorMsg;

	sSearchDir = Utils::GetTestDataFolder();
    sSearchDir += L"debugInfo";

	// Set cache size
	const int MAX_CACHE_SIZE = 4;
	bSetMaxCount = PdbCache.SetMaxEntryCount(MAX_CACHE_SIZE);
	TEST_ASSERT(bSetMaxCount);

	// Add search dir
    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_SYMBOL_STORE, true);
    TEST_ASSERT(bAddSearchDir);

	m_pPdbCache = &PdbCache;

	// Run worker thread
	m_nThread = 1;
	WorkerThread.Run(this);

	// Do our work
	for(i=0; i<TRIAL_COUNT; i++)
	{
		printf(".");
		fflush(stdout);

		// Copy the file into symbol store
#ifdef _WIN32
#define SLASHW L"\\"
#else
#define SLASHW L"/"
#endif

		sSrcFile = Utils::GetTestDataFolder() + SLASHW L"CrashRpt.pdb";

		sDstFile = sSearchDir + SLASHW L"CrashRpt.pdb";
		nCreateDir = CreateDir(sDstFile.c_str());
		TEST_ASSERT(nCreateDir==0 || errno==EEXIST);

		sDstFile += SLASHW L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1";
		sDstDir = sDstFile;
		
		sDstFile += SLASHW L"CrashRpt.pdb";
		for(nAttempt=0; nAttempt<30; nAttempt++)
		{
			// Remove dir if already exists
			RmDir(sDstDir.c_str(), true);
			nCreateDir = CreateDir(sDstDir.c_str());
			TEST_ASSERT(nCreateDir==0 || errno==EEXIST);

			// Copy the file
			bCopy = copy_file(sSrcFile, sDstFile, true);
			if(bCopy)
				break;
			// Copy may fail if another thread is currently using the cache
			// entry.
			// Wait for a while and try again
			Sleep(100);
		}
		TEST_ASSERT(bCopy);

		// Try to find PDB by GUID+Age - assume success
		bFind2 = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll", &pPdbReader, &pPeReader, &nEntry, &sErrorMsg);

#ifdef _WIN32
		TEST_ASSERT_MSG(bFind2 && pPdbReader && nEntry>=0, "Find error: %s", sErrorMsg.c_str());
#else
		// In Linux, the line above may fail because of PDB file inode still being used by another thread.
		// We need to performs several attempts in such a case.
		if(!bFind2)
		{
            for(nAttempt=0; nAttempt<30; nAttempt++)
            {
                // Try again
                bFind2 = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll", &pPdbReader, &pPeReader, &nEntry, &sErrorMsg);		if(bCopy)
                if(bFind2)
                    break;
                // Wait for a while
                Sleep(100);
            }
		}
		TEST_ASSERT_MSG(bFind2 && pPdbReader && nEntry>=0, "find error (on attempt %d): %s", nAttempt, sErrorMsg.c_str());
#endif

		// Do something with it
		pHeaders = pPdbReader->GetHeadersStream();
		TEST_ASSERT(pHeaders);
		sGUIDnAge = pHeaders->GetGUIDnAge();
		TEST_ASSERT(sGUIDnAge==L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1");

		// Use one of two scenarios
		n = rand() % 2;
		if(n==0)
		{
			// Release the entry
			bRelease = m_pPdbCache->ReleaseCacheEntry(nEntry);
			TEST_ASSERT(bRelease);

			// Delete file
			bDelete = m_pPdbCache->DeleteCachedFile(sDstFile, true);
			TEST_ASSERT(bDelete);
		}
		else // Do the same in reverse order
		{
			// Delete file
			bDelete = m_pPdbCache->DeleteCachedFile(sDstFile, true);
			TEST_ASSERT(bDelete);

			// Release the entry
			bRelease = m_pPdbCache->ReleaseCacheEntry(nEntry);
			TEST_ASSERT(bRelease);
		}

		// Try to find PDB by GUID+Age when such a file is not in symbol store - assume failure
		bFind = PdbCache.FindPdb(L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll", &pPdbReader, &pPeReader, &nEntry);
		TEST_ASSERT_MSG(!bFind && pPdbReader==NULL && nEntry==-1, "Expected that initially PDB could not be found (not in cache), but it was found!");
	}

	// Wait until thread exists
	m_bStopThread = true;
	WorkerThread.Wait();

	// Check that all entries are unreferenced
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount<=MAX_CACHE_SIZE);

	__TEST_CLEANUP__;

	// Wait until thread exists
	m_bStopThread = true;
	WorkerThread.Wait();

	// This will unlock the file
	PdbCache.Clear();

	// Delete file
	RemoveFile(sDstFile);
}

void PdbCacheTests::DoDeleteCachedFile()
{
	bool bFind = false;
	CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
	bool bRelease = false;
	CPdbHeadersStream* pHeaders = NULL;
	std::wstring sGUIDnAge;

	struct _GUIDnName
	{
		const wchar_t* szGUID;
		const wchar_t* szPdbName;
		const wchar_t* szPeName;
	};

	const int GUID_COUNT = 3;
	_GUIDnName aGUIDs[GUID_COUNT] =
	{
		{L"1ec1af16-e895-4cb2-b80d-c7b71d2ae9aa1", L"CrashRpt.pdb", L"CrashRpt.dll"},
		{L"6dbf5b63-6c53-4cc1-a9c4-5a97a7c66cad1", L"CrashRptd.pdb", L"CrashRptd.dll"},
		{L"563d79cb-74dd-4193-9b88-70200b7baf513", L"crashcon.pdb", L"crashcon.exe"}
	};

	for(;;)
	{
		if(m_bStopThread)
			return;

		// Sleep for a while
		int nSleepTime = rand() % 200;
		Sleep(nSleepTime);

		// Choose GUID randomly
		int n = rand() % GUID_COUNT;

		printf(",%d", n);
		fflush(stdout);

		// Call with correct GUID and file name - assume success
		bFind = m_pPdbCache->FindPdb(aGUIDs[n].szGUID, aGUIDs[n].szPdbName, aGUIDs[n].szPeName, &pPdbReader, &pPeReader, &nEntry);
		if(n>0)
		{
			TEST_ASSERT_MSG(bFind && pPdbReader && nEntry>=0, "Error find cache entry %s %s",
				strconv::w2a(aGUIDs[n].szGUID).c_str(), strconv::w2a(aGUIDs[n].szPdbName).c_str());
		}

		if(bFind)
		{
			// Do something with it
			pHeaders = pPdbReader->GetHeadersStream();
			TEST_ASSERT(pHeaders);
			sGUIDnAge = pHeaders->GetGUIDnAge();
			TEST_ASSERT(sGUIDnAge==aGUIDs[n].szGUID);

			// Release the entry
			bRelease = m_pPdbCache->ReleaseCacheEntry(nEntry);
			TEST_ASSERT(bRelease);
		}
	}

	__TEST_CLEANUP__;
}

void PdbCacheTests::Test_FindPdb_not_exact_age()
{
	CPdbCache PdbCache;
	bool bAddSearchDir = false;
    std::wstring sSearchDir;
    CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
	int nEntry = 0;
	bool bFind = false;
	bool bRelease = false;	
	PdbCacheStat Stat;
	bool bGetStat = false;

	sSearchDir = Utils::GetTestDataFolder();
#ifdef WIN32
    sSearchDir += L"\\debugInfo";
#else
    sSearchDir += L"/debugInfo";
#endif

	// Add symbol store dir - assume success
    bAddSearchDir = PdbCache.AddPdbSearchDir(sSearchDir, PDB_SYMBOL_STORE, false);
    TEST_ASSERT(bAddSearchDir);

	// Find PDB by unmatched GUID+Age - assume success
    bFind = PdbCache.FindPdb(L"563d79cb-74dd-4193-9b88-70200b7baf514", L"crashcon.pdb", L"crashcon.exe", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(!bFind && !pPdbReader && nEntry<0);

	// Find PDB by GUID (no matter what age) - assume success
    bFind = PdbCache.FindPdb(L"563d79cb-74dd-4193-9b88-70200b7baf514", L"crashcon.pdb", L"crashcon.exe", &pPdbReader, &pPeReader, &nEntry, NULL, false);
    TEST_ASSERT(bFind && pPdbReader && nEntry>=0);

	// Release the entry
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease);

    // Find PDB by correct GUID+Age - assume success
    bFind = PdbCache.FindPdb(L"563d79cb-74dd-4193-9b88-70200b7baf513", L"crashcon.pdb", L"crashcon.exe", &pPdbReader, &pPeReader, &nEntry);
    TEST_ASSERT(bFind && pPdbReader && nEntry>=0);
		
	// Release the entry
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease);

	// Find PDB by GUID twice (no matter what age) - assume success
    bFind = PdbCache.FindPdb(L"563d79cb-74dd-4193-9b88-70200b7baf514", L"crashcon.pdb", L"crashcon.exe", &pPdbReader, &pPeReader, &nEntry, NULL, false);
    TEST_ASSERT(bFind && pPdbReader && nEntry>=0);

	// Release the entry
	bRelease = PdbCache.ReleaseCacheEntry(nEntry);
	TEST_ASSERT(bRelease);

	// Get entry count
	bGetStat = PdbCache.GetCacheStat(&Stat);
	TEST_ASSERT(bGetStat);
	TEST_ASSERT(Stat.m_nEntryCount==1);
	TEST_ASSERT(Stat.m_nUnrefCount==1);

	__TEST_CLEANUP__;
}