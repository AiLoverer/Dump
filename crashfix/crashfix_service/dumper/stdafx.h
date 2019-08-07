// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include <stdio.h>

#ifdef _WIN32
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif
#include <errno.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <shellapi.h>
#include <Windns.h>
#include <Wspiapi.h>
#include <Psapi.h>

#include <tchar.h>
#include <windows.h>
#include <Tlhelp32.h>
#include <direct.h>
#else
#include "TypeDefs.h"
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#endif

#include <vector>
#include <map>
#include <time.h>
#include <math.h>


// TODO: reference additional headers your program requires here
