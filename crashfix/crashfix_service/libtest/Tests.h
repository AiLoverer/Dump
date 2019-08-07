//! \file Tests.h
//! \brief Base unit test functionality.
//! \author Oleg Krivtsov
//! \date 2010

#pragma once
#include "stdafx.h"

//!< What action to perform
enum eAction
{
    GET_NAMES, //!< Return test names.
    RUN_TESTS  //!< Run tests.
};

//! \class CTestCase
//! \brief A test case class.
class CTestCase
{
public:

    //! Constructor
    CTestCase(CTestCase* pParentCase=NULL);

	//! Destructor
	virtual ~CTestCase();

    //! Allocates resources used by tests in this suite
    virtual void SetUp() = 0;

    //! Frees resources used by tests in this suite
    virtual void TearDown() = 0;

    //! Returns name and description
    virtual void GetCaseInfo(std::string& sName, std::string& sDescription) = 0;

    //! Returns the list of tests in this test case or runs tests, depending on action parameter.
    virtual void DoWithMyTests(eAction action, std::vector<std::string>& test_list) = 0;

    //! Runs all or some tests from this test case.
    bool Run(std::set<std::string>& TestCasesToRun);

    //! Returns test list in this test suite
    virtual std::vector<std::string> GetTestList(std::set<std::string>& TestCasesToRun, bool bIncludeChildren = false);

    //! Returns parent test case.
    CTestCase* GetParentCase();

    //! Sets parent case.
    void SetParentCase(CTestCase* pParent);

    //! Returns count of child test suites.
    UINT GetChildCaseCount();

    //! Returns i-th child test case.
    CTestCase* GetChildCase(UINT i);

    //! Adds a child test case.
    void AddChildCase(CTestCase* pChildCase);

    //! Returns the list of errors.
    std::vector<std::string> GetErrorList(bool bIncludeChildren = false);

	//! Clears error list.
    void ClearErrors();

	//! Adds an error message to the list.
    void AddErrorMsg(const char* szFunction, const char* szAssertion, const char* szMsg, ...);

protected:

	//! This method is executed before entering the test function.
    bool BeforeTest(const char* szFunction);

	//! This method is executed after entering the test function.
    void AfterTest(const char* szFunction);

private:

	/* Variables used internally. */

    CTestCase* m_pParentCase;                 //!< Parent test case
    std::vector<CTestCase*> m_apChildCases;   //!< The list of child test cases
    std::vector<std::string> m_asErrorMsg;    //!< The list of error messages
    bool m_bSetUpFailed;                      //!< Set up failed?
    bool m_bTestFailed;                       //!< Some test failed?
};

//! This macro introduces the test map.
#define BEGIN_TEST_MAP( TestCase , Description)\
    virtual void GetCaseInfo(std::string& sName, std::string& sDescription)\
{\
    sName = std::string( #TestCase );\
    sDescription = std::string( Description );\
}\
    virtual void DoWithMyTests(eAction action, std::vector<std::string>& test_list)\
{

//! This macro declares a test function.
#define REGISTER_TEST( Test )\
    if(action==GET_NAMES)\
    test_list.push_back( #Test );\
else\
{\
    if(BeforeTest( #Test ))\
    Test();\
    AfterTest( #Test);\
}

//! This macro ends the test map.
#define END_TEST_MAP() }

//! \class TopLevelTestCase
//! \brief A test case containing all other test cases.
class TopLevelTestCase : public CTestCase
{
public:

    BEGIN_TEST_MAP( TopLevelTestCase, "All tests")
        UNREFERENCED_PARAMETER(test_list);
        UNREFERENCED_PARAMETER(action);
    END_TEST_MAP()

	//! Constructor.
    TopLevelTestCase()
        :CTestCase(NULL)
    {
    }

	//! Set up callback.
    virtual void SetUp()
    {
    }

	//! Tear down callback.
    virtual void TearDown()
    {
    }

};

//! \class CTestRegistry
//! \brief Contains the registry of all test cases and tests.
class CTestRegistry
{
public:

	//! Returns singleton of the registry.
    static CTestRegistry* GetRegistry();

	//! Constructor.
    CTestRegistry();

	//! Destructor.
	~CTestRegistry();

	//! Returns top test case.
    CTestCase* GetTopCase();

private:

    CTestCase* m_pTopCase; //!< The top-level test case.
};

//! Global var containing a pointer to the test case currently being executed.
extern CTestCase* g_pCurTestCase;

//! Tests for assertion expr, and if assertion fails, prints an test name and assertion expr where assertion happened, and goes to __CLEANUP__ section.
#define TEST_ASSERT(expr)\
    if(!(expr)) { g_pCurTestCase->AddErrorMsg(__FUNCTION__, #expr, NULL); \
    goto test_cleanup; }

//! Tests for assertion expr, and if assertion fails, prints test name, an assertion expr and error message msg, and goes to __CLEANUP__ section.
#define TEST_ASSERT_MSG(expr, ...)\
	if(!(expr)) { g_pCurTestCase->AddErrorMsg((__FUNCTION__), (#expr), __VA_ARGS__); \
    goto test_cleanup; }

//! Tests for assertion expr, and if assertion fails, prints test name, an assertion expr, then continues the test from the next instruction.
#define TEST_EXPECT(expr)\
    if(!(expr)) { g_pCurTestCase->AddErrorMsg(__FUNCTION__, #expr); }

//! Tests for assertion expr, and if assertion fails, prints test name, an assertion expr, an error message msg, then continues the test from the next instruction.
#define TEST_EXPECT_MSG(expr, ...)\
	if(!(expr)) { g_pCurTestCase->AddErrorMsg(__FUNCTION__, #expr, __VA_ARGS__); }

//! Test cleanup label. Instructions below the label are executed on test clean up stage.
#define __TEST_CLEANUP__ test_cleanup:

//! \class CTestCaseRegistrator
//! \brief A helper class for registering test cases in its constructor.
template <class T>
class CTestCaseRegistrator
{
public:

	//! Class constructor.
    CTestCaseRegistrator()
    {
        CTestRegistry* pRegistry = CTestRegistry::GetRegistry();
        CTestCase* pTopCase = pRegistry->GetTopCase();
        CTestCase* pCase = new T();
        pCase->SetParentCase(pTopCase);
        pTopCase->AddChildCase(pCase);
    }
};

//! The macro for registering a test case.
#define REGISTER_TEST_CASE( Case ) CTestCaseRegistrator<Case> __reg_##Case;

