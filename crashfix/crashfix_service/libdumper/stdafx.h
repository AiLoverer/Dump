// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifdef _WIN32
#define PRIu64 "llu"
#define PRIx64 "llx"
#define PRIX64 "llx"
#define PRIX64W L"llx"
#else
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif

#include <stdio.h>

#ifdef _WIN32 // Windows includes
#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#include <tchar.h>

#include <errno.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <wininet.h>
#include <shellapi.h>
#include <Windns.h>
#include <Wspiapi.h>
#include <Psapi.h>

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
#include <sddl.h>
#include <AccCtrl.h>
#include <Aclapi.h>
#else // Linux
#include "TypeDefs.h"
#include <stdarg.h>
#include <memory.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <fstream>
#include <stddef.h>
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
#include <errno.h>
#include <iostream>
#include <sstream>
#include <cwchar>

#pragma warning(disable:4201)

#define min(a,b)            (((a) < (b)) ? (a) : (b))
