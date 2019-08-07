//! \file PdbHeadersStream.cpp
//! \brief PDB headers stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbHeadersStream.h"
#include "Buffer.h"
#include "strconv.h"

CPdbHeadersStream::CPdbHeadersStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult)
{
    *pbResult = Init(pPdbReader, pStream);
}

CPdbHeadersStream::~CPdbHeadersStream()
{

}

BOOL CPdbHeadersStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream)
{
    //! PDB headers stream (stream #1)
    //! The stream has the following structure:
    //! 1. Header (28 bytes) having age and GUID info.
    //! 2. Array of zero-terminated strings follow the header.
    //! 3. Array of 12 DWORD values follow the strings (purpose unknown).

    // Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;

    // Allocate buffer for the entire stream
    DWORD dwStreamLen = pStream->GetStreamLen();
    CBuffer buf(dwStreamLen);
    DWORD dwBytesRead = 0;
    BOOL bRead = pStream->ReadData(buf, dwStreamLen, &dwBytesRead, FALSE);
    if(!bRead || dwBytesRead!=dwStreamLen)
        return FALSE;

    PPDB_HEADERS_STREAM pHeaders = (PPDB_HEADERS_STREAM)(LPBYTE)buf;

    // Get time stamp
    m_TimeStamp = pHeaders->m_dwTimeStamp;

    // Format string time stamp
    char szTime[64]="";
#ifdef _WIN32
    ctime_s(szTime, 64, &m_TimeStamp);
#else
    m_sTimeStamp = strconv::a2w(ctime(&m_TimeStamp));
#endif
    m_sTimeStamp = std::wstring(strconv::a2w(szTime));
    // Get build age
    m_dwAge = pHeaders->m_dwAge;
    // Get GUID
    m_GUID = pHeaders->m_Guid;
    // Format string GUID
    wchar_t szBuf[120];
    swprintf(szBuf,
        120,
        L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
        m_GUID.Data1,
        m_GUID.Data2,
        m_GUID.Data3,
        UINT(m_GUID.Data4[0]),
        UINT(m_GUID.Data4[1]),
        UINT(m_GUID.Data4[2]),
        UINT(m_GUID.Data4[3]),
        UINT(m_GUID.Data4[4]),
        UINT(m_GUID.Data4[5]),
        UINT(m_GUID.Data4[6]),
        UINT(m_GUID.Data4[7]));
    m_sGUID = std::wstring(szBuf);

    // Read array of strings
    int cbNames = pHeaders->m_cbNames;
    int i;
    std::string sName;
    for(i=0;i<cbNames;i++)
    {
        char c = *(char*)( ((LPBYTE)buf)+sizeof(PDB_HEADERS_STREAM)+i );
        if(c==0)
        {
            m_asNames.push_back(sName);
            sName.clear();
        }
        else
            sName += c;
    }

    //assert(m_asNames.size()==3);

    // Read array of DWORDs
    int nCount = dwStreamLen-sizeof(PDB_HEADERS_STREAM)-cbNames;
    assert(nCount%4==0);
    //assert(nCount/4==12);
    nCount/=4;

    LPDWORD p = (LPDWORD)( ((LPBYTE)buf)+sizeof(PDB_HEADERS_STREAM)+cbNames );
    for(i=0; i<nCount; i++)
    {
        m_adwNumbers.push_back(p[i]);
    }

    return TRUE;
}

std::wstring CPdbHeadersStream::GetTimeStamp()
{
    return m_sTimeStamp;
}

DWORD CPdbHeadersStream::GetAge()
{
    return m_dwAge;
}

std::wstring CPdbHeadersStream::GetGUID()
{
    return m_sGUID;
}

std::wstring CPdbHeadersStream::GetGUIDnAge()
{
	std::wstringstream wstream;
	wstream << m_sGUID;
	wstream << m_dwAge;
	return wstream.str();
}
