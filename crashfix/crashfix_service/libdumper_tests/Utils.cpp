#include "stdafx.h"
#include "Utils.h"
#include "strconv.h"

#ifdef _WIN32
std::wstring Utils::GetModulePath(HMODULE hModule)
{
    wstring string;
    wchar_t buf[_MAX_PATH];
    GetModuleFileNameW(hModule, buf, _MAX_PATH);
    *(wcsrchr(buf,'\\'))=0; // remove executable name
    string = buf;

    if(string[string.length()-1]!='\\')
        string += L"\\";
    return string;
}
#else

std::wstring Utils::GetExePath()
{
    char path[PATH_MAX]="";
    char dest[PATH_MAX]="";

    pid_t pid = getpid();
    if (readlink(path, dest, PATH_MAX) == -1)
    {
        return std::wstring(L"");

    }
    // Remove last part of path
    char* c = strrchr(dest, '/');
    *(c+1) = 0;

    return strconv::a2w(dest);
}

#endif

std::wstring Utils::GetDataFolder()
{
    std::wstring sPath;
#ifdef _WIN32
    sPath = GetModulePath(NULL);
    sPath += L"..\\data\\";
#else
    sPath =  GetExePath();
    sPath += L"../data/";
#endif
    return sPath;
}

wstring Utils::GetTestDataFolder()
{
    wstring sPath;
#ifdef _WIN32
    sPath =  GetModulePath(NULL);
    sPath += L"..\\test_data\\";
#else
    sPath =  GetExePath();
    sPath += L"../test_data/";
#endif
    return sPath;
}

bool Utils::CompareFiles(std::string sFile1, std::string sFile2)
{
	bool bResult = false;
	FILE* f1 = NULL;
	FILE* f2 = NULL;
	BYTE buf1[256];
	BYTE buf2[256];
	size_t n1 = 0;
	size_t n2 = 0;

	// Open two files
	f1 = fopen(sFile1.c_str(), "rb");
	f2 = fopen(sFile2.c_str(), "rb");
	if(f1==NULL || f2==NULL)
		goto cleanup;

	// Read both files and compare content
	for(;;)
	{
		n1 = fread(buf1, 1, 256, f1);
		n2 = fread(buf2, 1, 256, f2);
		if(n1==0 && n2==0)
		{
			// EOF reached
			break;
		}

		if(memcmp(buf1, buf2, n1)!=0)
			goto cleanup; // Files are different
	}

	// Files match exactly
	bResult = true;

cleanup:

	if(f1)
		fclose(f1);

	if(f2)
		fclose(f2);

	return bResult;
}
