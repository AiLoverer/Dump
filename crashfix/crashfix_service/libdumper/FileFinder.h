//! \file FileFinder.h
//! \brief File finder class.
//! \author Oleg Krivtsov
//! \date May 2012

//! Contains information on a found file.
class CFindFileInfo
{
public:
	std::wstring m_sFileName;    //!< Full path to file.
	std::wstring m_sRelFileName; //!< File name without path.
	bool m_bIsRegular;           //!< Regular file
	bool m_bIsDirectory;         //!< Directory

};

//! \class CFileFinder
//! \brief A helper class for enumerating files in a directory.
class CFileFinder
{
public:

	//! Constructor
	CFileFinder();

	//! Destructor.
	~CFileFinder();

	//! Locates the first file.
	//! @return true on success; otherwise false.
	//! @param[in] sSearchPattern File search pattern.
	//! @param[out] pFileInfo On output, receives file information.
	bool FindFirstFile(std::wstring sSearchPattern, CFindFileInfo* pFileInfo);

	//! Locates the next file.
	//! @return true on success; otherwise false.
	//! @param[out] pFileInfo On output, receives file information.
	bool FindNextFile(CFindFileInfo* pFileInfo);

private:

	std::wstring m_sSearchPattern; //!< Search pattern.
	std::wstring m_sDirectory; //!< Search directory name.
#ifdef _WIN32
	HANDLE m_hFind;
	WIN32_FIND_DATAW m_ffd;
#else
    DIR* m_dirp;
#endif

};
