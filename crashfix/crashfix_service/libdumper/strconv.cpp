//! \file strconv.cpp
//! \brief String conversion functions
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "strconv.h"

static bool bSetLocale = false;

std::wstring strconv::a2w(std::string str)
{
    if(!bSetLocale)
    {
        setlocale (LC_ALL,"");
        bSetLocale = true;
    }

	// Calculate output buffer size
    size_t nmax = 0;
#ifdef _WIN32
    mbstowcs_s(&nmax, NULL, 0, str.c_str(), 0);
#else
    nmax = mbstowcs(NULL, str.c_str(), 0);
#endif

	// Check if buffer size calculated
	if(nmax==0 || nmax==(size_t)-1)
		return std::wstring(); // error 

	// Allocate buffer
    wchar_t* buf = new wchar_t[nmax+1];
	if(buf==NULL)
		return std::wstring(); // error 

	// Convert string
    size_t count = 0;
#ifdef _WIN32
    mbstowcs_s(&count, buf, nmax, str.c_str(), nmax+1);
#else
    mbstowcs(buf, str.c_str(), nmax+1);
#endif

	buf[nmax]=0; // ensure buffer is zero terminated

    std::wstring wstr(buf);

	// Free buffer
    delete [] buf;

    return wstr;
}

std::string strconv::w2a(std::wstring str)
{
    if(!bSetLocale)
    {
        setlocale (LC_ALL,"");
        bSetLocale = true;
    }

    size_t nmax = 0;
#ifdef _WIN32
    wcstombs_s(&nmax, NULL, 0, str.c_str(), 0);
#else
    nmax = wcstombs(NULL, str.c_str(), 0);    
#endif

	if(nmax==0 || nmax==(size_t)(-1))
        return "";

    char* buf = new char[nmax+1];
	if(buf==NULL)
		return "";

    size_t count = 0;
#ifdef _WIN32
    wcstombs_s(&count, buf, nmax, str.c_str(), nmax+1);
#else
    wcstombs(buf, str.c_str(), nmax+1);
#endif
	
	buf[nmax] = 0; // ensure buffer is zero terminated
    std::string wstr(buf);
    delete [] buf;

    return wstr;
}

std::string strconv::w2utf8(std::wstring str)
{
#ifdef _WIN32

	// Calculate required buffer size
	int count = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, NULL, 0, NULL, NULL);
    if(count==0)
    {
       return "";
    }

    // Convert UNICODE->UTF8
    char* buf = new char[count];
	int result = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), -1, (LPSTR)buf, count, NULL, NULL);
    if(result==0)
    {
        delete [] buf;
        return "";
    }

	std::string utf8(buf);
	delete [] buf;
	return utf8;

#else
	// On Linux do widechar->multibyte conversion.
	return w2a(str);
#endif
}

std::wstring strconv::utf82w(std::string str)
{
#ifdef _WIN32

	 // Calculate required buffer size
	int count = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
	if(count==0)
	{
		return NULL;
	}

	// Convert UNICODE->UTF8
	LPWSTR buf = new wchar_t[count];
	int result = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, (LPWSTR)buf, count);
	if(result==0)
	{
		delete [] buf;
		return NULL;
	}

	std::wstring wstr(buf);
	delete [] buf;
	return wstr;

#else
	// On Linux do multibyte->widechar conversion.
	return a2w(str);
#endif
}

std::string strconv::a2utf8(std::string str)
{
	return w2utf8(a2w(str));
}

std::string strconv::utf82a(std::string str)
{
	return w2a(utf82w(str));
}

std::string strconv::n2a(const char* fmt, ...)
{
	char buf[64]="";
	va_list args;
	va_start(args,fmt);
	vsprintf(buf, fmt, args);
	return std::string(buf);
}

std::wstring strconv::n2w(const wchar_t* fmt, ...)
{
	wchar_t buf[64]=L"";
	va_list args;
	va_start(args,fmt);
#ifdef _WIN32
	wvsprintf(buf, fmt, args);
#else
	vswprintf(buf, 63, fmt, args);
#endif
	return std::wstring(buf);
}

std::string strconv::format(int nMaxChar, const char* fmt, ...)
{
	char* buf = new char[nMaxChar];
	va_list args;
	va_start(args, fmt);
	vsnprintf(buf, nMaxChar, fmt, args);
	std::string sRes = buf;
	delete [] buf;
	return sRes;
}

std::wstring strconv::formatW(int nMaxChar, const wchar_t* fmt, ...)
{
	wchar_t* buf = new wchar_t[nMaxChar];
	va_list args;
	va_start(args, fmt);
#ifdef _WIN32
	wvnsprintf(buf, nMaxChar, fmt, args);
#else
	vswprintf(buf, nMaxChar, fmt, args);
#endif
	std::wstring sRes = buf;
	delete [] buf;
	return sRes;
}
