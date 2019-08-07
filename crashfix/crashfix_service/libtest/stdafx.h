#pragma once

#include <stdio.h>

#ifdef _WIN32 // Windows includes
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif
#include <tchar.h>
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <atlbase.h>
#include <atlapp.h>
extern CAppModule _Module;
#define _WTL_NO_CSTRING
#include <atlstr.h>
#include <objbase.h>
#include <comdef.h>
#include <Tlhelp32.h>
#include <direct.h>
#include <share.h>
#include <wincrypt.h>
#include <Wbemidl.h>
#include <intrin.h>
#include <conio.h>
#else // Linux
#include "../libdumper/TypeDefs.h"
#include <stdarg.h>
#include <memory.h>
#include <termios.h>
#include <errno.h>
#include <sys/time.h>
#endif

#include <stdio.h>
#include <vector>
#include <map>
#include <set>
#include <time.h>
#include <math.h>
#include <wchar.h>
#include <string>
#include <assert.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#pragma warning(disable:4201)

//#define min(a,b)            (((a) < (b)) ? (a) : (b))
