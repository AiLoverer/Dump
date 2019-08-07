//! \file Tests.cpp
//! \brief Base unit test functionality.
//! \author Oleg Krivtsov
//! \date 2010

#include "stdafx.h"
#include "Tests.h"

// Uncomment the following line to enable memory leak detection with Visual Leak Detector
//#define _DETECT_MEMORY_LEAKS

#if defined(_WIN32) && defined(_DETECT_MEMORY_LEAKS)
    #include <vld.h>
    #pragma comment(lib, "vld.lib")
#endif

// Function forward declarations
std::vector<std::string> explode(std::string str, std::string separators = " \t");
void trim2(std::string& str, const char* szTrim=" \t\n");
int fork();

double _microtime()
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (double)(tv.tv_sec)*1000+(double)(tv.tv_usec)/1000;
#endif
}

// The main function
int main(int argc, char** argv)
{
    CTestRegistry* pRegistry = CTestRegistry::GetRegistry();
    CTestCase* pTopCase = pRegistry->GetTopCase();

	char szBuffer[1024]="";
	char* szSuiteList = szBuffer;

	if(argc==1)
	{
		// Print the list of test cases

		printf("The list of avaliable test cases:\n");

		UINT nCaseCount = pTopCase->GetChildCaseCount();
		UINT i;
		for(i=0; i<nCaseCount; i++)
		{
			CTestCase* pSuite = pTopCase->GetChildCase(i);
			std::string sCaseName;
			std::string sDescription;
			pSuite->GetCaseInfo(sCaseName, sDescription);

			printf(" - %s : %s\n", sCaseName.c_str(), sDescription.c_str());
			fflush(stdout);
		}

		printf("\nEnter which test cases to run (separate names by space) or enter empty line to run all test cases.\n");
		printf("Your choice > ");
	#if _MSC_VER>=1400
		szSuiteList = gets_s(szBuffer, 1024);
	#else
		szSuiteList = fgets(szBuffer, 1024, stdin);
	#endif
	}

    // Create the list of test suites to run
    std::string sSuiteList = szSuiteList;
    std::vector<std::string> aTokens = explode(sSuiteList);
    std::set<std::string> aTestCasesToRun;
	unsigned i;
    for(i=0; i<aTokens.size(); i++)
        aTestCasesToRun.insert(aTokens[i]);

    // Determine how many tests to run

    std::vector<std::string> test_list = pTopCase->GetTestList(aTestCasesToRun, true);
    unsigned nTestsToRun = test_list.size();

    printf("\nRunning tests...\n");

	double start_time = _microtime();

    pTopCase->Run(aTestCasesToRun);

	double stop_time = _microtime();
	double fDuration = (stop_time-start_time)/1000;

    printf("\n\n=== Summary ===\n\n");

    // Print all errors (if exist)
    std::vector<std::string> error_list = pTopCase->GetErrorList(true);
    for(i=0; i<error_list.size(); i++)
    {
        printf("%u: %s\n", i+1, error_list[i].c_str());
    }

	printf("\n  Time elapsed: %0.2f sec\n", fDuration);
    printf("    Test count: %u\n", nTestsToRun);
    unsigned nErrorCount = error_list.size();
    printf("  Tests passed: %u\n", nTestsToRun-nErrorCount);
    printf("  Tests failed: %u\n", nErrorCount);

    // Wait for key press
#ifdef _WIN32
    _getch();
#else
    getchar();
#endif

	// Free test registry
	delete pRegistry;

    // Return non-zero value if there were errors
    return nErrorCount==0?0:1;
}

// Helper function that removes spaces from the beginning and end of the string
void trim2(std::string& str, const char* szTrim)
{
    std::string::size_type pos = str.find_last_not_of(szTrim);
    if(pos != std::string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(szTrim);
        if(pos != std::string::npos) str.erase(0, pos);
    }
    else str.erase(str.begin(), str.end());
}

// Helper function that splits a string into list of tokens
std::vector<std::string> explode(std::string str, std::string separators)
{
    std::vector<std::string> aTokens;

    size_t pos = 0;
    for(;;)
    {
        pos = str.find_first_of(separators, 0);

        std::string sToken = str.substr(0, pos);
        if(pos!=std::string::npos)
            str = str.substr(pos+1);

        trim2(sToken);
        if(sToken.length()>0)
            aTokens.push_back(sToken);

        if(pos==std::string::npos)
            break;
    }

    return aTokens;
}



//--------------------------------------------------------
// CTestCase impl
//--------------------------------------------------------

CTestCase* g_pCurTestCase = NULL;

CTestCase::CTestCase(CTestCase* pParentCase)
{
    m_pParentCase = pParentCase;
}

CTestCase::~CTestCase()
{
	size_t i;
	for(i=0; i<m_apChildCases.size(); i++)
	{
		delete m_apChildCases[i];
	}
}

std::vector<std::string> CTestCase::GetTestList(std::set<std::string>& CasesToRun, bool bIncludeChildren)
{
    std::vector<std::string> test_list;

    if(GetParentCase()!=NULL && CasesToRun.size()!=0)
    {
        std::string sSuiteName;
        std::string sSuiteDescription;
        GetCaseInfo(sSuiteName, sSuiteDescription);

        // Check if this suite's name is in list
        std::set<std::string>::iterator it = CasesToRun.find(sSuiteName);
        if(it==CasesToRun.end())
            return test_list; // This suite is not in list
    }

    test_list.push_back("SetUp");

    DoWithMyTests(GET_NAMES, test_list);

    if(bIncludeChildren)
    {
        UINT i;
        for(i=0; i<GetChildCaseCount(); i++)
        {
            CTestCase* pChildSuite = GetChildCase(i);

            std::vector<std::string> child_test_list = pChildSuite->GetTestList(CasesToRun, bIncludeChildren);

            UINT j;
            for(j=0; j<child_test_list.size(); j++)
            {
                test_list.push_back(child_test_list[j]);
            }
        }
    }

    test_list.push_back("TearDown");

    return test_list;
}

