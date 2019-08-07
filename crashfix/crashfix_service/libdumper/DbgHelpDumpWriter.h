//! \file DbgHelpDumpWriter.h
//! \brief Minidump writing using dbghelp API.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once

#ifdef _WIN32

#include "stdafx.h"
#include "dbghelp.h"

//! \class CDbgHelpDumpWriter
//! \brief Writes the minidump using dbghelp API.
class CDbgHelpDumpWriter
{
public:

    // Construction/destruction
    CDbgHelpDumpWriter();
    ~CDbgHelpDumpWriter();

    //! Sets path to dbghelp.dll
    BOOL SetDbgHelpPath(LPCWSTR szDbgHelpPath);

    //! Writes a minidump file.
    BOOL Write(ULONG uProcessID, ULONG uThreadID, PEXCEPTION_POINTERS pExcPtrs, LPCTSTR szFileName);

    std::wstring GetErrorMsg();

protected:

    static BOOL CALLBACK MiniDumpCallback(PVOID CallbackParam, PMINIDUMP_CALLBACK_INPUT CallbackInput,
        PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);

    BOOL OnMinidumpProgress(const PMINIDUMP_CALLBACK_INPUT CallbackInput,
        PMINIDUMP_CALLBACK_OUTPUT CallbackOutput);

    std::wstring m_sDbgHelpPath;   //!< Path to dbghelp.dll
    std::wstring m_sErrorMsg;      //!< Last error message
};

#endif
