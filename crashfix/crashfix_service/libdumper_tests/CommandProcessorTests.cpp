#include "stdafx.h"
#include "Tests.h"
#include "CommandProcessor.h"
#include "Utils.h"
#include "strconv.h"
#include "Misc.h"
#include "Log.h"
#include "PdbCache.h"

class CommandProcessorTests : public CTestCase
{
    BEGIN_TEST_MAP(CommandProcessorTests, "CCommandProcessor class tests")
        REGISTER_TEST(Test_DumpCrashReport);		
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_DumpCrashReport();	
};


REGISTER_TEST_CASE( CommandProcessorTests );

void CommandProcessorTests::SetUp()
{
}

void CommandProcessorTests::TearDown()
{
}

void CommandProcessorTests::Test_DumpCrashReport()
{
	std::wstring sFileName = Utils::GetTestDataFolder();
	std::wstring sDstFileName;
	sFileName += L"crashreport.zip";
	sDstFileName=sFileName+L".tmp";

	std::string sFileNameA = strconv::w2a(sFileName).c_str();
	std::string sDstFileNameA = strconv::w2a(sDstFileName).c_str();

	CCommandProcessor cp;
	CLog log;
	CPdbCache cache;
	int nResult = -1;

	const int argc = 4;
	char* argv[] = 
	{
		"dumper",
		"--dump-crash-report",
		(char*)sFileNameA.c_str(),
		(char*)sDstFileNameA.c_str()
	};

	cp.SubstituteLog(&log, FALSE);
	cp.SubstitutePdbCache(&cache, FALSE);

	nResult = cp.Run(argc, argv);
	TEST_ASSERT_MSG(nResult==0, "Error running command: %s", cp.GetErrorMsg().c_str());

    __TEST_CLEANUP__;

#ifdef _WIN32
	_unlink(strconv::w2a(sDstFileName).c_str());
#else
	unlink(strconv::w2a(sDstFileName).c_str());
#endif
}

