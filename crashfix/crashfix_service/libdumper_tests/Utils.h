#pragma once
#include "stdafx.h"

namespace Utils
{

#ifdef _WIN32
    //! Returns path to a module (EXE or DLL)
    std::wstring GetModulePath(HMODULE hModule);
#else
    //! Returns path to current executable module
    std::wstring GetExePath();
#endif

    //! Return path to data folder
    std::wstring GetDataFolder();

    //! Returns path to test_data folder
    std::wstring GetTestDataFolder();

	//! Compares content of two files and returns true on exact match.
	bool CompareFiles(std::string sFile1, std::string sFile2);
};
