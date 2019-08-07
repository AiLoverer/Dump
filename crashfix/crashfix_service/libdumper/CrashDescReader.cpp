#include "stdafx.h"
#include "CrashDescReader.h"
#include "tinyxml.h"
#include "strconv.h"

// Exception types
#define CR_WIN32_STRUCTURED_EXCEPTION   0    //!< SEH exception (deprecated name, use \ref CR_SEH_EXCEPTION instead).
#define CR_SEH_EXCEPTION                0    //!< SEH exception.
#define CR_CPP_TERMINATE_CALL           1    //!< C++ terminate() call.
#define CR_CPP_UNEXPECTED_CALL          2    //!< C++ unexpected() call.
#define CR_CPP_PURE_CALL                3    //!< C++ pure virtual function call (VS .NET and later).
#define CR_CPP_NEW_OPERATOR_ERROR       4    //!< C++ new operator fault (VS .NET and later).
#define CR_CPP_SECURITY_ERROR           5    //!< Buffer overrun error (VS .NET only).
#define CR_CPP_INVALID_PARAMETER        6    //!< Invalid parameter exception (VS 2005 and later).
#define CR_CPP_SIGABRT                  7    //!< C++ SIGABRT signal (abort).
#define CR_CPP_SIGFPE                   8    //!< C++ SIGFPE signal (flotating point exception).
#define CR_CPP_SIGILL                   9    //!< C++ SIGILL signal (illegal instruction).
#define CR_CPP_SIGINT                   10   //!< C++ SIGINT signal (CTRL+C).
#define CR_CPP_SIGSEGV                  11   //!< C++ SIGSEGV signal (invalid storage access).
#define CR_CPP_SIGTERM                  12   //!< C++ SIGTERM signal (termination request).

CCrashDescReader::CCrashDescReader()
{
    m_bInitialized = false;
    m_dwExceptionType = 0;
    m_dwFPESubcode = 0;
    m_dwExceptionCode = 0;
    m_dwInvParamLine = 0;
}

CCrashDescReader::~CCrashDescReader()
{
}

