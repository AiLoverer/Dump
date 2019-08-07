// MainDlg.cpp : implementation of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "resource.h"

#include "aboutdlg.h"
#include "MainDlg.h"
#include "strconv.h"
#include "Misc.h"

#define SECTION_MAINDLG _T("MainDlg")
#define ENTRY_WINDOW_POS _T("WindowPos")

CMainDlg::CMainDlg()
{
	m_bUpdatingFileList = FALSE;
}

CMainDlg::~CMainDlg()
{
}

BOOL CMainDlg::PreTranslateMessage(MSG* pMsg)
{
	return CWindow::IsDialogMessage(pMsg);
}

BOOL CMainDlg::OnIdle()
{
	return FALSE;
}

LRESULT CMainDlg::OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// center the dialog on the screen
	CenterWindow();

	// set icons
	HICON hIcon = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXICON), ::GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	SetIcon(hIcon, TRUE);
	HICON hIconSmall = (HICON)::LoadImage(_Module.GetResourceInstance(), MAKEINTRESOURCE(IDR_MAINFRAME), 
		IMAGE_ICON, ::GetSystemMetrics(SM_CXSMICON), ::GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);
	SetIcon(hIconSmall, FALSE);

	// register object for message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->AddMessageFilter(this);
	pLoop->AddIdleHandler(this);

	UIAddChildWindowContainer(m_hWnd);

	// Bind constrols
	m_btnBrowse = GetDlgItem(IDC_BROWSE);
	m_chkRecursiveSearch = GetDlgItem(IDC_RECURSIVESEARCH);
	m_btnUpload = GetDlgItem(IDC_UPLOAD);
	m_listSort.SubclassWindow(GetDlgItem(IDC_LIST));  
    m_listView.SubclassWindow(m_listSort.m_hWnd);	
	m_pgsProgress = GetDlgItem(IDC_PROGRESS);
	m_editPath = GetDlgItem(IDC_PATH);
	m_editURL = GetDlgItem(IDC_URL);
	m_editProjectName = GetDlgItem(IDC_PROJECTNAME);
	m_statCurActionTitle = GetDlgItem(IDC_CURACTIONTITLE);
	m_statCurAction = GetDlgItem(IDC_CURACTION);
	m_listLog = GetDlgItem(IDC_LOG);
	
	// Insert list view columns
	m_listView.InsertColumn(0, _T("File"), LVCFMT_LEFT, 350);
	m_listView.InsertColumn(1, _T("Status"), LVCFMT_LEFT, 100);
	m_listView.InsertColumn(2, _T("Size"), LVCFMT_LEFT, 100);
	m_listSort.SetSortColumn(0); // Sort by file name
	
	// Insert list view columns for log
	m_listLog.SetExtendedListViewStyle(LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	m_listLog.InsertColumn(0, _T("Message"), LVCFMT_LEFT, 1000);

	CString sMsg;		
	sMsg += "[";
	sMsg += GetTimeStamp();
	sMsg += "] ";
	sMsg += "Press the Upload Now button to start file upload.";
	m_listLog.InsertItem(0, sMsg);

	// Init progress bar
	m_pgsProgress.SetRange(0, 100);

	m_statCurActionTitle.EnableWindow(FALSE);
	m_statCurAction.EnableWindow(FALSE);
	m_pgsProgress.EnableWindow(FALSE);

	// Init dialog resizing
	DlgResize_Init();

	LoadPersistentSettings();

	// Fill file list
	FillFileList();

	UpdateUI();
	
	return TRUE;
}

LRESULT CMainDlg::OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	SavePersistentSettings();

	// unregister message filtering and idle updates
	CMessageLoop* pLoop = _Module.GetMessageLoop();
	ATLASSERT(pLoop != NULL);
	pLoop->RemoveMessageFilter(this);
	pLoop->RemoveIdleHandler(this);

	return 0;
}

LRESULT CMainDlg::OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CAboutDlg dlg;
	dlg.DoModal();
	return 0;
}

LRESULT CMainDlg::OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	// TODO: Add validation code 
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CloseDialog(wID);
	return 0;
}

LRESULT CMainDlg::OnBrowseClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CFolderDialog dlg(m_hWnd, _T("Select PDB Directory"));
	INT_PTR nResult = dlg.DoModal(m_hWnd);
	if(nResult==IDOK)
	{
		CString sFolderName = dlg.m_szFolderPath;		
		CUploader* pUploader = CUploader::GetInstance();
		pUploader->DeleteAllSearchPatterns();
		pUploader->AddSearchPattern(sFolderName.GetBuffer());
	}
	
	FillFileList();
	UpdateUI();

	return 0;
}

