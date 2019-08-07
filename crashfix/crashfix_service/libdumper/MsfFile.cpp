//! \file MsfFile.cpp
//! \brief Multi-stream (MSF) file reading functionality.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStruct.h"
#include "PdbStreamStruct.h"

//--------------------------------------------------------
// CMsfFile impl
//--------------------------------------------------------

CMsfFile::CMsfFile()
{
    // Init internal variables
    m_bInitialized = FALSE;
    m_dwPdbVersion = 0;
    m_dwPageSize = 0;
    m_dwPageCount = 0;
    m_pRootStream = NULL;
    m_pIndexDirStream = NULL;
}

CMsfFile::~CMsfFile()
{
    Destroy();
}

bool CMsfFile::Init(std::wstring sFileName)
{
    // If already initialized, destroy
    if(m_bInitialized)
        Destroy();

    // Resulting status
    bool bStatus = FALSE;
    DWORD dwSkipBytes = 0;
    BOOL bRead = 0;
    DWORD dwStreamCount = 0;
    DWORD dwBytesCopied = 0;
    CMsfStream* pIndexDirStream = NULL;


    // Save current file name
    m_sFileName = sFileName;

    // Init file mapping
    if(!m_fm.Init(sFileName))
        goto cleanup; // File mapping initialization error

    // Read PDB header
    if(!ReadHeader())
        goto cleanup; // File has invalid or unsupported format

    // Read root stream

    pIndexDirStream = GetIndexDirStream();
    pIndexDirStream->SetStreamPos(0);
    bRead = pIndexDirStream->ReadData((LPBYTE)&dwStreamCount, sizeof(DWORD), &dwBytesCopied, TRUE);
    if(!bRead || dwBytesCopied!=sizeof(DWORD))
        return FALSE;

    dwSkipBytes = sizeof(DWORD)+dwStreamCount*sizeof(DWORD);
    UINT i;
    for(i=0; i<dwStreamCount; i++)
    {
        DWORD dwStreamLen = 0;
        m_pIndexDirStream->SetStreamPos((i+1)*sizeof(DWORD));
        BOOL bRead = m_pIndexDirStream->ReadData((LPBYTE)&dwStreamLen, sizeof(DWORD), &dwBytesCopied, TRUE);
        if(!bRead || dwBytesCopied!=sizeof(DWORD))
            return FALSE;

        if(dwStreamLen==PDB_UNUSED_32)
            continue;

        CMsfStream* pStream = new CMsfStream(i, dwStreamLen, this);
        m_apStreams.push_back(pStream);

        if(dwStreamLen==0)
            continue;

        m_pIndexDirStream->SetStreamPos(dwSkipBytes);

        DWORD dwPageCount = (dwStreamLen-1)/m_dwPageSize+1;
        UINT j;
        for(j=0; j<dwPageCount; j++)
        {
            DWORD dwPage = 0;
            BOOL bRead = m_pIndexDirStream->ReadData((LPBYTE)&dwPage, sizeof(DWORD), &dwBytesCopied, TRUE);
            if(!bRead || dwBytesCopied!=sizeof(DWORD))
                goto cleanup;

            pStream->AddPage(dwPage);
        }
        dwSkipBytes += dwPageCount*sizeof(DWORD);
    }

    bStatus = TRUE;

cleanup:

    if(bStatus)
    {
        m_bInitialized = TRUE;
    }
    else
    {
        Destroy();
    }


    return bStatus;
}

bool CMsfFile::IsInitialized()
{
    return m_bInitialized;
}

std::wstring CMsfFile::GetFileName()
{
    return m_sFileName;
}

void CMsfFile::Destroy()
{
    if(!m_bInitialized)
        return; // Not init yet

    m_bInitialized = FALSE;

    m_fm.Destroy();

    m_sFileName.clear();

    m_dwPdbVersion = 0;
    m_dwPageCount = 0;
    m_dwPageSize = 0;

    if(m_pRootStream)
    {
        delete m_pRootStream;
        m_pRootStream = NULL;
    }

    if(m_pIndexDirStream)
    {
        delete m_pIndexDirStream;
        m_pIndexDirStream = NULL;
    }

    UINT i;
    for(i=0; i<m_apStreams.size(); i++)
    {
        delete m_apStreams[i];
        m_apStreams[i] = NULL;
    }

    m_apStreams.clear();

}

