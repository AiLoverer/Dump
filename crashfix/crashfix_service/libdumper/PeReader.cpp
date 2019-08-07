//! \file PeReader.cpp
//! \brief Portable executable (PE) format reader.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PeReader.h"
#include "PeStruct.h"
#include "strconv.h"

CPeReader::CPeReader()
{
    m_bInitialized = FALSE;
    m_dwMachineType = 0;
    m_bDebugInfoStripped = FALSE;
    m_bIsDLL = FALSE;
    m_bIsPE32Plus = FALSE;
    m_pCvRecord = NULL;
    m_dwCvRecSize = 0;
    m_pMiscRecord = NULL;
    m_dwMiscRecSize = 0;
    m_pVersionInfo = NULL;
}

CPeReader::~CPeReader()
{
    Destroy();
}

BOOL CPeReader::Init(std::wstring sFileName)
{
    BOOL bStatus = FALSE;
    DWORD dwOffs = 0;
    PIMAGE_DATA_DIRECTORY pDebugDataDirectory;
    IMAGE_DEBUG_DIRECTORY DebugDirectory;
    ULONG uIndex = 0;
    ULONG uNumberOfDebugDirs = 0;
    DWORD uBytesRead = 0;
    DWORD dwSizeOfOptionalHeader = 0;
    int nSectionCount = 0;
    PIMAGE_NT_HEADERS pNtHeader = NULL;
    DWORD dwNtHeaderOffs = 0;
    PIMAGE_DOS_HEADER pDosHeader = NULL;
    DWORD dwDebugDataDirectoryVA = 0;

    // Check if already initialized
    if(m_bInitialized)
        Destroy();

    // Save file name
    m_sFileName = sFileName;

    // Init file mapping
    BOOL bInit = m_fm.Init(sFileName);
    if(!bInit)
        goto exit;

    // Get DOS header
    pDosHeader = ( PIMAGE_DOS_HEADER ) m_fm.CreateView(0, sizeof(IMAGE_DOS_HEADER));

    if(pDosHeader==NULL)
        goto exit;

    dwNtHeaderOffs = pDosHeader->e_lfanew;

    // Get NT header
    pNtHeader = (PIMAGE_NT_HEADERS) m_fm.CreateView(dwNtHeaderOffs, sizeof(IMAGE_NT_HEADERS));

    if(pNtHeader==NULL)
        goto exit;

    // Check signature
    if( IMAGE_NT_SIGNATURE != pNtHeader->Signature )
        goto exit;

    // Check that the image file is valid and can be run.
    // If this flag is not set, it indicates a linker error.
    if(0==(pNtHeader->FileHeader.Characteristics & IMAGE_FILE_EXECUTABLE_IMAGE))
        goto exit;

    m_DateTimeStamp = pNtHeader->FileHeader.TimeDateStamp;
    m_dwCheckSum = pNtHeader->OptionalHeader.CheckSum;
    m_dwMachineType = pNtHeader->FileHeader.Machine;
    m_bDebugInfoStripped = 0!=(pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DEBUG_STRIPPED);
    m_bIsDLL = 0!=(pNtHeader->FileHeader.Characteristics & IMAGE_FILE_DLL);
    m_bIsPE32Plus = pNtHeader->OptionalHeader.Magic==0x20b;

	dwSizeOfOptionalHeader = pNtHeader->FileHeader.SizeOfOptionalHeader;
	nSectionCount = pNtHeader->FileHeader.NumberOfSections;

    // Get debug data directory
    if(!m_bIsPE32Plus)
		pDebugDataDirectory = &pNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	else
	{
		PIMAGE_NT_HEADERS64 pNtHeader64 =
			(PIMAGE_NT_HEADERS64) m_fm.CreateView(dwNtHeaderOffs, sizeof(IMAGE_NT_HEADERS64));
		pDebugDataDirectory = &pNtHeader64->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
	}

    uNumberOfDebugDirs = pDebugDataDirectory->Size/sizeof(IMAGE_DEBUG_DIRECTORY);

    // Check debug directory size
    if((pDebugDataDirectory->Size % sizeof(IMAGE_DEBUG_DIRECTORY))!= 0)
        return FALSE;

	dwDebugDataDirectoryVA = pDebugDataDirectory->VirtualAddress;

    // Read sections
    dwOffs = dwNtHeaderOffs + offsetof(IMAGE_NT_HEADERS, OptionalHeader) + dwSizeOfOptionalHeader;
    int i;
    for(i=0;i<nSectionCount; i++)
    {
        IMAGE_SECTION_HEADER* pSection =
            (IMAGE_SECTION_HEADER*)m_fm.CreateView(dwOffs, sizeof(IMAGE_SECTION_HEADER));
        if(pSection==NULL)
            break;

        m_aSectionHeaders.push_back(*pSection);

        // Look for ".rcsrc" section containing resources
        if(memcmp(pSection->Name, ".rsrc", 5)==0)
        {
            ULONG uResourceDirRva = pSection->PointerToRawData;
            ULONG uResourceDirSize = pSection->SizeOfRawData;

			assert(m_pVersionInfo==NULL);
            BrowseResourceDirForVersionInfo(&m_aSectionHeaders[m_aSectionHeaders.size()-1], 0, 0, uResourceDirRva,
                uResourceDirRva, uResourceDirSize, &m_pVersionInfo);
        }

        dwOffs += sizeof(IMAGE_SECTION_HEADER);
    }

    // Look for CodeView record.
    for (uIndex = 0; uIndex < uNumberOfDebugDirs; uIndex++ )
    {
        // Read debug directory
		DWORD dwDebugDataDirectoryRVA = 0;
		BOOL bGetRVA = GetRvaByVA(dwDebugDataDirectoryVA, dwDebugDataDirectoryRVA);
		if(!bGetRVA)
			return FALSE;

		DWORD uDebugDirRva = dwDebugDataDirectoryRVA+uIndex*sizeof(IMAGE_DEBUG_DIRECTORY);

        if(!ReadImageMemory(uDebugDirRva,
            &DebugDirectory, sizeof(IMAGE_DEBUG_DIRECTORY), &uBytesRead) ||
            uBytesRead!=sizeof(IMAGE_DEBUG_DIRECTORY))
            return FALSE; // Couldn't read debug directory

        if(DebugDirectory.Type==IMAGE_DEBUG_TYPE_CODEVIEW)
        {
            m_pCvRecord = new BYTE[DebugDirectory.SizeOfData];
            m_dwCvRecSize = DebugDirectory.SizeOfData;

            // Read CodeView data
            if(!ReadImageMemory(DebugDirectory.PointerToRawData,
                m_pCvRecord, m_dwCvRecSize, &uBytesRead) ||
                uBytesRead!=m_dwCvRecSize)
                return FALSE; // Couldn't read CodeView data

            if(1/*m_dwCvRecSize==sizeof(CV_INFO_PDB70)*/ /*&&
                memcpy(m_pCvRecord, CV_SIGNATURE_PDB70, 4)==0*/)
            {
                CV_INFO_PDB70* pCvInfo = (CV_INFO_PDB70*)m_pCvRecord;
                m_nCvAge = pCvInfo->Age;
                wchar_t szBuf[120]=L"";
                swprintf(szBuf,
                    120,
                    L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                    pCvInfo->Signature.Data1,
                    pCvInfo->Signature.Data2,
                    pCvInfo->Signature.Data3,
                    UINT(pCvInfo->Signature.Data4[0]),
                    UINT(pCvInfo->Signature.Data4[1]),
                    UINT(pCvInfo->Signature.Data4[2]),
                    UINT(pCvInfo->Signature.Data4[3]),
                    UINT(pCvInfo->Signature.Data4[4]),
                    UINT(pCvInfo->Signature.Data4[5]),
                    UINT(pCvInfo->Signature.Data4[6]),
                    UINT(pCvInfo->Signature.Data4[7]));
                m_sCvPdbGUID = std::wstring(szBuf);
                m_sCvPdbName = strconv::a2w(std::string((char*)pCvInfo->PdbFileName));
            }
        }
        else if(DebugDirectory.Type==IMAGE_DEBUG_TYPE_MISC)
        {
            m_pCvRecord = new BYTE[DebugDirectory.SizeOfData];
            m_dwMiscRecSize = DebugDirectory.SizeOfData;

            // Read misc data
            if(!ReadImageMemory(DebugDirectory.AddressOfRawData,
                m_pMiscRecord, m_dwMiscRecSize, &uBytesRead) ||
                uBytesRead!=m_dwMiscRecSize)
                return FALSE; // Couldn't read CodeView data
        }
    }


    // Done
    bStatus = TRUE;
    m_bInitialized = TRUE;

exit:

    if(!bStatus)
    {
        Destroy();
    }

    return bStatus;
}