bool CCrashDescReader::Init(std::wstring sFileName)
{
    TiXmlDocument doc;
    FILE* f = NULL;

    if(m_bInitialized)
        Destroy();
    
    // Check that the file exists
#ifdef _WIN32
    _tfopen_s(&f, sFileName.c_str(), L"rb");
#else
    std::string sUtf8FileName = strconv::w2a(sFileName);
    f = fopen(sUtf8FileName.c_str(), "rb");
#endif

    if(f==NULL)
        return false; // File can't be opened

    // Open XML document
    bool bLoaded = doc.LoadFile(f);
    if(!bLoaded)
    {
        fclose(f);
        return false; // XML is corrupted
    }

    TiXmlHandle hDoc(&doc);

    TiXmlHandle hRoot = hDoc.FirstChild("CrashRpt").ToElement();
    if(hRoot.ToElement()==NULL)
    {
        if(LoadXmlv10(hDoc)==0)
        {
            fclose(f);
            return true;
        }

        return false; // Invalid XML structure
    }

    // Get generator version

    const char* szCrashRptVersion = hRoot.ToElement()->Attribute("version");
    if(szCrashRptVersion!=NULL)
    {
        m_dwGeneratorVersion = atoi(szCrashRptVersion);
    }

    // Get CrashGUID
    TiXmlHandle hCrashGUID = hRoot.ToElement()->FirstChild("CrashGUID");
    if(hCrashGUID.ToElement())
    {
        TiXmlText* pTextElem = hCrashGUID.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sCrashGUID = strconv::a2w(text);
        }
    }

    // Get AppName
    TiXmlHandle hAppName = hRoot.ToElement()->FirstChild("AppName");
    if(hAppName.ToElement())
    {
        TiXmlText* pTextElem = hAppName.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sAppName = strconv::a2w(text);
        }
    }

    // Get AppVersion
    TiXmlHandle hAppVersion = hRoot.ToElement()->FirstChild("AppVersion");
    if(hAppVersion.ToElement())
    {
        TiXmlText* pTextElem = hAppVersion.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sAppVersion = strconv::a2w(text);
        }
    }

    // Get ImageName
    TiXmlHandle hImageName = hRoot.ToElement()->FirstChild("ImageName");
    if(hImageName.ToElement())
    {
        TiXmlText* pTextElem = hImageName.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sImageName = strconv::a2w(text);
        }
    }

    // Get OperatingSystem
    TiXmlHandle hOperatingSystem = hRoot.ToElement()->FirstChild("OperatingSystem");
    if(hOperatingSystem.ToElement())
    {
        TiXmlText* pTextElem = hOperatingSystem.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sOperatingSystem = strconv::a2w(text);
        }
    }

    // Get GeoLocation
    TiXmlHandle hGeoLocation = hRoot.ToElement()->FirstChild("GeoLocation");
    if(hGeoLocation.ToElement())
    {
        TiXmlText* pTextElem = hGeoLocation.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sGeoLocation = strconv::a2w(text);
        }
    }

    // Get OSIs64Bit
    m_bOSIs64Bit = FALSE;
    TiXmlHandle hOSIs64Bit = hRoot.ToElement()->FirstChild("OSIs64Bit");
    if(hOSIs64Bit.ToElement())
    {
        TiXmlText* pTextElem = hOSIs64Bit.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_bOSIs64Bit = atoi(text);
        }
    }

    // Get SystemTimeUTC
    TiXmlHandle hSystemTimeUTC = hRoot.ToElement()->FirstChild("SystemTimeUTC");
    if(hSystemTimeUTC.ToElement())
    {
        TiXmlText* pTextElem = hSystemTimeUTC.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sSystemTimeUTC = strconv::a2w(text);
        }
    }

    // Get ExceptionType
    TiXmlHandle hExceptionType = hRoot.ToElement()->FirstChild("ExceptionType");
    if(hExceptionType.ToElement())
    {
        TiXmlText* pTextElem = hExceptionType.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_dwExceptionType = atoi(text);
        }
    }

    // Get UserEmail
    TiXmlHandle hUserEmail = hRoot.ToElement()->FirstChild("UserEmail");
    if(hUserEmail.ToElement())
    {
        TiXmlText* pTextElem = hUserEmail.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sUserEmail = strconv::a2w(text);
        }
    }

    // Get ProblemDecription
    TiXmlHandle hProblemDescription = hRoot.ToElement()->FirstChild("ProblemDescription");
    if(hProblemDescription.ToElement())
    {
        TiXmlText* pTextElem = hProblemDescription.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sProblemDescription = strconv::a2w(text);
        }
    }

    // Get ExceptionCode (for SEH exceptions only)
    if(m_dwExceptionType==CR_SEH_EXCEPTION)
    {
        TiXmlHandle hExceptionCode = hRoot.ToElement()->FirstChild("ExceptionCode");
        if(hExceptionCode.ToElement())
        {
            TiXmlText* pTextElem = hExceptionCode.FirstChild().Text();
            if(pTextElem)
            {
                const char* text = pTextElem->Value();
                if(text)
                    m_dwExceptionCode = atoi(text);
            }
        }
    }

    // Get FPESubcode (for FPE exceptions only)
    if(m_dwExceptionType==CR_CPP_SIGFPE)
    {
        TiXmlHandle hFPESubcode = hRoot.ToElement()->FirstChild("FPESubcode");
        if(hFPESubcode.ToElement())
        {
            TiXmlText* pTextElem = hFPESubcode.FirstChild().Text();
            if(pTextElem)
            {
                const char* text = pTextElem->Value();
                if(text)
                    m_dwFPESubcode = atoi(text);
            }
        }
    }

    // Get InvParamExpression, InvParamFunction, InvParamFile, InvParamLine
    // (for invalid parameter exceptions only)
    if(m_dwExceptionType==CR_CPP_INVALID_PARAMETER)
    {
        TiXmlHandle hInvParamExpression = hRoot.ToElement()->FirstChild("InvParamExpression");
        if(hInvParamExpression.ToElement())
        {
            TiXmlText* pTextElem = hInvParamExpression.FirstChild().Text();
            if(pTextElem)
            {
                const char* text = pTextElem->Value();
                if(text)
                    m_sInvParamExpression = strconv::a2w(text);
            }
        }

        TiXmlHandle hInvParamFunction = hRoot.ToElement()->FirstChild("InvParamFunction");
        if(hInvParamFunction.ToElement())
        {
            TiXmlText* pTextElem = hInvParamFunction.FirstChild().Text();
            if(pTextElem)
            {
                const char* text = pTextElem->Value();
                if(text)
                    m_sInvParamFunction = strconv::a2w(text);
            }
        }

        TiXmlHandle hInvParamFile = hRoot.ToElement()->FirstChild("InvParamFile");
        if(hInvParamFile.ToElement())
        {
            TiXmlText* pTextElem = hInvParamFile.FirstChild().Text();
            if(pTextElem)
            {
                const char* text = pTextElem->Value();
                if(text)
                    m_sInvParamFile = strconv::a2w(text);
            }
        }

        TiXmlHandle hInvParamLine = hRoot.ToElement()->FirstChild("InvParamLine");
        if(hInvParamLine.ToElement())
        {
            TiXmlText* pTextElem = hInvParamLine.FirstChild().Text();
            if(pTextElem)
            {
                const char* text = pTextElem->Value();
                if(text)
                    m_dwInvParamLine = atoi(text);
            }
        }
    }

    // Get GUI resource count
    TiXmlHandle hGUIResourceCount = hRoot.ToElement()->FirstChild("GUIResourceCount");
    if(hGUIResourceCount.ToElement())
    {
        TiXmlText* pTextElem = hGUIResourceCount.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sGUIResourceCount = strconv::a2w(text);
        }
    }

    // Get open handle count
    TiXmlHandle hOpenHandleCount = hRoot.ToElement()->FirstChild("OpenHandleCount");
    if(hOpenHandleCount.ToElement())
    {
        TiXmlText* pTextElem = hOpenHandleCount.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sOpenHandleCount = strconv::a2w(text);
        }
    }

    // Get memory usage in KB
    TiXmlHandle hMemoryUsageKbytes = hRoot.ToElement()->FirstChild("MemoryUsageKbytes");
    if(hMemoryUsageKbytes.ToElement())
    {
        TiXmlText* pTextElem = hMemoryUsageKbytes.FirstChild().Text();
        if(pTextElem)
        {
            const char* text = pTextElem->Value();
            if(text)
                m_sMemoryUsageKbytes = strconv::a2w(text);
        }
    }

    // Get file items list
    TiXmlHandle hFileList = hRoot.ToElement()->FirstChild("FileList");
    if(!hFileList.ToElement())
    {
        // This may work for reports generated by v1.2.1
        hFileList = hRoot.ToElement()->FirstChild("FileItems");
    }
    if(hFileList.ToElement())
    {
        TiXmlHandle hFileItem = hFileList.ToElement()->FirstChild("FileItem");
        while(hFileItem.ToElement())
        {
            const char* szFileName = hFileItem.ToElement()->Attribute("name");
            const char* szFileDescription = hFileItem.ToElement()->Attribute("description");

            std::wstring sFileName, sFileDescription;
            if(szFileName!=NULL)
                sFileName = strconv::a2w(szFileName);
            if(szFileName!=NULL)
                sFileDescription = strconv::a2w(szFileDescription);

            FileItem fi;
            fi.m_sFileName = sFileName;
            fi.m_sFileDesc = sFileDescription;

            m_aFileItems.push_back(fi);

            hFileItem = hFileItem.ToElement()->NextSibling();
        }
    }

    // Get custom property list
    TiXmlHandle hCustomProps = hRoot.ToElement()->FirstChild("CustomProps");
    if(hCustomProps.ToElement())
    {
        TiXmlHandle hProp = hCustomProps.ToElement()->FirstChild("Prop");
        while(hProp.ToElement())
        {
            const char* szName = hProp.ToElement()->Attribute("name");
            const char* szValue = hProp.ToElement()->Attribute("value");

            std::wstring sName, sValue;
            if(szName!=NULL)
                sName = strconv::a2w(szName);
            if(szValue!=NULL)
                sValue = strconv::a2w(szValue);

            CustomProp cp;
            cp.m_sPropName = sName;
            cp.m_sPropVal = sValue;

            m_aCustomProps.push_back(cp);

            hProp = hProp.ToElement()->NextSibling();
        }
    }

    fclose(f);

    // OK
    m_bInitialized = true;
    return true;
}

