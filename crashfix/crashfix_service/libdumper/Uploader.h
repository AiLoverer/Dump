//! \file Uploader.h
//! \brief Incapsulates PDB file upload functionality.
//! \author Oleg Krivtsov
//! \date Feb 2012

#pragma once
#include "stdafx.h"
#include "RegProfile.h"
#include "AssyncNotification.h"

//! This message is sent by uploader to notification window when a file item's status has changed.
//! wParam = item_index
//! LOWORD(lParam) = old status
//! HIWORD(lParam) = new status
#define WM_ITEM_STATUS_CHANGED (WM_APP+100)

//! This message is sent by uploader to notification window when all files were uploaded.
#define WM_UPLOAD_COMPLETE (WM_APP+101)

// File upload status
enum FILE_UPLOAD_STATUS
{  
    FUS_PENDING    = 0,  // Status pending.  	
	FUS_CHECKING   = 1,  // File is being checking.
	FUS_UPLOADING  = 2,  // File is being uploaded.
    FUS_UPLOADED   = 3,  // File was uploaded ok.
    FUS_FAILED     = 4,  // File upload has failed.	
	FUS_INVALID    = 5,  // File is not a valid PDB file.
	FUS_IGNORED    = 6,  // File already presents in the database.
};

//! Describes a file prepared for uploading.
struct FileItem
{
	//! Constructor.
	FileItem()
	{
		m_Status = FUS_PENDING;
		m_bChecked = TRUE;
	}

	std::wstring m_sFileName;    //!< File name.
	ULONG64 m_uFileSize;         //!< File size.
	FILE_UPLOAD_STATUS m_Status; //!< Upload status.
	CString m_sErrorMsg;         //!< Error msg.
	BOOL m_bChecked;             //!< Is this file item selected for upload?
};

//! Info passed to callback function
struct CallbackInfo
{
	enum eReason
	{
		REASON_ITEM_STATUS_CHANGED = 0, //!< Status of file item has changed.
		REASON_PROGRESS_UPDATED    = 1, //!< Progress of the item has been updated
		REASON_UPLOAD_COMPLETE     = 2, //!< All files were uploaded
	} m_Reason;

	int m_nIndex;    //!< File index.
	FileItem* m_pfi; //!< File item.
	int m_nProgress; //!< Current progress of the item.	
};

//! Upload callback function.
typedef BOOL (CALLBACK *LPUPLOADCALLBACK) (CallbackInfo* pInfo, LPVOID lpParam);

//! Contains upload statistics.
struct UploadStats
{
	UploadStats()
	{
		Reset();
	}

	void Reset()
	{
		m_nTotal = 0;
		m_nUploaded = 0;
		m_nIgnored = 0;
		m_nErrors = 0;		
	}

	int m_nTotal;    //!< Total number of files processed.
	int m_nUploaded; //!< Count of files that were uploaded successfully.
	int m_nIgnored;  //!< Count of files that were ignored (already exist in database).
	int m_nErrors;   //!< Count of files that failed to upload.	
};

//! \class CUploader
//! \brief Incapsulates command line parsing and command execution.
class CUploader
{
public:

	//! Returns singleton instance of this class.
	static CUploader* GetInstance();

	//! Constructor.
	CUploader();

	//! Destructor.
	virtual ~CUploader();

	//! Loads settings from profile.
	void LoadPersistentSettings();

	//! Saves settings to profile.
	void SavePersistentSettings();
	
	//! Returns error message.
	std::string GetErrorMsg();

	//! Returns upload URL.
	std::string GetUploadURL();

	//! Sets URL for uploading.
	void SetUploadURL(std::string sURL);

	//! Gets project name.
	std::string GetProjectName();

	//! Sets project name.
	void SetProjectName(std::string sProjectName);

	//! Adds a search pattern.
	void AddSearchPattern(std::wstring sSearchPattern);

	//! Returns count of search patterns.
	int GetSearchPatternCount();

	//! Returns search pattern by its zero-based index.
	bool GetSearchPatternByIndex(int nIndex, std::wstring& sSearchPattern);

	//! Deletes all search patterns.
	void DeleteAllSearchPatterns();

	//! Sets recursive/non-recursive search mode.
	void SetRecursiveSearch(bool bRecursiveSearch);

	//! Returns true if search is recursive, otherwise returns false.
	bool IsSearchRecursive();

	//! Fills file item list by searching for files using the specified search patterns.	
	bool FillFileList();

	//! Returns count of files selected by the current search filter.
	int GetFileCount();

	//! Returns file item by 
	FileItem* GetFileItem(int nIndex);

	//! Selects/unselects all file items
	void SelectAllFileItems(bool bSelect);

	//! Starts upload of selected files.
	bool StartFileUpload();

	//! Cancels file upload.
	void CancelFileUpload();

	//! Blocks until uploader worker thread exits.
	void WaitForUploadCompletion();

	//! Returns true if upload is in progress.
	bool IsUploadingNow();

	//! Returns the index of file being uploaded now.
	int GetFileBeingUploaded();

	//! Sets the window that will receive notifications from uploader.
	void SetNotifyWindow(HWND hWndNotify);

	//! Sets callback function (an alternative of notify window).
	void SetCallback(LPUPLOADCALLBACK pfnCallback, LPVOID lpParam);

	//! Returns pointer to owned profile
	CRegProfile* GetProfile();

	//! Returns assync notification.
	AssyncNotification* GetAssync();

	//! Returns upload statistics.
	UploadStats GetStatistics();

	//! Formats string by file upload status.
	static std::string FileStatusToStr(FILE_UPLOAD_STATUS status);

private:
	
	//! Fills internal file item list using the specified file search pattern.
	bool SearchFiles(std::wstring sSearchPattern, int nLevel);
	
	//! Worker thread procedure.
	static DWORD WINAPI UploadThread(LPVOID lpParam);

	//! Uploads selected files in another thread.
	void InternalUploadFiles();

	//! Updates file status and notifies the main thread.
	void SetFileStatus(int nIndex, FILE_UPLOAD_STATUS NewStatus);

	//! Notifies the callback function about file upload progress.
	void NotifyFileUploadProgress(int nIndex);

	//! Notifies the callback function about upload completion.
	void NotifyUploadComplete();
	
	/* Privately used variables. */

	static CUploader* m_pInstance; //!< Singleton of this class.
	std::string m_sErrorMsg;  //!< Last error message.
	std::string m_sUploadURL; //!< Upload URL.
	std::set<std::wstring> m_asSearchPatterns; //!< The list of search patterns.
	std::string m_sProjectName; //!< Project name.
	bool m_bRecursiveSearch;  //!< Search mode (recursive or not).
	std::vector<FileItem> m_aFileItems;  //!< The list of files to upload.
	CRegProfile m_Profile;    //!< Persistent profile.
	bool m_bUploadingNow;     //!< If we are uploading files now?
	int m_nFileBeingUploaded; //!< Index of the file currently being uploaded.
	HWND m_hWndNotify;        //!< Window that will receive notifications.
	LPUPLOADCALLBACK m_pfnCallback; //!< Upload callback.
	LPVOID m_lpCallbackParam; //!< Callback parameter.
	HANDLE m_hThread;         //!< Handle to working thread.
	AssyncNotification m_Assync; //!< Assync notification.	
	UploadStats m_Stats;      //!< Upload statistics.
};


