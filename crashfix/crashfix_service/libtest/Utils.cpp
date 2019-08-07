#include "stdafx.h"
#include "Utils.h"

#ifdef _WIN32
std::wstring Utils::GetModulePath(HMODULE hModule)
{
	std::wstring string;
    wchar_t buf[_MAX_PATH];
    GetModuleFileNameW(hModule, buf, _MAX_PATH);
    *(wcsrchr(buf,'\\'))=0; // remove executable name
    string = buf;

    if(string[string.length()-1]!='\\')
        string += L"\\";
    return string;
}
#endif

std::wstring Utils::GetDataFolder()
{
    std::wstring sPath;
#ifdef _WIN32
    sPath = GetModulePath(NULL);
    sPath += L"..\\data\\";
#else
    sPath += L"../data/";
#endif
    return sPath;
}

std::wstring Utils::GetTestDataFolder()
{
	std::wstring sPath;
#ifdef _WIN32
    sPath =  GetModulePath(NULL);
    sPath += L"..\\test_data\\";
#else
    sPath = GetCurrentDir();
    sPath += L"../test_data/";
#endif
    return sPath;
}

std::wstring Utils::GetCurrentDir()
{
	std::wstring sPath;
#ifdef _WIN32
    sPath =  GetModulePath(NULL);
    //sPath += L"\\";
#else
    FILE * fp = NULL;
    char res[512] = "";
    wchar_t wres[1024] = L"";
    fp = popen("pwd","r");
    if(fp!=NULL)
    {
        int nBytes = fread(res, 1, 511, fp);
        fclose(fp);
        if(nBytes==0)
            return sPath;
        mbstowcs(wres, res, 1024);
        sPath = std::wstring(wres);
        sPath += L"/";
        sPath.erase(std::remove(sPath.begin(), sPath.end(), L'\r'), sPath.end());
        sPath.erase(std::remove(sPath.begin(), sPath.end(), L'\n'), sPath.end());
    }
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

		if(n1!=n2)
            goto cleanup; // Files have different length

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

