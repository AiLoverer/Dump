#ifdef _WIN32

#pragma once
#include "stdafx.h"
#include "MiniDumpWriter.h"
#include "DumpStruct.h"

struct MDW_ExceptionInfo
{

};

struct MDW_ThreadInfo
{

};

struct MDW_ModuleInfo
{
    MINIDUMP_MODULE m_Module;
    ULONG32 m_uTimeDateStamp;
    ULONG32 m_uCheckSum;
    std::wstring m_sModuleName;
    std::wstring m_sImagePath;
    VS_FIXEDFILEINFO* m_pVersionInfo;
    LPBYTE m_pCvRecord;
    ULONG m_uCvRecSize;
    LPBYTE m_pMiscRecord;
    ULONG m_uMiscRecSize;
};

struct LocationDesc
{
    LPVOID m_pData;
    DWORD m_cbData;
    ULONG64 m_uLocationRva;
};

class CMiniDumpWriterImpl : public IMiniDumpWriter
{
public:

    CMiniDumpWriterImpl();
    ~CMiniDumpWriterImpl();

    BOOL Write(ULONG uProcessID, ULONG uThreadID, void* pExcPtrs, LPCWSTR szFileName);
    std::wstring GetErrorMsg();

private:

    // Takes a snapshot of process threads and modules
    BOOL GetProcessSnapshot();

    // Suspends/resumes all threads of a process
    BOOL SuspendProcessThreads(BOOL bSuspend);

    BOOL GetExceptionInfo();
    BOOL GetProcessThreadList();
    BOOL GetProcessModuleList();
    BOOL CalcVariableRVAs();

    // Returns various info from a PE file
    BOOL GetImageInfo(ULONG_PTR ModuleBaseAddress, ULONG32* pTimeDateStamp, ULONG32* pCheckSum,
        LPBYTE* ppCvRecord, LPDWORD pdwCvRecSize, LPBYTE* ppMiscRecord, LPDWORD pdwMiscRecSize,
        VS_FIXEDFILEINFO** ppVersionInfo);

    // Browses a resource directory in ".rcsrc" PE file section
    BOOL BrowseResourceDirForVersionInfo(int nLevel, ULONG_PTR uResourceDataSectionOffs, ULONG_PTR uResourceDirTableOffs,
        ULONG_PTR uResourceDirRva, ULONG uResourceDirSize, VS_FIXEDFILEINFO** ppFixedFileInfo);

    BOOL WriteHeader();
    BOOL WriteStreamDirectory(ULONG uStreamType, ULONG32 uDirectoryRva, ULONG uDataSize);
    BOOL WriteSystemInfoStream();
    BOOL WriteMiscInfoStream();
    BOOL WriteExceptionStream();
    BOOL WriteThreadList();
    BOOL WriteModuleList();
    BOOL WriteMemoryList();

    void CleanUp();

    /* Member variables */

    std::wstring m_sErrorMsg;
    ULONG m_uProcessID;
    ULONG m_uThreadID;
    PEXCEPTION_POINTERS m_pExcPtrs;
    std::wstring m_sFileName;
    FILE* m_fp;
    HANDLE m_hProcess;
    HANDLE m_hSnapshot;
    DWORD m_dwProcessPriorityClass;

    MINIDUMP_EXCEPTION_STREAM m_ExcStream;
    std::vector<MINIDUMP_THREAD> m_aThreads;
    std::vector<MDW_ModuleInfo> m_aModules;
    std::vector<MINIDUMP_LOCATION_DESCRIPTOR> m_aLocations;
    std::vector<MINIDUMP_MEMORY_DESCRIPTOR> m_aMemRanges;
};

#endif
