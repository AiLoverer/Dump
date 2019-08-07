//! \file CrashReportReader.cpp
//! \brief Reads a crash report ZIP archive.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "CrashReportReader.h"
#include "CrashDescReader.h"
#include "MiniDumpReader.h"
#include "Misc.h"
#include "strconv.h"
#include "unzip.h"

CCrashReportReader::CCrashReportReader()
{
    m_hZip = 0;
    m_pDescReader = NULL;
    m_pDmpReader = NULL;
}

CCrashReportReader::~CCrashReportReader()
{
	Destroy();
}

BOOL CCrashReportReader::Init(std::wstring sFileName)
{
    int status = -1;
    int zr = 0;
    int xml_find_res = UNZ_END_OF_LIST_OF_FILE;
    int dmp_find_res = UNZ_END_OF_LIST_OF_FILE;
    char szXmlFileName[1024]="";
    char szDmpFileName[1024]="";
    char szFileName[1024]="";
    std::wstring sCalculatedMD5Hash;
    std::string sAppName;
    unz_file_info_s fi;

    SetErrorMsg(L"Unspecified error.");

	if(IsInitialized())
		Destroy();

	m_sFileName = sFileName;
	    
    // Open ZIP archive
    m_hZip = unzOpen(strconv::w2a(sFileName).c_str());
    if(m_hZip==NULL)
    {
        SetErrorMsg(L"Error opening ZIP archive.");
        goto exit;
    }

    // Look for v1.1 crash description XML
    xml_find_res = unzLocateFile(m_hZip, (const char*)"crashrpt.xml", 1);
    zr = unzGetCurrentFileInfo(m_hZip, &fi, szXmlFileName, 1024, NULL, 0, NULL, 0);

    // Look for v1.1 crash dump
    dmp_find_res = unzLocateFile(m_hZip, (const char*)"crashdump.dmp", 1);
    zr = unzGetCurrentFileInfo(m_hZip, &fi, szDmpFileName, 1024, NULL, 0, NULL, 0);

    // If xml and dmp still not found, assume it is v1.0
    if(xml_find_res!=UNZ_OK && dmp_find_res!=UNZ_OK)
    {
        // Look for .dmp file
        zr = unzGoToFirstFile(m_hZip);
        if(zr==UNZ_OK)
        {
            for(;;)
            {
				unz_file_info fi;
                zr = unzGetCurrentFileInfo(m_hZip, &fi, szDmpFileName, 1024, NULL, 0, NULL, 0);
                if(zr!=UNZ_OK)
                    break;

                std::string sFileName = szDmpFileName;

                // Get file extension
                size_t dot_pos = sFileName.rfind('.');
                std::string sExt;
                if(dot_pos!=sFileName.npos)
                {
                    sAppName = sFileName.substr(0, dot_pos);
                    sExt = sFileName.substr(dot_pos+1);
                }

                if(sExt.compare("dmp")==0)
                {
                    // DMP found
                    dmp_find_res = UNZ_OK;
                    break;
                }

                zr=unzGoToNextFile(m_hZip);
                if(zr!=UNZ_OK)
                    break;
            }
        }

        // Assume the name of XML is the same as DMP
        std::string sXmlName = sAppName + ".xml";
        zr = unzLocateFile(m_hZip, sXmlName.c_str(), 1);
		unz_file_info fi;
        zr = unzGetCurrentFileInfo(m_hZip, &fi, szXmlFileName, 1024, NULL, 0, NULL, 0);
        if(zr==UNZ_OK)
        {
            xml_find_res = UNZ_OK;
        }
    }

    // Check that at least xml found (dmp can be optional)
    if(xml_find_res!=UNZ_OK)
    {
        SetErrorMsg(L"File is not a valid crash report (XML is missing).");
        goto exit; // XML or DMP not found
    }

    // Load crash description data
    if(xml_find_res==UNZ_OK)
    {
        std::wstring sTmpName = GetTempFileName();

        zr = UnzipFile(m_hZip, szXmlFileName, sTmpName.c_str());
        if(zr!=0)
        {
            SetErrorMsg(L"Error extracting ZIP item.");
            // Remove temp file
			int nUnlink = RemoveFile(sTmpName.c_str());
			assert(nUnlink==0);
            goto exit; // Can't unzip ZIP element
        }

		// Create crash desc reader
		m_pDescReader = new CCrashDescReader;
		if(!m_pDescReader)
		{			
			SetErrorMsg(L"Error allocating crash description reader (out of memory).");
            goto exit; 
		}

        bool bResult = m_pDescReader->Init(sTmpName.c_str());
        
		// Remove temp file
		int nUnlink = RemoveFile(sTmpName.c_str());
		assert(nUnlink==0);
        
		if(!bResult)
        {
            SetErrorMsg(L"Crash description file is not a valid XML file.");
            goto exit; // Corrupted XML
        }		
    }

    // Extract minidump file
    if(dmp_find_res==UNZ_OK)
    {
        std::wstring sTmpName = GetTempFileName();

        zr = UnzipFile(m_hZip, szDmpFileName, sTmpName.c_str());
        if(zr!=0)
        {
            // Remove temp file
			int nUnlink = RemoveFile(sTmpName.c_str());
			assert(nUnlink==0);

            SetErrorMsg(L"Error extracting ZIP item.");
            goto exit; // Can't unzip ZIP element
        }

        m_sMiniDumpTempName = sTmpName;

		m_pDmpReader = new CMiniDumpReader;
		if(!m_pDmpReader)
		{
			SetErrorMsg(L"Error allocating minidump reader (out of memory).");
            goto exit; 
		}

        // Open minidump file
        BOOL bReadMiniDump = m_pDmpReader->Init(m_sMiniDumpTempName);
        if(!bReadMiniDump)
        {
            delete m_pDmpReader;
            m_pDmpReader = NULL;
            SetErrorMsg(L"Error opening minidump file.");
            goto exit;
        }
    }

    if(m_pDescReader->GetGeneratorVersion()==1000)
    {
        SetErrorMsg(L"Too old version of crash report.");
    }

    // Enumerate contained files
    zr = unzGoToFirstFile(m_hZip);
    if(zr==UNZ_OK)
    {
        for(;;)
        {
            zr = unzGetCurrentFileInfo(m_hZip,
                &fi, szFileName, 1024, NULL, 0, NULL, 0);
            if(zr!=UNZ_OK)
                break;

            std::wstring sFileName = strconv::a2w(szFileName);
            m_ContainedFiles.push_back(sFileName);

            zr=unzGoToNextFile(m_hZip);
            if(zr!=UNZ_OK)
                break;
        }
    }

    SetErrorMsg(L"Success.");
    status = 0;

exit:

    if(status!=0)
    {
        if(m_pDescReader!=NULL)
        {
            delete m_pDescReader;
            m_pDescReader = NULL;
        }

        if(m_pDmpReader)
        {
            delete m_pDmpReader;
            m_pDmpReader = NULL;
        }

        if(m_hZip!=0)
        {
            unzClose(m_hZip);
            m_hZip = NULL;
        }
    }

    return status==0?TRUE:FALSE;
}

