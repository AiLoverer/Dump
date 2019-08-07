//! \file CrashDescReader.h
//! \brief Reads a crash description XML file.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include <map>
#include "tinyxml.h"

struct FileItem
{
    std::wstring m_sFileName;
    std::wstring m_sFileDesc;
};

struct CustomProp
{
    std::wstring m_sPropName;
    std::wstring m_sPropVal;
};

//! \class CCrashDescReader
//! \brief Reads a crash description XML file.
class CCrashDescReader
{
public:

    //! Constructor.
    CCrashDescReader();

    //! Destructor.
    virtual ~CCrashDescReader();

    //! Opens a crash description XML file.
    bool Init(std::wstring sFileName);

    //! Returns TRUE if file open
    bool IsInitialized();

    //! Closes the opened file.
    void Destroy();

    //! Returns version of CrashRpt that generated the error report.
    DWORD GetGeneratorVersion();

    //! Returns crash GUID.
    std::wstring GetCrashGUID();

    //! Returns application name.
    std::wstring GetAppName();

    //! Return application version
    std::wstring GetAppVersion();

    //! Returns executable image name.
    std::wstring GetImageName();

    //! Returns the date when this report was created (UTC).
    std::wstring GetCreationDate();

    //! Returns OS name (as it presented in user's registry).
    std::wstring GetOSFriendlyName();

    //! Returns TRUE if operating system is 64-bit
    BOOL IsOS64Bit();

    //! Returns georgraphic location
    std::wstring GetGeoLocation();

    //! Returns count of GUI resources
    std::wstring GetGUIResourceCount();

    //! Returns count of open handles
    std::wstring GetOpenHandleCount();

    //! Returns memory usage (in KB)
    std::wstring GetMemoryUsage();

    //! Returns exception type
    DWORD GetExceptionType();

    //! Converts exception type to string
    static std::wstring ExceptionTypeToStr(DWORD dwExcType);

    //! Returns user's email
    std::wstring GetUserEmail();

    //! Returns problem description string
    std::wstring GetProblemDescription();

    //! Returns count of file items
    int GetFileItemCount();

    //! Returns file item
    FileItem* GetFileItem(int nFileItem);

    //! Returns number of custom props
    int GetCustomPropCount();

    //! Returns custom prop
    CustomProp* GetCustomProp(int nProp);

private:

    //! Loads XML description from CrashRpt v.1.0.0.
    int LoadXmlv10(TiXmlHandle hDoc);

    bool m_bInitialized;        //!< Initialized or not?

    DWORD m_dwGeneratorVersion; //!< CrashRpt version.

    std::wstring m_sCrashGUID;  //!< Crash GUID.
    std::wstring m_sAppName;    //!< Application name.
    std::wstring m_sAppVersion; //!< Application version.
    std::wstring m_sImageName;  //!< Image file name
    std::wstring m_sOperatingSystem; //!< OS name
    BOOL m_bOSIs64Bit;          //!< 32-bit OS or 64-bit os?
    std::wstring m_sSystemTimeUTC; //!< Crash time UTC
    std::wstring m_sGeoLocation; //! Geographic location

    DWORD m_dwExceptionType;    //!< Type of the exception
    DWORD m_dwExceptionCode;    //!< SEH exception code

    DWORD m_dwFPESubcode;       //!< FPE exception subcode

    std::wstring m_sInvParamExpression; //!< Invalid paramteter expression
    std::wstring m_sInvParamFunction;   //!< Invalid param function name
    std::wstring m_sInvParamFile; //!< Invalid param file name
    DWORD m_dwInvParamLine;       //!< Invalid param line number

    std::wstring m_sUserEmail;    //!< User email
    std::wstring m_sProblemDescription; //!< User's problem description

    std::wstring m_sMemoryUsageKbytes; //!< Memory usage in KB
    std::wstring m_sGUIResourceCount;  //!< Count of GUI handles
    std::wstring m_sOpenHandleCount;   //!< Count of open handles

    std::vector<FileItem> m_aFileItems; //!< File list
    std::vector<CustomProp> m_aCustomProps; //!< Custom props list
};

