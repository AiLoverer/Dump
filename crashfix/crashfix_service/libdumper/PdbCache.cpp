//! \file PdbCache.cpp
//! \brief PDB file cache.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbCache.h"
#include "strconv.h"
#include "Misc.h"
#include "FileFinder.h"

CPdbCache::CPdbCache()
{
	m_nEntryIdSeed = 0;
    m_nMaxEntryCount = 100;
	m_uMaxMemUsageKB = 100*1024; // 100 MB
}

CPdbCache::~CPdbCache()
{
    Clear();
}

bool CPdbCache::SetMaxEntryCount(int nMaxEntryCount)
{
    // Validate input param
    if(nMaxEntryCount<=0 || nMaxEntryCount>1000)
        return false;

    CAutoLock lock(&m_AccessLock);
    m_nMaxEntryCount = nMaxEntryCount;

	// Remove entries beyond the limit
	CheckCacheOverflow();
    return true;
}

int CPdbCache::GetMaxEntryCount()
{
	return m_nMaxEntryCount;
}

// Sets maximum memory usage in KB.
bool CPdbCache::SetMaxMemUsage(size_t uMaxMemUsageKB)
{
	m_uMaxMemUsageKB = uMaxMemUsageKB;
	return true;
}

//! Returns current memory usage in KB.
size_t CPdbCache::GetMaxMemUsageKB()
{
	return m_uMaxMemUsageKB;
}

bool CPdbCache::AddPdbSearchDir(
		std::wstring sPathToSearchDir,
		ePdbDirSearchMode SearchMode,
		bool bSearchRecursively)
{
	// Add a directory to PDB search list.

	// Check if directory really exists.
#ifdef _WIN32
	DWORD dwAttrs = GetFileAttributesW(sPathToSearchDir.c_str());
	if(dwAttrs==INVALID_FILE_ATTRIBUTES || (dwAttrs&FILE_ATTRIBUTE_DIRECTORY)==0)
		return false; // Directory does not exist.
#else
    struct stat st_buf;
    int status = stat (strconv::w2a(sPathToSearchDir).c_str(), &st_buf);
    if (status != 0)
        return false;

    if (!S_ISDIR (st_buf.st_mode))
        return false;
#endif

    CAutoLock lock(&m_AccessLock);

	// Add directory to our list
    _SearchDirInfo sdi;
    sdi.m_sPath = sPathToSearchDir;
#ifdef _WIN32
	std::replace(sdi.m_sPath.begin(), sdi.m_sPath.end(), '/', '\\');
#else
	std::replace(sdi.m_sPath.begin(), sdi.m_sPath.end(), '\\', '/');
#endif
    sdi.m_bSearchRecursively = bSearchRecursively;
	sdi.m_SearchMode = SearchMode;
    m_aSearchDirs[sPathToSearchDir] = sdi;

    return true;
}

bool
CPdbCache::FindPdb(
	std::wstring sGUIDnAge,
	std::wstring sPdbFileName,
	std::wstring sImageFileName,
	CPdbReader** ppPdbReader,
	CPeReader** ppPeReader,
	int* pnEntry,
	std::string* psErrorMsg,
	bool bExactMatchBuildAge
	)
{
    // Init output variables
    if(ppPdbReader!=NULL)
        *ppPdbReader = NULL;

    if(ppPeReader!=NULL)
		*ppPeReader = NULL;

	if(pnEntry!=NULL)
		*pnEntry = -1;

	if(psErrorMsg)
		*psErrorMsg = "Unspecified error";

    // Validate input

	if(sGUIDnAge.empty() ||
        sPdbFileName.empty())
	{
		if(psErrorMsg)
			*psErrorMsg = "Either GUIDnAge or path should be specified";
        return false; // Either GUIDnAge or path should be specified
	}

	if(ppPdbReader==NULL ||
	   ppPeReader==NULL ||
	   pnEntry==NULL)
	{
		if(psErrorMsg)
			*psErrorMsg = "Output variables should be set";
		return false; // Output variables should be set
	}

    bool bStatus = false;
    CPdbReader* pPdbReader = NULL;
    CPeReader* pPeReader = NULL;
    std::wstring sDir;
    std::wstring sFile;
    std::wstring sBaseName;
    std::wstring sExt;
    std::wstring sPdbName;
    std::wstring sPeName;
    std::map<std::wstring, _SearchDirInfo>::iterator it;
	int nEntry = -1;
	std::string sErrorMsg;

	// Fix slashes in file path (OS dependent)
	FixSlashesInFilePath(sPdbFileName);
	FixSlashesInFilePath(sImageFileName);

    // If GUID+Age is specified, check if we have a PDB with such GUID+Age loaded
    if(!sGUIDnAge.empty())
    {
		eFindExistingEntryResult res = FindExistingEntry(sGUIDnAge, sPdbFileName,
			sImageFileName, &pPdbReader, &pPeReader, &nEntry, bExactMatchBuildAge);
		if(res==FEE_FOUND)
		{
			sErrorMsg = "Success";
			bStatus = true;
			goto cleanup;
		}

		if(res==FEE_PENDING_DELETE)
		{
			sErrorMsg = "File is found in cache, but it is marked for deletion";
			bStatus = false;
			goto cleanup;
		}
    }

    // Try to find matching PDB file from search dirs

    // Get image file base name (without path and extension)
    SplitFileName(sPdbFileName, sDir, sFile, sBaseName, sExt);
    sPdbName = sBaseName + L".pdb";
    SplitFileName(sImageFileName, sDir, sFile, sBaseName, sExt);
    sPeName = sBaseName + L".";
    sPeName += sExt;

    for(it=m_aSearchDirs.begin(); it!=m_aSearchDirs.end(); it++)
    {
        _SearchDirInfo& sdi = it->second;

		bStatus = SearchDirectory(sdi.m_sPath, sdi.m_SearchMode, sdi.m_bSearchRecursively,
            sPdbName, sPeName, sGUIDnAge, &pPdbReader, &pPeReader, &nEntry, sErrorMsg, bExactMatchBuildAge);
        if(bStatus)
            break;
    }

	sErrorMsg = "Exhausted search";

cleanup:

    if(bStatus)
    {
		// Return the results
        *ppPdbReader = pPdbReader;
        *ppPeReader = pPeReader;
		*pnEntry = nEntry;
    }

	if(psErrorMsg)
		*psErrorMsg = sErrorMsg;

    return bStatus;
}

