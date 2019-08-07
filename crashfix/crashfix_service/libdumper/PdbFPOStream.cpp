//! \file PdbFPOStream.cpp
//! \brief PDB stream containing FPO records.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbFPOStream.h"
#include "Buffer.h"
#include "Log.h"
#include "strconv.h"


CPdbFPOStream::CPdbFPOStream()
{
    m_bNewFPO = FALSE;
}

CPdbFPOStream::CPdbFPOStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL bNewFPO, BOOL* pbResult)  
{
    *pbResult = Init(pPdbReader, pStream, bNewFPO);
}

BOOL CPdbFPOStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL bNewFPO)
{
    // Frame pointer omission (FPO) stream has stream number 5 (or 11 for new FPO format).
    // FPO stream is a sequence of _FPO_DATA records (or _FPO_DATA_V2 for new FPO format).

    // Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;

    m_bNewFPO = bNewFPO;

    // Get stream len
    DWORD dwStreamLen = pStream->GetStreamLen();

    // Get length of FPO record
    DWORD dwFPODataLen = m_bNewFPO?sizeof(_FPO_DATA_V2):sizeof(_FPO_DATA);

    // Stream len must be a multiple of _FPO_DATA structure len
    if(dwStreamLen%dwFPODataLen!=0)
    {
		// Stream len is invalid
        return FALSE;
    }

	// Read the first DWORD of the stream (check if this is a valid FPO stream).
	DWORD dwHeader = 0;	
    DWORD dwBytesRead = 0;  
    BOOL bRead = pStream->ReadData((LPBYTE)&dwHeader, 4, &dwBytesRead, FALSE);
    if(!bRead || dwBytesRead!=4)
        return FALSE;

	if(dwHeader==0x4)
	{
		// FPO stream usually doesn't start with such a signature, probably this is a compiland stream
		return FALSE;
	}
		
    // Get count of FPO records in the stream
    int nCount = dwStreamLen/dwFPODataLen;

    // Read FPO records
    int i;  
    for(i=0; i<nCount; i++)
    {
        FPO_DATA_RECORD rec;

        // Read a record
        DWORD dwBytesRead = 0;  
        BOOL bRead = pStream->ReadData((LPBYTE)&rec, dwFPODataLen, &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=dwFPODataLen)
            break;

        // Add to list
        DWORD ulOffStart = 0;
        if(m_bNewFPO)
            ulOffStart = rec.fpo2.ulOffStart;
        else
            ulOffStart = rec.fpo.ulOffStart;

        m_FPORecords.push_back(rec);
        m_RVAs[ulOffStart] = (int)m_FPORecords.size()-1;
    }

    return TRUE;
}

CPdbFPOStream::~CPdbFPOStream()
{
}

BOOL CPdbFPOStream::IsNewFPO()
{
    return m_bNewFPO;
}

int CPdbFPOStream::GetRecordCount()
{
    return (int)m_FPORecords.size();
}

FPO_DATA_RECORD* CPdbFPOStream::GetFPORecord(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_FPORecords.size())
        return NULL;

    return &m_FPORecords[nIndex];
}

int CPdbFPOStream::FindFPORecordByAddr(DWORD dwAddr)
{
    std::map<DWORD, int>::iterator it = m_RVAs.upper_bound(dwAddr);
    if(it==m_RVAs.end())
        return -1; // Record not found.
	
	if(it==m_RVAs.begin())
	    return -1; // Record not found.

	it--; // prev record
    int nIndex = it->second;

    if(m_bNewFPO)
    {
        if(m_FPORecords[nIndex].fpo2.ulOffStart<=dwAddr && 
			dwAddr<m_FPORecords[nIndex].fpo2.ulOffStart+m_FPORecords[nIndex].fpo2.cbProcSize)
        {
            return nIndex;
        }
    }
    else
    {
        if(m_FPORecords[nIndex].fpo.ulOffStart<=dwAddr && 
			dwAddr<m_FPORecords[nIndex].fpo.ulOffStart+m_FPORecords[nIndex].fpo.cbProcSize)
        {
            return nIndex;
        }
    }

    return -1;
}
