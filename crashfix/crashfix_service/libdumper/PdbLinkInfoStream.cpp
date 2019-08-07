//! \file PdbLinkInfoStream.cpp
//! \brief PDB link info stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbLinkInfoStream.h"
#include "Buffer.h"
#include "Log.h"

CPdbLinkInfoStream::CPdbLinkInfoStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult)  
{
    *pbResult = Init(pPdbReader, pStream);
}

CPdbLinkInfoStream::~CPdbLinkInfoStream()
{
}

BOOL CPdbLinkInfoStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream)
{
    // Link info stream has number 4.
    // Link info stream has the following structure:
    // 1. Header (24 bytes).
    // 2. Array of zero-terminated strings representing linker command line.
    // 3. Some sequence of DWORD or WORD values (purpose unqnown).

    // Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;
    
    // Get stream length
    DWORD dwStreamLen = pStream->GetStreamLen();
    // Allocate buffer for entire stream
    CBuffer buf(dwStreamLen);

    // Read entire stream
    DWORD dwBytesRead = 0;    
    BOOL bRead = pStream->ReadData(buf, dwStreamLen, &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=dwStreamLen)
        return FALSE;

    // Read header

    PLINK_INFO_HEADER pHdr = (PLINK_INFO_HEADER)(LPBYTE)buf;
    m_dwDataLength = pHdr->m_dwDataLength;
    m_dwAge = pHdr->m_dwAge;

    // Read sequence of zero-separated strings following the header

    std::string str;  
    UINT i;
    for(i=0; i<m_dwDataLength; i++)
    {
        char c = (char)pHdr->m_chStrings[i];

        if(c!=0) 
        {
            str += c;
        }
        else
        {
            // End of string
            if(str.length()==0)
                break;

            // Add string to the list
            m_aStrings.push_back(str);
            str.clear();
        }
    }

    assert(m_aStrings.size()==3);

    // Read the sequence of DWORDs??? following the strings


    return TRUE;
}

DWORD CPdbLinkInfoStream::GetDataLength()
{
    return m_dwDataLength;
}

DWORD CPdbLinkInfoStream::GetAge()
{
    return m_dwAge;
}

UINT CPdbLinkInfoStream::GetStringCount()
{
    return (UINT)m_aStrings.size();
}

std::string CPdbLinkInfoStream::GetString(UINT i)
{
    return m_aStrings[i];
}