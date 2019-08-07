#include "stdafx.h"
#include "Tests.h"
#include "Outputter.h"
#include "Utils.h"
#include "strconv.h"
#include "Misc.h"

class OutputterTests : public CTestCase
{
    BEGIN_TEST_MAP(OutputterTests, "COutputter class tests")
        REGISTER_TEST(Test_OutputText);
		REGISTER_TEST(Test_OutputXML);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_OutputText();
	void Test_OutputXML();
};

REGISTER_TEST_CASE( OutputterTests );

void OutputterTests::SetUp()
{
}

void OutputterTests::TearDown()
{
}

void OutputterTests::Test_OutputText()
{
	FILE* f = NULL;
	COutputter doc;
	bool bCompare = false;
	std::wstring sCorrectFile = Utils::GetTestDataFolder();
#ifdef _WIN32
    sCorrectFile += L"outputter\\testdoc.txt";
#else
    sCorrectFile += L"outputter/linux/testdoc.txt";
#endif

	f = fopen("testdoc.txt", "wt");
	TEST_ASSERT(f!=NULL);

	// Write document in text mode
	doc.Init(f, OUTPUT_TEXT);
	doc.BeginDocument("Test Document");
	doc.BeginSection("General");
	doc.PutRecord("Name1", "Value1");
	doc.PutRecord("Name2", "Value2");
	doc.EndSection();
	doc.BeginSection("Table");
	doc.BeginTableRow();
	doc.PutTableCell(10, false, "Cell1");
	doc.PutTableCell(20, true, "Cell2");
	doc.EndTableRow();
	doc.EndSection();
	doc.EndDocument();
	fclose(f);
	f = NULL;

	// Compare document with the correct one
	bCompare = Utils::CompareFiles("testdoc.txt", strconv::w2a(sCorrectFile));
	TEST_ASSERT(bCompare==true);

    __TEST_CLEANUP__;

	if(f)
		fclose(f);

	RemoveFile(L"testdoc.txt");
}

void OutputterTests::Test_OutputXML()
{

	FILE* f = NULL;
	COutputter doc;
	wstring sCorrectFile = Utils::GetTestDataFolder();
#ifdef _WIN32
    sCorrectFile += L"outputter\\testdoc.xml";
#else
    sCorrectFile += L"outputter/linux/testdoc.xml";
#endif
	TiXmlDocument xml;
	bool bCompare = false;
	bool bLoadXML = false;

	f = fopen("testdoc.xml", "wt");
	TEST_ASSERT(f!=NULL);

	// Write document in XML mode
	doc.Init(f, OUTPUT_XML);
	doc.BeginDocument("Test XML Document");
	doc.BeginSection("FirstSection");
	doc.PutRecord("Prop1_32", "Value 1");
	doc.PutRecord("Property2", "Value & 2");
	doc.EndSection();
	doc.BeginSection("Table2324");
	doc.BeginTableRow();
	doc.PutTableCell(10, false, "Cell1");
	doc.PutTableCell(10, false, "Cell2");
	doc.PutTableCell(20, true, "Cell3");
	doc.EndTableRow();
	doc.BeginTableRow();
	doc.PutTableCell(10, false, "Cell4");
	doc.PutTableCell(10, false, "Cell5");
	doc.PutTableCell(20, true, "std::string data(x+y=0);");
	doc.EndTableRow();
	doc.EndSection();
	doc.EndDocument();
	fclose(f);
	f = NULL;

	// Check that this is a valid XML document
	bLoadXML = xml.LoadFile("testdoc.xml");
	TEST_ASSERT(bLoadXML==true);

	// Compare document with the correct one
	bCompare = Utils::CompareFiles("testdoc.xml", strconv::w2a(sCorrectFile).c_str());
	TEST_ASSERT(bCompare==true);

    __TEST_CLEANUP__;

	if(f)
		fclose(f);

	RemoveFile(L"testdoc.xml");
}