bool CMsfFile::ReadHeader()
{
    DWORD dwFileSize = (DWORD)m_fm.GetSize();
    m_dwPdbVersion = 0;

    // Check if we have PDB v.2.00
    LPBYTE pSig200 = m_fm.CreateView(0, sizeof(PDB_SIGNATURE_200));
    if(pSig200!=NULL && memcmp(pSig200, PDB_SIGNATURE_200, sizeof(PDB_SIGNATURE_200))==0)
    {
        m_dwPdbVersion = PDB_VERSION_200;
    }

    if(m_dwPdbVersion==0)
    {
        // Check if we have PDB v.7.00
        LPBYTE pSig700 = m_fm.CreateView(0, sizeof(PDB_SIGNATURE_700));
        if(pSig700!=NULL && memcmp(pSig700, PDB_SIGNATURE_700, sizeof(PDB_SIGNATURE_700))==0)
            m_dwPdbVersion = PDB_VERSION_700;
    }

    if(m_dwPdbVersion==0)
        return false; // File signature check failed

    if(m_dwPdbVersion==PDB_VERSION_200)
    {
        PPDB_HEADER_200 pHeader = (PPDB_HEADER_200)m_fm.CreateView(0, sizeof(PDB_HEADER_200));
        if(pHeader->wPageCount*pHeader->dwPageSize!=dwFileSize)
            return false; // File size is invalid

        // We do not support PDB 2.0
        return false;
    }
    else if(m_dwPdbVersion==PDB_VERSION_700)
    {
        PPDB_HEADER_700 pHeader = (PPDB_HEADER_700)m_fm.CreateView(0, sizeof(PDB_HEADER_700));

        // Check file size
        if(pHeader->dwPageCount*pHeader->dwPageSize!=dwFileSize)
            return false; // File size is invalid

        // Get page count
        m_dwPageCount = pHeader->dwPageCount;

        // Get page size in bytes
        m_dwPageSize = pHeader->dwPageSize;

        // Create view of the first page
        pHeader = (PPDB_HEADER_700)m_fm.CreateView(0, m_dwPageSize);

        // Init root stream
        DWORD dwStreamLen = ((pHeader->dwRootStreamSize-1)/m_dwPageSize+1)*sizeof(DWORD);
        m_pRootStream = new CMsfStream(0, dwStreamLen, this);
        // Add pages to the root stream
        DWORD dwPages = (dwStreamLen-1)/m_dwPageSize+1;
        UINT i;
        for(i=0; i<dwPages; i++)
        {
            m_pRootStream->AddPage(pHeader->adwRootStreamPages[i]);
        }

        // Init index dir stream
        m_pIndexDirStream = new CMsfStream(0, pHeader->dwRootStreamSize, this);
        // Add pages to the index dir stream
        dwPages = (pHeader->dwRootStreamSize-1)/m_dwPageSize+1;
        for(i=0; i<dwPages; i++)
        {
            DWORD dwPage = 0;
            DWORD dwBytesRead = 0;
            BOOL bRead = m_pRootStream->ReadData((LPBYTE)&dwPage, sizeof(DWORD), &dwBytesRead, TRUE);
            if(!bRead || dwBytesRead!=sizeof(DWORD))
                return false;

            m_pIndexDirStream->AddPage(dwPage);
        }
    }

    // Done
    return true;
}

CMsfStream* CMsfFile::GetRootStream()
{
    // Return root stream
    return m_pRootStream;
}

CMsfStream* CMsfFile::GetIndexDirStream()
{
    // Return index dir stream
    return m_pIndexDirStream;
}

int CMsfFile::GetStreamCount()
{
    // Return stream count
    return (int)m_apStreams.size();
}

CMsfStream* CMsfFile::GetStream(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_apStreams.size())
        return NULL; // Invalid stream index

    return m_apStreams[nIndex];
}

