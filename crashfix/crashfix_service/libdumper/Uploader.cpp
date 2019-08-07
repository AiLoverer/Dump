//! \file Uploader.cpp
//! \brief Incapsulates PDB file upload functionality.
//! \author Oleg Krivtsov
//! \date Feb 2012

#pragma once
#include "stdafx.h"
#include "Uploader.h"
#include "Misc.h"
#include "HttpRequestSender.h"
#include "strconv.h"
#include "PdbReader.h"

#define SECTION_GENERAL   _T("General")
#define ENTRY_UPLOAD_URL  _T("UploadURL")
#define ENTRY_PROJECT_NAME  _T("ProjectName")
#define ENTRY_SEARCH_PATTERN  _T("SearchPattern")
#define ENTRY_RECURSIVE_SEARCH _T("RecursiveSearch")

CUploader* CUploader::m_pInstance = NULL;

CUploader* CUploader::GetInstance()
{
	if(m_pInstance==NULL)
	{
		m_pInstance = new CUploader();
	}

	return m_pInstance;
}

CUploader::CUploader()
{
	m_bRecursiveSearch = false;
	m_bUploadingNow = false;
	m_hWndNotify = NULL;
	m_pfnCallback = NULL;
	m_lpCallbackParam = NULL;
	m_hThread = NULL;
	m_nFileBeingUploaded = -1;

	m_Profile.Initialize(_T("Software\\CrashFix\\Uploader"));

	LoadPersistentSettings();
}

CUploader::~CUploader()
{
	SavePersistentSettings();
}

void CUploader::LoadPersistentSettings()
{
	TCHAR szBuffer[512] = _T("");

	LPCTSTR szUploadURL = m_Profile.GetProfileString(SECTION_GENERAL, ENTRY_UPLOAD_URL, _T("http://127.0.0.1/crashfix/index.php/debugInfo/uploadExternal"), szBuffer, 512);
	m_sUploadURL = strconv::w2a(szUploadURL);

	LPCTSTR szProjectName = m_Profile.GetProfileString(SECTION_GENERAL, ENTRY_PROJECT_NAME, _T("YourProjectName"), szBuffer, 512);
	m_sProjectName = strconv::w2a(szProjectName);

	LPCTSTR szSearchPattern = m_Profile.GetProfileString(SECTION_GENERAL, ENTRY_SEARCH_PATTERN, NULL, szBuffer, 512);
	if(szSearchPattern!=NULL)
		AddSearchPattern(szSearchPattern);

	bool bRecursiveSearch = 0!=m_Profile.GetProfileInt(SECTION_GENERAL, ENTRY_RECURSIVE_SEARCH, 0);
	SetRecursiveSearch(bRecursiveSearch);
}

void CUploader::SavePersistentSettings()
{
	m_Profile.WriteProfileString(SECTION_GENERAL, ENTRY_UPLOAD_URL, strconv::a2w(m_sUploadURL).c_str());
	m_Profile.WriteProfileString(SECTION_GENERAL, ENTRY_PROJECT_NAME, strconv::a2w(m_sProjectName).c_str());

	if(m_asSearchPatterns.size()!=0)
	{
		std::wstring sSearchPattern;
		GetSearchPatternByIndex(0, sSearchPattern);

		m_Profile.WriteProfileString(SECTION_GENERAL, ENTRY_SEARCH_PATTERN, sSearchPattern.c_str());
	}

	m_Profile.WriteProfileInt(SECTION_GENERAL, ENTRY_RECURSIVE_SEARCH, m_bRecursiveSearch);
}

std::string CUploader::GetErrorMsg()
{
	// Return last error message.
	return m_sErrorMsg;
}

std::string CUploader::GetUploadURL()
{
	return m_sUploadURL;
}

void CUploader::SetUploadURL(std::string sURL)
{
	m_sUploadURL = sURL;
}

void CUploader::AddSearchPattern(std::wstring sSearchPattern)
{
	m_asSearchPatterns.insert(sSearchPattern);
}

int CUploader::GetSearchPatternCount()
{
	return (int)m_asSearchPatterns.size();
}

bool CUploader::GetSearchPatternByIndex(int nIndex, std::wstring& sSearchPattern)
{
	sSearchPattern.clear();

	std::set<std::wstring>::iterator it = m_asSearchPatterns.begin();	
	int i = 0;
	while(it!=m_asSearchPatterns.end() && i<nIndex) it++;
		
	if(it==m_asSearchPatterns.end() || i<nIndex)
		return false;

	sSearchPattern = *it;
	return true;
}

