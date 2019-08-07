#include "stdafx.h"
#include "Tests.h"
#include "MtClient.h"
#include "Misc.h"
#include "ServiceHelpers.h"
#include "Utils.h"
#include "strconv.h"
#include "FileFinder.h"

class MultiThreadedTests : public CTestCase
{
    BEGIN_TEST_MAP(MultiThreadedTests, "Daemon stress tests")
		REGISTER_TEST(Test_DaemonStatus);		
		REGISTER_TEST(Test_DumpCrashReport_assync);		
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

	void Test_DaemonStatus();	
	void Test_DumpCrashReport_assync();	
};

REGISTER_TEST_CASE( MultiThreadedTests );


void MultiThreadedTests::SetUp()
{
	bool bStart = false;
	std::string sErrorMsg;

	// First start daemon - assume success
    bStart = StartDaemon(sErrorMsg);
    TEST_ASSERT_MSG(bStart, sErrorMsg.c_str());

	__TEST_CLEANUP__;
}

void MultiThreadedTests::TearDown()
{
	std::string sErrorMsg;
	StopDaemon(sErrorMsg);
}

void MultiThreadedTests::Test_DaemonStatus()
{    
    bool bRun1 = false;
    bool bRun5 = false;
    bool bRun20 = false;
    bool bRun50 = false;
    bool bStop = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;

    client.AddRequest("daemon status\n", 0);

    // Run tests with 1 client thread
    bRun1 = client.Run(1, 1000, sErrorMsg);
    TEST_ASSERT_MSG(bRun1, sErrorMsg.c_str());

	// Run tests with 5 client threads
    bRun5 = client.Run(5, 2000, sErrorMsg);
    TEST_ASSERT_MSG(bRun5, sErrorMsg.c_str());

	// Run tests with 20 client threads
    bRun20 = client.Run(20, 3000, sErrorMsg);
    TEST_ASSERT_MSG(bRun20, sErrorMsg.c_str());

	// Run tests with 50 client threads
    bRun50 = client.Run(50, 5000, sErrorMsg);
    TEST_ASSERT_MSG(bRun50, sErrorMsg.c_str());
	
	// Run tests with 5 client threads for 2 min
    bRun5 = client.Run(5, 5000, sErrorMsg);
    TEST_ASSERT_MSG(bRun5, sErrorMsg.c_str());

    __TEST_CLEANUP__;    
}

void MultiThreadedTests::Test_DumpCrashReport_assync()
{    
    bool bRun1 = false;
    bool bRun5 = false;
	bool bRun10 = false;
    bool bRun20 = false;
    bool bRun50 = false;
    bool bStop = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;
	std::wstring sFileName;
	std::string sOutFile;	
	CFileFinder FileFinder;
	CFindFileInfo ffi;
	bool bFind = false;
	std::vector<std::wstring> asCrashReports;
	std::wstring sTmpDir;
	int nCreateDir = -1;

	sFileName = Utils::GetTestDataFolder();
	sFileName += L"crashReports\\WTLDemo\\1.4.0.0\\*.zip";

	// Get the list of crash reports in the directory
	bFind = FileFinder.FindFirstFile(sFileName, &ffi);
	while(bFind)
	{
		asCrashReports.push_back(ffi.m_sFileName);
		bFind = FileFinder.FindNextFile(&ffi);
	}
	// Ensure there are enough crash reports found
	TEST_ASSERT(asCrashReports.size()>=10);

	// Create temp out dir
	sTmpDir = Utils::GetTestDataFolder();
	sTmpDir += L"temp_dir1";
	RmDir(sTmpDir, false);
	nCreateDir = CreateDir(sTmpDir);
	TEST_ASSERT_MSG(nCreateDir==0, "Error creating directory '%s'", strconv::w2a(sTmpDir).c_str());

	size_t i;
	for(i=0; i<asCrashReports.size(); i++)
	{
		sFileName = asCrashReports[i];

		sOutFile = strconv::w2a(sTmpDir)+"\\result{thread_number}.xml";

		std::ostringstream stCommand;
		stCommand << "assync dumper --dump-crash-report \"" << strconv::w2a(sFileName) << "\" \"" << sOutFile << "\"\n";
	
		client.AddAssyncRequest(stCommand.str().c_str(), 0);		
	}

	// Run tests with 10 client threads for 2 sec
    bRun10 = client.Run(10, 2000, sErrorMsg);
    TEST_ASSERT_MSG(bRun10, sErrorMsg.c_str());

	// Run tests with 5 client threads for 2 minutes
    bRun5 = client.Run(5, 2*60*1000, sErrorMsg);
    TEST_ASSERT_MSG(bRun5, sErrorMsg.c_str());	

    __TEST_CLEANUP__;    

	// Delete temp directory
    RmDir(sTmpDir, false);
}
