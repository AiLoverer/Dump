#include "stdafx.h"
#include "Tests.h"
#include "MtClient.h"
#include "Misc.h"
#include "ServiceHelpers.h"
#include "Utils.h"
#include "strconv.h"
#include "FileFinder.h"
#include "tinyxml.h"

class RequestTests : public CTestCase
{
    BEGIN_TEST_MAP(RequestTests, "Daemon request tests")
		REGISTER_TEST(Test_PdbImport);
		REGISTER_TEST(Test_ExtractFile);
		REGISTER_TEST(Test_DaemonStatus);
		REGISTER_TEST(Test_DaemonStatus_assync);
		REGISTER_TEST(Test_DaemonGetLicenseInfo);
		REGISTER_TEST(Test_DumpCrashReport_assync);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

	void Test_DaemonStatus();
	void Test_DaemonStatus_assync();
	void Test_DaemonGetLicenseInfo();
	void Test_PdbImport();
	void Test_DumpCrashReport_assync();
	void Test_ExtractFile();
};

REGISTER_TEST_CASE( RequestTests );


void RequestTests::SetUp()
{
	bool bStart = false;
	std::string sErrorMsg;

	// First start daemon - assume success
    bStart = StartDaemon(sErrorMsg);
    TEST_ASSERT_MSG(bStart, sErrorMsg.c_str());

	/* initialize random seed: */
	srand ( (unsigned int)time(NULL) );

	__TEST_CLEANUP__;
}

void RequestTests::TearDown()
{
	std::string sErrorMsg;
	StopDaemon(sErrorMsg);
}