void CUploader::DeleteAllSearchPatterns()
{
	m_asSearchPatterns.clear();
}

bool CUploader::IsSearchRecursive()
{
	return m_bRecursiveSearch;
}

void CUploader::SetRecursiveSearch(bool bRecursiveSearch)
{
	m_bRecursiveSearch = bRecursiveSearch;
}

bool CUploader::FillFileList()
{
	bool bResult = false;
	std::set<std::wstring>::iterator it;

	// Delete all file items
	m_aFileItems.clear();
	
	// Ensure we have at least one search pattern
	if(m_asSearchPatterns.size()==0)
	{
		m_sErrorMsg = "There are no search patterns specified.";
		goto cleanup;
	}

	// Search for files using search patterns specified.	
	for(it=m_asSearchPatterns.begin(); it!=m_asSearchPatterns.end(); it++)
	{
		std::wstring sSearchPattern = *it;
		
		if(!SearchFiles(sSearchPattern, 0))
			goto cleanup;
	}

	m_sErrorMsg.clear();
	bResult = true;

cleanup:

	return bResult;
}

bool CUploader::SearchFiles(std::wstring sSearchPattern, int nLevel)
{
	bool bResult = false;
	HANDLE hFind = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ffd;
	std::wstring sSearchDir;
	std::wstring sFileName;
	std::wstring sBaseFileName;
	std::wstring sExtension;
		
	// If the pattern is a directory name, append *.pdb
	DWORD dwAttr = GetFileAttributes(sSearchPattern.c_str());
	if(dwAttr!=0xFFFFFFFF && (dwAttr&FILE_ATTRIBUTE_DIRECTORY)!=0)
	{
		sSearchPattern += L"\\*.pdb";
	}

	// Split search pattern
	SplitFileName(sSearchPattern, sSearchDir, sFileName, sBaseFileName, sExtension);

	// Search for files in current folder
    hFind = FindFirstFileW(sSearchPattern.c_str(), &ffd);
    BOOL bFind = TRUE;
    while(hFind!=INVALID_HANDLE_VALUE && bFind)
    {
		// Skip directories
		if((ffd.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)!=0)
		{
			bFind = FindNextFileW(hFind, &ffd);
			continue;
		}

        std::wstring sFileName;
		if(!sSearchDir.empty())
		{
			sFileName = sSearchDir + L"\\";
		}
		sFileName += std::wstring(ffd.cFileName);
        
		FileItem fi;
		fi.m_sFileName = sFileName;
		fi.m_Status = FUS_PENDING;
		
		LARGE_INTEGER lFileSize;
		HANDLE hFile = CreateFile(sFileName.c_str(), 
            GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, NULL, NULL); 
        if(hFile!=INVALID_HANDLE_VALUE)
		{
			BOOL bGetSize = GetFileSizeEx(hFile, &lFileSize);
			if(bGetSize)
				fi.m_uFileSize = lFileSize.QuadPart;
			CloseHandle(hFile);
		}		

		m_aFileItems.push_back(fi);

        bFind = FindNextFileW(hFind, &ffd);
    }

	FindClose(hFind);

    // Search for files in subfolders
    if(m_bRecursiveSearch)
    {
		std::wstring sSearchPath = sSearchDir;
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
				if(!SearchFiles(sSubDirName, nLevel+1))
					goto cleanup;
			}

			bFind = FindNextFileW(hFind, &ffd);
		}
    }
	
	m_sErrorMsg.clear();
	bResult = true;

cleanup:

	if(hFind)
		FindClose(hFind);

	return bResult;
}

int CUploader::GetFileCount()
{
	return (int) m_aFileItems.size();
}

FileItem* CUploader::GetFileItem(int nIndex)
{
	if(nIndex<0 || nIndex>=(int)m_aFileItems.size())
		return NULL;

	return &m_aFileItems[nIndex];
}

void CUploader::SelectAllFileItems(bool bSelect)
{
	int i;
	for(i=0; i<GetFileCount(); i++)
	{
		FileItem* pfi = GetFileItem(i);
		if(pfi==NULL)
			continue;

		pfi->m_bChecked = bSelect;
	}
}

CRegProfile* CUploader::GetProfile()
{
	return &m_Profile;
}

