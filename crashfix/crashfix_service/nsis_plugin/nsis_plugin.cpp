// nsis_plugin.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "exdll.h"
#include "Config.h"

#ifdef _MANAGED
#pragma managed(push, off)
#endif

extern "C" void __declspec(dllexport) detect_httpd(
  HWND hwndParent, 
  int string_size, 
	char *variables, 
  stack_t **stacktop)
{
	EXDLL_INIT();

	TCHAR str[1024];
	ULONG len = 1024;
	char szPath[1024]="";
	bool bDetected = false;

	CRegKey regKey;
	if(ERROR_SUCCESS==regKey.Open(HKEY_LOCAL_MACHINE, _T("SYSTEM\\CurrentControlSet\\services\\Apache2.2")))
	{
		if(ERROR_SUCCESS==regKey.QueryStringValue(_T("ImagePath"), str, &len))
		{
			bDetected = true;
		}
	}

	CStringA sPath = str;
	int nPos = sPath.Find("\\bin\\httpd.exe");
	if(nPos<0)
		bDetected = false;
	sPath = sPath.Mid(0, nPos);
	sPath.Replace("\"", "");
		
	if(bDetected)
		pushstring(sPath.GetBuffer());	
	else
		pushstring("");	
}

extern "C" void __declspec(dllexport) open_folder_dialog(
  HWND hwndParent, 
  int string_size, 
	char *variables, 
  stack_t **stacktop)
{
	EXDLL_INIT();
		
	char szBuffer[1024]="";
	popstring(szBuffer);
	HWND hWndParent = (HWND)atoi(szBuffer);

	CFolderDialog dlg(hWndParent, _T("Select Directory"));
	INT_PTR nResult = dlg.DoModal(hWndParent);
	if(nResult==IDOK)
	{
		CStringA sFolderName = dlg.m_szFolderPath;		
		pushstring(sFolderName.GetBuffer());
	}
	else
	{
		pushstring("");
	}	
}

extern "C" void __declspec(dllexport) open_file_dialog(
  HWND hwndParent, 
  int string_size, 
	char *variables, 
  stack_t **stacktop)
{
	EXDLL_INIT();
		
	char szBuffer[1024]="";
	popstring(szBuffer);
	HWND hWndParent = (HWND)atoi(szBuffer);

	CFileDialog dlg(TRUE, _T("lic"), NULL, OFN_FILEMUSTEXIST, 
		_T("CrashFix License Files (*.lic)\0*.lic\0All Files (*.*)\0*.*\0\0"), hWndParent);
	INT_PTR nResult = dlg.DoModal(hWndParent);
	if(nResult==IDOK)
	{
		CStringA sFileName = dlg.m_szFileName;		
		pushstring(sFileName.GetBuffer());
	}
	else
	{
		pushstring("");
	}	
}

extern "C" void __declspec(dllexport) read_config_string(
  HWND hwndParent, 
  int string_size, 
	char *variables, 
  stack_t **stacktop)
{
	EXDLL_INIT();
		
	char szFileName[1024]="";
	char szParamName[1024]="";
	char szValue[1024]="";
	popstring(szFileName);
	popstring(szParamName);	

	//MessageBoxA(NULL, szFileName, szParamName, 0);
	bool bInit = false;
	CConfig config(szFileName, &bInit);
	char* szResult = config.getProfileString(szParamName, szValue, 1024);
	if(szResult!=NULL)
		pushstring(szResult);	
	else
		pushstring("");	
}


extern "C" void __declspec(dllexport) write_config_string(
  HWND hwndParent, 
  int string_size, 
	char *variables, 
  stack_t **stacktop)
{
	EXDLL_INIT();
		
	char szFileName[1024]="";
	char szParamName[1024]="";
	char szValue[1024]="";
	popstring(szFileName);
	popstring(szParamName);
	popstring(szValue);	

	//MessageBoxA(NULL, szFileName, szParamName, 0);
	bool bInit = false;
	CConfig config(szFileName, &bInit);
	int nResult = config.writeProfileString(szParamName, szValue);
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