BOOL CPeReader::IsInitialized()
{
    return m_bInitialized;
}

void CPeReader::Destroy()
{
    m_bInitialized = FALSE;
    m_sFileName.empty();
    m_fm.Destroy();
    m_dwMachineType = 0;
    m_bDebugInfoStripped = TRUE;
    m_bIsDLL = FALSE;
    m_dwCheckSum = 0;
    m_DateTimeStamp = 0;
    m_sCvPdbName.empty();
    m_sCvPdbGUID.empty();
    m_nCvAge = -1;

    if(m_pCvRecord!=NULL)
    {
        delete m_pCvRecord;
        m_pCvRecord = NULL;
    }

    if(m_pMiscRecord!=NULL)
    {
        delete m_pMiscRecord;
        m_pMiscRecord = NULL;
    }

    if(m_pVersionInfo!=NULL)
    {
        delete m_pVersionInfo;
        m_pVersionInfo = NULL;
    }
}

std::wstring CPeReader::GetFileName()
{
	return m_sFileName;
}

DWORD CPeReader::GetMachineType()
{
    return m_dwMachineType;
}

time_t CPeReader::GetDateTimeStamp()
{
    return m_DateTimeStamp;
}

DWORD CPeReader::GetCheckSum()
{
    return m_dwCheckSum;
}