LRESULT CMainDlg::OnRecursiveSearchClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CUploader* pUploader = CUploader::GetInstance();
	bool bRecursive = pUploader->IsSearchRecursive();
	pUploader->SetRecursiveSearch(!bRecursive);

	UpdateUI();
	FillFileList();
	return 0;
}

LRESULT CMainDlg::OnUploadClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CUploader* pUploader = CUploader::GetInstance();

	m_statCurActionTitle.EnableWindow(TRUE);
	m_statCurAction.EnableWindow(TRUE);
	m_pgsProgress.EnableWindow(TRUE);
	
	TCHAR szBuffer[512] = _T("");

	m_editURL.GetWindowText(szBuffer, 512);
	std::string sURL = strconv::w2a(szBuffer);
	CUploader::GetInstance()->SetUploadURL(sURL);

	m_editProjectName.GetWindowText(szBuffer, 512);
	std::string sProjectName = strconv::w2a(szBuffer);
	CUploader::GetInstance()->SetProjectName(sProjectName);

	if(!pUploader->IsUploadingNow())
	{	
		// This will update status field of all items
		FillFileList(FALSE);

		CString sMsg;		
		sMsg += "[";
		sMsg += GetTimeStamp();
		sMsg += "] ";
		sMsg += "Upload has been started.";
		m_listLog.InsertItem(m_listLog.GetItemCount(), sMsg);

		// We want to receive notifications
		pUploader->SetNotifyWindow(m_hWnd);

		// Start upload
		pUploader->StartFileUpload();
		
		m_btnUpload.SetWindowText(_T("Cancel"));

		SetTimer(0, 300, NULL);
	}
	else
	{
		pUploader->CancelFileUpload();
		m_btnUpload.SetWindowText(_T("Cancelling..."));	
		m_btnUpload.EnableWindow(FALSE);
	}

	return 0;
}

LRESULT CMainDlg::OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	CUploader* pUploader = CUploader::GetInstance();
	AssyncNotification* pAssync = pUploader->GetAssync();

	int nProgressPct;
	std::vector<CString> msg_log;
	pAssync->GetProgress(nProgressPct, msg_log);

	m_pgsProgress.SetPos(nProgressPct);

	size_t i;
	for(i=0; i<msg_log.size(); i++)
	{
		m_statCurAction.SetWindowText(msg_log[i]);	

		CString sMsg;		
		sMsg += "[";
		sMsg += GetTimeStamp();
		sMsg += "] ";
		sMsg += msg_log[i];
		int nItem = m_listLog.InsertItem(m_listLog.GetItemCount(), sMsg);
		m_listLog.EnsureVisible(nItem, TRUE);
	}	

	return 0;
}

LRESULT CMainDlg::OnItemStatusChanged(UINT /*uMsg*/, WPARAM wParam, LPARAM lParam, BOOL& /*bHandled*/)
{
	int nFileItem = (int)wParam;
	FILE_UPLOAD_STATUS OldStatus = (FILE_UPLOAD_STATUS)LOWORD(lParam);
	FILE_UPLOAD_STATUS NewStatus = (FILE_UPLOAD_STATUS)HIWORD(lParam);

	CUploader* pUploader = CUploader::GetInstance();

	int i;
	for(i=0; i<m_listView.GetItemCount(); i++)
	{
		DWORD dwItemData = m_listView.GetItemData(i);
		if(nFileItem!=dwItemData)
			continue;
		
		std::string sStatus = pUploader->FileStatusToStr(NewStatus);
		m_listView.SetItemText(i, 1, strconv::a2w(sStatus).c_str());	
	}

	if(NewStatus==FUS_CHECKING)
	{
		FileItem* pFileItem = pUploader->GetFileItem(nFileItem);		
		CString sMsg;		
		sMsg += "[";
		sMsg += GetTimeStamp();
		sMsg += "] ";
		sMsg += "Starting upload of file: ";
		sMsg += pFileItem->m_sFileName.c_str();	
		m_listLog.InsertItem(m_listLog.GetItemCount(), sMsg);
	}

	return 0;
}

LRESULT CMainDlg::OnUploadComplete(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
{
	// Stop timer
	KillTimer(0);

	// Hide progress controls
	m_statCurActionTitle.EnableWindow(FALSE);
	m_statCurAction.EnableWindow(FALSE);
	m_statCurAction.SetWindowText(L"");
	m_pgsProgress.EnableWindow(FALSE);

	// Reset progress bar
	m_pgsProgress.SetPos(0);

	// Set button text
	m_btnUpload.SetWindowText(_T("Upload Now"));		
	m_btnUpload.EnableWindow(TRUE);

	CString sMsg;		
	sMsg += "[";
	sMsg += GetTimeStamp();
	sMsg += "] ";
	sMsg += "Upload has been finished.";
	int nItem = m_listLog.InsertItem(m_listLog.GetItemCount(), sMsg);
	m_listLog.EnsureVisible(nItem, TRUE);
	
	return 0;
}

LRESULT CMainDlg::OnPathKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR szBuffer[512] = _T("");
	m_editPath.GetWindowText(szBuffer, 512);
	
	CUploader* pUploader = CUploader::GetInstance();
	pUploader->DeleteAllSearchPatterns();
	pUploader->AddSearchPattern(szBuffer);

	UpdateUI();
	return 0;
}

