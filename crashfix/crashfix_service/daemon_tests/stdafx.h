// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif

#include <stdio.h>
#include <vector>
#include <set>
#include <sstream>

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
#include <netdb.h>
#else
#include <tchar.h>
#include "winsock2.h"
#endif //!_WIN32

// TODO: reference additional headers your program requires here