BOOL CPeReader::IsDebugInfoStripped()
{
    return m_bDebugInfoStripped;
}

BOOL CPeReader::IsDLL()
{
    return m_bIsDLL;
}

BOOL CPeReader::IsPE32Plus()
{
    return m_bIsPE32Plus;
}

CFileMemoryMapping* CPeReader::GetFileMapping()
{
    return &m_fm;
}

int CPeReader::GetSectionHeaderCount()
{
    return (int)m_aSectionHeaders.size();
}

IMAGE_SECTION_HEADER* CPeReader::GetSectionHeader(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_aSectionHeaders.size())
        return NULL;

    return &m_aSectionHeaders[nIndex];
}

BOOL CPeReader::ReadImageMemory(DWORD dwOffs, LPVOID lpBuffer, int nSize, DWORD* lpNumberOfBytesRead)
{
    *lpNumberOfBytesRead = 0;
    LPBYTE pData = m_fm.CreateView(dwOffs, nSize);
    if(pData && nSize>0)
    {
        memcpy(lpBuffer, pData, nSize);
        *lpNumberOfBytesRead = nSize;
    }
    else
        return FALSE;

    return TRUE;
}

BOOL CPeReader::BrowseResourceDirForVersionInfo(
    IMAGE_SECTION_HEADER* pSection,
    int nLevel,
    DWORD uResourceDataSectionOffs,
    DWORD uResourceDirTableOffs,
    DWORD uResourceDirRva,
    ULONG /*uResourceDirSize*/,
    VS_FIXEDFILEINFO** ppFixedFileInfo)
{
    *ppFixedFileInfo = NULL;
    IMAGE_RESOURCE_DIRECTORY ResourceDir;
    DWORD uBytesRead = 0;

    // Read resource dir
    if(!ReadImageMemory(uResourceDirRva,
        &ResourceDir, sizeof(IMAGE_RESOURCE_DIRECTORY), &uBytesRead) ||
        uBytesRead!=sizeof(IMAGE_RESOURCE_DIRECTORY))
        return FALSE; // Couldn't read resource dir

    ULONG uIdEntryCount = ResourceDir.NumberOfIdEntries;
    UINT i;
    for(i=0; i<uIdEntryCount; i++)
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY DirEntry;

        // Read resource entry
        if(!ReadImageMemory((uResourceDirRva+sizeof(IMAGE_RESOURCE_DIRECTORY)+i*sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY)),
            &DirEntry, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY), &uBytesRead) ||
            uBytesRead!=sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))
            return FALSE; // Couldn't read resource entry

        if(nLevel<2)
        {
            if(nLevel==1 || MAKEINTRESOURCE(DirEntry.Id)==RT_VERSION && DirEntry.DataIsDirectory==1)
            {
                BrowseResourceDirForVersionInfo(pSection, nLevel+1, uResourceDataSectionOffs, uResourceDirTableOffs,
                    uResourceDirTableOffs+DirEntry.OffsetToDirectory, sizeof(IMAGE_RESOURCE_DIRECTORY), ppFixedFileInfo);
            }
        }
        else
        {
            IMAGE_RESOURCE_DATA_ENTRY DataEntry;
            DWORD uResourceDataEntryRva = uResourceDirTableOffs+DirEntry.OffsetToData;

            // Read resource data entry
            if(!ReadImageMemory(uResourceDataEntryRva,
                &DataEntry, sizeof(IMAGE_RESOURCE_DATA_ENTRY), &uBytesRead) ||
                uBytesRead!=sizeof(IMAGE_RESOURCE_DATA_ENTRY))
                return FALSE;

            // Read resource data
            DWORD uResourceDataRva = uResourceDataSectionOffs+DataEntry.OffsetToData-pSection->VirtualAddress+pSection->PointerToRawData;
            LPBYTE pBuffer = new BYTE[DataEntry.Size];
            if(!ReadImageMemory((uResourceDataRva),
                pBuffer, DataEntry.Size, &uBytesRead) ||
                uBytesRead!=DataEntry.Size)
                return FALSE;

            VS_VERSIONINFO* pVersionInfo = (VS_VERSIONINFO*)pBuffer;

            *ppFixedFileInfo = new VS_FIXEDFILEINFO;
            memcpy(*ppFixedFileInfo, &pVersionInfo->Value, sizeof(VS_FIXEDFILEINFO));

            delete [] pBuffer;

            return TRUE;
        }
    }

    return TRUE;
}

