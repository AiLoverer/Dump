//! \file PdbDebugInfoStream.h
//! \brief PDB debug info (DBI) stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStreamStruct.h"
#include "BasePdbStream.h"

//! DBI module info
struct DBI_ModuleInfo
{
    int m_nModuleIndex;              //!< Module index
    DBI_MODULE_INFO m_Info;          //!< Module info
    std::string m_sSrcModuleName;    //!< Source name 
    std::string m_sTargetModuleName; //!< Target name
};

class CPdbReader;

//! \class CPdbDebugInfoStream
//! \brief Implements the debug info (DBI) stream of a PDB file.
class CPdbDebugInfoStream : public CBasePdbStream
{
public:

    // Construction/destruction
    CPdbDebugInfoStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult);
    virtual ~CPdbDebugInfoStream();

    //! Initializes the debug info stream.
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream);

    //! Returns processor architecture (x86 or x64)
    DWORD GetMachineType();

    //! Returns index of GSI stream
    int GetGSIStreamIndex();

    //! Returns index of PSI stream
    int GetPSIStreamIndex();

    //! Returns index of symbol stream
    int GetSymbolStreamIndex();

    //! Returns module count.
    int GetModuleCount();

    //! Returns module info by its zero-based index
    DBI_ModuleInfo* GetModuleInfo(int nIndex);

    //! Returns count of section contribution records.
    int GetSectionContribCount();

    //! Returns section contrib record index.
    DBI_SECTION_CONTRIB* GetSectionContrib(int nIndex);

    //! Returns index o a section contrib record that owns the specified address.
    int FindSectionContribByAddr(DWORD64 dwAddr);

    //! Returns count of source file names.
    int GetSrcFileNameCount();

    //! Returns source file name by index
    const wchar_t* GetSrcFileName(int nIndex);

    //! Searches for a source file name by its offset
    int FindSrcFileNameByOffs(DWORD dwNameOffs);

    int FindSrcFileNameByModule(int nModuleIndex, int nCheckSumIndex);

private:

    struct _SrcFileInfo
    {
        std::wstring m_sFileName;   // File name string.
        DWORD m_dwFileNameOffset;  // Offset of the file name string from the beginning of names block.
    };

    struct _ECPair
    {
        DWORD m_dwFirst;
        DWORD m_dwSecond;
    };

    DWORD m_dwMachineType;    //!< Processor architecture
    int m_nGlobalSymStream;   //!< Number of a stream containing global symbols
    int m_nPublicSymStream;   //!< Number of a stream containing public symbols
    int m_nSymStream;         //!< Number of a stream containing symbol records
    std::vector<DBI_ModuleInfo> m_aModules; //!< List of module info
    std::vector<DBI_SECTION_CONTRIB> m_aSectionContrib; //!< Section contrib
    std::map<DWORD, int> m_aSectionContribRvas;
    std::vector<WORD> m_aModStart;          //!< Offsets to module start 
    std::vector<WORD> m_aRefCount;          //!< Module reference count
    std::vector<DWORD> m_aNameRefs;         //!< Offsets to module names  
    std::vector<_SrcFileInfo> m_asFileNames; //!< Src file names  
    std::map<DWORD, int> m_aFileNameOffs;
    std::map<std::wstring, int> m_aFileNameIndex;
    std::vector<_SrcFileInfo> m_asECFileNames; //!< 
    std::map<DWORD, int> m_aECFileNameOffs;
    std::vector<_ECPair> m_aECMappings;
};

