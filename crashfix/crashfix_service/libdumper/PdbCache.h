//! \file PdbCache.h
//! \brief PDB file cache.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "PdbReader.h"
#include "PeReader.h"
#include "CritSec.h"

//! Modes for PDB search
enum ePdbDirSearchMode
{
	PDB_USUAL_DIR    = 0x1, //!< Search by trying PDB files in turn.
	PDB_SYMBOL_STORE = 0x2  //!< Assume the search dir is organized as symbol store.
};

//! PDB cache statistics.
struct PdbCacheStat
{	
	int m_nEntryCount;    //!< Current total number of entries.
	int m_nUnrefCount;    //!< Count of unreferenced entries.
};

//! \class CPdbCache
//! \brief Shared storage for PDB files.
class CPdbCache
{
public:

    //! Constructor.	
    CPdbCache();

	//! Destructor.
    virtual ~CPdbCache();

    //! Sets maximum size of cache
	//! @param[in] nMaxEntryCount Maximum entry count in the cache.
	//! @return true on success; otherwise false.
    bool SetMaxEntryCount(int nMaxEntryCount);

	//! Returns maximum entry count in cache.
	int GetMaxEntryCount();

	//! Sets maximum memory usage in KB.
	bool SetMaxMemUsage(size_t uMaxMemUsageKB);
	
	//! Returns current memory usage in KB.
	size_t GetMaxMemUsageKB();
	
    //! Adds a PDB search directory to the search list.
	//! @param[in] sPathToSearchDir Path to the directory.
	//! @param[in] SearchMode Search mode.
	//! @param[in] bSearchRecursively Whether to search recursively or not.
	//! @return true on success; otherwise false.
    bool AddPdbSearchDir(
		std::wstring sPathToSearchDir, 
		ePdbDirSearchMode SearchMode,
		bool bSearchRecursively);

    //! Searches for matching PDB file.
    //! @param[in] sGUIDnAge GUID of the PDB file concatenated with Age. Optional.	
    //! @param[in] sPdbFileName PDB file name hint. Optional.
    //! @param[in] sImagePath Path to the executable module.
	//! @param[out] ppPdbReader On output, receives a pointer to a newly created CPdbReader object.
	//! @param[out] ppPeReader On output, receives a pointer to a newly created PeReader object.
	//! @param[out] pHandle On output, receives handle to the cache entry.
	//! @param[out] psErrorMsg On output, receives error message. Optional.
	//! @param[in] bExactMatchBuildAge Optional. If to require the exact match of build age (default is true).
    bool FindPdb(
		std::wstring sGUIDnAge, 
		std::wstring sPdbFileName, 
		std::wstring sImageFileName, 
        CPdbReader** ppPdbReader, 
		CPeReader** ppPeReader,
		int* pnHandle,
		std::string* psErrorMsg = NULL,
		bool bExactMathBuildAge = TRUE
		);

	//! Releases the cache entry previously locked with FindPdb() method call.
	//! @param[in] nEntry Handle to the cache entry to release
	//! @return true if entry has been released; false if entry not found.
	bool ReleaseCacheEntry(int nEntry);

	// Returns cash statistics
	bool GetCacheStat(PdbCacheStat* pStat);

    //! Removes all cache entries (clears the cache). 
	//! All handles become invalid after this operation.
    void Clear();

	//! Removes an entry from cache (if the entry exists in cache) and optionally deletes the corresponding file on disk.
	//! @param[in] sPath Absolute path to the PDB file. Required.	    
	//! @param[in] bDeleteFile Optional. If specified, the corresponding disk file is deleted permanently.
	//! @return true if entry has been found and deleted; returns false if entry not found in cache.
	bool DeleteCachedFile(std::wstring sPath, bool bDeleteFile);

private:

	enum eFindExistingEntryResult
	{
		FEE_NOT_FOUND      = 0, // Entry not found in cache.
		FEE_FOUND          = 1, // Entry has been found.
		FEE_PENDING_DELETE = 2  // Entry exists in cache, but marked for deletion.
	};

	//! Checks if an entry with given GUID is already cached, and if so, returns it.
	CPdbCache::eFindExistingEntryResult FindExistingEntry(
		std::wstring sGUIDnAge, 
		std::wstring sPdbFileName, 
		std::wstring sImageFileName, 
        CPdbReader** ppPdbReader, 
		CPeReader** ppPeReader,
		int* pnHandle,
		bool bExactMatchBuildAge
		);

    //! Loads PDB file, checks its GUID and if matches, adds to cache.
	//! @return Returns true if matching PDB and/or image have been found.
	//! @param[in] sPdbFileName PDB file name.
	//! @param[in] sPeFileName Image file name.
	//! @param[in] sGUIDnAge File GUID concatenated with Age.
	//! @param[out] ppPdbReader On output, receives a pointer to a newly created CPdbReader object.
	//! @param[out] ppPeReader On output, receives a pointer to a newly created PeReader object.
	//! @param[in] If true, the cache entry is marked belonging to symbol store.
    bool TryPdbFile(
		std::wstring sPdbFileName, 
		std::wstring sPeFileName, 
        std::wstring sGUIDnAge, 
		CPdbReader** ppPdbReader, 
		CPeReader** ppPeReader,
		int* pnEntry,
		bool bSymStore,
		std::string& sErrorMsg,
		bool bExactMatchBuildAge);