CPdbCache::eFindExistingEntryResult CPdbCache::FindExistingEntry(
		std::wstring sGUIDnAge,
		std::wstring sPdbFileName,
		std::wstring sImageFileName,
        CPdbReader** ppPdbReader,
		CPeReader** ppPeReader,
		int* pnEntry,
		bool bExactMatchBuildAge
		)
{
	CAutoLock lock(&m_AccessLock);
	eFindExistingEntryResult res = FEE_NOT_FOUND;
	int nEntry = -1;
	CPdbReader* pPdbReader = NULL;
	CPeReader* pPeReader = NULL;

	// Try to find exact match of GUID+Age string
    std::map<std::wstring, int>::iterator it = m_aGuidIndex.find(sGUIDnAge);
    if(it==m_aGuidIndex.end())
	{
		if(!bExactMatchBuildAge)
		{
			// If exact match of GUID+Age not found, try to find at least match of GUID
			std::wstring sGUID = sGUIDnAge.substr(0, 36);
			it = m_aGuidIndex.lower_bound(sGUID);
			if(it!=m_aGuidIndex.end())
			{
				std::wstring sTheirGUID = it->first.substr(0, 36);
				if(sTheirGUID.compare(sGUID)!=0)
				{
					res = FEE_NOT_FOUND;
					goto cleanup; // Not found such a GUID
				}
			}
			else
			{
				res = FEE_NOT_FOUND;
				goto cleanup; // Not found such a GUID
			}
		}
		else
		{
			res = FEE_NOT_FOUND;
			goto cleanup; // Not found such a GUID
		}
	}

    // Such a GUID found
    nEntry = it->second;

	// Check if this entry is maked for deletion...
	if(m_aEntries[nEntry].m_bPendingDelete)
	{
		// ... do not allow to addref this entry, it will be deleted as
		// soon as the last reference to it is released.
		res = FEE_PENDING_DELETE;
		goto cleanup;
	}

	// Increment ref count
	m_aEntries[nEntry].m_nRefCount++;

	// Get PDB reader and PE reader
    pPdbReader = m_aEntries[nEntry].m_pPdbReader;
    pPeReader = m_aEntries[nEntry].m_pPeReader;

	// Check if entry was unreferenced
	if(m_aEntries[nEntry].m_nRefCount==1)
	{
		// Remove it from unref index
		std::multimap<time_t, int>::iterator access_it = m_aUnrefIndex.find(m_aEntries[nEntry].m_AccessTime);
		while(access_it!=m_aUnrefIndex.end() &&
			access_it->second!=nEntry)
		{
			access_it++;
		}
		m_aUnrefIndex.erase(access_it);
	}

	// Update access time of the entry
    time_t CurTime;
    time(&CurTime);
    m_aEntries[nEntry].m_AccessTime = CurTime;

	// Success
    res = FEE_FOUND;

cleanup:

	if(res==FEE_FOUND)
    {
		// Return the results
        *ppPdbReader = pPdbReader;
        *ppPeReader = pPeReader;
		*pnEntry = nEntry;
    }

    return res;
}

