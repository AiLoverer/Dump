#include "stdafx.h"
#include "Tests.h"
#include "Config.h"
#include "Utils.h"
#include "strconv.h"
#include "Misc.h"

#ifdef _WIN32
#define PATH_SEPARATORW L"\\"
#else
#define PATH_SEPARATORW L"/"
#endif

class ConfigTests : public CTestCase
{
    BEGIN_TEST_MAP(ConfigTests, "CConfig class tests")
        REGISTER_TEST(Test_Read);
		REGISTER_TEST(Test_Write);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Read();
	void Test_Write();
};


REGISTER_TEST_CASE( ConfigTests );

void ConfigTests::SetUp()
{
}

void ConfigTests::TearDown()
{
}

void ConfigTests::Test_Read()
{
	char szBuf[4096] = "";
	std::wstring sFileName = Utils::GetTestDataFolder();
#ifdef _WIN32
	sFileName += L"conf\\crashfixd.conf.win32";
#else
	sFileName += L"conf/crashfixd.conf.linux";
#endif
	CConfig config;
	bool bInit = false;
	bool bInit2 = false;
	int nRestartDaemon = -1;
	const char* szFileName = NULL;
	char* szLicenseFile = NULL;
	char* szNotExistingStr = NULL;
	char* szWebRootDir = NULL;
	char* szSmtpPassword = NULL;
	int nCacheSize = -1;
	int nNotExistingInt = -1;

	// Init config file with incorrect filename - assume failure
    CConfig config2("notexisting.conf", &bInit2);
	TEST_ASSERT(bInit2==FALSE);

	// Init config file - assume success
	bInit = config.Init(strconv::w2a(sFileName).c_str());
	TEST_ASSERT(bInit==TRUE);

	// Ensure file name is correct
	szFileName = config.GetFileName();
	TEST_ASSERT(strconv::a2w(szFileName)==sFileName);

	szLicenseFile = config.getProfileString("LICENSE_FILE", szBuf, 4096);
	TEST_ASSERT(szLicenseFile!=NULL);
#ifdef _WIN32
	TEST_ASSERT(strcmp(szLicenseFile, "licenses\\crashfix.lic")==0);
#else
	TEST_ASSERT(strcmp(szLicenseFile, "/usr/share/crashfix/licenses/crashfix.lic")==0);
#endif

	// Try to read non-existing string - assume failure
	szNotExistingStr = config.getProfileString("NOT_EXISTING_STR", szBuf, 4096);
	TEST_ASSERT(szNotExistingStr==NULL);

	szWebRootDir = config.getProfileString("WEB_ROOT_DIR", szBuf, 4096);
	TEST_ASSERT(szWebRootDir!=NULL);
#ifdef _WIN32
	TEST_ASSERT(strcmp(szWebRootDir, "C:\\Program Files (x86)\\Apache Software Foundation\\Apache2.2\\htdocs\\crashfix")==0);
#else
	TEST_ASSERT(strcmp(szWebRootDir, "/var/www/crashfix/")==0);
#endif

	szSmtpPassword = config.getProfileString("SMTP_PASSWORD", szBuf, 4096);
	TEST_ASSERT(szSmtpPassword!=NULL);
	TEST_ASSERT(strcmp(szSmtpPassword, "")==0);

	// Read MAX_DEBUG_INFO_CACHE_SIZE - assume success
	nCacheSize = config.getProfileInt("MAX_DEBUG_INFO_CACHE_SIZE", 0);
    TEST_ASSERT(nCacheSize==100);

	// Read NOT_EXISTING_INT - assume failure
	nNotExistingInt = config.getProfileInt("NOT_EXISTING_INT", -1);
    TEST_ASSERT(nNotExistingInt==-1);

	// Read RESTART_DAEMON_ON_CRASH - assume success
	nRestartDaemon = config.getProfileInt("RESTART_DAEMON_ON_CRASH", 0);
    TEST_ASSERT(nRestartDaemon==1);

    __TEST_CLEANUP__;
}

void ConfigTests::Test_Write()
{
	char szBuf[4096] = "";
	std::wstring sFileName = Utils::GetTestDataFolder();
	std::wstring sDstFileName;
#ifdef _WIN32
	sFileName += L"conf\\crashfixd.conf.win32";
#else
	sFileName += L"conf/crashfixd.conf.linux";
#endif
	sDstFileName=sFileName+L".tmp";
	bool bCopy = false;
	CConfig config;
	bool bInit = false;
	int nResult = -1;
	char* szLicenseFile = NULL;
	char* szPidFile = NULL;
	char* szNewParam = NULL;

	// Remove dest file if exists
	remove(strconv::w2a(sDstFileName).c_str());

	// Copy original config file to temp file
	bCopy = copy_file(sFileName, sDstFileName, true);
	TEST_ASSERT(bCopy);

	// Init config file - assume success
	bInit = config.Init(strconv::w2a(sDstFileName).c_str());
	TEST_ASSERT(bInit==TRUE);

	// Read parameter - assume success
	szLicenseFile = config.getProfileString("LICENSE_FILE", szBuf, 4096);
	TEST_ASSERT(szLicenseFile!=NULL);
#ifdef _WIN32
	TEST_ASSERT(strcmp(szLicenseFile, "licenses\\crashfix.lic")==0);
#else
	TEST_ASSERT(strcmp(szLicenseFile, "/usr/share/crashfix/licenses/crashfix.lic")==0);
#endif

	// Write parameter - assume success
	nResult = config.writeProfileString("LICENSE_FILE", "some/dir/name/license.lic");
	TEST_ASSERT(nResult == 0);

	// Read parameter - assume success
	szLicenseFile = config.getProfileString("LICENSE_FILE", szBuf, 4096);
	TEST_ASSERT(szLicenseFile!=NULL);
	TEST_ASSERT(strcmp(szLicenseFile, "some/dir/name/license.lic")==0);

	// Write parameter - assume success
	nResult = config.writeProfileString("PIDFILE", "some parameter with spaces");
	TEST_ASSERT(nResult == 0);

	// Read parameter - assume success
	szPidFile = config.getProfileString("PIDFILE", szBuf, 4096);
	TEST_ASSERT(szPidFile!=NULL);
	TEST_ASSERT(strcmp(szPidFile, "some parameter with spaces")==0);

	// Write new parameter - assume success
	nResult = config.writeProfileString("NEW_PARAM", "new parameter with spaces");
	TEST_ASSERT(nResult == 0);

	// Read parameter - assume success
	szNewParam = config.getProfileString("NEW_PARAM", szBuf, 4096);
	TEST_ASSERT(szNewParam!=NULL);
	TEST_ASSERT(strcmp(szNewParam, "new parameter with spaces")==0);

	__TEST_CLEANUP__;

	// Remove dest file if exists
	remove(strconv::w2a(sDstFileName).c_str());
}