LPBYTE CPeReader::GetCvRecord()
{
    return m_pCvRecord;
}

LPBYTE CPeReader::GetMiscRecord()
{
    return m_pMiscRecord;
}

BOOL CPeReader::GetCvInfo(std::wstring& sPdbName, std::wstring& sPdbGUID, int& nAge)
{
    if(m_pCvRecord==NULL)
        return FALSE;

    sPdbName = m_sCvPdbName;
    sPdbGUID = m_sCvPdbGUID;
    nAge = m_nCvAge;

    return TRUE;
}

int CPeReader::GetSectionIndexByVA(DWORD dwVirtualAddr)
{
    size_t i;
    for(i=0; i<m_aSectionHeaders.size(); i++)
    {
        IMAGE_SECTION_HEADER& Section = m_aSectionHeaders[i];
        if(dwVirtualAddr>=Section.VirtualAddress && dwVirtualAddr<Section.VirtualAddress+Section.SizeOfRawData)
        {
            return (int)i;
        }
    }

    return -1;
}

BOOL CPeReader::GetRvaByVA(DWORD dwVirtualAddr, DWORD& dwRVA)
{
    dwRVA = 0;

    int nSection = GetSectionIndexByVA(dwVirtualAddr);
    if(nSection<0)
        return FALSE;

    IMAGE_SECTION_HEADER* pSection = GetSectionHeader(nSection);
    dwRVA = dwVirtualAddr-pSection->VirtualAddress+pSection->PointerToRawData;

    return TRUE;
}

int CPeReader::GetSectionIndexByName(LPCSTR szSectionName)
{
    if(szSectionName == NULL || strlen(szSectionName)>IMAGE_SIZEOF_SHORT_NAME)
        return -1; // Invalid arg

    int i;
    for(i=0; i<GetSectionHeaderCount(); i++)
    {
        IMAGE_SECTION_HEADER* pSection = GetSectionHeader(i);
        if(memcmp(pSection->Name, szSectionName, min(IMAGE_SIZEOF_SHORT_NAME, strlen(szSectionName)))==0)
            return i;
    }

    return -1;
}


