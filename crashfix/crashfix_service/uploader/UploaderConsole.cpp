//! \file UploaderConsole.cpp
//! \brief Incapsulates command line parsing and command execution.
//! \author Oleg Krivtsov
//! \date Feb 2012

#include "stdafx.h"
#include "UploaderConsole.h"
#include "strconv.h"
#include "LibDumperVersion.h"

CUploaderConsole::CUploaderConsole()
{
	m_bVerbose = false;
}

std::string CUploaderConsole::GetErrorMsg()
{
	// Return last error message.
	return m_sErrorMsg;
}

void CUploaderConsole::PrintVersion()
{
	printf("\nCrashFix Uploader v." LIBDUMPER_VER " [" __DATE__ " " __TIME__ "]\n\n");
}

void CUploaderConsole::PrintUsage()
{
	// This method writes version and usage information.

	printf("\nCrashFix Uploader v." LIBDUMPER_VER " [" __DATE__ " " __TIME__ "]\n\n");

	printf("Usage:\n");
	printf("\n");
	printf(" uploader.exe [options] <command>\n");
	printf("\n");
	printf(" Options:\n");
	printf("\n");	
	printf("  -r             If this flag is specified, the directories are searched\n"); 
	printf("                 recursively. Used with -u command.\n"); 
	printf("\n");	
	printf("  -v             Enables verbose output. Used with -u command.\n"); 	
	printf("\n");			
	printf(" Commands:\n");
	printf("\n");	
	printf("  --version      Prints version and copyright information.\n");
	printf("\n");
	printf("  --help         Prints usage help.\n");
	printf("\n");
	printf("  -u <url> <project> <item> [<item> ...]  Uploads one or several items\n"); 
	printf("                 to specified URL and marks them belonging to the specified\n");	
	printf("                 project. The items may be either exact file or directory\n");	
	printf("                 paths or file search patterns.\n");	
	printf("\n");		
	printf(" Examples:\n");	
	printf("\n");	
	printf("  The example below uploads all PDB files from current directory and all\n");	
	printf("  subdirectories to the specified URL and saves them in Editor project:\n");	
	printf("\n");	
	printf("    uploader.exe -r -u http://myserver.com/crashfix/index.php/debugInfo/uploadExternal Editor *.pdb\n");	
	printf("\n");	
	printf("  The example below uploads Editor.pdb and MyDll.pdb files from current directory\n");	
	printf("  to the specified URL and saves them in Editor project:\n");	
	printf("\n");	
	printf("    uploader.exe -u http://myserver.com/crashfix/index.php/debugInfo/uploadExternal Editor Editor.pdb MyDll.pdb\n");	
	printf("\n");	
}

