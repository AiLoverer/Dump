//! \file PdbPSGSIStream.cpp
//! \brief PDB globally compacted public symbol info (PSGSI) stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbPSGSIStream.h"
#include "Buffer.h"
#include "Log.h"

CPdbPSGSIStream::CPdbPSGSIStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult)  
{
    *pbResult = Init(pPdbReader, pStream);
}

BOOL CPdbPSGSIStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream)
{
    // PSI stream contains globally compacted public symbols???
    // PSI stream usually has number 7.
    // PSI stream has the following structure:
    // 1. Main header (28 bytes).
    // 2. Subheader (signature, version...) follows the main header. This subheader is the same as in GSI stream (hash records+buckets).
    // 3. GSI-like Hash records follow subheader.
    // 4. GSI-like Bitmap (512 bytes) follow hash records.
    // 5. GSI-like Sort table follows bitmap.
    // 6. Address sort table follows the GSI-like sort table.
    // 7. 

    // Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;

    // Read header into memory
    CBuffer PSIHeader(sizeof(_PSGSIHDR));
    DWORD dwBytesRead = 0;
    BOOL bRead = pStream->ReadData(PSIHeader, sizeof(_PSGSIHDR), &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=sizeof(_PSGSIHDR))
        return FALSE;

    // Read header    
    _PSGSIHDR* pPSIHdr = (_PSGSIHDR*)(LPBYTE)PSIHeader;

    // Allocate buffer for stream header
    CBuffer buf(sizeof(GSI_STREAM_HEADER));

    // Read header into memory
    dwBytesRead = 0;
    bRead = pStream->ReadData(buf, sizeof(GSI_STREAM_HEADER), &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=sizeof(GSI_STREAM_HEADER))
        return FALSE;

    // Get pointer to header    
    GSI_STREAM_HEADER* pHdr = (GSI_STREAM_HEADER*)(LPBYTE)buf;

    // Read hash records
    int nHashRecCount = (pHdr->dwHashTableSize)/sizeof(GSI_HASH_RECORD);
    int i;
    for(i=0; i<nHashRecCount;i++)
    {
        CBuffer buf(sizeof(GSI_HASH_RECORD));
        DWORD dwBytesRead = 0;
        BOOL bRead = pStream->ReadData(buf, sizeof(GSI_HASH_RECORD), &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=sizeof(GSI_HASH_RECORD))
            return FALSE;

        GSI_HASH_RECORD* pHashRec = (GSI_HASH_RECORD*)(LPBYTE)buf;
        m_aHashRecs.push_back(*pHashRec);
    }

    //
    // Read hash buckets: read bitmap (512 bytes) and DWORD hash table entries
    //

    // Read bitmap
    UINT uBitmapSize = 512;
    CBuffer bitmap(uBitmapSize);
    bRead = pStream->ReadData(bitmap, uBitmapSize, &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=uBitmapSize)
        return FALSE;

    // Walk through bitmap
    UINT uOccupiedEntries = 0;
    for(i=0; i<(int)uBitmapSize; i++)
    { 
        BYTE uchByteVal = *(((LPBYTE)bitmap)+i);

        BYTE uchShift;
        for(uchShift=0; uchShift<8; uchShift++)
        {
            BYTE uchBitVal = (uchByteVal>>uchShift)&0x1;
            if(uchBitVal!=0)
            {
                uOccupiedEntries++;
            }
        }    
    }

    // Read DWORD hash table entries
    UINT uEntryCount = (pHdr->dwBucketTableSize-uBitmapSize)/sizeof(DWORD);
    for(i=0; i<(int)uEntryCount; i++)
    {
        DWORD dwEntry = 0;
        BOOL bRead = pStream->ReadData((LPBYTE)&dwEntry, sizeof(DWORD), &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=sizeof(DWORD))
            return FALSE;

        m_aHashBuckets.push_back(dwEntry);
    }

    // Read address map
    UINT uAddrCount = pPSIHdr->cbAddrMap/sizeof(DWORD);
    for(i=0; i<(int)uAddrCount;i++)
    {
        DWORD dwEntry = 0;
        BOOL bRead = pStream->ReadData((LPBYTE)&dwEntry, sizeof(DWORD), &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=sizeof(DWORD))
            return FALSE;

        m_aAddrEntries.push_back(dwEntry);
    }

    return TRUE;
}

CPdbPSGSIStream::~CPdbPSGSIStream()
{
}

int CPdbPSGSIStream::GetAddrEntryCount()
{
    // Return count of addr entries
    return (int)m_aAddrEntries.size();
}

BOOL CPdbPSGSIStream::GetAddrEntry(int nIndex, LPDWORD pdwAddrEntry)
{  
    // Validate input params
    if(pdwAddrEntry==NULL || nIndex<0 || nIndex>=(int)m_aAddrEntries.size())
        return FALSE; // Invalid param

    // Return i-th addr entry
    *pdwAddrEntry = m_aAddrEntries[nIndex];
    return TRUE;
}