bool CUploader::StartFileUpload()
{	
	// Set upload flag.
	m_bUploadingNow = true;
	m_Assync.Reset();

	// Reset status to pending
	int i;
	for(i=0; i<GetFileCount(); i++)
	{
		FileItem* pfi = GetFileItem(i);
		if(pfi==NULL || !pfi->m_bChecked)
			continue;

		SetFileStatus(i, FUS_PENDING);
	}

	// Create working thread.
	m_hThread = CreateThread(NULL, 0, UploadThread, this, 0, NULL);

	// Return immediately.
	return true;
}

void CUploader::CancelFileUpload()
{
	m_Assync.Cancel();
}

DWORD WINAPI CUploader::UploadThread(LPVOID lpParam)
{
	CUploader* pUploader = (CUploader*)lpParam;
	// Redirect back 
	pUploader->InternalUploadFiles();

	return 0;
}

void CUploader::InternalUploadFiles()
{
	m_sErrorMsg = "Unspecified error.";
	CHttpRequestSender RequestSender;	
	
	// Reset statistics.
	m_Stats.Reset();

	// Walk through file items
	int i;
	for(i=0; i<GetFileCount(); i++)
	{
		if(m_Assync.IsCancelled())
			goto exit;

		FileItem* pfi = GetFileItem(i);

		if(!pfi->m_bChecked || pfi->m_Status!=FUS_PENDING)
			continue; // Skip item

		m_Stats.m_nTotal++; 

		SetFileStatus(i, FUS_CHECKING);

		// Open PDB file to extract its GUID
		CPdbReader PdbReader;
		BOOL bInit = PdbReader.Init(pfi->m_sFileName);
		if(!bInit)
		{
			m_Stats.m_nIgnored++; 
			SetFileStatus(i, FUS_INVALID);			
			continue; // This is not a valid PDB file
		}

		if(PdbReader.IsAMD64())
		{
			m_Stats.m_nIgnored++; 
			SetFileStatus(i, FUS_INVALID);						
			continue; // This is not a valid PDB file
		}

		CPdbHeadersStream* pHeaders = PdbReader.GetHeadersStream();
		if(pHeaders==NULL)
		{
			m_Stats.m_nIgnored++; 
			SetFileStatus(i, FUS_INVALID);						
			continue; // This is not a valid PDB file
		}

		std::wstring sGUID = pHeaders->GetGUID();
		DWORD dwAge = pHeaders->GetAge();
		std::wstringstream wstream;
		wstream << sGUID;
		wstream << dwAge;
		std::wstring sGUIDnAge = wstream.str();

		// First check if such a file is already in the database
		CHttpRequest HttpRequest;
		HttpRequest.m_sUrl = m_sUploadURL.c_str();		
		HttpRequest.m_aTextFields["action"] = "Check";
		HttpRequest.m_aTextFields["DebugInfo[project_name]"] = m_sProjectName;
		HttpRequest.m_aTextFields["DebugInfo[guid]"] = strconv::w2a(sGUIDnAge);
		HttpRequest.m_aTextFields["DebugInfo[srcpath]"] = strconv::w2a(pfi->m_sFileName);

		// Send request
		BOOL bSend = RequestSender.SendAssync(HttpRequest, &m_Assync);
		if(!bSend)
		{			
			// Set item status to failed
			m_Stats.m_nErrors++; 
			SetFileStatus(i, FUS_FAILED);									
			continue;
		}
		
		// Wait for request completion
		int nStatus = m_Assync.WaitForCompletion();
		if(nStatus!=0)
		{
			m_Stats.m_nIgnored++; 
			SetFileStatus(i, FUS_IGNORED);			
			continue;
		}

		if(m_Assync.IsCancelled())
			goto exit;

		// Now we can upload the file
		HttpRequest.m_aTextFields["action"] = "UploadFile";		
		CHttpRequestFile f;
		f.m_sContentType = "application/octet-stream";
		f.m_sSrcFileName = pfi->m_sFileName.c_str();
		HttpRequest.m_aIncludedFiles["DebugInfo[fileAttachment]"] = f;

		// Send request
		m_Assync.Reset();
		bSend = RequestSender.SendAssync(HttpRequest, &m_Assync);
		if(!bSend)
		{			
			// Set item status failed
			m_Stats.m_nErrors++; 
			SetFileStatus(i, FUS_FAILED);			
		}

		// Set status to uploading
		SetFileStatus(i, FUS_UPLOADING);		

		// Wait for request completion
		if(m_pfnCallback)
		{
			do
			{				
				NotifyFileUploadProgress(i);
				Sleep(100);

				if(m_Assync.IsCancelled())
				{
					m_Stats.m_nErrors++; 
					goto exit;
				}
			}
			while(!m_Assync.IsCompleted());
		}
		else
		{
			nStatus = m_Assync.WaitForCompletion();
		}
		if(nStatus==0)
		{
			m_Stats.m_nUploaded++; 
			SetFileStatus(i, FUS_UPLOADED);			
		}
		else
		{
			m_Stats.m_nErrors++; 
			SetFileStatus(i, FUS_FAILED);			
		}
	}

exit:

	// Reset Uploading Now flag
	m_bUploadingNow = false;

	// Notify the main thread that we have done with uploading files.
	NotifyUploadComplete();
}

