//! \file Log.h
//! \brief Logging.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"

//! \class CLog
//! \brief Maintains a log file.
class CLog
{
public:

    // Construction/destruction
    CLog();
    virtual ~CLog(){};

    //! Opens log file
    BOOL init(std::wstring sPath, bool bRedirectStdStreams);

    //! Closes log file
    void term();

	//! Returns path to log file.
	std::wstring get_path();

    //! Sets logging level
    void set_level(int level);

    //! Sets maximum log size
    void set_max_size(ULONG64 max_size);

    //! Writes to log at n-th level
    void write(int level, const char* fmt, ...);
    void writeW(int level, const wchar_t* fmt, ...);

    //! Writes to log a binary buffer contents
    void write_raw(int level, LPBYTE buf, int buf_size);

    //! Writes to log a last system error message
    void log_last_error(int level=0, const char* szErrorMsg = NULL);

    //! Checks error log size and performs log rotation if log is too large.
    bool check_error_log_size();

private:

    //! Prints current time stamp.
	void print_timestamp();

    //! Renames a file error.log into error.log.1 and so on.
	bool recursively_rename_file(const char* szFileName, int number);

    std::wstring m_path;    //!< Path to log
    LONG64 m_max_size;      //!< Max log size
	int m_nMaxLogFileCount; //!< Max log file count.
    int m_level;            //!< Logging level
    FILE* m_LogFile;        //!< Handle to log file
    bool m_bRedirectStdStreams; //!< Do we redirect stdout and stderr streams to log file (Linux).
};

extern CLog g_log;

//! \class CLogEvent
//! \brief Log event.
class CLogEvent
{
public:

    CLogEvent(int level, std::wstring name)
    {
        g_log.writeW(level, L"Entering: %s\n", name.c_str());
        m_start = clock();
        m_level = level;
        m_name = name;
    }

    ~CLogEvent()
    {
        clock_t finish = clock();
        g_log.writeW(m_level, L"Leaving: %s.    ", m_name.c_str());
        double timeElapsed = (double)(finish - m_start) / CLOCKS_PER_SEC;
        g_log.writeW(m_level, L"Completed in: %0.3f sec.\n", timeElapsed);
    }
private:

    int m_level;         //!< Logging level.
    std::wstring m_name; //!< Event name.
    clock_t m_start;     //!< Event start time.
};

#ifdef _WIN32

// Formats error message by error code
std::wstring GetErrorMsg(DWORD errcode=GetLastError());

#endif
