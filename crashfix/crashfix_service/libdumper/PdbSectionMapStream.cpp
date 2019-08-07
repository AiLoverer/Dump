//! \file PdbSectionMapStream.cpp
//! \brief PDB section map stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbSectionMapStream.h"
#include "Buffer.h"
#include "strconv.h"

CPdbSectionMapStream::CPdbSectionMapStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult)
{
    *pbResult = Init(pPdbReader, pStream);
}

BOOL CPdbSectionMapStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream)
{
    // Section map stream usually has stream number #9
    // Section map stream is a sequence of IMAGE_SECTION_HEADER records.

    // Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;

    // Allocate buffer for the entire stream
    DWORD dwStreamLen = pStream->GetStreamLen();
    if(dwStreamLen%sizeof(IMAGE_SECTION_HEADER)!=0)
    {
        // Invalid stream length
        //assert(0); 
        return FALSE;
    }

    CBuffer buf(dwStreamLen);
    DWORD dwBytesRead = 0;
    BOOL bRead = pStream->ReadData(buf, dwStreamLen, &dwBytesRead, FALSE);
    if(!bRead || dwBytesRead!=dwStreamLen)
        return FALSE; 

    int nSectionCount = dwStreamLen/sizeof(IMAGE_SECTION_HEADER);
    int i;
    for(i=0; i<nSectionCount; i++)
    {
        IMAGE_SECTION_HEADER* pSection = ((IMAGE_SECTION_HEADER*)buf.GetPtr())+i;

        m_aSections.push_back(*pSection);
    }  

    return TRUE;  
}

CPdbSectionMapStream::~CPdbSectionMapStream()
{
}

int CPdbSectionMapStream::GetSectionCount()
{
    return (int)m_aSections.size();
}

IMAGE_SECTION_HEADER* CPdbSectionMapStream::GetSection(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_aSections.size())
        return NULL; // Invalid index

    return &m_aSections[nIndex];
}