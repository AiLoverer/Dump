#pragma once
#include "stdafx.h"

namespace Utils
{

#ifdef _WIN32
    //! Returns path to a module (EXE or DLL)
    std::wstring GetModulePath(HMODULE hModule);
#endif

    //! Return path to data folder
    std::wstring GetDataFolder();

    //! Returns path to test_data folder
    std::wstring GetTestDataFolder();

	//! Returns path to current directory
    std::wstring GetCurrentDir();

	//! Compares content of two files and returns true on exact match.
	bool CompareFiles(std::string sFile1, std::string sFile2);

};
