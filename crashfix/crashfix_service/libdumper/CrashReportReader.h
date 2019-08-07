//! \file CrashReportReader.h
//! \brief Reads a crash report ZIP archive.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "unzip.h"
#include "CrashDescReader.h"
#include "MiniDumpReader.h"

//! \class CCrashReportReader
//! \brief Reads a crash report file.
class CCrashReportReader
{
public:

    //! Constructor
    CCrashReportReader();

    //! Destructor
    virtual ~CCrashReportReader();

    //! Opens crash report file.
    //! @param[in] sFileName Path to crash report file.
    BOOL Init(std::wstring sFileName);

    //! Determines if file is open or not
    BOOL IsInitialized();

    //! Destroys this object
    void Destroy();

	//! Gets MD5 hash of the file.
	std::wstring GetMD5Hash();

	//! Returns file size.
	DWORD GetFileSize();

    //! Extracts a file from error report
    BOOL ExtractFile(LPCWSTR lpszFileName, LPCWSTR lpszFileSaveAs, BOOL bOverwriteExisting);

    //! Returns pointer to crash description XML reader.
    CCrashDescReader* GetCrashDescReader();

    //! Returns pointer to minidump reader.
    CMiniDumpReader* GetMiniDumpReader();
		
	//! Returns the last error message.
	std::wstring GetErrorMsg();

private:

    //! Unzips a file from ZIP archive.
    int UnzipFile(unzFile hZip, const char* szFileName, const wchar_t* szOutFileName);

    //! Generates a unique temp file name.
    std::wstring GetTempFileName();

    //! Sets error message
    void SetErrorMsg(LPCWSTR pszErrorMsg);

	std::wstring m_sFileName;             //!< ZIP file name.
    unzFile m_hZip;                       //!< Handle to the ZIP archive
    CCrashDescReader* m_pDescReader;      //!< Pointer to the crash description reader object
    CMiniDumpReader* m_pDmpReader;        //!< Pointer to the minidump reader object
    std::wstring m_sMiniDumpTempName;     //!< The name of the tmp file to store extracted minidump in
    std::vector<std::wstring> m_ContainedFiles; // The list of files contained in the report
    std::wstring m_sErrorMsg;             //!< Last error message
};

