#pragma once
#include "stdafx.h"

//! The following macros are used for parsing the command line
#define args_left() (argc-cur_arg)
#define arg_exists() (cur_arg<argc && argv[cur_arg]!=NULL)
#define get_arg() ( arg_exists() ? argv[cur_arg]:NULL )
#define skip_arg() cur_arg++
#define cmp_arg(val) (arg_exists() && (0==strcmp(argv[cur_arg], val)))

//! The following macros are used to define version
#define  MAKE_VERSION_HEX(A, B, C)   (((A) << 16) | ((B) << 8) | ((C) << 0))
#define  MAKE_VERSION_DEC(A, B, C)   (((A) * 10000) + ((B) * 100) + ((C) * 1))
#define  MAKE_VERSION_TEXT(A, B, C, D)    #A "." #B "." #C " " #D

/* Miscellaneous functions. */

//! Gets current time (in milliseconds) in platform-independent manner.
double microtime();

//! Splits file path into directory name, base file name and extension
void SplitFileName(std::wstring sPath, std::wstring& sDirectory, std::wstring& sFileName,
                   std::wstring& sBaseFileName, std::wstring& sExtension);

//! Creates a directory
int CreateDir(std::wstring sPath);

//! Removes a directory
int RmDir(std::wstring sPath, bool bFailIfNonEmpty);

//! Removes a file
int RemoveFile(std::wstring sPath);

//! Copies one file to another
bool copy_file(std::wstring& sExistingFileName, std::wstring& sNewFileName, bool bFailIfExists);

//! Returns time as string (uses UTC time format).
void Time2String(time_t t, std::string& sTime);

//! Converts UTC string to local time.
void String2Time(std::string sUTC, time_t& t);

void trim3(std::string& str, const char* szTrim=" \t\r\n");

void wtrim(std::wstring& str, const wchar_t* szTrim=L" \t\r\n");

//! Replaces an occurrense of substring in a string with another substring
std::string & replace(std::string & subj, std::string old, std::string neu);

//! Calculates MD5 hash of a file
int CalcFileMD5Hash(std::wstring sFileName, std::wstring& sMD5Hash);

//! Calculates MD5 hash of a string
std::wstring CalcStringMD5(std::string str);

// Returns current memory usage in KB.
size_t GetMemoryUsage();

//! Replaces slashes with backslashes (for Windows) and vise versa (for Linux).
void FixSlashesInFilePath(std::wstring& sPath);

#ifdef _WIN32

//! Spawns another process and, optionally, blocks until it extis
int execute(const char* szCmdLine, bool bWait = true, int* pnPid = NULL);

//! Tokenise string (reenterable)
char *strtok_r(char *str, const char *delim, char **nextp);

//! Returns path to module
std::wstring GetModulePath(HMODULE hModule);

//! Returns module name
std::wstring GetModuleName(HMODULE hModule);

//! Returns parent directory name
std::wstring GetParentDir(std::wstring sPath);

//! Converts file size number to string
CString FileSizeToStr(ULONG64 uFileSize);

#else

//! Grabs a character from console input
int _getch();

//! Sleeps for milliseconds
void Sleep(int msec);

#endif
