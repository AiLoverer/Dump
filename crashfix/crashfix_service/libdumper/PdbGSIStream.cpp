//! \file PdbGSIStream.cpp
//! \brief PDB global symbol info (GSI) stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbGSIStream.h"
#include "Buffer.h"
#include "Log.h"

CPdbGSIStream::CPdbGSIStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult) 
{
    *pbResult = Init(pPdbReader, pStream);
}

BOOL CPdbGSIStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream)
{
    // GSI stream contains globally compacted symbols???
    // GSI stream usually has number 6.
    // GSI stream has the following structure:
    // 1. Header (16 bytes) contains signature (4 bytes), version (4 bytes), hash table size (4 bytes), bucket table size (4 bytes).
    // 2. Hash table follows the header. Hash table is sequence of pairs (4byte hash, 4byte refcount).
    // 3. Bucket table follows the hash table.
    // Bucket table has the following structure:
    // 1. 512-byte bitmap (covers 512x8=4096 elements).
    // 2. Something sort table (sequence of sorted DWORD elements).

    // Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;

    // Allocate buffer for stream header
    CBuffer buf(sizeof(GSI_STREAM_HEADER));

    // Read header into memory
    DWORD dwBytesRead = 0;
    BOOL bRead = pStream->ReadData(buf, sizeof(GSI_STREAM_HEADER), &dwBytesRead, TRUE);
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
                uOccupiedEntries++;
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

    return TRUE;
}

CPdbGSIStream::~CPdbGSIStream()
{
}