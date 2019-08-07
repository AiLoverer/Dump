// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <tchar.h>
#include <conio.h>

#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#define _WTL_NO_CSTRING
#include <atlstr.h>

#else
#include "TypeDefs.h"
#endif

#include <stdio.h>
#include <vector>
#include <set>
#include <string>
#include <algorithm>
#include <memory.h>

// Unicode string type
typedef std::basic_string<wchar_t> wstring;