BOOL CCrashReportReader::IsInitialized()
{
    return m_hZip?TRUE:FALSE;
}

void CCrashReportReader::Destroy()
{
	if(m_pDescReader)
	{
		delete m_pDescReader;
		m_pDescReader = NULL;
	}

	if(m_pDmpReader)
	{
		delete m_pDmpReader;
		m_pDmpReader = NULL;
	}

	if(m_hZip)
    {
        unzClose(m_hZip);
        m_hZip = NULL;
    }

	if(!m_sMiniDumpTempName.empty())
	{
		// Remove minidump temp file
		int nUnlink = RemoveFile(m_sMiniDumpTempName.c_str());
		assert(nUnlink==0);
		m_sMiniDumpTempName.clear();
	}
	
	m_ContainedFiles.clear();
	m_sFileName.clear();

    // OK.
    SetErrorMsg(L"Success.");
}

std::wstring CCrashReportReader::GetMD5Hash()
{
	std::wstring sMD5;
	CalcFileMD5Hash(m_sFileName, sMD5);
	return sMD5;
}

int CCrashReportReader::UnzipFile(unzFile hZip, const char* szFileName, const wchar_t* szOutFileName)
{
    int status = -1;
    int zr=0;
    int open_file_res = 0;
    FILE* f = NULL;
    BYTE buff[1024];
    int read_len = 0;
    std::string sUtf8OutFileName = strconv::w2a(szOutFileName);

    zr = unzLocateFile(hZip, szFileName, 1);
    if(zr!=UNZ_OK)
        return -1;

    open_file_res = unzOpenCurrentFile(hZip);
    if(open_file_res!=UNZ_OK)
        goto cleanup;

#if _MSC_VER>=1400
    _tfopen_s(&f, szOutFileName, _T("wb"));
#else
    f = fopen(sUtf8OutFileName.c_str(), "wb");
#endif

    if(f==NULL)
        goto cleanup;

    for(;;)
    {
        read_len = unzReadCurrentFile(hZip, buff, 1024);

        if(read_len<0)
            goto cleanup;

        if(read_len==0)
            break;

        size_t written = fwrite(buff, read_len, 1, f);
        if(written!=1)
            goto cleanup;
    }

    status = 0;

cleanup:

    if(open_file_res==UNZ_OK)
        unzCloseCurrentFile(hZip);

    if(f!=NULL)
        fclose(f);

    return status;
}