void CUploader::WaitForUploadCompletion()
{
	// Wait until worker thread exits
	WaitForSingleObject(m_hThread, INFINITE);
}

bool CUploader::IsUploadingNow()
{
	return m_bUploadingNow;
}

int CUploader::GetFileBeingUploaded()
{
	return -1;
}

void CUploader::SetNotifyWindow(HWND hWndNotify)
{
	m_hWndNotify = hWndNotify;
}

AssyncNotification* CUploader::GetAssync()
{
	return &m_Assync;
}

std::string CUploader::FileStatusToStr(FILE_UPLOAD_STATUS status)
{
	std::string sStatus = "Unknown";
	if(status==FUS_PENDING)
		sStatus = "Pending";
	else if(status==FUS_CHECKING)
		sStatus = "Checking";
	else if(status==FUS_UPLOADING)
		sStatus = "Uploading";
	else if(status==FUS_UPLOADED)
		sStatus = "Uploaded";
	else if(status==FUS_FAILED)
		sStatus = "Failed";
	else if(status==FUS_INVALID)
		sStatus = "Invalid";
	else if(status==FUS_IGNORED)
		sStatus = "Ignored";

	return sStatus;
}

void CUploader::SetCallback(LPUPLOADCALLBACK pfnCallback, LPVOID lpParam)
{
	m_pfnCallback = pfnCallback;
	m_lpCallbackParam = lpParam;
}

void CUploader::SetFileStatus(int nIndex, FILE_UPLOAD_STATUS NewStatus)
{
	FileItem* pfi = GetFileItem(nIndex);
	if(pfi==NULL)
		return;

	pfi->m_Status = NewStatus;

	if(IsWindow(m_hWndNotify))
	{
		PostMessage(m_hWndNotify, WM_ITEM_STATUS_CHANGED, nIndex, MAKELONG(pfi->m_Status, NewStatus));		
	}	

	if(m_pfnCallback)
	{
		// Fill info structure
		CallbackInfo ci;
		ci.m_Reason = CallbackInfo::REASON_ITEM_STATUS_CHANGED;
		ci.m_nIndex = nIndex;
		ci.m_pfi = pfi;
		ci.m_nProgress = 0;

		// Call back
		m_pfnCallback(&ci, m_lpCallbackParam);
	}	
}

void CUploader::NotifyFileUploadProgress(int nIndex)
{
	FileItem* pfi = GetFileItem(nIndex);
	if(pfi==NULL)
		return;

	if(m_pfnCallback)
	{
		// Fill info structure
		CallbackInfo ci;
		ci.m_Reason = CallbackInfo::REASON_PROGRESS_UPDATED;
		ci.m_nIndex = nIndex;
		ci.m_pfi = pfi;
		std::vector<CString> msg_log;
		m_Assync.GetProgress(ci.m_nProgress, msg_log);		

		// Call back
		m_pfnCallback(&ci, m_lpCallbackParam);
	}
}

void CUploader::NotifyUploadComplete()
{
	if(IsWindow(m_hWndNotify))
		PostMessage(m_hWndNotify, WM_UPLOAD_COMPLETE, 0, 0);

	if(m_pfnCallback)
	{
		// Fill info structure
		CallbackInfo ci;
		ci.m_Reason = CallbackInfo::REASON_UPLOAD_COMPLETE;
		ci.m_nIndex = -1;
		ci.m_pfi = NULL;
		ci.m_nProgress=100;		

		// Call back
		m_pfnCallback(&ci, m_lpCallbackParam);
	}
}

void CUploader::SetProjectName(std::string sProjectName)
{
	m_sProjectName = sProjectName;
}

std::string CUploader::GetProjectName()
{
	return m_sProjectName;
}

UploadStats CUploader::GetStatistics()
{
	return m_Stats;
}