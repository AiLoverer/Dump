//! \file PdbDebugInfoStream.cpp
//! \brief PDB debug info (DBI) stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbDebugInfoStream.h"
#include "Buffer.h"
#include "Log.h"
#include "PdbReader.h"
#include "PdbSectionMapStream.h"
#include "strconv.h"

CPdbDebugInfoStream::CPdbDebugInfoStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult)  
{
    *pbResult = Init(pPdbReader, pStream);
}

BOOL CPdbDebugInfoStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream)
{
    // DBI stream has number 3.
    // DBI stream has the following structure:
    // 1. Header DBI_STREAM_HEADER_700 (64 bytes). The header contains sizes of each section that follow the header.
    // 2. Module info section. Array of variable-sized blocks (DBI_MODULE_INFO followed by two zero-terminated strings).
    //    This section contains info on every module contributed to this PDB module.
    // 3. Section contribution. 4-byte header (its purpose unknown), followed by sequence of DBI_SECTION_CONTRIB structures.
    // 4. Section bitmap. Each bit state means free/occupied section???.
    // 5. File index section. Described in cv4_sym.pdf.
    // 6. Another file section. Header (10 bytes), the array of zero-terminated strings, then array of DWORDs.

    // First init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;

    CPdbSectionMapStream* pSectionStream = NULL;

    // Get stream length
    //DWORD dwStreamLen = pStream->GetStreamLen();
    CBuffer header(sizeof(DBI_STREAM_HEADER_700));

    // Read entire stream into memory
    DWORD dwBytesRead = 0;
    BOOL bRead = pStream->ReadData((LPBYTE)header, sizeof(DBI_STREAM_HEADER_700), &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=sizeof(DBI_STREAM_HEADER_700))
        return FALSE;

    // Read header
    DBI_STREAM_HEADER_700* pHdr = (DBI_STREAM_HEADER_700*)(LPBYTE)header;

    assert(pHdr->dwSignature==0xFFFFFFFF);
    assert(pHdr->dwVersion==0x1310977);

    m_dwMachineType = pHdr->wMachine;
    m_nGlobalSymStream = pHdr->wGSStreamNumber;
    m_nPublicSymStream = pHdr->wPSStreamNumber;
    m_nSymStream = pHdr->wSymStreamNumber;

    // Get section map stream
    pSectionStream = pPdbReader->GetSectionMapStream();      
    assert(pSectionStream!=NULL);
    if(!pSectionStream)
        return FALSE;

    DWORD dwOffs = sizeof(DBI_STREAM_HEADER_700);

    // Read MODI records
    while((LONG)(dwOffs-sizeof(DBI_STREAM_HEADER_700))<pHdr->cbModuleInfo)
    {
        pStream->SetStreamPos(dwOffs);

        DBI_MODULE_INFO modi;

        DWORD dwBytesRead = 0;  
        BOOL bRead = pStream->ReadData((LPBYTE)&modi, sizeof(DBI_MODULE_INFO), &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=sizeof(DBI_MODULE_INFO))
            break;

        dwOffs += sizeof(DBI_MODULE_INFO);

        std::string sModuleName;
        std::string sModuleName2;

        // Read module name
        char c = modi.uchName[0];
        while(c!='\0')
        {
            sModuleName += c;

            bRead = pStream->ReadData((LPBYTE)&c, sizeof(char), &dwBytesRead, TRUE);
            if(!bRead || dwBytesRead!=sizeof(char))
                break;      

            dwOffs++;
        }

        // Read module name    
        for(;;)
        {
            bRead = pStream->ReadData((LPBYTE)&c, sizeof(char), &dwBytesRead, TRUE);
            if(!bRead || dwBytesRead!=sizeof(char))
                break;      

            dwOffs++;

            if(c==0)
                break;

            sModuleName2 += c;
        }

        if(dwOffs%4!=0)
        {
            dwOffs += 4-dwOffs%4;
        }

        // Add to list
        DBI_ModuleInfo mi;
        mi.m_nModuleIndex = (int)m_aModules.size();
        mi.m_Info = modi;
        mi.m_sSrcModuleName = sModuleName;
        mi.m_sTargetModuleName = sModuleName2;
        m_aModules.push_back(mi);
    }

    // Read section contrib section

    dwOffs = sizeof(DBI_STREAM_HEADER_700) + 
        pHdr->cbModuleInfo+4;

    pStream->SetStreamPos(dwOffs);

    DWORD cbSectionContrib = pHdr->cbSectionContrib;

    int nSectionCount = cbSectionContrib/sizeof(DBI_SECTION_CONTRIB);
    //ATLASSERT(cbSectionContrib%sizeof(DBI_SECTION_CONTRIB)==0);

    int i;
    for(i=0; i<nSectionCount; i++)
    {
        DBI_SECTION_CONTRIB sc;

        DWORD dwBytesRead = 0;  
        BOOL bRead = pStream->ReadData((LPBYTE)&sc, sizeof(DBI_SECTION_CONTRIB), &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=sizeof(DBI_SECTION_CONTRIB))
            break;

        m_aSectionContrib.push_back(sc);

        if(sc.SectionContrib40.wSectionIndex>0)
        {
            IMAGE_SECTION_HEADER* pSection = pSectionStream->GetSection(sc.SectionContrib40.wSectionIndex-1);
            DWORD dwRVA = pSection->VirtualAddress+sc.SectionContrib40.lOffset;            
            m_aSectionContribRvas[dwRVA] = (int)m_aSectionContrib.size()-1;
        }
    }

    // Read section map (bitmap).

    UINT uBitmapSize = pHdr->cbSectionMap;
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

    // Read file info section

    dwOffs = sizeof(DBI_STREAM_HEADER_700) + 
        pHdr->cbModuleInfo + 
        pHdr->cbSectionContrib +
        pHdr->cbSectionMap;

    DWORD cbFileInfo = pHdr->cbFileInfo;

    CBuffer buf(cbFileInfo);
    pStream->SetStreamPos(dwOffs);
    bRead = pStream->ReadData((LPBYTE)buf, cbFileInfo, &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=cbFileInfo)
        return FALSE;  

    dwOffs = 0;

    // Read count of modules
    WORD wModuleCount = *(LPWORD)((LPBYTE)buf+dwOffs);
    dwOffs += 2;

    // Read count of references
    WORD wRefCount = *(LPWORD)((LPBYTE)buf+dwOffs);
    dwOffs += 2;

    // Read array of indices into the NameOffset table for each module. Each
    // index is the start of the file name references for each module.
    for(i=0; i<wModuleCount; i++)
    {
        WORD wModuleStart = *(LPWORD)((LPBYTE)buf+dwOffs);
        m_aModStart.push_back(wModuleStart);
        dwOffs+=2;
    }  

    // Read number of file name references per module.
    for(i=0; i<wModuleCount; i++)
    {
        WORD wRefCount = *(LPWORD)((LPBYTE)buf+dwOffs);
        m_aRefCount.push_back(wRefCount);
        dwOffs+=2;
    }

    // Read array of offsets into the Names table. For each module, the offset to
    // first referenced file name is at NameRef[ModStart] and continues for
    // cRefCnt entries.
    for(i=0; i<wRefCount; i++)
    {
        DWORD dwNameOffs = *(LPDWORD)((LPBYTE)buf+dwOffs);
        m_aNameRefs.push_back(dwNameOffs);
        dwOffs+=4;
    }

    // Read file names.  
    DWORD dwNamesStart = dwOffs;    
    std::string sName;
    _SrcFileInfo sfi;
    sfi.m_dwFileNameOffset = 0;
    for(;;)
    {
		if(dwOffs>=cbFileInfo)
			break; // out of buffer

        char c = *(char*)((LPBYTE)buf+dwOffs);

        if(c!=0)
            sName += c;
        else
        {
            if(sName.length()==0)
                break;
            else
            {        
                sfi.m_sFileName = strconv::a2w(sName);                
                m_asFileNames.push_back(sfi);
                m_aFileNameOffs[sfi.m_dwFileNameOffset] = (int)m_asFileNames.size()-1;
                m_aFileNameIndex[sfi.m_sFileName] = sfi.m_dwFileNameOffset;
                sfi.m_dwFileNameOffset = dwOffs-dwNamesStart+1;
                sName.clear();                
            }
        }

        dwOffs+=1;
    }

    // Read dbghdr and ecinfo

    // Read file info section

    dwOffs = sizeof(DBI_STREAM_HEADER_700) + 
        pHdr->cbModuleInfo + 
        pHdr->cbSectionContrib +
        pHdr->cbSectionMap + 
        pHdr->cbFileInfo;

    dwNamesStart = dwOffs;

    DWORD cbECInfo = pHdr->cbDbgHdr+pHdr->cbECInfo;

    buf.Reallocate(cbECInfo);
    pStream->SetStreamPos(dwOffs);
    bRead = pStream->ReadData((LPBYTE)buf, cbECInfo, &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=cbECInfo)
        return FALSE;  

    DBI_ECINFO_HEADER* pECHdr = (DBI_ECINFO_HEADER*)(LPBYTE)buf;

    assert(pECHdr->dwSignature==0xEFFEEFFE);
    
    // Read file names.
    dwOffs = 0;
    sfi.m_dwFileNameOffset = 0;
    std::string sFileName;
    for(;dwOffs<pECHdr->cbNames;dwOffs++)
    {
        char c = *(char*)((LPBYTE)buf+sizeof(DBI_ECINFO_HEADER)+dwOffs);

        if(c!=0)
            sFileName += c;
        else
        {
            sfi.m_sFileName = strconv::a2w(sFileName);                
            m_asECFileNames.push_back(sfi);
            m_aECFileNameOffs[sfi.m_dwFileNameOffset] = (int)m_asECFileNames.size()-1;
            sfi.m_dwFileNameOffset = dwOffs+1;            
            sFileName.clear();      
        }   
    }

    // Read array of <DWORD, DWORD> pairs
    for(;dwOffs+8<cbECInfo;)
    {        
        DWORD dwFirst = *(DWORD*)((LPBYTE)buf+sizeof(DBI_ECINFO_HEADER)+dwOffs);
        dwOffs+=4;
        DWORD dwSecond = *(DWORD*)((LPBYTE)buf+sizeof(DBI_ECINFO_HEADER)+dwOffs);
        dwOffs+=4;

        _ECPair pair;
        pair.m_dwFirst = dwFirst;
        pair.m_dwSecond = dwSecond;

        m_aECMappings.push_back(pair);
    }

    return TRUE;
}

