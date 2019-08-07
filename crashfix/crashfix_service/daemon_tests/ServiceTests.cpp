#include "stdafx.h"
#include "Tests.h"
#include "MtClient.h"
#include "ServiceHelpers.h"

class ServiceTests : public CTestCase
{
    BEGIN_TEST_MAP(ServiceTests, "Daemon service start/stop/restart tests")
		REGISTER_TEST(Test_StartStopDaemon);
		REGISTER_TEST(Test_StartRestartStopDaemon);
		REGISTER_TEST(Test_StartKillDaemon);		
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

	void Test_StartStopDaemon();
	void Test_StartRestartStopDaemon();
	void Test_StartKillDaemon();       
};


REGISTER_TEST_CASE( ServiceTests );

void ServiceTests::SetUp()
{
}

void ServiceTests::TearDown()
{
}

void ServiceTests::Test_StartStopDaemon()
{
    bool bStart = false;
    bool bStop = false;
    std::string sErrorMsg;

	bStart = StartDaemon(sErrorMsg);
	TEST_ASSERT_MSG(bStart, sErrorMsg.c_str());

	bStop = StopDaemon(sErrorMsg);
	TEST_ASSERT_MSG(bStop, sErrorMsg.c_str());

	__TEST_CLEANUP__;

	StopDaemon(sErrorMsg);
}

void ServiceTests::Test_StartRestartStopDaemon()
{
    bool bStart = false;
    bool bRestart = false;
    bool bStop = false;
    std::string sErrorMsg;

	bStart = StartDaemon(sErrorMsg);
	TEST_ASSERT_MSG(bStart, sErrorMsg.c_str());

	bRestart = RestartDaemon(sErrorMsg);
	TEST_ASSERT_MSG(bRestart, sErrorMsg.c_str());

	bStop = StopDaemon(sErrorMsg);
	TEST_ASSERT_MSG(bStop, sErrorMsg.c_str());

	__TEST_CLEANUP__;

	StopDaemon(sErrorMsg);
}

void ServiceTests::Test_StartKillDaemon()
{
    bool bStart = false;
    bool bStop = false;
    std::string sErrorMsg;

	bStart = StartDaemon(sErrorMsg);
	TEST_ASSERT_MSG(bStart, sErrorMsg.c_str());

	bStop = KillDaemon(sErrorMsg);
	TEST_ASSERT_MSG(bStop, sErrorMsg.c_str());

	__TEST_CLEANUP__;

	StopDaemon(sErrorMsg);
}

