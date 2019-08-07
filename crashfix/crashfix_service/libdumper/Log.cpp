//! \file Log.cpp
//! \brief Logging.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "Log.h"
#include "strconv.h"

// global log object
CLog g_log;

//-------------------------------------------------------------------------
// constructor
//-------------------------------------------------------------------------

CLog::CLog()
{
	m_LogFile = NULL;

	// Logging level 0 by default
	set_level(0);

	// No max size by default
	m_max_size = 0;

	m_nMaxLogFileCount = 10;

	m_bRedirectStdStreams = false;
}

//-------------------------------------------------------------------------
// opens log file
//-------------------------------------------------------------------------

BOOL CLog::init(std::wstring sPath, bool bRedirectStdStreams)
{
	m_path = sPath;

#ifdef _WIN32
    bRedirectStdStreams=false; // Disable this in Windows
#endif

    m_bRedirectStdStreams = bRedirectStdStreams;

    if(!bRedirectStdStreams)
    {

    	if(sPath.compare(L"stdout")==0)
        {
            m_LogFile = stdout;
        }
        else
        {
#ifdef _WIN32
            m_LogFile = _wfsopen(sPath.c_str(), L"at", _SH_DENYWR);
#else
            m_LogFile = fopen(strconv::w2utf8(m_path).c_str(), "ab");
#endif
        }

        if(m_LogFile==NULL)
            return FALSE;

        // Log file opened successfully
        return TRUE;
    }
    else
    {
#ifndef _WIN32
        // Linux fopen understands UTF-8 encoded names
        std::string sUtf8Name = strconv::w2a(sPath);

        //m_LogFile = fopen(sUtf8Name.c_str(), "wb");

        fflush(stderr);
        fflush(stdout);

        if(freopen(sUtf8Name.c_str(), "ab", stderr)==NULL)
        {
            printf("Can't open log file for appending: %s.\n", sUtf8Name.c_str());
        }

        if(freopen(sUtf8Name.c_str(), "ab", stdout)==NULL)
        {
            printf("Can't redirect stdout to file %s.\n", sUtf8Name.c_str());
        }

        m_LogFile = stderr;
        return TRUE;
#else
        return FALSE;
#endif
    }
}

//-------------------------------------------------------------------------
// Closes the log file
//-------------------------------------------------------------------------

void CLog::term()
{
#ifdef _WIN32
    if(m_LogFile)
        fclose(m_LogFile);

    m_LogFile = NULL;
#else

    // Linux

#endif
}

std::wstring CLog::get_path()
{
	return m_path;
}

//-------------------------------------------------------------------------
// set logging level
// @level	(IN) logging level
//					-1  == logging off
//					 N  == write to log all strings with level less or equal to N
//-------------------------------------------------------------------------

void CLog::set_level(int level)
{
	m_level=level;
}

//-------------------------------------------------------------------------
// sets the maximum log file size (bytes)
// @max_size	(IN) maximum file size
//-------------------------------------------------------------------------

void CLog::set_max_size(ULONG64 max_size)
{
	m_max_size = max_size;
}

void CLog::print_timestamp()
{
	// get current time
	time_t cur_time = 0;
	time(&cur_time);
	char szDateTime[64] = "";

	struct tm* ltimeinfo = localtime(&cur_time );
#ifndef _WIN32
	strftime(szDateTime, 63,  "%a, %d %b %Y %H:%M:%S %Z", ltimeinfo);
	fprintf(m_LogFile, "[%s] ", szDateTime);
#else
	TIME_ZONE_INFORMATION tzi;
	GetTimeZoneInformation(&tzi);

	int diff_hours = -tzi.Bias/60;
	int diff_mins = abs(tzi.Bias%60);

	strftime(szDateTime, 63,  "%a, %d %b %Y %H:%M:%S", ltimeinfo);

	// write timestamp
	fprintf(m_LogFile, "[%s %c%02d%02d] ", szDateTime, diff_hours>=0?'+':'-', diff_hours, diff_mins);
#endif
}


//-------------------------------------------------------------------------
// writes a string to log
// @level	(IN) logging level (greater or equal to zero)
// @fmt		(IN) format string
// @...		(IN) argument list
//-------------------------------------------------------------------------

void CLog::writeW(int level, const wchar_t* fmt, ...)
{
	if(m_path.empty())
		return;

	if(level<=m_level && m_LogFile!=NULL)
	{
	    print_timestamp();

		va_list args;
		va_start(args,fmt);
		vfwprintf(m_LogFile, fmt, args);

		fflush(m_LogFile);
	}
}