LRESULT CMainDlg::OnUrlKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	TCHAR szBuffer[512] = _T("");
	m_editURL.GetWindowText(szBuffer, 512);

	std::string sURL = strconv::w2a(szBuffer);
	CUploader::GetInstance()->SetUploadURL(sURL);

	UpdateUI();
	return 0;
}

LRESULT CMainDlg::OnListItemChanging(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	if(m_bUpdatingFileList)
		return 0;
	
	NMLISTVIEW* pnmlv = (NMLISTVIEW *)pnmh;
    if(pnmlv && pnmlv->iItem>=0 && (pnmlv->uChanged&LVIF_STATE) && 
        ((pnmlv->uNewState&LVIS_STATEIMAGEMASK)!=(pnmlv->uOldState&LVIS_STATEIMAGEMASK)))
    {
		// Forbid unchecking items when upload in progress
		if(CUploader::GetInstance()->IsUploadingNow())
            return TRUE;    
    }

	return 0;
}

LRESULT CMainDlg::OnListItemChanged(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	if(m_bUpdatingFileList)
		return 0;

	NMLISTVIEW* pnmlv = (NMLISTVIEW *)pnmh;
    if(pnmlv->iItem>=0 && (pnmlv->uChanged&LVIF_STATE) && 
        ((pnmlv->uNewState&LVIS_STATEIMAGEMASK)!=(pnmlv->uOldState&LVIS_STATEIMAGEMASK)))
    {   
		DWORD dwItemData = m_listView.GetItemData(pnmlv->iItem);		

		// Toggle item check
		CUploader* pUploader = CUploader::GetInstance();
		FileItem* pfi = pUploader->GetFileItem((int)dwItemData);
		if(pfi)
			pfi->m_bChecked = !pfi->m_bChecked;

		m_listView.SetCheckState(pnmlv->iItem, pfi->m_bChecked?TRUE:FALSE);
    }  

	return 0;
}

LRESULT CMainDlg::OnListRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{  
    LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) pnmh;

    POINT pt;
    GetCursorPos(&pt);

    CMenu popup_menu;
    popup_menu.LoadMenu(IDR_POPUP);

    CMenu submenu = popup_menu.GetSubMenu(0);  

    submenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, m_hWnd); 
    return 0;
}

LRESULT CMainDlg::OnPopupSelectAll(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CUploader* pUploader = CUploader::GetInstance();
	pUploader->SelectAllFileItems(true);
	FillFileList(FALSE);
	return 0;
}

LRESULT CMainDlg::OnPopupUnselectAll(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
	CUploader* pUploader = CUploader::GetInstance();
	pUploader->SelectAllFileItems(false);

	FillFileList(FALSE);
	return 0;
}

LRESULT CMainDlg::OnLogRClick(int /*idCtrl*/, LPNMHDR pnmh, BOOL& /*bHandled*/)
{
	LPNMITEMACTIVATE lpnmitem = (LPNMITEMACTIVATE) pnmh;

    POINT pt;
    GetCursorPos(&pt);

    CMenu popup_menu;
    popup_menu.LoadMenu(IDR_POPUP);

    CMenu submenu = popup_menu.GetSubMenu(1);  

    submenu.TrackPopupMenu(TPM_LEFTALIGN, pt.x, pt.y, m_hWnd);     

	return 0;
}

LRESULT CMainDlg::OnCopySel(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CString sData;
    int i;
    for(i=0; i<m_listLog.GetItemCount(); i++)
    {
        DWORD dwState = m_listLog.GetItemState(i, LVIS_SELECTED);
        if(dwState==0)
            continue;

        TCHAR buf[4096];
        buf[0]=0;
        int n = m_listLog.GetItemText(i, 0, buf, 4095);
        sData += CString(buf,n);
        sData += "\r\n";
    }

    SetClipboard(sData);  

    return 0;
}

