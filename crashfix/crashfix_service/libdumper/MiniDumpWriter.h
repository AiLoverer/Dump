#pragma once
#include "stdafx.h"

class IMiniDumpWriter
{
public:

    virtual BOOL Write(ULONG uProcessID, ULONG uThreadID, void* pExcPtrs, LPCWSTR szFileName) = 0;
    virtual std::wstring GetErrorMsg() = 0;
};

class CMiniDumpWriterFactory
{
public:

    static BOOL CreateInstance(IMiniDumpWriter** ppWriter);
};
