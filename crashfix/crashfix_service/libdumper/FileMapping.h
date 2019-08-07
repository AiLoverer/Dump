//! \file FileMapping.h
//! \brief File memory mapping.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "CritSec.h"

//! \class CFileMemoryMapping
//! \brief Maps a portion of disk file to memory.
class CFileMemoryMapping
{
public:

    // Construction/destruction
    CFileMemoryMapping();
    virtual ~CFileMemoryMapping();

    //! Initializes the file mapping.
    //! @param[in] szFileName File name.
    BOOL Init(std::wstring sFileName);

    //! Destroys the file mapping.
    void Destroy();

    //! Gets size of the file.
    ULONG64 GetSize();

    //! Creates view of a portion of file.
    //! @param[in] dwOffset Offset of the view from the file beginning.
    //! @param[in] dwLength Length in bytes of the view.
    LPBYTE CreateView(DWORD dwOffset, DWORD dwLength);

private:

    struct ViewInfo
    {
        void* m_pStartPtr; //!< View start pointer
        size_t m_Size;     //!< View size
    };

#ifdef _WIN32
    HANDLE m_hFile;		          //!< Handle to current file
    HANDLE m_hFileMapping;		  //!< Memory mapped object
#else
    int m_fd;   //!< File descriptor
#endif
    DWORD m_dwAllocGranularity; //!< System allocation granularity
    ULONG64 m_uFileLength;		  //!< Size of the file.
    CCritSec m_csLock;          //!< Sync object.
    std::map<DWORD, ViewInfo> m_aViewStartPtrs; //!< The list of mapped portions of file.
};
