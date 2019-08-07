//! \file FileMapping.cpp
//! \brief File memory mapping.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "FileMapping.h"
#include "strconv.h"

CFileMemoryMapping::CFileMemoryMapping()
{
#ifdef _WIN32
    m_hFile = INVALID_HANDLE_VALUE;
    m_hFileMapping = NULL;

    SYSTEM_INFO si;
    GetSystemInfo(&si);
    m_dwAllocGranularity = si.dwAllocationGranularity;
#else
    m_fd = -1;
    m_dwAllocGranularity = sysconf(_SC_PAGESIZE);
#endif

    m_uFileLength = 0;


}

CFileMemoryMapping::~CFileMemoryMapping()
{
    Destroy();
}


BOOL CFileMemoryMapping::Init(std::wstring sFileName)
{
#ifdef _WIN32
    if(m_hFile!=INVALID_HANDLE_VALUE)
    {
        Destroy();
    }

    m_hFile = CreateFileW(sFileName.c_str(), FILE_READ_DATA, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
    if(m_hFile == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}

    m_hFileMapping = CreateFileMappingW(m_hFile, 0, PAGE_READONLY, 0, 0, 0);
    LARGE_INTEGER size;
    GetFileSizeEx(m_hFile, &size);
    m_uFileLength = size.QuadPart;
#else
    if(m_fd!=-1)
    {
        Destroy();
    }
    // Linux understands UTF-8 file names
    std::string sUtf8FileName = strconv::w2a(sFileName);
    m_fd = open(sUtf8FileName.c_str(), O_RDONLY);
    if(m_fd==-1)
        return FALSE;

    struct stat st;
    stat(sUtf8FileName.c_str(), &st);

    m_uFileLength = st.st_size;
#endif

    return TRUE;
}

void CFileMemoryMapping::Destroy()
{
    std::map<DWORD, ViewInfo>::iterator it;
    for(it=m_aViewStartPtrs.begin(); it!=m_aViewStartPtrs.end(); it++)
    {
        ViewInfo& vi = it->second;
#ifdef _WIN32
        UnmapViewOfFile(vi.m_pStartPtr);
#else
        munmap(vi.m_pStartPtr, vi.m_Size);
#endif
    }
    m_aViewStartPtrs.clear();

#ifdef _WIN32
    if(m_hFileMapping!=NULL)
    {
        CloseHandle(m_hFileMapping);
    }

    if(m_hFile!=INVALID_HANDLE_VALUE)
    {
        CloseHandle(m_hFile);
    }

    m_hFileMapping = NULL;
    m_hFile = INVALID_HANDLE_VALUE;
#else

    if(m_fd!=-1)
        close(m_fd);
    m_fd = -1;
#endif

    m_uFileLength = 0;

}

ULONG64 CFileMemoryMapping::GetSize()
{
    return m_uFileLength;
}

LPBYTE CFileMemoryMapping::CreateView(DWORD dwOffset, DWORD dwLength)
{
    if(dwOffset+dwLength>m_uFileLength)
        return NULL;

#ifdef _WIN32
    DWORD dwThreadId = GetCurrentThreadId();
#else
    DWORD dwThreadId = getpid();
#endif
    DWORD dwBaseOffs = dwOffset-dwOffset%m_dwAllocGranularity;
    DWORD dwDiff = dwOffset-dwBaseOffs;
    LPBYTE pPtr = NULL;

    CAutoLock lock(&m_csLock);

    std::map<DWORD, ViewInfo>::iterator it = m_aViewStartPtrs.find(dwThreadId);
    if(it!=m_aViewStartPtrs.end())
    {
        ViewInfo& vi = it->second;
#ifdef _WIN32
        UnmapViewOfFile(vi.m_pStartPtr);
#else
        munmap(vi.m_pStartPtr, vi.m_Size);
#endif
    }

#ifdef _WIN32
    pPtr = (LPBYTE)MapViewOfFile(m_hFileMapping, FILE_MAP_READ, 0, dwBaseOffs, dwLength+dwDiff);
#else
    pPtr = (LPBYTE)mmap(NULL, dwLength + dwDiff, PROT_READ, MAP_PRIVATE, m_fd, dwBaseOffs);
    if(pPtr==MAP_FAILED)
    {
        perror(NULL);
        return NULL;
    }
#endif

    if(pPtr==NULL)
        return NULL;

    ViewInfo vi;
    vi.m_pStartPtr = pPtr;
    vi.m_Size = dwLength+dwDiff;

    if(it!=m_aViewStartPtrs.end())
    {
        it->second = vi;
    }
    else
    {
        m_aViewStartPtrs[dwThreadId] = vi;
    }

    return (pPtr+dwDiff);
}