bool CPdbCache::SearchDirectory(std::wstring sSearchDir, ePdbDirSearchMode SearchMode, bool bRecursive,
                          std::wstring sPdbName, std::wstring sPeName, std::wstring sGUIDnAge,
                          CPdbReader** ppPdbReader, CPeReader** ppPeReader, int* pnEntry,
							std::string& sErrorMsg, bool bExactMatchBuildAge)
{
    // Check if directory name begins with "debugInfo" magic symbols
    if(SearchMode==PDB_SYMBOL_STORE || sSearchDir.rfind(L"debugInfo")!=sSearchDir.npos)
    {
        // The directory is a symbol storage
        return SearchSymStorageDir(sSearchDir, sPdbName, sPeName,
			sGUIDnAge, ppPdbReader, ppPeReader, pnEntry, sErrorMsg, bExactMatchBuildAge);
    }

#ifdef _WIN32
    HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ffd;
    std::wstring sSearchPath;

    sSearchPath = sSearchDir;
    sSearchPath += L"\\";
    sSearchPath += sPdbName;

    // Search for PDB file in current folder
    hFind = FindFirstFileW(sSearchPath.c_str(), &ffd);
    BOOL bFind = TRUE;
    while(hFind!=INVALID_HANDLE_VALUE && bFind)
    {
        std::wstring sPdbFileName = sSearchDir;
        sPdbFileName += L"\\" + std::wstring(ffd.cFileName);

        std::wstring sPeFileName = sSearchDir;
        sPeFileName += L"\\" + sPeName;

        if(TryPdbFile(sPdbFileName, sPeFileName, sGUIDnAge, ppPdbReader, ppPeReader, pnEntry, false, sErrorMsg, bExactMatchBuildAge))
		{
			FindClose(hFind);
            return true;
		}

        bFind = FindNextFileW(hFind, &ffd);
    }

	FindClose(hFind);

    // Search for PDB if subfolders
    if(bRecursive)
    {
		sSearchPath = sSearchDir;
		sSearchPath += L"\\*";

	    hFind = FindFirstFileW(sSearchPath.c_str(), &ffd);
		BOOL bFind = TRUE;
		while(hFind!=INVALID_HANDLE_VALUE && bFind)
		{
			if(wcscmp(ffd.cFileName, L".")!=0 &&
			   wcscmp(ffd.cFileName, L"..")!=0 &&
			   (ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0)
			{
				std::wstring sSubDirName = sSearchDir;
				sSubDirName += L"\\";
				sSubDirName += std::wstring(ffd.cFileName);
				if(SearchDirectory(sSubDirName, SearchMode, bRecursive, sPdbName, sPeName,
					sGUIDnAge, ppPdbReader, ppPeReader, pnEntry, sErrorMsg, bExactMatchBuildAge))
				{
					FindClose(hFind);
					return true;
				}
			}

			bFind = FindNextFileW(hFind, &ffd);
		}
    }

	FindClose(hFind);

#else

    std::string sUtf8SearchDir = strconv::w2a(sSearchDir);
    DIR* dirp = opendir(sUtf8SearchDir.c_str());

    while(dirp)
    {
        //int errno = 0;
        struct dirent* pd = readdir(dirp);
        if(pd==NULL)
        {
            closedir(dirp);
            break;
        }

        if(strcmp(pd->d_name, ".")!=0 &&
           strcmp(pd->d_name, "..")!=0)
        {
            std::string sPdbFileName = sUtf8SearchDir;
            sPdbFileName += '/';
            sPdbFileName += pd->d_name;

            std::string sPeFileName = sUtf8SearchDir;
            sPeFileName += "/";
            sPeFileName += strconv::w2utf8(sPeName);

            size_t pos = sPdbFileName.rfind('.');
            std::string sExt = sPdbFileName.substr(pos);
            std::transform(sExt.begin(), sExt.end(), sExt.begin(), tolower);

            struct stat st;
            if(0==stat(sPdbFileName.c_str(), &st))
            {
                if(sExt.compare(".pdb")==0 && S_ISREG(st.st_mode))
                {
                    if(TryPdbFile(strconv::a2w(sPdbFileName), strconv::a2w(sPeFileName),
						sGUIDnAge, ppPdbReader, ppPeReader, pnEntry, false, sErrorMsg, bExactMatchBuildAge))
                       return true;
                }
                else if(bRecursive && S_ISDIR(st.st_mode))
                {
                    if(SearchDirectory(strconv::a2w(sPdbFileName), SearchMode,
						bRecursive, sPdbName, sPeName, sGUIDnAge, ppPdbReader, ppPeReader, pnEntry, sErrorMsg, bExactMatchBuildAge))
                       return true;
                }
            }
        }
    }


#endif

    return false;
}

bool CPdbCache::SearchSymStorageDir(std::wstring sSearchDir, std::wstring sPdbName,
        std::wstring sPeName, std::wstring sGUIDnAge,
        CPdbReader** ppPdbReader, CPeReader** ppPeReader, int* pnEntry
		,std::string& sErrorMsg, bool bExactMatchBuildAge)
{
#ifdef _WIN32
    char slash = '\\';
#else
    char slash = '/';
#endif

    std::wstring sPdbFileName = sSearchDir;
    sPdbFileName += slash;
    sPdbFileName += sPdbName;
    sPdbFileName += slash;
    sPdbFileName += sGUIDnAge;
	sPdbFileName += slash;
	sPdbFileName += sPdbName;

    std::wstring sPeFileName = sSearchDir;
    sPeFileName += sPeName;

	// Try to find exact match of GUID+Age string
    if(TryPdbFile(sPdbFileName, sPeFileName, sGUIDnAge, ppPdbReader, ppPeReader, pnEntry, true, sErrorMsg, bExactMatchBuildAge))
	{
        return true;
	}

	if(!bExactMatchBuildAge)
	{
		// If exact match not found, try to find at least matching GUID
		std::wstring sGUID = sGUIDnAge.substr(0, 36);
		std::wstring sAge = sGUIDnAge.substr(36);
		int nAge = wcstol(sAge.c_str(), NULL, 10);
		std::wstring sSearchPattern = sSearchDir;
		sSearchPattern += slash;
		sSearchPattern += sPdbName;
		sSearchPattern += slash;
		sSearchPattern += sGUID;
		sSearchPattern += L"*";

		CFindFileInfo ffi;
		CFileFinder ff;
		std::map<int, std::wstring> asCandidates;
		bool bFound = ff.FindFirstFile(sSearchPattern, &ffi);
		while(bFound)
		{
			// Check if file is a directory
			if(!ffi.m_bIsDirectory)
			{
				bFound = ff.FindNextFile(&ffi);
				continue;
			}

			// Check if file's GUID matches
			std::wstring sTheirGUID = ffi.m_sRelFileName.substr(0, 36);
			if(sTheirGUID.compare(sGUID)!=0)
			{
				bFound = ff.FindNextFile(&ffi);
				continue;
			}

			// Save in the list of candidates
			std::wstring sTheirAge = ffi.m_sRelFileName.substr(36);
			int nTheirAge = wcstol(sTheirAge.c_str(), NULL, 10);
			asCandidates[abs(nTheirAge-nAge)] = ffi.m_sFileName;

			// Go to next file
			bFound = ff.FindNextFile(&ffi);
		}

		// Try candidates, prefer those ones having the smallest age difference
		std::map<int, std::wstring>::iterator it = asCandidates.begin();
		while(it!=asCandidates.end())
		{
			// Try to open the file
			sPdbFileName = it->second;
			sPdbFileName += slash;
			sPdbFileName += sPdbName;
			if(TryPdbFile(sPdbFileName, sPeFileName, sGUIDnAge, ppPdbReader, ppPeReader, pnEntry, true, sErrorMsg, bExactMatchBuildAge))
			{
				return true;
			}
		}
	}

    return false;
}

bool CPdbCache::TryPdbFile(std::wstring sPdbFileName, std::wstring sPeFileName,
                           std::wstring sGUIDnAge, CPdbReader** ppPdbReader,
						   CPeReader** ppPeReader, int* pnEntry, bool bSymStore,
							std::string& sErrorMsg, bool bExactMatchBuildAge)
{
	// Statuses returned by this method
	enum eTryPdbStatus
	{
		TPS_SUCCESS                 =  0, // This PDB file matches.
		TPS_ERROR_PDB_GUID_MISMATCH = -2, // The file's GUID+Age does not match.
		TPS_ERROR_INVALID_PDB       = -3, // This file is not a valid PDB file.
		TPS_ERROR_DUPLICATE_PDB     = -4, // Such a file was already indexed.
		TPS_FILE_DOES_NOT_EXIST     = -5, // Such a file does not exist on disk.
	};

	eTryPdbStatus Status = TPS_ERROR_INVALID_PDB;
	int nEntry = -1;
	CPdbReader* pPdbReader = NULL;
	CPeReader* pPeReader = NULL;
	CPdbHeadersStream* pHeaders = NULL;
	std::wstring sGUIDnAge2;
	std::wstring sGUID;
	std::wstring sGUID2;
	bool bGUIDMatch = false;
	time_t CurTime;
	_CacheEntry Entry;
	std::map<std::wstring, _PathEntry>::iterator path_it;
	bool bLoadingWait = false;
	_PathEntry PathEntry;

	// Init output
    if(ppPdbReader!=NULL)
        *ppPdbReader = NULL;

    if(ppPeReader!=NULL)
        *ppPeReader = NULL;

	if(pnEntry!=NULL)
		*pnEntry = -1;

	sErrorMsg = "Unspecified error";

	m_AccessLock.Lock();

	FixSlashesInFilePath(sPdbFileName);
	FixSlashesInFilePath(sPeFileName);

	// Check if file exists on disk
	BOOL bExists = FALSE;
#ifdef _WIN32
	DWORD dwAttrs = GetFileAttributesW(sPdbFileName.c_str());
	if(dwAttrs!=INVALID_FILE_ATTRIBUTES && (dwAttrs&FILE_ATTRIBUTE_DIRECTORY)==0)
		bExists = TRUE;
#else
	struct stat st;
	if(0==stat(strconv::w2a(sPdbFileName).c_str(), &st) && S_ISREG(st.st_mode))
		bExists = TRUE;
#endif
	if(!bExists)
	{
		// File does not exist.
		sErrorMsg = "File does not exist on disk";
		m_AccessLock.Unlock();
		Status = TPS_FILE_DOES_NOT_EXIST;
		goto cleanup;
	}

	// Check if such file path already presents
	path_it = m_aPathIndex.find(sPdbFileName);
	if(path_it!=m_aPathIndex.end())
	{
		if(path_it->second.m_Type==ENTRY_LOADED)
		{
			//wprintf(L"Entry already indexed, checking its GUID %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
			// Such file path already was indexed, we need to check its GUID
			eFindExistingEntryResult res =  FindExistingEntry(sGUIDnAge, sPdbFileName,
								sPeFileName, &pPdbReader, &pPeReader, &nEntry, bExactMatchBuildAge);

			m_AccessLock.Unlock();

			if(res==FEE_FOUND)
			{
				// GUID match
				sErrorMsg = "Success";
				Status = TPS_SUCCESS;
				goto cleanup;
			}

			if(res==FEE_PENDING_DELETE)
			{
				sErrorMsg = "File found in cache, but it is marked for deletion";
				Status = TPS_FILE_DOES_NOT_EXIST;
				goto cleanup;
			}

			// Such a PDB file was already indexed, but its GUID does not match ours
			sErrorMsg = "File found in cache, but its GUID does not match";
			Status = TPS_ERROR_PDB_GUID_MISMATCH;
			goto cleanup;
		}
		else if(path_it->second.m_Type==ENTRY_INVALID)
		{
			// Such file already was marked as invalid PDB file
			//wprintf(L"Error, entry is invalid %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
			m_AccessLock.Unlock();
			sErrorMsg = "File is not a valid PDB file";
			Status = TPS_ERROR_INVALID_PDB;
			goto cleanup;
		}
		else if(path_it->second.m_Type==ENTRY_LOADING)
		{
			// This file is being loaded in another thread. We need to wait until it is loaded.
			//wprintf(L"Waiting while file loaded in another thread %s %s", sGUIDnAge.c_str(), sPdbFileName.c_str());
			path_it->second.m_nLoadRefCount++; // mark this path entry as load waiting
			m_AccessLock.Unlock();

			for(;;)
			{
				// Wait for 100 msec
				Sleep(100);

				// Retry
				m_AccessLock.Lock();
				path_it = m_aPathIndex.find(sPdbFileName);
				if(path_it->second.m_Type==ENTRY_LOADING)
				{
					//wprintf(L"Wait again %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
					// Wait again
					m_AccessLock.Unlock();
					continue;
				}

				// Entry seems to be loaded. Check its status.
				if(path_it->second.m_Type==ENTRY_LOADED)
				{
					bLoadingWait = true;

					// Get loaded entry
					nEntry = path_it->second.m_nEntryId;
					_CacheEntry& Entry = m_aEntries[nEntry];
					// Compare GUIDnAge
					if(Entry.m_sGUIDnAge==sGUIDnAge)
					{
						// Match!
						Status = TPS_SUCCESS;
						pPdbReader = Entry.m_pPdbReader;
						pPeReader = Entry.m_pPeReader;
						Entry.m_nRefCount++;

						m_AccessLock.Unlock();
						//wprintf(L"Success, entry loaded and GUID matches %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
						sErrorMsg = "Success, entry loaded and GUID matches";
						goto cleanup;
					}
					else if(!bExactMatchBuildAge)
					{
						// Compare GUIDs
						std::wstring sGUID = Entry.m_sGUIDnAge.substr(0, 36);
						std::wstring sTheirGUID = sGUIDnAge.substr(0, 36);
						if(sGUID.compare(sTheirGUID)==0)
						{
							// Match!
							Status = TPS_SUCCESS;
							pPdbReader = Entry.m_pPdbReader;
							pPeReader = Entry.m_pPeReader;
							Entry.m_nRefCount++;

							m_AccessLock.Unlock();
							//wprintf(L"Success, entry loaded and GUID matches %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
							sErrorMsg = "Success, entry loaded and GUID matches";
							goto cleanup;
						}
					}
					else
					{
						m_AccessLock.Unlock();
						// Entry loaded, but its GUID does not match ours
						//wprintf(L"Error, entry loaded, but GUID mismatch %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
						sErrorMsg = "Entry was loaded, but GUID does not match";
						Status = TPS_ERROR_PDB_GUID_MISMATCH;
						goto cleanup;
					}
				}
				else
				{
					// Entry loaded, but it is invalid PDB file
					//wprintf(L"Error, entry loaded, but PDB is invalid %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
					m_AccessLock.Unlock();
					sErrorMsg = "Entry was loaded, but file is not a valid PDB file";
					Status = TPS_ERROR_INVALID_PDB;
					goto cleanup;
				}


			}
		}
		else // entry unloaded
		{
			//wprintf(L"Entry unloaded, load from file %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
			// Just continue to loading stage
		}
	}

	// Mark this file as loading now
	// (will update its status later depending on PDB init status)
	PathEntry.m_Type = ENTRY_LOADING;
	PathEntry.m_nEntryId = -1;
	PathEntry.m_nLoadRefCount = 0;
	m_aPathIndex[sPdbFileName] = PathEntry;
	m_AccessLock.Unlock();

    // Create PDB reader
    pPdbReader = new CPdbReader();
    if(!pPdbReader->Init(sPdbFileName))
    {
		//wprintf(L"Error, PDB is invalid %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
        // Couldn't init PDB reader
		sErrorMsg = "Error reading PDB file";
		Status = TPS_ERROR_INVALID_PDB;
        goto cleanup;
    }

    // Create PE reader
    pPeReader = new CPeReader();
    if(!pPeReader->Init(sPeFileName))
    {
        // Couldn't init PE reader
		// This is not a critical problem
		delete pPeReader;
		pPeReader = NULL;
    }

    // Get GUID
    pHeaders = pPdbReader->GetHeadersStream();
    if(pHeaders==NULL)
    {
		//wprintf(L"Error, PDB GUI get error %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
        // Couldn't get GUID from PDB reader
		sErrorMsg = "Couldn't get GUID from PDB reader";
		Status = TPS_ERROR_INVALID_PDB;
        goto cleanup;
    }

	// Get GUID+Age
    sGUIDnAge2 = pHeaders->GetGUIDnAge();
	if(sGUIDnAge2.compare(sGUIDnAge)==0)
		bGUIDMatch = true;
	else if(!bExactMatchBuildAge)
	{
		sGUID = sGUIDnAge.substr(0, 36);
		sGUID2 = sGUIDnAge2.substr(0, 36);
		if(sGUID.compare(sGUID2)==0)
			bGUIDMatch = true;
	}

	{
		CAutoLock lock(&m_AccessLock);

		if(m_aGuidIndex.find(sGUIDnAge2)!=m_aGuidIndex.end())
		{
			// Such a PDB GUID already was indexed!
			//wprintf(L"Error, such PDB guid already indexed %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());
			sErrorMsg = "Error, such PDB GUID already indexed";
			Status = TPS_ERROR_DUPLICATE_PDB;
			goto cleanup;
		}

		if(pPeReader)
		{
			std::wstring sPePdbName;
			std::wstring sPeGUID;
			int nPeAge = -1;
			BOOL bGetCvInfo = pPeReader->GetCvInfo(sPePdbName, sPeGUID, nPeAge);
			std::wstringstream wstream;
			wstream << sPeGUID;
			wstream << nPeAge;
			std::wstring sPeGUIDnAge = wstream.str();
			if(!bGetCvInfo || sPeGUIDnAge!=sGUIDnAge2)
			{
				// GUID mismatch - wrong image file
				// Not critical
				delete pPeReader;
				pPeReader = NULL;
			}
		}

		// Generate unique ID for new entry
		nEntry = GetUniqueEntryId();

		_PathEntry& PathEntry = m_aPathIndex[sPdbFileName];

		// Insert new entry to cache
		time(&CurTime);
		Entry.m_sPdbPath = sPdbFileName;
		Entry.m_AccessTime = CurTime;
		if(!bGUIDMatch)
		{
			// Set ref count to zero
			Entry.m_nRefCount = 0;
		}
		else
			Entry.m_nRefCount = 1;
		if(PathEntry.m_nLoadRefCount>0)
			Entry.m_nRefCount += PathEntry.m_nLoadRefCount;
		Entry.m_pPdbReader = pPdbReader;
		Entry.m_pPeReader = pPeReader;
		Entry.m_sGUIDnAge = sGUIDnAge2;
		Entry.m_bSymStore = bSymStore;
		m_aEntries[nEntry] = Entry;

		if(Entry.m_nRefCount==0)
		{
			// Add to unref index
			std::pair<time_t, int> pair(Entry.m_AccessTime, nEntry);
			m_aUnrefIndex.insert(pair);
		}

		// Update path index
		PathEntry.m_nEntryId = nEntry;
		PathEntry.m_Type = ENTRY_LOADED;
		PathEntry.m_nLoadRefCount = 0; // reset loader ref count


		// Insert PDB GUID+Age into GUID index
		m_aGuidIndex[sGUIDnAge2] = nEntry;

		// Compare GUIDs
		if(!bGUIDMatch)
		{
			// GUID mismatch!
			//wprintf(L"GUID mismatch, entry added %s %s\n", sGUIDnAge2.c_str(), sPdbFileName.c_str());
			sErrorMsg = "Error, GUID mismatch";
			Status = TPS_ERROR_PDB_GUID_MISMATCH;
			goto cleanup;
		}
	}

	// Success
	sErrorMsg = "Success, matching entry loaded";
	Status = TPS_SUCCESS;
	//wprintf(L"Success, matching entry loaded %s %s\n", sGUIDnAge.c_str(), sPdbFileName.c_str());

cleanup:

	if(bLoadingWait && nEntry>=0)
		ReleaseCacheEntry(nEntry);

#ifdef _DEBUG
    CheckCacheIntegrity();
#endif

	// Ensure cache size is below the limit
	CheckCacheOverflow();

#ifdef _DEBUG
    CheckCacheIntegrity();
#endif

	if(Status==TPS_SUCCESS)
	{
		// Return PDB reader object
		*ppPdbReader = pPdbReader;
		*ppPeReader = pPeReader;
		*pnEntry = nEntry;
	}
	else
	{
		if(Status!=TPS_ERROR_PDB_GUID_MISMATCH)
		{
			// Some critical error occurred
			// Delete PDB and PE readers
			if(pPdbReader)
				delete pPdbReader;
			if(pPeReader)
				delete pPeReader;

			if(Status==TPS_ERROR_INVALID_PDB || Status==TPS_ERROR_DUPLICATE_PDB)
			{
				// mark this path as invalid to avoid loading it the next time.
				CAutoLock lock(&m_AccessLock);
				_PathEntry& PathEntry = m_aPathIndex[sPdbFileName];
				PathEntry.m_Type = bSymStore?ENTRY_UNLOADED:ENTRY_INVALID;
				PathEntry.m_nEntryId = -1;
			}
		}
	}

	// Return true to indicate success; otherwise false.
    return Status==TPS_SUCCESS?true:false;
}

bool CPdbCache::ReleaseCacheEntry(int nEntry)
{
	CAutoLock lock(&m_AccessLock);


	// Find an entry with such ID
	std::map<int, _CacheEntry>::iterator e_it = m_aEntries.find(nEntry);
	if(e_it==m_aEntries.end())
	{
		// Possible error - invalid argument
		return false;
	}

	if(e_it->second.m_nRefCount==0)
	{
		return false; // This entry is already released!
	}

	_CacheEntry& Entry = e_it->second;
	Entry.m_nRefCount--; // Decrement ref count

	// Check if ref count is zero
	if(Entry.m_nRefCount==0)
	{
		if(Entry.m_bPendingDelete)
		{
			std::wstring sPath = Entry.m_sPdbPath;

			// We can delete the entry right now
			if(!EraseCacheEntry(nEntry))
			{
				// Entry cannot be erased from cache?
				return false;
			}

#ifdef _WIN32
			// We can delete the file right now
			return DeletePdbFile(sPath);
#else
			// In Linux, we have already deleted the hardlinks to file,
			// so just return now.
			return true;
#endif
		}

		size_t uMemUsageKB = GetMemoryUsage();
		if(uMemUsageKB==0 || uMemUsageKB>m_uMaxMemUsageKB)
		{
			// Reduce memory consumtion
			if(Entry.m_pPdbReader!=NULL)
				Entry.m_pPdbReader->ReduceMemoryConsumption();
		}

		// Insert this entry into unref index
		time_t CurTime;
		time(&CurTime);
		Entry.m_AccessTime = CurTime; // update access time of the entry
	    std::pair<time_t, int> Pair(CurTime, nEntry);
		m_aUnrefIndex.insert(Pair);
	}

	// Ensure cache size is below the limit
	CheckCacheOverflow();

	// OK
	return true;
}

void CPdbCache::CheckCacheOverflow()
{
	// Removes cache entries beyound the allowed limit

    CAutoLock lock(&m_AccessLock);

	if(m_nMaxEntryCount==0)
		return; // No limit

    // Check cache size
    while(m_aUnrefIndex.size()!=0 &&
		  (int)m_aEntries.size()>=m_nMaxEntryCount)
    {
       // Remove the oldest unreferenced entry

        std::multimap<time_t, int>::iterator access_it = m_aUnrefIndex.begin();
        int nEntry = access_it->second;

		std::map<int, _CacheEntry>::iterator e_it = m_aEntries.find(nEntry);
		_CacheEntry& Entry = e_it->second;
        if(Entry.m_pPdbReader!=NULL)
		{
			delete Entry.m_pPdbReader;
			Entry.m_pPdbReader = NULL;
		}
		if(Entry.m_pPeReader!=NULL)
		{
			delete Entry.m_pPeReader;
			Entry.m_pPeReader = NULL;
		}

		std::map<std::wstring, _PathEntry>::iterator path_it = m_aPathIndex.find(Entry.m_sPdbPath);
		path_it->second.m_Type = ENTRY_UNLOADED;
		path_it->second.m_nEntryId = -1;

        m_aGuidIndex.erase(Entry.m_sGUIDnAge);
        m_aUnrefIndex.erase(access_it);
        m_aEntries.erase(e_it);
    }
}

void CPdbCache::CheckCacheIntegrity()
{
	// Should be used in Debug mode only to ensure that
	// the cache is in consistent state.

	CAutoLock lock(&m_AccessLock);

    assert(m_nMaxEntryCount>=0);
    assert(m_aEntries.size()==m_aGuidIndex.size());
    assert(m_aEntries.size()<=m_aPathIndex.size());

	std::map<int, _CacheEntry>::iterator e_it;
	for(e_it=m_aEntries.begin(); e_it!=m_aEntries.end(); e_it++)
	{
		_CacheEntry& Entry = e_it->second;

		if(Entry.m_nRefCount==0)
		{
			assert(Entry.m_pPdbReader!=NULL);

			// Ensure this entry is listed in unref list
			std::multimap<time_t, int>::iterator access_it = m_aUnrefIndex.find(Entry.m_AccessTime);
			while(access_it!=m_aUnrefIndex.end() &&
				access_it->second!=e_it->first)
			{
				access_it++;
			}
			assert(access_it!=m_aUnrefIndex.end());
			assert(access_it->first==Entry.m_AccessTime);
			assert(access_it->second==e_it->first);
		}
	}
}

void CPdbCache::Clear()
{
	// Frees all cache entries.
	// After that, all open handles to cache entries become invalid.

    CAutoLock lock(&m_AccessLock);

	std::map<int, _CacheEntry>::iterator e_it;
	for(e_it=m_aEntries.begin(); e_it!=m_aEntries.end(); e_it++)
    {
		_CacheEntry& Entry = e_it->second;

		if(Entry.m_pPdbReader!=NULL)
			delete Entry.m_pPdbReader;

		if(Entry.m_pPeReader!=NULL)
			delete Entry.m_pPeReader;
    }

    m_aEntries.clear();
    m_aGuidIndex.clear();
    m_aPathIndex.clear();
    m_aUnrefIndex.clear();
}

int CPdbCache::GetUniqueEntryId()
{
	// Generate an unique ID for cache entry

	int nId = -1;
	for(;;)
	{
		// Generate new entry ID
		nId = ++m_nEntryIdSeed;

		// Ensure that entry with such an ID does not exist
		if(m_aEntries.find(nId)==m_aEntries.end())
			break;
	}

	// Return generated ID
	return nId;
}

bool CPdbCache::GetCacheStat(PdbCacheStat* pStat)
{
	if(pStat==NULL)
		return false; // invalid arg

	CAutoLock lock(&m_AccessLock);

	pStat->m_nEntryCount = (int)m_aEntries.size();
	pStat->m_nUnrefCount = (int)m_aUnrefIndex.size();

	return true;
}

bool CPdbCache::DeleteCachedFile(std::wstring sPath, bool bDeleteFile)
{
	FixSlashesInFilePath(sPath);

	CAutoLock lock(&m_AccessLock);
	bool bStatus = false;
	int nEntry = -1;
	std::multimap<time_t, int>::iterator unref_it;

    std::map<std::wstring, _PathEntry>::iterator path_it = m_aPathIndex.find(sPath);
    if(path_it==m_aPathIndex.end())
	{
		// Not found such a file in cache.

		if(bDeleteFile)
		{
			// We can delete PDB file right now
			DeletePdbFile(sPath);
		}

		goto cleanup;
	}

    // Such a PDB file found in cache
	nEntry = path_it->second.m_nEntryId;

	// Mark entry as pending delete. It will be deleted when
	// ref count becomes zero.
	m_aEntries[nEntry].m_bPendingDelete = true;

#ifndef _WIN32
		// In Linux, we can delete the file right now (its inode will still be available
		// until the file is unlocked, but hard links will be unset).
		DeletePdbFile(sPath);
#endif

	// Check if entry is unreferenced
	if(m_aEntries[nEntry].m_nRefCount==0)
	{
		// We can delete the entry from cache right now
		EraseCacheEntry(nEntry);

#ifdef _DEBUG
		CheckCacheIntegrity();
#endif

#ifdef _WIN32
		// In Windows, can delete the file now since it is now unlocked.
		DeletePdbFile(sPath);
#endif
	}

	// Success
    bStatus = true;

cleanup:

	return bStatus;
}

bool CPdbCache::EraseCacheEntry(int nEntry)
{
	// Find an entry with such an ID
	std::map<int, _CacheEntry>::iterator e_it = m_aEntries.find(nEntry);
	if(e_it==m_aEntries.end())
		return false; // Not found

	_CacheEntry& Entry = e_it->second;

	// Delete PDB reader
	if(Entry.m_pPdbReader!=NULL)
	{
		delete Entry.m_pPdbReader;
		Entry.m_pPdbReader = NULL;
	}

	// Delete PE reader
	if(Entry.m_pPeReader!=NULL)
	{
		delete Entry.m_pPeReader;
		Entry.m_pPeReader = NULL;
	}

	// If entry was in path index...
	std::map<std::wstring, _PathEntry>::iterator path_it = m_aPathIndex.find(Entry.m_sPdbPath);
	if(path_it->second.m_Type == ENTRY_LOADING)
		return true; // This entry is currently being loaded, we cannot touch it

	// ... mark the entry as unloaded
	path_it->second.m_Type = ENTRY_UNLOADED;
	path_it->second.m_nEntryId = -1;

	// If entry was in unref index, delete it from that index
	std::multimap<time_t, int>::iterator access_it = m_aUnrefIndex.find(Entry.m_AccessTime);
	while(access_it!=m_aUnrefIndex.end() && access_it->second!=nEntry) access_it++;
	if(access_it!=m_aUnrefIndex.end())
		m_aUnrefIndex.erase(access_it);

	// Delete entry from GUID index
    m_aGuidIndex.erase(Entry.m_sGUIDnAge);

	// Erase entry from entry list
    m_aEntries.erase(e_it);

	return true;
}

bool CPdbCache::DeletePdbFile(std::wstring sPath)
{
	FixSlashesInFilePath(sPath);

	// First check if file is located inside of a search directory
	bool bMatch = false;
	std::map<std::wstring, _SearchDirInfo>::iterator it;
	for(it=m_aSearchDirs.begin(); it!=m_aSearchDirs.end(); it++)
	{
		_SearchDirInfo& info = it->second;
		int pos = sPath.find(info.m_sPath);
		if(pos==0)
		{
			// The file is located in this search path
			bMatch = true;
			break;
		}
	}
	if(!bMatch)
		return false; // File is not in PDB cache search path

	// Remove the file itself
	int nRes = RemoveFile(sPath);
	if(nRes!=0)
		return false; // Error deleting file

	std::wstring sDir;
    std::wstring sFile;
    std::wstring sBaseFileName;
    std::wstring sExtension;
	std::wstring sSubDirName;
	SplitFileName(sPath, sDir, sFile, sBaseFileName, sExtension);
	SplitFileName(sDir, sSubDirName, sFile, sBaseFileName, sExtension);

	// Remove the containing folder (if empty)
	RmDir(sDir, true);

	// Remove the outer containing folder (if empty)
	RmDir(sSubDirName, true);

	return true;
}
