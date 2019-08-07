// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

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
#else // Linux
#include "TypeDefs.h"
#include <stdarg.h>
#include <dirent.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <map>
#include <time.h>
#include <math.h>
#include <wchar.h>
#include <string>
#include <assert.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
using namespace std;
#include <queue>
#include <set>

#ifndef _WIN32
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>     /* in_addr structure */
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <syslog.h>
#include <stdarg.h>
#else
#include "winsock2.h"
#include <process.h>
#endif //!_WIN32



#include <vector>
#include <string>
#include <sstream>
