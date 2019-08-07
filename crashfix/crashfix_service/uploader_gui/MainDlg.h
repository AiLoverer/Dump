// MainDlg.h : interface of the CMainDlg class
//
/////////////////////////////////////////////////////////////////////////////

#pragma once
#include "stdafx.h"
#include "Uploader.h"

class CMainDlg : 
	public CDialogImpl<CMainDlg>, 
	public CUpdateUI<CMainDlg>,
	public CMessageFilter, 
	public CIdleHandler,
	public CDialogResize<CMainDlg>
{
public:
	enum { IDD = IDD_MAINDLG };

	CMainDlg();
	virtual ~CMainDlg();

	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnIdle();

	BEGIN_UPDATE_UI_MAP(CMainDlg)
	END_UPDATE_UI_MAP()

	BEGIN_DLGRESIZE_MAP(CMainDlg)    
        DLGRESIZE_CONTROL(IDC_PATH, DLSZ_SIZE_X)    
		DLGRESIZE_CONTROL(IDC_BROWSE, DLSZ_MOVE_X)                
        DLGRESIZE_CONTROL(IDC_LIST, DLSZ_SIZE_X|DLSZ_SIZE_Y)        
		DLGRESIZE_CONTROL(IDC_LOGTITLE, DLSZ_MOVE_Y)		
		DLGRESIZE_CONTROL(IDC_LOG, DLSZ_SIZE_X|DLSZ_MOVE_Y)		
		DLGRESIZE_CONTROL(IDC_CURACTIONTITLE, DLSZ_MOVE_Y)        
		DLGRESIZE_CONTROL(IDC_CURACTION, DLSZ_MOVE_Y|DLSZ_SIZE_X)        
		DLGRESIZE_CONTROL(IDC_PROGRESS, DLSZ_MOVE_Y|DLSZ_SIZE_X)
		DLGRESIZE_CONTROL(IDC_URLTITLE, DLSZ_MOVE_Y)		
		DLGRESIZE_CONTROL(IDC_URL, DLSZ_MOVE_Y|DLSZ_SIZE_X)		
		DLGRESIZE_CONTROL(IDC_PROJECTNAMETITLE, DLSZ_MOVE_Y)		
		DLGRESIZE_CONTROL(IDC_PROJECTNAME, DLSZ_SIZE_X|DLSZ_MOVE_Y)			
		DLGRESIZE_CONTROL(IDC_UPLOAD, DLSZ_MOVE_X|DLSZ_MOVE_Y)							
    END_DLGRESIZE_MAP()

	BEGIN_MSG_MAP(CMainDlg)
		MESSAGE_HANDLER(WM_INITDIALOG, OnInitDialog)
		MESSAGE_HANDLER(WM_DESTROY, OnDestroy)
		MESSAGE_HANDLER(WM_TIMER, OnTimer)
		MESSAGE_HANDLER(WM_ITEM_STATUS_CHANGED, OnItemStatusChanged)
		MESSAGE_HANDLER(WM_UPLOAD_COMPLETE, OnUploadComplete)
		
		COMMAND_ID_HANDLER(ID_APP_ABOUT, OnAppAbout)
		COMMAND_ID_HANDLER(IDOK, OnOK)
		COMMAND_ID_HANDLER(IDCANCEL, OnCancel)
		COMMAND_ID_HANDLER(IDC_BROWSE, OnBrowseClicked)
		COMMAND_ID_HANDLER(IDC_RECURSIVESEARCH, OnRecursiveSearchClicked)
		COMMAND_ID_HANDLER(IDC_UPLOAD, OnUploadClicked)		
		COMMAND_HANDLER(IDC_PATH, EN_KILLFOCUS, OnPathKillFocus)
		COMMAND_HANDLER(IDC_URL, EN_KILLFOCUS, OnUrlKillFocus)
		COMMAND_ID_HANDLER(ID_POPUP1_SELECTALL, OnPopupSelectAll)
		COMMAND_ID_HANDLER(ID_POPUP1_UNSELECTALL, OnPopupUnselectAll)

		NOTIFY_HANDLER(IDC_LIST, LVN_ITEMCHANGING, OnListItemChanging)
        NOTIFY_HANDLER(IDC_LIST, LVN_ITEMCHANGED, OnListItemChanged)
		NOTIFY_HANDLER(IDC_LIST, NM_RCLICK, OnListRClick)	

		NOTIFY_HANDLER(IDC_LOG, NM_RCLICK, OnLogRClick)	
		COMMAND_ID_HANDLER(ID_POPUP2_COPYSEL, OnCopySel)
		COMMAND_ID_HANDLER(ID_POPUP2_COPYLOG, OnCopyLog)

		CHAIN_MSG_MAP(CDialogResize<CMainDlg>)
	END_MSG_MAP()

// Handler prototypes (uncomment arguments if needed):
//	LRESULT MessageHandler(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
//	LRESULT CommandHandler(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/)
//	LRESULT NotifyHandler(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/)

	LRESULT OnInitDialog(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnDestroy(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnTimer(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnItemStatusChanged(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnUploadComplete(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/);
	LRESULT OnAppAbout(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnOK(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnCancel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnBrowseClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnRecursiveSearchClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnUploadClicked(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPathKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnUrlKillFocus(WORD /*wNotifyCode*/, WORD /*wID*/, HWND /*hWndCtl*/, BOOL& /*bHandled*/);	
	LRESULT OnListItemChanging(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
    LRESULT OnListItemChanged(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);	
	LRESULT OnListRClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnPopupSelectAll(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnPopupUnselectAll(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);
	LRESULT OnLogRClick(int /*idCtrl*/, LPNMHDR /*pnmh*/, BOOL& /*bHandled*/);
	LRESULT OnCopySel(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);  
    LRESULT OnCopyLog(WORD /*wNotifyCode*/, WORD wID, HWND /*hWndCtl*/, BOOL& /*bHandled*/);  

	void CloseDialog(int nVal);

	//! Loads dialog's persistent state
	void LoadPersistentSettings();

	//! Saves dialog's persistent state
	void SavePersistentSettings();

	//! Fills file list control.
	void FillFileList(BOOL bSearch=TRUE);

	//! Updates control states.
	void UpdateUI();

	//! Uploads next selected file.
	BOOL UploadNextFile();

	//! Helper function that formats time stamp string.
	CString GetTimeStamp();

	int SetClipboard(CString& sData);

	CButton m_btnBrowse;
	CButton m_chkRecursiveSearch;
	CButton m_btnUpload;
	CEdit m_editURL;
	CEdit m_editProjectName;
	CEdit m_editPath;
	CCheckListViewCtrl m_listView;
	CSortListViewCtrl m_listSort;
	CProgressBarCtrl m_pgsProgress;
	CStatic m_statCurActionTitle;
	CStatic m_statCurAction;
	CListViewCtrl m_listLog;

	BOOL m_bUpdatingFileList;	
};
