#include "stdafx.h"
#include "Misc.h"
#include "strconv.h"
#include "md5.h"

double microtime()
{
#ifdef _WIN32
    return GetTickCount();
#else
    struct timeval tv;
    struct timezone tz;
    gettimeofday(&tv, &tz);
    return (double)(tv.tv_sec)*1000+(double)(tv.tv_usec)/1000;
#endif
}

size_t GetMemoryUsage()
{
	size_t uVirtualMem = 0;
#ifdef _WIN32
	PROCESS_MEMORY_COUNTERS_EX pmc;
	memset(&pmc, 0, sizeof(PROCESS_MEMORY_COUNTERS_EX));
	pmc.cb = sizeof(PROCESS_MEMORY_COUNTERS_EX);
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    uVirtualMem = pmc.PrivateUsage/1024;
#else
	int tSize = 0, resident = 0, share = 0;
    std::ifstream buffer("/proc/self/statm");
    buffer >> tSize >> resident >> share;
    buffer.close();

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    double rss = resident * page_size_kb;
    //cout << "RSS - " << rss << " kB\n";

    double shared_mem = share * page_size_kb;
    //cout << "Shared Memory - " << shared_mem << " kB\n";

    //cout << "Private Memory - " << rss - shared_mem << "kB\n";
    uVirtualMem = rss-shared_mem;
#endif

	return uVirtualMem;
}

#ifndef _WIN32
int _getch( )
{
  static struct termios oldt, newt;
  tcgetattr(0, &oldt); /* grab old terminal i/o settings */
  newt = oldt; /* make new settings same as old settings */
  newt.c_lflag &= ~ICANON; /* disable buffered i/o */
  newt.c_lflag &= ~ECHO; /* set echo mode */
  tcsetattr(0, TCSANOW, &newt); /* use these new terminal i/o settings now */

  int ch = getchar();

  tcsetattr(0, TCSANOW, &oldt); /* Restore old terminal i/o settings */
  return ch;
}
#endif

void SplitFileName(std::wstring sPath, std::wstring& sDirectory,
                   std::wstring& sFileName, std::wstring& sBaseName, std::wstring& sExtension)
{
    sFileName = sPath;
#ifdef _WIN32
    std::replace(sFileName.begin(), sFileName.end(), '/', '\\');
#else
    std::replace(sFileName.begin(), sFileName.end(), '\\', '/');
#endif
    sBaseName = sFileName;
#ifdef _WIN32
    size_t slash_pos = sFileName.rfind('\\');
#else
    size_t slash_pos = sFileName.rfind('/');
#endif
    if(slash_pos!=sFileName.npos)
    {
        sDirectory = sFileName.substr(0, slash_pos);
        sFileName = sFileName.substr(slash_pos+1);
        sBaseName = sFileName;
    }

    size_t dot_pos = sFileName.rfind('.');
    if(dot_pos!=sFileName.npos)
    {
        sExtension = sFileName.substr(dot_pos+1);
        sBaseName = sFileName.substr(0, dot_pos);
    }
}