void CCrashDescReader::Destroy()
{
    m_bInitialized = false;
    m_dwGeneratorVersion = 0;
    m_sCrashGUID.clear();
    m_sAppName.clear();
    m_sAppVersion.clear();
    m_sImageName.clear();
    m_sOperatingSystem.clear();
    m_bOSIs64Bit = FALSE;
    m_sSystemTimeUTC.clear();
    m_sGeoLocation.clear();
    m_dwExceptionType = 0;
    m_dwExceptionCode = 0;
    m_dwFPESubcode = 0;
    m_sInvParamExpression.clear();
    m_sInvParamFunction.clear();
    m_sInvParamFile.clear();
    m_dwInvParamLine = 0;
    m_sUserEmail.clear();
    m_sProblemDescription.clear();
    m_sMemoryUsageKbytes.clear();
    m_sGUIResourceCount.clear();
    m_sOpenHandleCount.clear();
    m_aFileItems.clear();
    m_aCustomProps.clear();
}

int CCrashDescReader::LoadXmlv10(TiXmlHandle hDoc)
{
    TiXmlHandle hRoot = hDoc.FirstChild("Exception").ToElement();
    if(hRoot.ToElement()==NULL)
    {
        return -3; // Invalid XML structure
    }

    // Set CrashRpt version to 1000

    m_dwGeneratorVersion = 1000;

    // Get ExceptionRecord element

    TiXmlHandle hExceptionRecord = hRoot.FirstChild("ExceptionRecord").ToElement();

    if(hExceptionRecord.ToElement()!=NULL)
    {
        const char* szImageName = hRoot.ToElement()->Attribute("ModuleName");
        if(szImageName!=NULL)
        {
            m_sImageName = strconv::a2w(szImageName);

            //m_sAppName = Utility::GetBaseFileName(szImageName);
        }
    }

    // OK
    m_bInitialized = true;
    return 0;
}