LRESULT CMainDlg::OnCopyLog(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
{
    CString sData;
    int i;
    for(i=0; i<m_listLog.GetItemCount(); i++)
    {
        TCHAR buf[4096];
        buf[0]=0;
        int n = m_listLog.GetItemText(i, 0, buf, 4095);
        sData += CString(buf,n);
        sData += "\r\n";
    }

    SetClipboard(sData);  

    return 0;
}

void CMainDlg::CloseDialog(int nVal)
{
	DestroyWindow();
	::PostQuitMessage(nVal);
}

void CMainDlg::LoadPersistentSettings()
{	
	CRegProfile* pProfile = CUploader::GetInstance()->GetProfile();

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	if(pProfile->GetProfileBinary(SECTION_MAINDLG, ENTRY_WINDOW_POS, (LPBYTE)&wp, sizeof(WINDOWPLACEMENT)))
	{
		SetWindowPlacement(&wp);
	}
}

void CMainDlg::SavePersistentSettings()
{
	CRegProfile* pProfile = CUploader::GetInstance()->GetProfile();

	WINDOWPLACEMENT wp;
	wp.length = sizeof(WINDOWPLACEMENT);
	GetWindowPlacement(&wp);

	pProfile->WriteProfileBinary(SECTION_MAINDLG, ENTRY_WINDOW_POS, (LPBYTE)&wp, sizeof(WINDOWPLACEMENT));	
}

void CMainDlg::FillFileList(BOOL bSearch)
{
	// Display hourglass cursor while filling the list
	CWaitCursor wc;

	m_bUpdatingFileList = TRUE;

	// First remove all items from the list
	m_listView.DeleteAllItems();

	// Update file list
	CUploader* pUploader = CUploader::GetInstance();
	
	if(bSearch)
		pUploader->FillFileList();
	
	// Walk through files selected for upload
	int nCount = pUploader->GetFileCount();
	int i;
	for(i=0; i<nCount; i++)
	{
		FileItem* pfi = pUploader->GetFileItem(i);
		if(pfi==NULL)
			continue;

		int nItem = m_listView.InsertItem(0, pfi->m_sFileName.c_str());
		m_listView.SetItemData(nItem, i);

		CUploader* pUploader = CUploader::GetInstance();
		std::string sStatus = pUploader->FileStatusToStr(pfi->m_Status);
		m_listView.SetItemText(nItem, 1, strconv::a2w(sStatus).c_str());		

		CString sFileSize = FileSizeToStr(pfi->m_uFileSize);
		m_listView.SetItemText(nItem, 2, sFileSize);

		m_listView.SetCheckState(nItem, pfi->m_bChecked);

	}

	m_bUpdatingFileList = FALSE;
}

void CMainDlg::UpdateUI()
{
	CUploader* pUploader = CUploader::GetInstance();

	// Update Path edit box
	std::wstring sSearchPattern;
	if(pUploader->GetSearchPatternByIndex(0, sSearchPattern))
		m_editPath.SetWindowText(sSearchPattern.c_str());

	// Update Recursive Search check box state
	bool bRecursiveSearch = pUploader->IsSearchRecursive();
	m_chkRecursiveSearch.SetCheck(bRecursiveSearch?BST_CHECKED:BST_UNCHECKED);

	// Update Upload URL edit box
	std::string sURL = pUploader->GetUploadURL();
	m_editURL.SetWindowText(CStringW(sURL.c_str()));

	// Update Project Name edit box
	std::string sProjectName = pUploader->GetProjectName();
	m_editProjectName.SetWindowText(CStringW(sProjectName.c_str()));
}

CString CMainDlg::GetTimeStamp()
{
	// get current time
	time_t cur_time = 0;
	time(&cur_time);
	char szDateTime[64] = "";

	struct tm ltimeinfo;
	localtime_s(&ltimeinfo, &cur_time);
	strftime(szDateTime, 63,  "%H:%M:%S", &ltimeinfo);

	return CString(szDateTime);
}

int CMainDlg::SetClipboard(CString& sData)
{
    if (OpenClipboard())
    {
        EmptyClipboard();
        HGLOBAL hClipboardData;
        DWORD dwSize = (sData.GetLength()+1)*sizeof(TCHAR);
        hClipboardData = GlobalAlloc(GMEM_DDESHARE, dwSize);
        TCHAR* pszData = (TCHAR*)GlobalLock(hClipboardData);
        if(pszData!=NULL)
        {      
            _tcsncpy_s(pszData, dwSize/sizeof(TCHAR), sData, sData.GetLength()*sizeof(TCHAR));
            GlobalUnlock(hClipboardData);
#ifdef _UNICODE
            SetClipboardData(CF_UNICODETEXT, hClipboardData);
#else
            SetClipboardData(CF_TEXT, hClipboardData);    
#endif
            CloseClipboard();
            return 0;
        }
        CloseClipboard();
    }

    return 1;
}