void RequestTests::Test_DaemonStatus()
{
    bool bExec = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;
	int nServerRetCode = -1;
	std::string sServerResponse;

	// Execute daemon status command - assume success
    bExec = client.ExecuteRequest("daemon status\n", nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
	TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d", nServerRetCode);

    __TEST_CLEANUP__;
}

void RequestTests::Test_DaemonStatus_assync()
{
	bool bExec = false;
	bool bExec2 = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;
	int nServerRetCode = -1;
	std::string sServerResponse;

	// Execute assync command - assume success
    bExec = client.ExecuteRequest("assync daemon status\n", nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
	TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d", nServerRetCode);

	// Sleep for some time
	Sleep(1000);

	// Get assync command status - assume it has been completed
    bExec = client.ExecuteRequest("daemon get-assync-info\n", nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());

	// Execute assync command another time - assume success
    bExec2 = client.ExecuteRequest("assync daemon status\n", nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec2, sErrorMsg.c_str());
	TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d", nServerRetCode);

	// Sleep for some time
	Sleep(1000);

	// Get assync command status another time - assume it has been completed
    bExec = client.ExecuteRequest("daemon get-assync-info -erase-completed\n", nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());

	// Get assync command status another time - assume no commands left
    bExec = client.ExecuteRequest("daemon get-assync-info\n", nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
	TEST_ASSERT_MSG(nServerRetCode==1, "Invalid ret code %d", nServerRetCode);
	TEST_ASSERT_MSG(sServerResponse.compare("1 Such a command ID not found.\n")==0, "Invalid response %s", sServerResponse.c_str());

    __TEST_CLEANUP__;

}

void RequestTests::Test_PdbImport()
{
	bool bExec = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;
	int nServerRetCode = -1;
	std::string sServerResponse;
	std::ostringstream stCommand;
	std::wstring sPdbName;
	std::wstring sSymDir;
	std::wstring sOutFile;
	int nCreateDir = -1;

	sPdbName = Utils::GetTestDataFolder();
	sPdbName += L"CrashRpt.pdb";

	// Create symbol store dir
	sSymDir = Utils::GetTestDataFolder();
	sSymDir += L"sym";
	RmDir(sSymDir, false);
	nCreateDir = CreateDir(sSymDir);
	TEST_ASSERT_MSG(nCreateDir==0, "Error creating directory '%s'", strconv::w2a(sSymDir).c_str());

	sOutFile = sSymDir;
#ifdef _WIN32
	sOutFile += L"\\CrashRpt.pdb.txt";
#else
    sOutFile += L"/CrashRpt.pdb.txt";
#endif

	// Format command
	stCommand << "dumper --import-pdb \"" <<
		strconv::w2utf8(sPdbName) << "\" \"" <<
		strconv::w2utf8(sSymDir) << "\" \"" <<
		strconv::w2utf8(sOutFile) << "\"\n";

	// Execute daemon status command - assume success
    bExec = client.ExecuteRequest(stCommand.str().c_str(), nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
	TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d, msg = %s", nServerRetCode, sServerResponse.c_str());

	__TEST_CLEANUP__;

	// Delete sym directory
    RmDir(sSymDir, false);

}

void RequestTests::Test_ExtractFile()
{
	bool bExec = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;
	int nServerRetCode = -1;
	std::string sServerResponse;
	std::ostringstream stCommand;
	std::wstring sCrashReportFileName;
	std::wstring sFileItemName;
	std::wstring sOutFile;
	struct stat st;
	int nStatRes = -1;

	sCrashReportFileName = Utils::GetTestDataFolder();
	sCrashReportFileName += L"crashreport.zip";

	sFileItemName = L"crashdump.dmp";

	sOutFile = Utils::GetCurrentDir();
#ifdef _WIN32
	sOutFile += L"\\crashdump.dmp";
#else
    sOutFile += L"/crashdump.dmp";
#endif

	// Format command
	stCommand << "dumper --extract-file \"" <<
		strconv::w2utf8(sCrashReportFileName) << "\" \"" <<
		strconv::w2utf8(sFileItemName) << "\" \"" <<
		strconv::w2utf8(sOutFile) << "\"\n";

	// Execute daemon command - assume success
    bExec = client.ExecuteRequest(stCommand.str().c_str(), nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
	TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d", nServerRetCode);

	// Check that file really exists and is of correct length
	nStatRes = stat(strconv::w2a(sOutFile).c_str(), &st);
	TEST_ASSERT_MSG(nStatRes==0, "Couldn't stat output file.");
	TEST_ASSERT_MSG(st.st_size==16159, "Output file length is incorrect.");

	__TEST_CLEANUP__;
}

void RequestTests::Test_DaemonGetLicenseInfo()
{
    bool bExec = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;
	int nServerRetCode = -1;
	std::string sServerResponse;
	std::wstring sOutFile;
	std::string sCmdLine;

	sOutFile = Utils::GetCurrentDir();
	sOutFile += L"license_info.xml";

	sCmdLine = "daemon get-license-info \"";
	sCmdLine += strconv::w2utf8(sOutFile);
	sCmdLine += "\"\n";

	// Execute daemon status command - assume success
    bExec = client.ExecuteRequest(sCmdLine.c_str(), nServerRetCode, sServerResponse, sErrorMsg);
	TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
	TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d", nServerRetCode);

    __TEST_CLEANUP__;
}

void RequestTests::Test_DumpCrashReport_assync()
{
	bool bExec = false;
    CMtClient client("127.0.0.1", 50);
    std::string sErrorMsg;
	int nServerRetCode = -1;
	std::string sServerResponse;
	std::string sCommand;
	std::wstring sFileName;
	std::wstring sTmpDir;
	std::wstring sOutFile;
	int nCreateDir = -1;
	std::vector<std::wstring> asCrashReports;
	CFileFinder FileFinder;
	CFindFileInfo ffi;
	bool bFind = false;
	std::wstring sTmpName;
	TiXmlDocument doc;
	std::string sCmdId;
	int pos1, pos2, pos3, pos4;

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
		std::ostringstream stCommand;
		sFileName = asCrashReports[i];

		sOutFile = sTmpDir+L"\\result.xml";

		// Format command
		stCommand << "assync dumper --dump-crash-report \"" <<
			strconv::w2utf8(sFileName) << "\" \"" <<
			strconv::w2utf8(sOutFile) << "\"\n";
		sCommand = stCommand.str();

		// Execute daemon command - assume success
		bExec = client.ExecuteRequest(sCommand.c_str(), nServerRetCode, sServerResponse, sErrorMsg);
		TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
		TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d, msg = %s", nServerRetCode, sServerResponse.c_str());

		// Get command id
		pos1 = sServerResponse.find('{');
		pos2 = sServerResponse.find('}', pos1+1);
		sCmdId = sServerResponse.substr(pos1+1, pos2-pos1-1);

		// Wait until command is finished
		for(;;)
		{
			std::string sRetMsg;

			std::ostringstream stCommand;
			stCommand << "daemon get-assync-info -erase-completed  " << sCmdId << "\n";
			sCommand = stCommand.str();

			bExec = client.ExecuteRequest(sCommand.c_str(), nServerRetCode, sServerResponse, sErrorMsg);
			TEST_ASSERT_MSG(bExec, sErrorMsg.c_str());
			TEST_ASSERT_MSG(nServerRetCode==0, "Invalid ret code %d, msg = %s", nServerRetCode, sServerResponse.c_str());

			if(sServerResponse.find("still executing")!=sServerResponse.npos)
			{
				Sleep(1000);
				continue;
			}

			// Get command id and return message
			pos1 = sServerResponse.find('{');
			pos2 = sServerResponse.find('}', pos1+1);
			sCmdId = sServerResponse.substr(pos1+1, pos2-pos1-1);

			pos3 = sServerResponse.find('{', pos2+1);
			pos4 = sServerResponse.find('}', pos3+1);
			sRetMsg = sServerResponse.substr(pos3+1, pos4-pos3-1);

			TEST_ASSERT(sRetMsg=="0 Success");
			break;
		}

		// Check that result is a valid XML file
		bool bLoad = doc.LoadFile(strconv::w2a(sOutFile).c_str());
		TEST_ASSERT(bLoad);

		// Get element
		TiXmlHandle hElem = doc.RootElement();
		TEST_ASSERT(hElem.ToElement()!=NULL);
		hElem = hElem.FirstChild("Summary");
		TEST_ASSERT(hElem.ToElement()!=NULL);

		// Remove file
		remove(strconv::w2a(sOutFile).c_str());
	}

	__TEST_CLEANUP__;

	// Delete temp directory
    RmDir(sTmpDir, false);

}