bool CTestCase::Run(std::set<std::string>& CasesToRun)
{
    ClearErrors();

    if(GetParentCase()!=NULL && CasesToRun.size()!=0)
    {
        std::string sCaseName;
        std::string sCaseDescription;
        GetCaseInfo(sCaseName, sCaseDescription);

        // Check if this suite's name is in list
        std::set<std::string>::iterator it = CasesToRun.find(sCaseName);
        if(it==CasesToRun.end())
            return true; // This suite is not in list
    }

    g_pCurTestCase = this;

    BeforeTest("SetUp");
    SetUp();
    AfterTest("SetUp");

    if(m_bTestFailed)
        m_bSetUpFailed = true;

    std::vector<std::string> test_list;
    DoWithMyTests(RUN_TESTS, test_list);

    UINT i;
    for(i=0; i<m_apChildCases.size(); i++)
    {
        m_apChildCases[i]->Run(CasesToRun);
    }

    int nErrors = (int)m_asErrorMsg.size();

    g_pCurTestCase = this;

    if(BeforeTest("TearDown"))
        TearDown();
    AfterTest("TearDown");

    if(nErrors!=(int)m_asErrorMsg.size())
        return false; // TearDown has failed

    g_pCurTestCase = NULL;

    return m_asErrorMsg.size()==0?true:false;
}

CTestCase* CTestCase::GetParentCase()
{
    return m_pParentCase;
}

void CTestCase::SetParentCase(CTestCase* pParent)
{
    m_pParentCase = pParent;
}

UINT CTestCase::GetChildCaseCount()
{
    return (UINT)m_apChildCases.size();
}

void CTestCase::AddChildCase(CTestCase* pChildCase)
{
    m_apChildCases.push_back(pChildCase);
}

CTestCase* CTestCase::GetChildCase(UINT i)
{
    return m_apChildCases[i];
}

bool CTestCase::BeforeTest(const char* szFunction)
{
    m_bTestFailed = false;
    std::string sCaseName;
    std::string sCaseDescription;
    GetCaseInfo(sCaseName, sCaseDescription);

    printf(" - %s::%s... ", sCaseName.c_str(), szFunction);
    fflush(stdout);

    if(m_bSetUpFailed)
    {
        m_bTestFailed = true;
        AddErrorMsg(szFunction, "SetUp Failure", NULL);
        return false; // Prevent running test
    }

    return true;
}

void CTestCase::AfterTest(const char* szFunction)
{
    UNREFERENCED_PARAMETER(szFunction);

    if(!m_bTestFailed)
        printf("OK.\n");
    else
    {
        printf("Failed.\n");

        std::string sErrorMsg = m_asErrorMsg[m_asErrorMsg.size()-1];
        sErrorMsg = "!!! " + sErrorMsg;
        sErrorMsg += "\n";
        printf("%s", sErrorMsg.c_str());
    }
    fflush(stdout);
}

void CTestCase::AddErrorMsg(const char* szFunction, const char* szAssertion, const char* szMsg, ...)
{
    m_bTestFailed = true;
	char szBuffer[4096];
	memset(szBuffer, 0, 4096);
	if(szMsg!=NULL)
	{
		va_list arg_list;
		va_start(arg_list, szMsg);
		vsnprintf(szBuffer, 4095, szMsg, arg_list);
	}

    std::string sMsg = "In test: ";
    sMsg += szFunction;
    sMsg += " Expr: ";
    sMsg += szAssertion;
	if(szMsg!=NULL)
	{
		sMsg += " Msg: ";
		sMsg += szBuffer;
	}
    m_asErrorMsg.push_back(sMsg);

	std::string sErrorMsg = "!!! " + sMsg;
    sErrorMsg += "\n";
    printf("%s", sErrorMsg.c_str());
    fflush(stdout);
}

std::vector<std::string> CTestCase::GetErrorList(bool bIncludeChildren)
{
    std::vector<std::string> asErrors = m_asErrorMsg;

    if(bIncludeChildren)
    {
        UINT i;
        for(i=0; i<GetChildCaseCount(); i++)
        {
            CTestCase* pChildCase = GetChildCase(i);
            std::vector<std::string> asChildErrors = pChildCase->GetErrorList(bIncludeChildren);

            UINT j;
            for(j=0; j<asChildErrors.size(); j++)
            {
                asErrors.push_back(asChildErrors[j]);
            }
        }
    }

    return asErrors;
}

void CTestCase::ClearErrors()
{
    m_bSetUpFailed = false;
    m_bTestFailed = false;
    m_asErrorMsg.clear();
}

//--------------------------------------------------------
// CTestRegistry implementation
//--------------------------------------------------------

CTestRegistry* CTestRegistry::GetRegistry()
{
    static CTestRegistry* pRegistry = NULL;

    if(pRegistry==NULL)
        pRegistry = new CTestRegistry();

    return pRegistry;
}

CTestRegistry::CTestRegistry()
{
    m_pTopCase = NULL;
}

CTestRegistry::~CTestRegistry()
{
	if(m_pTopCase)
		delete m_pTopCase;
}

CTestCase* CTestRegistry::GetTopCase()
{
    if(m_pTopCase==NULL)
        m_pTopCase = new TopLevelTestCase();

    return m_pTopCase;
}
