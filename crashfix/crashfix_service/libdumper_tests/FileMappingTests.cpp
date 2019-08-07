#include "stdafx.h"
#include "Tests.h"
#include "FileMapping.h"
#include "Utils.h"

class FileMemoryMappingTests : public CTestCase
{
    BEGIN_TEST_MAP(FileMemoryMappingTests, "CFileMemoryMapping class tests")
        REGISTER_TEST(Test_Init);
        REGISTER_TEST(Test_CreateView);
        REGISTER_TEST(Test_GetSize);

    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_Init();
    void Test_CreateView();
    void Test_GetSize();

};


REGISTER_TEST_CASE( FileMemoryMappingTests );

void FileMemoryMappingTests::SetUp()
{
}

void FileMemoryMappingTests::TearDown()
{
}

void FileMemoryMappingTests::Test_Init()
{
    CFileMemoryMapping fm;
    BOOL bInit = FALSE;
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";

    // Init with empty file name - assume failure
    bInit = fm.Init(L"");
    TEST_ASSERT(!bInit);

    // Init with correct file name - assume success
    bInit = fm.Init(sFileName);
    TEST_ASSERT(bInit);


    __TEST_CLEANUP__;
}

void FileMemoryMappingTests::Test_CreateView()
{
    CFileMemoryMapping fm;
    BOOL bInit = FALSE;   
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    LPBYTE pPtr = NULL;

    // Create view on closed filemapping - assume failure
    pPtr = fm.CreateView(0, 255);
    TEST_ASSERT(pPtr==NULL);

    // Init with correct file name - assume success
    bInit = fm.Init(sFileName);
    TEST_ASSERT(bInit);

    // Create view on opened filemapping - assume success
    pPtr = fm.CreateView(0, 255);
    TEST_ASSERT(pPtr!=NULL);

    // Create another view on opened filemapping - assume success
    pPtr = fm.CreateView(254, 255);
    TEST_ASSERT(pPtr!=NULL);

    fm.Destroy();

    // Create view on closed filemapping - assume success
    pPtr = fm.CreateView(0, 255);
    TEST_ASSERT(pPtr==NULL);

    __TEST_CLEANUP__;
}

void FileMemoryMappingTests::Test_GetSize()
{
    CFileMemoryMapping fm;
    BOOL bInit = FALSE;   
    std::wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    ULONG64 uSize = 0;
    
    // Get size of closed filemapping - assume zero size
    uSize = fm.GetSize();
    TEST_ASSERT(uSize==0);

    // Init with correct file name - assume success
    bInit = fm.Init(sFileName);
    TEST_ASSERT(bInit);

    // Get size of opened filemapping - assume non-zero
    uSize = fm.GetSize();
    TEST_ASSERT(uSize==1346560);

    fm.Destroy();

    // Get size of closed filemapping - assume zero size
    uSize = fm.GetSize();
    TEST_ASSERT(uSize==0);

    __TEST_CLEANUP__;
}
