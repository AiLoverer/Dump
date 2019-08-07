#include "stdafx.h"
#include "Tests.h"
#include "MsfFile.h"
#include "Utils.h"
#include "Buffer.h"

class MsfStreamTests : public CTestCase
{
    BEGIN_TEST_MAP(MsfStreamTests, "CMsfStream class tests")
        REGISTER_TEST(Test_GetStreamLen);
        REGISTER_TEST(Test_GetStreamPos);
        REGISTER_TEST(Test_SetStreamPos);
        REGISTER_TEST(Test_SetStreamPosRel);
        REGISTER_TEST(Test_ReadData);
    END_TEST_MAP()

public:

    CMsfFile m_MsfFile;

    void SetUp();
    void TearDown();

    void Test_GetStreamLen();
    void Test_GetStreamPos();
    void Test_SetStreamPos();
    void Test_SetStreamPosRel();
    void Test_ReadData();
};


REGISTER_TEST_CASE( MsfStreamTests );

void MsfStreamTests::SetUp()
{
    wstring sFileName = Utils::GetTestDataFolder();
    sFileName += L"crashtestd.pdb";
    //sFileName += L"CrashRptd.pdb";

    bool bInit = m_MsfFile.Init(sFileName.c_str());
    TEST_ASSERT(bInit);

    __TEST_CLEANUP__;
}

void MsfStreamTests::TearDown()
{
}

void MsfStreamTests::Test_GetStreamLen()
{
    CMsfStream* pStream = NULL;
    CMsfStream* pStream36 = NULL;

    pStream = m_MsfFile.GetStream(0);
    TEST_ASSERT(pStream);
    TEST_ASSERT(pStream->GetStreamLen()==4900);

    pStream36 = m_MsfFile.GetStream(36);
    TEST_ASSERT(pStream36);
    TEST_ASSERT(pStream36->GetStreamLen()==1248);

    __TEST_CLEANUP__;

}

void MsfStreamTests::Test_GetStreamPos()
{
    int nPos = -1;
    int nPos2 = -1;
    BOOL bSet = FALSE;

    CMsfStream* pStream = m_MsfFile.GetStream(13);
    TEST_ASSERT(pStream);

    // Assume stream pos is 0 at first
    nPos = pStream->GetStreamPos();
    TEST_ASSERT(nPos==0);

    // Modify stream pos
    bSet = pStream->SetStreamPos(15);
    TEST_ASSERT(bSet);

    // Check new stream pos
    nPos2 = pStream->GetStreamPos();
    TEST_ASSERT(nPos2==15);

    __TEST_CLEANUP__;
}

void MsfStreamTests::Test_SetStreamPos()
{
    BOOL bSetPos = FALSE;
    BOOL bSetPos2 = FALSE;

    CMsfStream* pStream = m_MsfFile.GetStream(15);
    TEST_ASSERT(pStream);

    // Set stream pos 0 - assume success
    bSetPos = pStream->SetStreamPos(0);
    TEST_ASSERT(bSetPos);

    // Set stream pos >= stream len - assume failure
    bSetPos2 = pStream->SetStreamPos(pStream->GetStreamLen());
    TEST_ASSERT(bSetPos2);

    __TEST_CLEANUP__;
}

void MsfStreamTests::Test_SetStreamPosRel()
{
    BOOL bSetPos = FALSE;
    BOOL bSetPos2 = FALSE;
    BOOL bSetPos3 = FALSE;
    int nPos = -1;

    CMsfStream* pStream = m_MsfFile.GetStream(16);
    TEST_ASSERT(pStream);

    // Set stream pos 0 - assume success
    bSetPos = pStream->SetStreamPos(0);
    TEST_ASSERT(bSetPos);

    // Decrease stream pos by 2 - assume failure
    bSetPos2 = pStream->SetStreamPosRel(-2);
    TEST_ASSERT(!bSetPos2);

    // Increase stream pos by 2 - assume success
    bSetPos3 = pStream->SetStreamPosRel(2);
    TEST_ASSERT(bSetPos3);

    // Check new stream pos
    nPos = pStream->GetStreamPos();
    TEST_ASSERT(nPos==2);

    __TEST_CLEANUP__;
}

void MsfStreamTests::Test_ReadData()
{
    CBuffer buf(256);
    CBuffer buf2(512);
    BOOL bRead = FALSE;
    BOOL bRead2 = FALSE;
    BOOL bRead3 = FALSE;
    BOOL bRead4 = FALSE;
    BOOL bRead5 = FALSE;
    int nStreamLen = -1;
    BOOL bSetPos = FALSE;
    DWORD dwBytesRead = 0;

    CMsfStream* pStream = m_MsfFile.GetStream(17);
    TEST_ASSERT(pStream);

    // Assume stream is not empty
    nStreamLen = pStream->GetStreamLen();
    TEST_ASSERT(nStreamLen>0);

    // Set stream pos 0 - assume success
    bSetPos = pStream->SetStreamPos(0);
    TEST_ASSERT(bSetPos);

    // Pass NULL buffer - assume failure
    dwBytesRead = 100;
    bRead = pStream->ReadData(NULL, 5, &dwBytesRead, TRUE);
    TEST_ASSERT(!bRead);
    TEST_ASSERT(dwBytesRead == 0); // Assume initialized bytes read
    TEST_ASSERT(pStream->GetStreamPos()==0); // Assume stream pos didn't change

    // Read 256 bytes to buffer - assume success
    dwBytesRead = 200;
    bRead2 = pStream->ReadData(buf, 256, &dwBytesRead, FALSE);
    TEST_ASSERT(bRead2);
    TEST_ASSERT(dwBytesRead == 256); // Assume initialized bytes read
    TEST_ASSERT(pStream->GetStreamPos()==0); // Assume stream pos didn't change

    // Read 256 bytes to buffer - assume success
    bRead3 = pStream->ReadData(buf, 256, NULL, FALSE);
    TEST_ASSERT(bRead3);
    TEST_ASSERT(pStream->GetStreamPos()==0); // Assume stream pos didn't change

    // Read 512 bytes to buffer and shift pos - assume success
    dwBytesRead = 200;
    bRead4 = pStream->ReadData(buf2, 512, &dwBytesRead, TRUE);
    TEST_ASSERT(bRead4);
    TEST_ASSERT(dwBytesRead == 512); // Assume initialized bytes read
    TEST_ASSERT(pStream->GetStreamPos()==512); // Assume stream pos has changed

    // Read 512 bytes to buffer and shift pos - assume success
    bRead5 = pStream->ReadData(buf2, 512, NULL, TRUE);
    TEST_ASSERT(bRead5);
    TEST_ASSERT(pStream->GetStreamPos()==1024); // Assume stream pos has changed

    __TEST_CLEANUP__;
}
