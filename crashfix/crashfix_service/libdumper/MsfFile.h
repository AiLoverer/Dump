//! \file MsfFile.h
//! \brief Multi-stream (MSF) file reading functionality.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "PdbStruct.h"
#include "FileMapping.h"
#include "MsfStream.h"

//! \class CPdbReader
//! \brief Manages a PDB file streams.
class CMsfFile
{
    friend class CMsfStream;
public:

    // Construction/destruction
    CMsfFile();
    virtual ~CMsfFile();

    //! Initializes the reader.
    //! @param[in] szFileName File name.
    bool Init(std::wstring sFileName);  

    //! Returns true if initialized.
    bool IsInitialized();

    //! Closes the file.
    void Destroy();

    //! Returns file name, if initialized; otherwise empty string.
    std::wstring GetFileName();

    //! Returns root stream.
    CMsfStream* GetRootStream();

    //! Gets index dir stream.
    CMsfStream* GetIndexDirStream();

    //! Gets stream count in the PDB file.
    int GetStreamCount();

    //! Returns stream by index, or NULL if there is no stream with such index.
    //! @param[in] nIndex Zero-based stream index.
    CMsfStream* GetStream(int nIndex);

private:

    //! Reads PDB file header.
    bool ReadHeader();

    bool m_bInitialized;                  //!< TRUE if initialized
    CFileMemoryMapping m_fm;              //!< File mapping
    std::wstring m_sFileName;             //!< PDB file name  
    DWORD m_dwPdbVersion;                 //!< PDB file version
    DWORD m_dwPageCount;                  //!< Page count
    DWORD m_dwPageSize;                   //!< Page size in bytes
    CMsfStream* m_pRootStream;            //!< Root stream
    CMsfStream* m_pIndexDirStream;        //!< Index directory stream
    std::vector<CMsfStream*> m_apStreams; //!< The list of PDB streams
};