int CUploaderConsole::Run(int argc, char* argv[])
{
	// This method parses the command line and executes the command.

	int nResult = 1;
	int cur_arg = 1;

	std::string sCommand;	
	std::string sURL;
	std::string sProject;
	bool bRecursiveSearch = false;
	std::vector<std::string> asItems;	

	m_sErrorMsg = "Unspecified error.";

	// Walk through cmdline arguments
	while(args_left()!=0)
	{
		if(cmp_arg("--version")) // Print version and copyright
		{
			skip_arg();
			sCommand += "version";			
		}
		else if(cmp_arg("--help")) // Print usage help
		{
			skip_arg();
			sCommand += "help";			
		}
		else if(cmp_arg("-u")) // Upload files
		{
			skip_arg();
			sCommand += "upload";			

			// Get upload URL
			char* szURL = get_arg();
			if(szURL==NULL)
			{
				m_sErrorMsg = "URL expected.";
				goto cleanup;
			}
			sURL = szURL;
			skip_arg();

			// Get project name
			char* szProject = get_arg();
			if(szProject==NULL)
			{
				m_sErrorMsg = "Project name expected.";
				goto cleanup;
			}
			sProject = szProject;
			skip_arg();

			// Consider arguments left as items to upload
			while(args_left())
			{
				char* szItem = get_arg();				
				asItems.push_back(szItem);
				skip_arg();
			}

			// Check at least one item entered
			if(asItems.size()==0)
			{
				m_sErrorMsg = "One or several items expected.";
				goto cleanup;
			}
		}
		else if(cmp_arg("-r")) // Recursive upload
		{
			skip_arg();
			bRecursiveSearch = true;
		}		
		else if(cmp_arg("-v")) // Verbose
		{
			skip_arg();
			m_bVerbose = true;
		}		
		else
		{
			m_sErrorMsg = "Unexpected argument.";
			goto cleanup;
		}
	}

	if(sCommand=="version")
	{
		PrintVersion();
		nResult = 0;
	}
	else if(sCommand=="help")
	{
		PrintUsage();
		nResult = 0;
	}
	else if(sCommand=="upload")
	{	
		m_Uploader.SetUploadURL(sURL);
		m_Uploader.SetProjectName(sProject);

		m_Uploader.DeleteAllSearchPatterns();

		size_t i;
		for(i=0; i<asItems.size(); i++)
		{
			m_Uploader.AddSearchPattern(strconv::a2w(asItems[i]));
		}

		m_Uploader.SetRecursiveSearch(bRecursiveSearch);

		m_Uploader.SetCallback(UploadCallback, this);

		printf("Searching for files... ");

		m_Uploader.FillFileList();

		printf("%d files found.\n", m_Uploader.GetFileCount());
		if(m_Uploader.GetFileCount()!=0)
		{
			printf("Starting file upload to URL %s...\n", m_Uploader.GetUploadURL().c_str());
			nResult = m_Uploader.StartFileUpload();
			m_Uploader.WaitForUploadCompletion();			
		}

		UploadStats Stats = m_Uploader.GetStatistics();
		printf("\n----\nTotal: %d; Uploaded: %d; Failed: %d; Skipped: %d.", 
			Stats.m_nTotal, Stats.m_nUploaded, Stats.m_nErrors, Stats.m_nIgnored);

		if(Stats.m_nErrors==0)
		{
			// Success.
			nResult = 0; 
			m_sErrorMsg = "Succeeded.";
		}
		else
		{
			nResult = 1;
			m_sErrorMsg = "There were some errors.";
		}
	}
	else
	{
		m_sErrorMsg = "Unknown command.";
	}	
	
cleanup:

	printf("\n");
		
	return nResult;
}

BOOL CALLBACK CUploaderConsole::UploadCallback(CallbackInfo* pInfo, LPVOID lpParam)
{
	CUploaderConsole* pUploaderConsole = (CUploaderConsole*)lpParam;
	pUploaderConsole->OnUploadCallback(pInfo);
	return TRUE;
}

void CUploaderConsole::OnUploadCallback(CallbackInfo* pInfo)
{
	if(pInfo->m_Reason==CallbackInfo::REASON_ITEM_STATUS_CHANGED)
	{
		if(pInfo->m_pfi->m_Status==FUS_PENDING)
			return;

		if(m_nLastStatusChangedFile==pInfo->m_nIndex)
			printf("\r");
		else
			printf("\n");

		if(m_bVerbose)
		{
			// Verbose output
			int nProgress = 0;
			std::vector<CString> asMessages;
			m_Uploader.GetAssync()->GetProgress(nProgress, asMessages);
		
			// Clear space
			printf("                                                     \r"); 

			// Print messages
			unsigned i; 
			for(i=0; i<asMessages.size(); i++)
			{
				_tprintf(asMessages[i]);
				printf("\n");
			}
		}

		printf("%d of %d: %s [ %s ]          ", 
			pInfo->m_nIndex+1, 
			m_Uploader.GetFileCount(), 
			strconv::w2a(pInfo->m_pfi->m_sFileName).c_str(), 
			CUploader::FileStatusToStr(pInfo->m_pfi->m_Status).c_str());
	}
	else if(pInfo->m_Reason==CallbackInfo::REASON_PROGRESS_UPDATED)
	{
		if(m_nLastStatusChangedFile==pInfo->m_nIndex)
			printf("\r");
		else
			printf("\n");
		
		if(m_bVerbose)
		{
			// Verbose output
			int nProgress = 0;
			std::vector<CString> asMessages;
			m_Uploader.GetAssync()->GetProgress(nProgress, asMessages);
		
			// Clear space
			printf("                                                   \r"); 

			// Print messages
			unsigned i; 
			for(i=0; i<asMessages.size(); i++)
			{
				_tprintf(asMessages[i]);
				printf("\n");
			}
		}

		printf("%d of %d: %s [ %s %d%% ]      ", 
			pInfo->m_nIndex+1, 
			m_Uploader.GetFileCount(), 
			strconv::w2a(pInfo->m_pfi->m_sFileName).c_str(), 
			CUploader::FileStatusToStr(pInfo->m_pfi->m_Status).c_str(),
			pInfo->m_nProgress);
	}	

	m_nLastStatusChangedFile=pInfo->m_nIndex;

}