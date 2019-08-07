//! \file MsfStream.cpp
//! \brief A stream of a multi-stream (MSF) file.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "MsfFile.h"
#include "MsfStream.h"

#define MIN(a, b) ((a)<(b)?(a):(b))

//--------------------------------------------------------
// CMsfStream impl
//--------------------------------------------------------

CMsfStream::CMsfStream(DWORD dwStreamId, DWORD dwStreamLen, CMsfFile* pFile)
{
    // Init local variables
    m_dwStreamId = dwStreamId;  
    m_dwStreamLen = dwStreamLen;
    m_dwStreamPos = 0;
    m_pFile = pFile;
}

CMsfStream::~CMsfStream()
{
    m_pFile = NULL;
}

DWORD CMsfStream::GetStreamLen()
{
    // Return steam length
    return m_dwStreamLen;
}

BOOL CMsfStream::GetStreamPos()
{
    // Return current stream pos.
    return m_dwStreamPos;
}

BOOL CMsfStream::SetStreamPos(DWORD dwPos)
{
    // Validate input param
    if(dwPos>m_dwStreamLen)
        return FALSE; 

    // Modify current reading pos
    m_dwStreamPos = dwPos;
    return TRUE;
}

BOOL CMsfStream::SetStreamPosRel(LONG lDeltaPos)
{
    DWORD dwPos = GetStreamPos();
    LONG lNewPos = dwPos + lDeltaPos;
    return SetStreamPos(lNewPos);
}

BOOL CMsfStream::ReadData(LPBYTE pBuffer, DWORD cbBuffer, LPDWORD pdwBytesCopied, BOOL bShiftStreamPos)
{ 
    if(pdwBytesCopied!=NULL)
    {
        // Init output
        *pdwBytesCopied=0;
    }

    // Validate input parameters
    if(pBuffer==NULL ||
        cbBuffer==0)
    {
        // Invalid input parameter
        return FALSE;
    }

    // Init output param
    if(pdwBytesCopied)
        *pdwBytesCopied = 0;

    DWORD dwBytesLeftInStream = m_dwStreamLen-m_dwStreamPos;
    DWORD dwBytesToCopy = MIN(cbBuffer, dwBytesLeftInStream);
    DWORD dwBytesCopied = 0;  

    DWORD dwPageIndex = m_dwStreamPos/m_pFile->m_dwPageSize;
    DWORD dwOffsInPage = m_dwStreamPos%m_pFile->m_dwPageSize;
    DWORD dwBytesLeftInPage = m_pFile->m_dwPageSize-dwOffsInPage;

    for(;;)
    {
        if(dwBytesLeftInPage==0)
        {
            dwPageIndex++;
            dwOffsInPage = 0;
            dwBytesLeftInPage = m_pFile->m_dwPageSize;

            if(dwPageIndex>=m_aPages.size())
                break;      
        }

        DWORD dwBytesCanCopy = MIN(dwBytesToCopy, dwBytesLeftInPage);
        if(dwBytesCanCopy==0)
            break;

        LPBYTE pData = (LPBYTE)m_pFile->m_fm.CreateView(
            m_aPages[dwPageIndex]*m_pFile->m_dwPageSize+dwOffsInPage, dwBytesCanCopy); 

        memcpy(pBuffer+dwBytesCopied, pData, dwBytesCanCopy);    

        dwOffsInPage += dwBytesCanCopy;
        dwBytesCopied += dwBytesCanCopy;
        dwBytesToCopy -= dwBytesCanCopy;
        dwBytesLeftInPage = m_pFile->m_dwPageSize-dwOffsInPage;

        //if(dwBytesCopied==dwBytesToCopy)
        if(0==dwBytesToCopy)
            break;
    }

    // Increment stream pos
    if(bShiftStreamPos)
    {
        m_dwStreamPos += dwBytesCopied;
    }

    // Set actual bytes copied
    if(pdwBytesCopied)
        *pdwBytesCopied = dwBytesCopied;

    return TRUE;
}

void CMsfStream::AddPage(DWORD dwPage)
{
    // Insert page to the list of pages
    m_aPages.push_back(dwPage);
}