BOOL CCrashReportReader::ExtractFile(LPCWSTR lpszFileName, LPCWSTR lpszFileSaveAs, BOOL bOverwriteExisting)
{
    SetErrorMsg(L"Unspecified error.");

    int zr;

    zr = unzLocateFile(m_hZip, strconv::w2a(lpszFileName).c_str(), 1);
    if(zr!=UNZ_OK)
    {
        SetErrorMsg(L"Couldn't find the specified zip item.");
        return FALSE;
    }

    if(!bOverwriteExisting)
    {
#ifdef _WIN32
        // Check if such file already exists
        DWORD dwFileAttrs = GetFileAttributes(lpszFileSaveAs);
        if(dwFileAttrs!=INVALID_FILE_ATTRIBUTES && // such object exists
            dwFileAttrs!=FILE_ATTRIBUTE_DIRECTORY)  // and it is not a directory
        {
            SetErrorMsg(_T("Such file already exists."));
            return FALSE;
        }
#else

#endif
    }

    zr = UnzipFile(m_hZip, strconv::w2a(lpszFileName).c_str(), lpszFileSaveAs);
    if(zr!=UNZ_OK)
    {
        SetErrorMsg(L"Error extracting the specified zip item.");
        return FALSE;
    }

    SetErrorMsg(L"Success.");
    return TRUE;
}

void CCrashReportReader::SetErrorMsg(LPCWSTR pszErrorMsg)
{
    m_sErrorMsg = pszErrorMsg;
}

std::wstring CCrashReportReader::GetErrorMsg()
{
	return m_sErrorMsg;
}

CCrashDescReader* CCrashReportReader::GetCrashDescReader()
{
    return m_pDescReader;
}

CMiniDumpReader* CCrashReportReader::GetMiniDumpReader()
{
    return m_pDmpReader;
}

std::wstring CCrashReportReader::GetTempFileName()
{
#ifdef _WIN32
    wchar_t szTempDir[MAX_PATH - 14]   = L"";
    wchar_t szTempFile[MAX_PATH]       = L"";

    if (GetTempPath(MAX_PATH - 14, szTempDir))
        ::GetTempFileName(szTempDir, L"dumper", 0, szTempFile);

    return szTempFile;
#else

    char* szTmpName = tmpnam(NULL);
    return std::wstring(strconv::a2w(szTmpName));

#endif
}

DWORD CCrashReportReader::GetFileSize()
{	
#ifdef _WIN32
	struct _stat filestatus;
	if(0!=_wstat(m_sFileName.c_str() , &filestatus ))
		return 0;
#else
	struct stat filestatus;
	if(0!=stat(strconv::w2utf8(m_sFileName).c_str() , &filestatus ))
		return 0;
#endif
	return filestatus.st_size;
}