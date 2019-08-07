
#include "stdafx.h"
#include "FileFinder.h"
#include "Misc.h"
#include "strconv.h"

CFileFinder::CFileFinder()
{
#ifdef _WIN32
	m_hFind = INVALID_HANDLE_VALUE;
	memset(&m_ffd, 0, sizeof(WIN32_FIND_DATAW));
#else
    m_dirp = NULL;
#endif
}

CFileFinder::~CFileFinder()
{
#ifdef _WIN32
	if(m_hFind!=INVALID_HANDLE_VALUE)
		FindClose(m_hFind);
#else
    if(m_dirp!=NULL)
        closedir(m_dirp);
#endif
}

bool CFileFinder::FindFirstFile(std::wstring sSearchPattern, CFindFileInfo* pFileInfo)
{
	// Validate input param
	if(pFileInfo==NULL)
		return false;

	// Save search pattern
	m_sSearchPattern = sSearchPattern;

	// Get directory name
	std::wstring sFileName;
	std::wstring sBaseFileName;
	std::wstring sExtension;
	SplitFileName(sSearchPattern, m_sDirectory, sFileName, sBaseFileName, sExtension);

#ifdef _WIN32

    m_sDirectory+='\\';

	// Replace '/' with '\\'
	std::replace(m_sSearchPattern.begin(), m_sSearchPattern.end(), '/', '\\');

	m_hFind = ::FindFirstFileW(m_sSearchPattern.c_str(), &m_ffd);
	if(m_hFind==INVALID_HANDLE_VALUE)
		return false;

	pFileInfo->m_sRelFileName = m_ffd.cFileName;
	pFileInfo->m_sFileName = m_sDirectory+m_ffd.cFileName;
	pFileInfo->m_bIsRegular = (m_ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0;
	pFileInfo->m_bIsDirectory = (m_ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;
	return true;
#else

    m_sDirectory+='/';

    // Replace '\\' with '/'
	std::replace(m_sSearchPattern.begin(), m_sSearchPattern.end(), '\\', '/');

    m_dirp = opendir(strconv::w2utf8(m_sDirectory).c_str());

    while(m_dirp)
    {
        //int errno = 0;
        struct dirent* pd = readdir(m_dirp);
        if(pd==NULL)
        {
            break;
        }

        if(strcmp(pd->d_name, ".")!=0 &&
           strcmp(pd->d_name, "..")!=0)
        {
            pFileInfo->m_sFileName = m_sDirectory;
            pFileInfo->m_sFileName += strconv::utf82w(pd->d_name);
            pFileInfo->m_sRelFileName = strconv::utf82w(pd->d_name);
			pFileInfo->m_bIsRegular = true;
			pFileInfo->m_bIsDirectory = false;
			struct stat st;
            if(0==stat(strconv::w2utf8(pFileInfo->m_sFileName).c_str(), &st))
			{
				pFileInfo->m_bIsRegular = S_ISREG(st.st_mode);
				pFileInfo->m_bIsDirectory = S_ISDIR(st.st_mode);
			}

            return true;
        }
    }

    return false;
#endif


}

bool CFileFinder::FindNextFile(CFindFileInfo* pFileInfo)
{
	if(!pFileInfo)
		return false;

	pFileInfo->m_sFileName.clear();
	pFileInfo->m_sRelFileName.clear();

#ifdef _WIN32
    if(m_hFind==INVALID_HANDLE_VALUE)
		return false;

	BOOL bFind = ::FindNextFileW(m_hFind, &m_ffd);
	if(!bFind)
		return false;

	pFileInfo->m_sRelFileName = m_ffd.cFileName;
	pFileInfo->m_sFileName = m_sDirectory+m_ffd.cFileName;
	pFileInfo->m_bIsRegular = (m_ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)==0;
	pFileInfo->m_bIsDirectory = (m_ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0;

	return true;
#else
    while(m_dirp)
    {
        //int errno = 0;
        struct dirent* pd = readdir(m_dirp);
        if(pd==NULL)
        {
            break;
        }

        if(strcmp(pd->d_name, ".")!=0 &&
           strcmp(pd->d_name, "..")!=0)
        {
            pFileInfo->m_sFileName = m_sDirectory+strconv::utf82w(pd->d_name);
            pFileInfo->m_sRelFileName = strconv::utf82w(pd->d_name);
            pFileInfo->m_bIsRegular = true;
			pFileInfo->m_bIsDirectory = false;
			struct stat st;
            if(0==stat(strconv::w2utf8(pFileInfo->m_sFileName).c_str(), &st))
			{
				pFileInfo->m_bIsRegular = S_ISREG(st.st_mode);
				pFileInfo->m_bIsDirectory = S_ISDIR(st.st_mode);
			}
            return true;
        }
    }

    return false;
#endif
}
