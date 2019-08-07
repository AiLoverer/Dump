//! \file DbgHelpDumpWriter.cpp
//! \brief Minidump writing using dbghelp API.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "DbgHelpDumpWriter.h"
#include "Log.h"

CDbgHelpDumpWriter::CDbgHelpDumpWriter()
{
    m_sDbgHelpPath = L"dbghelp.dll";
}

CDbgHelpDumpWriter::~CDbgHelpDumpWriter()
{
}

BOOL CDbgHelpDumpWriter::SetDbgHelpPath(LPCWSTR szDbgHelpPath)
{
    m_sDbgHelpPath = szDbgHelpPath;
    return TRUE;
}

// This method creates minidump of the process
BOOL CDbgHelpDumpWriter::Write(ULONG uProcessID, ULONG uThreadID, PEXCEPTION_POINTERS pExcPtrs, LPCWSTR szFileName)
{
    BOOL bStatus = FALSE;
    HMODULE hDbgHelp = NULL;
    HANDLE hFile = NULL;
    MINIDUMP_EXCEPTION_INFORMATION mei;
    MINIDUMP_CALLBACK_INFORMATION mci;

    // Load dbghelp.dll
    hDbgHelp = LoadLibraryW(m_sDbgHelpPath.c_str());
    if(hDbgHelp==NULL)
    {
        //try again ... fallback to dbghelp.dll in path
        const std::wstring sDebugHelpDLL_name = L"dbghelp.dll";
        hDbgHelp = LoadLibraryW(sDebugHelpDLL_name.c_str());
    }

    if(hDbgHelp==NULL)
    {
        m_sErrorMsg = L"dbghelp.dll couldn't be loaded";
        goto cleanup;
    }

    // Create the minidump file
    hFile = CreateFile(
        szFileName,
        GENERIC_WRITE,
        0,
        NULL,
        CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL,
        NULL);

    if(hFile==INVALID_HANDLE_VALUE)
    {
        DWORD dwError = GetLastError();
        wchar_t szBuf[256]=L"";
        wsprintfW(szBuf, L"Couldn't create minidump file (error code=%ul)", dwError);
        m_sErrorMsg = szBuf;
        return FALSE;
    }

    // Set valid dbghelp API version
    typedef LPAPI_VERSION (WINAPI* LPIMAGEHLPAPIVERSIONEX)(LPAPI_VERSION AppVersion);
    LPIMAGEHLPAPIVERSIONEX lpImagehlpApiVersionEx =
        (LPIMAGEHLPAPIVERSIONEX)GetProcAddress(hDbgHelp, "ImagehlpApiVersionEx");
    assert(lpImagehlpApiVersionEx!=NULL);
    if(lpImagehlpApiVersionEx!=NULL)
    {
        API_VERSION CompiledApiVer;
        CompiledApiVer.MajorVersion = 6;
        CompiledApiVer.MinorVersion = 1;
        CompiledApiVer.Revision = 11;
        CompiledApiVer.Reserved = 0;
        LPAPI_VERSION pActualApiVer = lpImagehlpApiVersionEx(&CompiledApiVer);
        pActualApiVer;
        //ATLASSERT(CompiledApiVer.MajorVersion==pActualApiVer->MajorVersion);
        //ATLASSERT(CompiledApiVer.MinorVersion==pActualApiVer->MinorVersion);
        //ATLASSERT(CompiledApiVer.Revision==pActualApiVer->Revision);
    }

    // Write minidump to the file
    mei.ThreadId = uThreadID;
    mei.ExceptionPointers = pExcPtrs;
    mei.ClientPointers = TRUE;
    mci.CallbackRoutine = MiniDumpCallback;
    mci.CallbackParam = this;

    typedef BOOL (WINAPI *LPMINIDUMPWRITEDUMP)(
        HANDLE hProcess,
        DWORD ProcessId,
        HANDLE hFile,
        MINIDUMP_TYPE DumpType,
        CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam,
        CONST PMINIDUMP_USER_STREAM_INFORMATION UserEncoderParam,
        CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

    LPMINIDUMPWRITEDUMP pfnMiniDumpWriteDump =
        (LPMINIDUMPWRITEDUMP)GetProcAddress(hDbgHelp, "MiniDumpWriteDump");
    if(!pfnMiniDumpWriteDump)
    {
        m_sErrorMsg = L"Bad MiniDumpWriteDump function";
        goto cleanup;
    }

    HANDLE hProcess = OpenProcess(
        PROCESS_QUERY_INFORMATION|PROCESS_VM_READ,
        FALSE,
        uProcessID);
    if(hProcess==NULL)
    {
        m_sErrorMsg = L"Error opening process handle";
        goto cleanup;
    }

    BOOL bWriteDump = pfnMiniDumpWriteDump(
        hProcess,
        uProcessID,
        hFile,
        MiniDumpNormal,
        &mei,
        NULL,
        &mci);

    if(!bWriteDump)
    {
        DWORD dwLastError = GetLastError();
        std::wstring sMsg = ::GetErrorMsg(dwLastError);
        wchar_t szBuf[256]=L"";
        wsprintfW(szBuf, L"MiniDumpWriteDump function has failed, error code=0x%X, desc=%s", dwLastError, sMsg.c_str());
        m_sErrorMsg = szBuf;
        goto cleanup;
    }

    bStatus = TRUE;
    m_sErrorMsg = L"";

cleanup:

    // Close file
    if(hFile)
        CloseHandle(hFile);

    // Unload dbghelp.dll
    if(hDbgHelp)
        FreeLibrary(hDbgHelp);

    return bStatus;
}

// This callbask function is called by MinidumpWriteDump
BOOL CALLBACK CDbgHelpDumpWriter::MiniDumpCallback(
    PVOID CallbackParam,
    PMINIDUMP_CALLBACK_INPUT CallbackInput,
    PMINIDUMP_CALLBACK_OUTPUT CallbackOutput )
{
    // Delegate back
    CDbgHelpDumpWriter* pWriter = (CDbgHelpDumpWriter*)CallbackParam;
    return pWriter->OnMinidumpProgress(CallbackInput, CallbackOutput);
}

// This method is called when MinidumpWriteDump notifies us about
// currently performed action
BOOL CDbgHelpDumpWriter::OnMinidumpProgress(
    const PMINIDUMP_CALLBACK_INPUT CallbackInput,
    PMINIDUMP_CALLBACK_OUTPUT /*CallbackOutput*/)
{
    switch(CallbackInput->CallbackType)
    {
        //case CancelCallback:
        //  {
        //    // This callback allows to cancel minidump generation
        //  }
        //  break;

    case ModuleCallback:
        {
            // We are currently dumping some module

        }
        break;
    case ThreadCallback:
        {
            // We are currently dumping some thread
        }
        break;

    }

    return TRUE;
}

std::wstring CDbgHelpDumpWriter::GetErrorMsg()
{
    return m_sErrorMsg;
}