    //! Recursively searches for a matching PDB in a directory.
	//! @return Returns true if matching PDB and/or image have been found.
	//! @param[in] sSearchDir Path to search directory.
	//! @param[in] bRecursive Whether to search in subdirectories.
	//! @param[in] sPdbName PDB file name.
	//! @param[in] sPeName Image file name.
	//! @param[out] ppPdbReader On output, receives a pointer to a newly created CPdbReader object.
	//! @param[out] ppPeReader On output, receivew a pointer to a newly created PeReader object.
    bool SearchDirectory(
		std::wstring sSearchDir,
		ePdbDirSearchMode SearchMode,
		bool bRecursive,
        std::wstring sPdbName, 
		std::wstring sPeName, 
		std::wstring sGUID, 
        CPdbReader** ppPdbReader, 
		CPeReader** ppPeReader,
		int* pnEntry, 
		std::string& sErrorMsg,
		bool bExactMatchBuildAge);

    //! Searches for a PDB and/or image file in symbol storage directory.
	//! @return Returns true if matching PDB and/or image have been found.
	//! @param[in] sSearchDir Symbol storage directory name.
	//! @param[in] sPdbName PDB file name.
	//! @param[in] sPeName Image name.
	//! @param[out] ppPdbReader On output, receives a pointer to a newly created CPdbReader object.
	//! @param[out] ppPeReader On output, receivew a pointer to a newly created PeReader object.
    bool SearchSymStorageDir(
		std::wstring sSearchDir, 		
		std::wstring sPdbName, 
        std::wstring sPeName, 
		std::wstring sGUID, 
        CPdbReader** ppPdbReader, 
		CPeReader** ppPeReader,
		int* pnEntry, 
		std::string& sErrorMsg,
		bool bExactMatchBuildAge);

    //! Checks if cache maximum size reached and removes the oldest entries beyond the limit.
    void CheckCacheOverflow();

    // Validates cache integrity (used only for debug).
    void CheckCacheIntegrity();

	//! Creates new unique entry ID.
	int GetUniqueEntryId();

	//! Erases an entry from cache.
	bool EraseCacheEntry(int nEntry);

	//! Removes a PDB file from disk. Also removes a parent dir of PDB file if it is empty.
	//! @param[in] sPath Path to PDB file stored in cache.
	bool DeletePdbFile(std::wstring sPath);

    //! Search dir info.
    struct _SearchDirInfo
    {
        std::wstring m_sPath;            //!< Path to search dir
        bool m_bSearchRecursively;       //!< Search recursively?
		ePdbDirSearchMode m_SearchMode;  //!< How to search.
    };
		
	enum ePathEntryType
	{
		ENTRY_INVALID  =  0, // This path points to an invalid PDB file 		
		ENTRY_LOADING  =  1, // This PDB file is currently being loaded 
		ENTRY_LOADED   =  2, // This PDB file is valid and currently presents in cache
		ENTRY_UNLOADED =  3  // This PDB file is valid, but was unloaded from cache
	};

	struct _PathEntry
	{
		_PathEntry()
		{
			m_Type = ENTRY_INVALID;
			m_nLoadRefCount = 0;
			m_nEntryId = -1;			
		}

		ePathEntryType m_Type;  // Type
		int m_nLoadRefCount;    // Count of threads waiting for this entry to be loaded.
		int m_nEntryId;         // ID of the cache entry corresponding to this path (for loaded entries only).		
	};

    //! Cache entry info
    struct _CacheEntry
    {
        _CacheEntry()
        {			
            m_pPdbReader = NULL;
            m_pPeReader = NULL;
            m_AccessTime = 0;
			m_nRefCount = 0;
			m_bSymStore = false;
			m_bPendingDelete = false;
        }
				
        CPdbReader* m_pPdbReader;  //!< PDB reader
        CPeReader* m_pPeReader;    //!< PE reader.
        std::wstring m_sPdbPath;   //!< Path to PDB file
        std::wstring m_sPePath;    //!< Path to PE file
        std::wstring m_sGUIDnAge;  //!< GUID+Age.
        time_t m_AccessTime;       //!< Access time
		int m_nRefCount;           //!< Reference count.
		bool m_bSymStore;          //!< If this entry belongs to symbol store dir.
		bool m_bPendingDelete;     //!< If PDB/EXE file(s) corresponding to this entry should be deleted on entry release.
    };

	// Variables used internally.
    CCritSec m_AccessLock;                     //!< Sync object
    int m_nMaxEntryCount;                      //!< Maximum size of cache
	size_t m_uMaxMemUsageKB;                   //!< Maximum memory usage.
    std::map<std::wstring, _SearchDirInfo> m_aSearchDirs; //!< The list of search directories
    std::map<int, _CacheEntry> m_aEntries;     //!< The list of entries - pairs [entry_id, _CacheEntry]
	int m_nEntryIdSeed;                        //!< Entry ID seed
    std::map<std::wstring, int> m_aGuidIndex;  //!< Pairs [guid+age, entry_id]
    std::map<std::wstring, _PathEntry> m_aPathIndex;  //!< Pairs [path, _PathEntry]
    std::multimap<time_t, int> m_aUnrefIndex;  //!< Unreferenced entries index - pairs [access_time, entry_id]
};