void CLog::write(int level, const char* fmt, ...)
{
	if(m_path.empty()) return;
	if(level<=m_level && m_LogFile!=NULL)
	{
	    print_timestamp();

		va_list args;
		va_start(args,fmt);
		vfprintf(m_LogFile, fmt, args);
		fflush(m_LogFile);
	}
}


void CLog::write_raw(int level, LPBYTE buf, int buf_size)
{
	if(level<=m_level && m_LogFile!=NULL)
	{
		fwrite((void*)buf, buf_size,1, m_LogFile);
	}
	fflush(m_LogFile);
}

//-------------------------------------------------------------------------
// writes last Windows error description to log file
//-------------------------------------------------------------------------

void CLog::log_last_error(int level, const char* szErrorMsg)
{
#ifdef _WIN32
	DWORD le = GetLastError();
	LPTSTR msg = 0;
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, le, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&msg, 0, NULL);
	writeW(level, L"%s. Error %lu(%XH): %s", szErrorMsg, le,le,msg);
	GlobalFree(msg);
#else
    const char* szError = strerror(errno);
    write(level, "%s: %s\n", szErrorMsg, szError);
#endif
}

bool CLog::recursively_rename_file(const char* szFileName, int number)
{
    // This method is used for log rotation. It renames the given file
    // into filename.1, and its successors are renamed to filename.2,
    // filename.3 and so on.

    // format file name
    char szNumber[32]="";
    sprintf(szNumber, "%d", number);
    std::string sFileName = szFileName;
    if(number>0)
    {
        sFileName += ".";
        sFileName += szNumber;
    }

    // check if such file exists
    FILE* f = fopen(sFileName.c_str(), "rt");
    if(f==NULL)
    {
        return true; // file doesn't exist, nothing to rename
    }

    fclose(f);

	// Check if maximum log file count exceeded
	if(number>=m_nMaxLogFileCount)
	{
		// Remove this file
		remove(sFileName.c_str());
		return true;
	}

    // recursively rename predcessors of this file
    if(recursively_rename_file(szFileName, number+1)!=true)
    {
        return false;
    }

    // format new file name
    sprintf(szNumber, "%d", number+1);
    std::string sNewFileName = szFileName;
    sNewFileName += ".";
    sNewFileName += szNumber;

    const char* szOldName = sFileName.c_str();
    const char* szNewName = sNewFileName.c_str();

    // rename the file
    if( rename(szOldName, szNewName)!=0 )
    {
        write(0, "Error renaming log file: %s\n", sFileName.c_str());
    }

    return true;
}

bool CLog::check_error_log_size()
{
    if(m_path.empty() || m_max_size==0)
    {
        write(0, "check_error_log_size: invalid param.\n");
        // no error log filename specified?
        return false;
    }

    struct stat st;
    const char* szFileName = strconv::w2utf8(m_path).c_str();

    if(stat(szFileName, &st)!=0)
    {
        write(1, "check_error_log_size: couldn't stat log file.\n");
        return false;
    }

    if(st.st_size/1024 < m_max_size)
    {
        // log file is small yet
        write(1, "check_error_log_size: log file is small yet.\n");
        return true;
    }

    write(0, "check_error_log_size: log file has exceeded its maximum size; performing log rotation.\n");

    // Now close log (to be able to rename the log file).
    if(m_bRedirectStdStreams)
    {
        // Close standard file descriptors
        fclose(stderr);
        fclose(stdout);
    }
    else
    {
        fclose(m_LogFile);
    }
    m_LogFile = NULL;

    // Now rename file

    bool bRename = recursively_rename_file(strconv::w2a(m_path).c_str(), 0);

    // Now reopen the file again.
    BOOL bInit = init(m_path, m_bRedirectStdStreams);
	if(!bInit)
	{
	}

    if(!bRename)
        write(0, "Error renaming log file: %s\n", m_path.c_str());

    return true;
}

#ifdef _WIN32

std::wstring GetErrorMsg(DWORD errcode)
{
	LPWSTR msg = 0;
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_ALLOCATE_BUFFER,
		NULL, errcode, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),
		(LPWSTR)&msg, 0, NULL);
	msg[wcslen(msg)-2] = L'\0'; // Remove '\n'
	std::wostringstream s;
	s << msg << L" (Error code " << errcode << L")"; 
	GlobalFree(msg);
	return s.str();
}

#endif