CPdbDebugInfoStream::~CPdbDebugInfoStream()
{
}

DWORD CPdbDebugInfoStream::GetMachineType()
{
    return m_dwMachineType;
}

int CPdbDebugInfoStream::GetGSIStreamIndex()
{
    return m_nGlobalSymStream;
}

int CPdbDebugInfoStream::GetPSIStreamIndex()
{
    return m_nPublicSymStream;
}

int CPdbDebugInfoStream::GetSymbolStreamIndex()
{
    return m_nSymStream;
}

int CPdbDebugInfoStream::GetModuleCount()
{
    return (int)m_aModules.size();
}

DBI_ModuleInfo* CPdbDebugInfoStream::GetModuleInfo(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_aModules.size())
        return NULL;

    return &m_aModules[nIndex];
}

int CPdbDebugInfoStream::GetSectionContribCount()
{
    return (int)m_aSectionContrib.size();
}

DBI_SECTION_CONTRIB* CPdbDebugInfoStream::GetSectionContrib(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_aSectionContrib.size())
        return NULL;

    return &m_aSectionContrib[nIndex];
}

int CPdbDebugInfoStream::FindSectionContribByAddr(DWORD64 dwAddr)
{
    std::map<DWORD, int>::iterator it = m_aSectionContribRvas.lower_bound((DWORD)dwAddr);
    it--;
    if(it==m_aSectionContribRvas.end())
        return -1;

    return it->second;
}

int CPdbDebugInfoStream::GetSrcFileNameCount()
{
    return (int)m_asFileNames.size();
}

const wchar_t* CPdbDebugInfoStream::GetSrcFileName(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_asFileNames.size())
        return NULL;

    return m_asFileNames[nIndex].m_sFileName.c_str();
}

int CPdbDebugInfoStream::FindSrcFileNameByOffs(DWORD dwNameOffs)
{
    std::map<DWORD, int>::iterator it = m_aFileNameOffs.find(dwNameOffs);
    if(it==m_aFileNameOffs.end())
        return -1; // Not found

    return it->second;    
}

int CPdbDebugInfoStream::FindSrcFileNameByModule(int nModuleIndex, int nCheckSumIndex)
{
    if(nModuleIndex<0 || nModuleIndex>=(int)m_aModules.size())
        return -1;

    int nModStart = m_aModStart[nModuleIndex];
    int nRefCount = m_aRefCount[nModuleIndex];
    
	if(nCheckSumIndex<0 || 
		nCheckSumIndex>=nRefCount || 
		nModStart+nCheckSumIndex>=(int)m_aNameRefs.size())
        return -1;
    
    return FindSrcFileNameByOffs(m_aNameRefs[nModStart+nCheckSumIndex]);
}