int CreateDir(std::wstring sPath)
{
#ifdef _WIN32
    return _wmkdir(sPath.c_str());
#else
	int nRes = mkdir(strconv::w2a(sPath).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(nRes==0 || errno==EEXIST)
        return 0; // OK
	return 1; // Error
#endif
}

int RmDir(std::wstring sPath, bool bFailIfNonEmpty)
{
#ifdef _WIN32
	if(bFailIfNonEmpty)
	{
		BOOL bRemove = RemoveDirectory(sPath.c_str());
		return bRemove?0:1;
	}
	else	
	{
		SHFILEOPSTRUCT fop;
		memset(&fop, 0, sizeof(SHFILEOPSTRUCT));

		TCHAR szFrom[_MAX_PATH];
		memset(szFrom, 0, sizeof(TCHAR)*(_MAX_PATH));
		wcscpy_s(szFrom, _MAX_PATH, sPath.c_str());
		szFrom[sPath.length()+1] = 0;

		fop.fFlags |= FOF_SILENT;                // don't report progress
		fop.fFlags |= FOF_NOERRORUI;           // don't report errors
		fop.fFlags |= FOF_NOCONFIRMATION;        // don't confirm delete
		fop.wFunc = FO_DELETE;                   // REQUIRED: delete operation
		fop.pFrom = szFrom;                      // REQUIRED: which file(s)
		fop.pTo = NULL;                          // MUST be NULL
		fop.fFlags &= ~FOF_ALLOWUNDO;   // ..don't use Recycle Bin

		return SHFileOperation(&fop); // do it!
	}
#else
	// TODO
	std::string sDirNameUTF8 = strconv::w2utf8(sPath);
	return rmdir(sDirNameUTF8.c_str());
#endif
}

int RemoveFile(std::wstring sFilePath)
{
#ifdef _WIN32
	return _wunlink(sFilePath.c_str());
#else
	return unlink(strconv::w2utf8(sFilePath).c_str());
#endif
}

bool copy_file(std::wstring& sExistingFileName, std::wstring& sNewFileName, bool bFailIfExists)
{
#ifdef _WIN32
    BOOL bCopy = CopyFileW(sExistingFileName.c_str(), sNewFileName.c_str(), bFailIfExists);
    return bCopy!=FALSE;
#else
	std::stringstream stream;
	stream << "cp \"";
	stream << strconv::w2utf8(sExistingFileName);
	stream << "\" \"";
	stream << strconv::w2utf8(sNewFileName);
	stream << "\"";
	int nRes = system(stream.str().c_str());
	return nRes==0;
#endif
}

void Time2String(time_t t, std::string& sTime)
{
	sTime.empty();

    char szDateTime[64];
    struct tm* timeinfo = gmtime(&t);
    strftime(szDateTime, 64,  "%Y-%m-%dT%H:%M:%SZ", timeinfo);

    sTime = szDateTime;
}

void String2Time(std::string sUTC, time_t& t)
{
	std::string sYear = sUTC.substr(0, 4);
	std::string sMonth = sUTC.substr(5, 2);
	std::string sDay = sUTC.substr(8, 2);
	std::string sHour = sUTC.substr(11, 2);
	std::string sMin = sUTC.substr(14, 2);
	std::string sSec = sUTC.substr(17, 2);

	struct tm st;
    memset(&st, 0, sizeof(struct tm));
    st.tm_year = atoi(sYear.c_str()) - 1900; // year since 1900
	st.tm_mon = atoi(sMonth.c_str()) - 1; // month 0-11
	st.tm_mday = atoi(sDay.c_str());
	st.tm_hour = atoi(sHour.c_str());
	st.tm_min = atoi(sMin.c_str());
	st.tm_sec = atoi(sSec.c_str());
	st.tm_isdst = -1;

	time_t gmt = mktime(&st);

	// Convert to local time
	struct tm* lt = localtime(&gmt);
	t = mktime(lt);
}

// Helper function that removes spaces from the beginning and end of the string
void trim3(std::string& str, const char* szTrim)
{
    std::string::size_type pos = str.find_last_not_of(szTrim);
    if(pos != std::string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(szTrim);
        if(pos != std::string::npos) str.erase(0, pos);
    }
    else str.erase(str.begin(), str.end());
}

void wtrim(std::wstring& str, const wchar_t* szTrim)
{
    std::string::size_type pos = str.find_last_not_of(szTrim);
    if(pos != std::string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(szTrim);
        if(pos != std::string::npos) str.erase(0, pos);
    }
    else str.erase(str.begin(), str.end());
}

std::string & replace(std::string & subj, std::string old, std::string neu)
{
    size_t uiui = subj.find(old);
    if (uiui != std::string::npos)
    {
        subj.erase(uiui, old.size());
        subj.insert(uiui, neu);
    }
    return subj;
}

int CalcFileMD5Hash(std::wstring sFileName, std::wstring& sMD5Hash)
{
    BYTE buff[512];
    MD5 md5;
    MD5_CTX md5_ctx;
    unsigned char md5_hash[16];
    FILE* f = NULL;

#ifdef _WIN32
    _wfopen_s(&f, sFileName.c_str(), L"rb");
#else
    std::string sUtf8FileName = strconv::w2a(sFileName);
    f = fopen(sUtf8FileName.c_str(), "rb");
#endif

    if(f==NULL)
    {
        return -1; // Couldn't open file
    }

    md5.MD5Init(&md5_ctx);

    while(!feof(f))
    {
        size_t count = fread(buff, 1, 512, f);
        if(count>0)
        {
            md5.MD5Update(&md5_ctx, buff, (unsigned int)count);
        }
    }

    fclose(f);
    md5.MD5Final(md5_hash, &md5_ctx);

    int i;
    for(i=0; i<16; i++)
    {
        wchar_t number[10];
#ifdef _WIN32
        swprintf(number, 10, L"%02x", md5_hash[i]);
#else
        swprintf(number, 10, L"%02x", md5_hash[i]);
#endif
        sMD5Hash += number;
    }

   return 0;
}

std::wstring CalcStringMD5(std::string str)
{	
	std::wstring sMD5Hash;
    MD5 md5;
    MD5_CTX md5_ctx;
    unsigned char md5_hash[16];
    
	md5.MD5Init(&md5_ctx);
	md5.MD5Update(&md5_ctx, (unsigned char*)str.c_str(), str.length());
    md5.MD5Final(md5_hash, &md5_ctx);

    int i;
    for(i=0; i<16; i++)
    {
        wchar_t number[10];
#ifdef _WIN32
        swprintf(number, 10, L"%02x", md5_hash[i]);
#else
        swprintf(number, 10, L"%02x", md5_hash[i]);
#endif
        sMD5Hash += number;
    }

   return sMD5Hash;
}

void FixSlashesInFilePath(std::wstring& sPath)
{
#ifdef _WIN32
	std::replace(sPath.begin(), sPath.end(), '/', '\\');
#else
	std::replace(sPath.begin(), sPath.end(), '\\', '/');
#endif

}

#ifdef _WIN32

int execute(const char* szCmdLine, bool bWait, int* pnPid)
{
	if(pnPid)
		*pnPid = 0;

	STARTUPINFOA si;
	memset(&si, 0, sizeof(STARTUPINFO));
	si.cb = sizeof(STARTUPINFO);
	PROCESS_INFORMATION pi;
	BOOL bCreate = CreateProcessA(NULL, (LPSTR)szCmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if(!bCreate)
		return -1;

	if(pnPid)
		*pnPid = GetProcessId(pi.hProcess);

	if(!bWait)
		return 0;

	WaitForSingleObject(pi.hProcess, INFINITE);
	DWORD dwExitCode = 0;
	GetExitCodeProcess(pi.hProcess, &dwExitCode);
	CloseHandle(pi.hProcess);

	return (int)dwExitCode;
}

char *strtok_r(char *str, const char *delim, char **nextp)
{
    char *ret;

    if (str == NULL)
        str = *nextp;
    str += strspn(str, delim);
    if (*str == '\0')
        return NULL;
    ret = str;
    str += strcspn(str, delim);
    if (*str)
        *str++ = '\0';
    *nextp = str;
    return ret;
}

std::wstring GetModuleName(HMODULE hModule)
{
	wchar_t szBuf[_MAX_PATH]=L"";
	GetModuleFileNameW(hModule, szBuf, _MAX_PATH);
	return std::wstring(szBuf);
}

std::wstring GetModulePath(HMODULE hModule)
{
	wchar_t szBuf[_MAX_PATH]=L"";
	GetModuleFileNameW(hModule, szBuf, _MAX_PATH);
	TCHAR* ptr = _tcsrchr(szBuf,'\\');
	if(ptr!=NULL)
		*(ptr)=0; // remove executable name
	return std::wstring(szBuf);
}

std::wstring GetParentDir(std::wstring sPath)
{
	int pos = sPath.rfind(L'\\');
	if(pos!=sPath.npos)
		sPath = sPath.substr(0, pos);
	return sPath;
}

CString FileSizeToStr(ULONG64 uFileSize)
{
    CString sFileSize;

    if(uFileSize==0)
    {
        sFileSize = _T("0 KB");
    }
    else if(uFileSize<1024)
    {
        float fSizeKbytes = (float)uFileSize/(float)1024;
        TCHAR szStr[64];
#if _MSC_VER<1400
        _stprintf(szStr, _T("%0.1f KB"), fSizeKbytes);
#else
        _stprintf_s(szStr, 64, _T("%0.1f KB"), fSizeKbytes);
#endif
        sFileSize = szStr;
    }
    else if(uFileSize<1024*1024)
    {
        sFileSize.Format(_T("%I64u KB"), uFileSize/1024);
    }
    else
    {
        float fSizeMbytes = (float)uFileSize/(float)(1024*1024);
        TCHAR szStr[64];
#if _MSC_VER<1400
        _stprintf(szStr, _T("%0.1f MB"), fSizeMbytes);
#else
        _stprintf_s(szStr, 64, _T("%0.1f MB"), fSizeMbytes);
#endif
        sFileSize = szStr;
    }

    return sFileSize;
}
#else // Linux

void Sleep(int msec)
{
	struct timespec ts;
	ts.tv_sec = msec/1000;
	ts.tv_nsec = (msec%1000)*1000000L;
    nanosleep(&ts, NULL);
}

#endif //_WIN32


