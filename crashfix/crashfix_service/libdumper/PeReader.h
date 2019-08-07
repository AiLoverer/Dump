//! \file PeReader.h
//! \brief Portable executable (PE) format reader.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "FileMapping.h"

//! \class CPeReader
//! \brief Portable executable (PE) file format reader.
class CPeReader
{
public:

    //! Constructor
    CPeReader();

    //! Destructor
    ~CPeReader();

    //! Opens PE file.
    BOOL Init(std::wstring sFileName);

    //! Determines if file is open.
    BOOL IsInitialized();

    //! Frees all used resources.
    void Destroy();

	//! Returns loaded file name.
	std::wstring GetFileName();

    //! Returns pointer to debug data directory.
    PIMAGE_DATA_DIRECTORY GetDebugDataDirectory(ULONG_PTR LoadAddress);

    //! Returns PDB GUID for specified module
    BOOL GetDebugGuid(ULONG_PTR ModuleBaseAddress, GUID *Guid);

    //! Returns machine type
    DWORD GetMachineType();

    //! Returns the date the image was created
    time_t GetDateTimeStamp();

    //! Returns checksum
    DWORD GetCheckSum();

    //! TRUE if the image doesn't contain debug info
    BOOL IsDebugInfoStripped();

    //! TRUE if the image is DLL
    BOOL IsDLL();

    //! TRUE if the image is PE32+ image
    BOOL IsPE32Plus();

    //! Returns count of section headers.
    int GetSectionHeaderCount();

    //! Returns i-th section header.
    IMAGE_SECTION_HEADER* GetSectionHeader(int nIndex);

    //! Returns zero-based section index by section name.
    //! Returns zero-based section index or -1 if there is no such section.
    int GetSectionIndexByName(LPCSTR szSectionName);

    //! Determines what section owns the virtual address.
    //! Returns zero-based section index or -1 if there is no such section.
    int GetSectionIndexByVA(DWORD dwVirtualAddr);

    //! Determines RVA by VA
    BOOL GetRvaByVA(DWORD dwVirtualAddr, DWORD& dwRVA);

    //! Returns pointer to CodeView record.
    LPBYTE GetCvRecord();

    //! Returns CodeViewInfo.
    BOOL GetCvInfo(std::wstring& sPdbName, std::wstring& sPdbGUID, int& nAge);

    //! Returns pointer to misc record.
    LPBYTE GetMiscRecord();

    //! Returns pointer to owned file mapping object.
    CFileMemoryMapping* GetFileMapping();

    //! Reads image memory
    //! @param[in] dwOffs Offset from the beginning of image.
    //! @param[out] lpBuffer Buffer where to write data.
    //! @param[in] nSize Size of buffer.
    //! @param[out] lpNumberOfBytesRead Count of bytes read.
    BOOL ReadImageMemory(DWORD dwOffs, LPVOID lpBuffer, int nSize, DWORD* lpNumberOfBytesRead);

private:

    BOOL BrowseResourceDirForVersionInfo(IMAGE_SECTION_HEADER* pSection, int nLevel, DWORD uResourceDataSectionOffs,
        DWORD uResourceDirTableOffs, DWORD uResourceDirRva, ULONG /*uResourceDirSize*/, 
        VS_FIXEDFILEINFO** ppFixedFileInfo);
    
    BOOL m_bInitialized;       //!< Is this object initialized?
    std::wstring m_sFileName;  //!< Name of the file being opened
    CFileMemoryMapping m_fm;   //!< File mapping object.
    time_t m_DateTimeStamp;    //!< Date created
    DWORD m_dwCheckSum;        //!< Checksum
    DWORD m_dwMachineType;     //!< Machine type.
    VS_FIXEDFILEINFO* m_pVersionInfo; //!< Version info.
    LPBYTE m_pCvRecord;        //!< CodeView record.
    DWORD m_dwCvRecSize;       //!< Size of CodeView record.
    LPBYTE m_pMiscRecord;      //!< Misc record.
    DWORD m_dwMiscRecSize;     //!< Size of misc record.
    BOOL m_bDebugInfoStripped; //!< If debug info stripped from file?
    BOOL m_bIsDLL;             //!< This file represents DLL image.
    BOOL m_bIsPE32Plus;        //!< Is this images uses PE32+ format (x64)
    std::wstring m_sCvPdbName; //!< PDB file name
    std::wstring m_sCvPdbGUID; //!< PDB GUID
    int m_nCvAge;              //!< PDB age
    std::vector<IMAGE_SECTION_HEADER> m_aSectionHeaders; //!< The list of section headers
};