bool CCrashDescReader::IsInitialized()
{
    return m_bInitialized;
}

DWORD CCrashDescReader::GetGeneratorVersion()
{
    return m_dwGeneratorVersion;
}

std::wstring CCrashDescReader::GetCrashGUID()
{
    return m_sCrashGUID;
}

std::wstring CCrashDescReader::GetAppName()
{
    return m_sAppName;
}

std::wstring CCrashDescReader::GetAppVersion()
{
    return m_sAppVersion;
}

std::wstring CCrashDescReader::GetImageName()
{
    return m_sImageName;
}

std::wstring CCrashDescReader::GetCreationDate()
{
    return m_sSystemTimeUTC;
}

std::wstring CCrashDescReader::GetOSFriendlyName()
{
    return m_sOperatingSystem;
}

BOOL CCrashDescReader::IsOS64Bit()
{
    return m_bOSIs64Bit;
}

std::wstring CCrashDescReader::GetGeoLocation()
{
    return m_sGeoLocation;
}

std::wstring CCrashDescReader::GetGUIResourceCount()
{
    return m_sGUIResourceCount;
}

std::wstring CCrashDescReader::GetOpenHandleCount()
{
    return m_sOpenHandleCount;
}

std::wstring CCrashDescReader::GetMemoryUsage()
{
    return m_sMemoryUsageKbytes;
}

DWORD CCrashDescReader::GetExceptionType()
{
    return m_dwExceptionType;
}

std::wstring CCrashDescReader::ExceptionTypeToStr(DWORD dwExcType)
{
    // Exception types
    const wchar_t* exctypes[13] =
    {
        L"SEH exception",
        L"terminate call",
        L"unexpected call",
        L"pure virtual call",
        L"new operator fault",
        L"buffer overrun",
        L"invalid parameter",
        L"SIGABRT signal",
        L"SIGFPE signal",
        L"SIGILL signal",
        L"SIGINT signal",
        L"SIGSEGV signal",
        L"SIGTERM signal",
    };

    return std::wstring(exctypes[dwExcType]);
}

std::wstring CCrashDescReader::GetUserEmail()
{
    return m_sUserEmail;
}

std::wstring CCrashDescReader::GetProblemDescription()
{
    return m_sProblemDescription;
}

int CCrashDescReader::GetFileItemCount()
{
    return (int)m_aFileItems.size();
}

FileItem* CCrashDescReader::GetFileItem(int nFileItem)
{
    return &m_aFileItems[nFileItem];
}

int CCrashDescReader::GetCustomPropCount()
{
    return (int)m_aCustomProps.size();
}


CustomProp* CCrashDescReader::GetCustomProp(int nProp)
{
    return &m_aCustomProps[nProp];
}
