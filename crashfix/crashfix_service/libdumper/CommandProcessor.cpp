#include "stdafx.h"
#include "CommandProcessor.h"
#include "MiniDumpReader.h"
#include "MiniDumpWriter.h"
#include "DbgHelpDumpWriter.h"
#include "MsfFile.h"
#include "PdbReader.h"
#include "CrashReportReader.h"
#include "StackWalker.h"
#include "strconv.h"
#include "tinyxml.h"
#include "Log.h"
#include "Misc.h"
#include "Outputter.h"
#include "md5.h"

CCommandProcessor::CCommandProcessor()
{
    m_pLog = NULL;
    m_bLogIsOwned = true;

    m_pPdbCache = NULL;
    m_bPdbCacheIsOwned = true;
}

CCommandProcessor::~CCommandProcessor()
{
    if(m_pLog != NULL && m_bLogIsOwned)
    {
        m_pLog->term();
        delete m_pLog;
    }

    if(m_pPdbCache != NULL && m_bPdbCacheIsOwned)
    {
        delete m_pPdbCache;
    }
}


int CCommandProcessor::Run(int argc, char* argv[])
{
 	int cur_arg = 1;

	if(cmp_arg("--read-minidump"))
	{
		skip_arg();
		LPCSTR szFileName = get_arg();
		skip_arg();
		LPCSTR szOutFile = get_arg();
		return ReadDump(szFileName, szOutFile);
	}
#ifdef _WIN32
	else if(cmp_arg("--write-minidump"))
	{
		skip_arg();
		LPCSTR szOutFile = get_arg();
		return WriteDump(szOutFile);
	}
	else if(cmp_arg("--write-minidump-dbghelp"))
	{
		skip_arg();
		LPCSTR szOutFile = get_arg();
		return WriteDumpDbgHelp(szOutFile);
	}
#endif
	else if(cmp_arg("--read-pdb"))
	{
		skip_arg();
		LPCSTR szFileName = get_arg();
		skip_arg();
		LPCSTR szOutFile = get_arg();
		return ReadPdb(szFileName, szOutFile);
	}
	else if(cmp_arg("--dump-pdb-stream"))
	{
		skip_arg();
		LPCSTR szPdbFileName = get_arg();
		skip_arg();
		LPCSTR szStreamNum = get_arg();
		int nStream = atoi(szStreamNum);
		skip_arg();
		LPCSTR szOutFile = get_arg();
		return ExtractPdbStream(szPdbFileName, nStream, szOutFile);
	}
	else if(cmp_arg("--dump-pdb-streams"))
	{
		skip_arg();
		LPCSTR szPdbFileName = get_arg();
		skip_arg();
		LPCSTR szOutDir = get_arg();
		return ExtractPdbStreams(szPdbFileName, szOutDir);
	}
	else if(cmp_arg("--dia2dump"))
	{
		skip_arg();
		LPCSTR szPdbFileName = get_arg();
		skip_arg();
		LPCSTR szOutFile = get_arg();
		return Dia2Dump(szPdbFileName, szOutFile);
	}
	else if(cmp_arg("--dump-crash-report"))
	{
		skip_arg();
		LPCSTR szCrashRptFileName = get_arg();
		skip_arg();
		LPCSTR szOutFile = get_arg();
		skip_arg();
		
		LPCWSTR wszCrashRptFileName = NULL;
		LPCWSTR wszOutFile = NULL;
		LPCWSTR wszSymbolSearchDir = NULL;
		std::wstring sCrashRptFileName;
		std::wstring sOutFile;
		std::wstring sSymbolSearchDir;
		bool bRelaxBuildAge = false;
		if(szCrashRptFileName)
		{
			sCrashRptFileName = strconv::a2w(szCrashRptFileName);
			wszCrashRptFileName = sCrashRptFileName.c_str();
		}
		if(szOutFile)
		{
			sOutFile = strconv::a2w(szOutFile);
			wszOutFile = sOutFile.c_str();
		}
		
		LPCSTR szRelaxBuildAge = get_arg();
				
		if(szRelaxBuildAge!=NULL && strcmp(szRelaxBuildAge, "--relax-build-age")==0)
		{
			bRelaxBuildAge = true;
			skip_arg();
		}

		LPCSTR szSymbolSearchDir = get_arg();
		skip_arg();
		
		if(szSymbolSearchDir)
		{
			sSymbolSearchDir = strconv::a2w(szSymbolSearchDir);
			wszSymbolSearchDir = sSymbolSearchDir.c_str();
		}


		return DumpCrashReport(wszCrashRptFileName, wszOutFile, wszSymbolSearchDir, !bRelaxBuildAge);
	}
	else if(cmp_arg("--extract-file"))
	{
		skip_arg();
		LPCSTR szCrashRptFileName = get_arg();
		skip_arg();
		LPCSTR szFileItemName = get_arg();
		skip_arg();
		LPCSTR szOutFile = get_arg();

		LPCWSTR wszCrashRptFileName = NULL;
		LPCWSTR wszFileItemName = NULL;
		LPCWSTR wszOutFile = NULL;

		std::wstring sCrashRptFileName;
		std::wstring sFileItemName;
		std::wstring sOutFile;

		if(szCrashRptFileName)
		{
			sCrashRptFileName = strconv::a2w(szCrashRptFileName);
			wszCrashRptFileName = sCrashRptFileName.c_str();
		}

		if(szFileItemName)
		{
			sFileItemName = strconv::a2w(szFileItemName);
			wszFileItemName = sFileItemName.c_str();
		}

		if(szOutFile)
		{
			sOutFile = strconv::a2w(szOutFile);
			wszOutFile = sOutFile.c_str();
		}

		return ExtractFile(wszCrashRptFileName, wszFileItemName, wszOutFile);
	}
    else if(cmp_arg("--import-pdb"))
	{
		skip_arg();
		LPCSTR szPdbFileName = get_arg();
		skip_arg();
		LPCSTR szSymDir = get_arg();
		skip_arg();
		LPCSTR szOutFile = get_arg();
		skip_arg();

		if(args_left()!=0)
		{
			m_sErrorMsg = "Invalid argument count for --import-pdb";
			return -1;
		}

		LPCWSTR wszPdbFileName = NULL;
		LPCWSTR wszSymDir = NULL;
		LPCWSTR wszOutFile = NULL;
		std::wstring sPdbFileName;
		std::wstring sSymDir;
		std::wstring sOutFile;

		if(szPdbFileName)
		{
			sPdbFileName = strconv::a2w(szPdbFileName);
			wszPdbFileName = sPdbFileName.c_str();
		}

        if(szSymDir)
		{
			sSymDir = strconv::a2w(szSymDir);
			wszSymDir = sSymDir.c_str();
		}

		if(szOutFile)
		{
			sOutFile = strconv::a2w(szOutFile);
			wszOutFile = sOutFile.c_str();
		}

        return ImportPdb(wszPdbFileName, wszSymDir, wszOutFile);
    }
	else if(cmp_arg("--delete-debug-info"))
	{
		skip_arg();
		LPCSTR szPdbFileName = get_arg();
				
		return DeleteDebugInfo(strconv::a2w(szPdbFileName).c_str());
	}
	else
    {
        m_sErrorMsg = "CommandProcessor has encountered an unexpected argument '";
        LPCSTR szArg = get_arg();
        if(szArg!=NULL)
        {
            m_sErrorMsg += std::string(szArg);
        }
        m_sErrorMsg +="'.";
        return -1;
    }
}

std::string CCommandProcessor::GetErrorMsg()
{
    return m_sErrorMsg;
}

bool CCommandProcessor::InitLog(std::wstring sFileName, int nLoggingLevel)
{
    m_pLog = new CLog();
    BOOL bInit = m_pLog->init(sFileName, false);
    m_pLog->set_level(nLoggingLevel);
    return bInit?true:false;
}

CLog* CCommandProcessor::SubstituteLog(CLog* pLog, bool bOwn)
{
    CLog* pOldLog = m_pLog;

    m_pLog = pLog;
    m_bLogIsOwned = bOwn;

    return pOldLog;
}

CPdbCache* CCommandProcessor::SubstitutePdbCache(CPdbCache* pPdbCache, bool bOwn)
{
    CPdbCache* pOldCache = m_pPdbCache;

    m_pPdbCache = pPdbCache;
    m_bPdbCacheIsOwned = bOwn;

    return pOldCache;
}

void CCommandProcessor::PrintUsage()
{
    int nLevel = 0;

	m_pLog->write(nLevel, "Usage help:\n");
	m_pLog->write(nLevel, "dumper <command>, where the command is one of the following:\n");
	m_pLog->write(nLevel, "  --read-minidump <file_name> [out_file]    Read the dump file_name\n");
    m_pLog->write(nLevel, "                       and output results to out_file.\n");
    m_pLog->write(nLevel, "\n");
#ifdef _WIN32
	m_pLog->write(nLevel, "  --write-minidump <file_name>    Writes the minidump to file.\n");
    m_pLog->write(nLevel, "\n");
	m_pLog->write(nLevel, "  --write-minidump-dbghelp <file_name>    Writes the minidump\n");
    m_pLog->write(nLevel, "                       using dbghelp.dll.\n");
    m_pLog->write(nLevel, "\n");
#endif
	m_pLog->write(nLevel, "  --dump-pdb-stream <src_pdb_file> <stream_num> <out_file> Extract\n");
    m_pLog->write(nLevel, "                       a stream from a PDB file.\n");
    m_pLog->write(nLevel, "\n");
	m_pLog->write(nLevel, "  --dump-pdb-streams <src_pdb_file> <out_dir> Extract all streams\n");
    m_pLog->write(nLevel, "                       from a PDB file to destination directory.\n");
    m_pLog->write(nLevel, "\n");
	m_pLog->write(nLevel, "  --dia2dump <src_file> <out_file> Dumps symbols from a PDB\n");
    m_pLog->write(nLevel, "                       file to a text file.\n");
    m_pLog->write(nLevel, "\n");
	m_pLog->write(nLevel, "  --dump-crash-report <src_file> <out_file> Dumps contents of a\n");
    m_pLog->write(nLevel, "                       crash report file to a text file.\n");
    m_pLog->write(nLevel, "\n");
    m_pLog->write(nLevel, "  --import-pdb <pdb_file> <sym_dir> [out_file] Imports a PDB file named\n");
    m_pLog->write(nLevel, "                       <pdb_file> into symbol storage directory named <sym_dir>.\n");
	m_pLog->write(nLevel, "                       Optionally writes general PDB info to [out_file].\n");
    m_pLog->write(nLevel, "\n");
	m_pLog->write(nLevel, "  --extract-file <crash_report_file> <file_item_name> <out_file> Extracts\n");
	m_pLog->write(nLevel, "                       <file_item_name> from <crash_report_file> ZIP archive\n");
	m_pLog->write(nLevel, "                       and saves the file as <out_file> file.\n");
	m_pLog->write(nLevel, "\n");

}

int CCommandProcessor::ReadDump(LPCSTR szFileName, LPCSTR szOutFile)
{
    m_sErrorMsg = "Unspecified error";
	FILE* f = NULL;

	if(szOutFile==NULL)
		f = stdout;
	else
	{
#ifdef _WIN32
		fopen_s(&f, szOutFile, "wt");
#else
		f = fopen(szOutFile, "wt");
#endif
	}
	if(f==NULL)
		return 1;

	CMiniDumpReader dmp;
	std::wstring sFileName = strconv::a2w(szFileName);
	BOOL bRead = dmp.Init(sFileName.c_str());
	if(!bRead)
	{
		m_sErrorMsg = "Couldn't read minidump!\n";
		return 1;
	}

	MiniDumpHeaderInfo* pHeader = dmp.GetHeaderInfo();
	fprintf(f, "\n== Header Info ==\n\n");
	fprintf(f, "Version = 0x%X (0x%X)\n", pHeader->m_uVersion, pHeader->m_uVerImplSpecific);

	char szTime[64]="";
#ifdef _WIN32
	struct tm local_time;
	localtime_s(&local_time, &pHeader->m_TimeDateStamp);
	strftime(szTime, 64, "%c", &local_time);
#else
	struct tm* local_time = localtime(&pHeader->m_TimeDateStamp);
	strftime(szTime, 64, "%c", local_time);
#endif
	fprintf(f, "Timestamp = %s\n", szTime);
	fprintf(f, "Checksum = 0x%X\n", pHeader->m_uChecksum);

	std::wstring sDmpType = CMiniDumpReader::MiniDumpTypeToStr((ULONG)pHeader->m_uMinidumpType);
	fprintf(f, "Minidump type = 0x%llX (%s)\n",
		(unsigned long long)pHeader->m_uMinidumpType,
		strconv::w2a(sDmpType).c_str());
	fprintf(f, "Stream count = %d\n", pHeader->m_uStreamCount);

	fprintf(f, "\n== Stream Directory ==\n\n");
	int i;
	for(i=0; i<dmp.GetStreamCount(); i++)
	{
		MiniDumpStreamInfo* psi = dmp.GetStreamInfo(i);

		fprintf(f, "%d. Type = %d (%s) RVA = %u Size = %u\n",
			i,
			psi->m_uStreamType,
			strconv::w2a(CMiniDumpReader::StreamTypeToStr(psi->m_uStreamType)).c_str(),
			psi->m_uDataRva,
			psi->m_uDataSize);
	}

	fprintf(f, "\n== Minidump File Layout ==\n\n");

	ULONG64 uRva = 0;
	for(i=0; i<dmp.GetLayoutBlockCount(); i++)
	{
		MiniDumpBlockInfo* pBlock = dmp.GetLayoutBlock(i);

		if(uRva<pBlock->m_uRVA)
		{

			fprintf(f, "BlockType = UnusedSpace RVA = %u Size = %u\n",
				uRva,
				pBlock->m_uRVA);
		}
		else if(uRva>pBlock->m_uRVA)
		{
			fprintf(f, "BlockType = Overlapping RVA = %u Size = %u\n",
				uRva,
				uRva-pBlock->m_uRVA);
		}

		uRva = pBlock->m_uRVA+pBlock->m_uSize;

		fprintf(f, "BlockType = %s RVA = %u Size = %u\n",
			strconv::w2a(pBlock->m_sId).c_str(),
			pBlock->m_uRVA,
			pBlock->m_uSize);
	}

	fprintf(f, "\n== System Info ==\n\n");

	MiniDumpSystemInfo* pSysInfo = dmp.GetSystemInfo();

	fprintf(f, "Processor architecture = %u (%s)\n",
		pSysInfo->m_uProcessorArchitecture,
		strconv::w2a(CMiniDumpReader::ProcessorArchitectureToStr(pSysInfo->m_uProcessorArchitecture)).c_str());
	fprintf(f, "Processor level = %u\n", pSysInfo->m_uProcessorLevel);
	fprintf(f, "Processor revision = %u\n", pSysInfo->m_uProcessorRevision);
	fprintf(f, "Number of processors = %d\n", pSysInfo->m_uchNumberOfProcessors);
	fprintf(f, "OS version = %u.%u.%u %s\n",
		pSysInfo->m_ulVerMajor,
		pSysInfo->m_ulVerMinor,
		pSysInfo->m_ulVerBuild,
		strconv::w2a(pSysInfo->m_sServicePack).c_str());
	fprintf(f, "Product type = %d (%s)\n",
		pSysInfo->m_uchProductType,
		strconv::w2a(CMiniDumpReader::ProductTypeToStr(pSysInfo->m_uchProductType)).c_str());
	fprintf(f, "Platform ID = %d\n", pSysInfo->m_uPlatformId);

	fprintf(f, "\n== Misc Info ==\n\n");
	MiniDumpMiscInfo* pMiscInfo = dmp.GetMiscInfo();
	if(pMiscInfo->m_bProcessIdValid)
	{
		fprintf(f, "Process ID = %d\n", pMiscInfo->m_uProcessId);
	}
	if(pMiscInfo->m_bTimesValid)
	{
		char szTime[64]="";
#ifdef _WIN32
		struct tm local_time;
		localtime_s(&local_time, &pMiscInfo->m_ProcessCreateTime);
		strftime(szTime, 64, "%c", &local_time);
#else
		struct tm* local_time = localtime(&pMiscInfo->m_ProcessCreateTime);
		strftime(szTime, 64, "%c", local_time);
#endif
		fprintf(f, "Process create time = %s\n", szTime);
		fprintf(f, "Process user time (sec.) = %u\n", pMiscInfo->m_uProcessUserTime);
		fprintf(f, "Process kernel time (sec.) = %u\n", pMiscInfo->m_uProcessKernelTime);
	}
	if(pMiscInfo->m_bPowerInfoValid)
	{
		fprintf(f, "Processor max MHz = %u\n", pMiscInfo->m_uProcessorMaxMhz);
		fprintf(f, "Processor current MHz = %u\n", pMiscInfo->m_uProcessorCurrentMhz);
	}

	fprintf(f, "\n== Exception Info ==\n\n");
	MiniDumpExceptionInfo* pExcInfo = dmp.GetExceptionInfo();
	fprintf(f, "Exception code = 0x%X\n", pExcInfo->m_uExceptionCode);
	fprintf(f, "Exception address = %d (0x%d)\n", pExcInfo->m_uExceptionAddress, pExcInfo->m_uExceptionAddress);
	fprintf(f, "Thread ID = %u\n", pExcInfo->m_uThreadId);
	fprintf(f, "Thread context RVA = %u\n", pExcInfo->m_uExceptionThreadContextRva);

	fprintf(f, "\n== Thread List ==\n\n");

	for(i=0; i<dmp.GetThreadCount(); i++)
	{
		MiniDumpThreadInfo* pti = dmp.GetThreadInfo(i);

		fprintf(f, "%d. Thread ID = %u Teb=%d StackStart=%d StackSize=%u CtxRva=%u\n",
			i+1, pti->m_uThreadId, pti->m_u64Teb, pti->m_uStackStart, pti->m_uStackSize, pti->m_uThreadContextRva);
	}

	fprintf(f, "\n== Memory List ==\n\n");

	for(i=0; i<dmp.GetMemRangeCount(); i++)
	{
		MiniDumpMemRange* pmr = dmp.GetMemRangeInfo(i);

		fprintf(f, "%d. Start = %d Size=%u\n",
			i+1, pmr->m_uStart, pmr->m_uSize);
	}

    fprintf(f, "\n= END =\n");
    m_sErrorMsg = "Success";

	return 0;
}

#ifdef _WIN32

int CCommandProcessor::WriteDump(LPCSTR szOutFile)
{
    m_sErrorMsg = "Unspecified error";

	DWORD dwProcessId = 0;
	DWORD dwThreadId = 0;
	EXCEPTION_POINTERS* pExcPtrs = NULL;
	std::wstring sFileName = GetModulePath(NULL)+_T("\\crash.ini");
	const int BUFF_SIZE=1024;
	TCHAR szBuff[BUFF_SIZE] = _T("");

	GetPrivateProfileString(_T("Crash"), _T("ProcessId"), _T("0"), szBuff, BUFF_SIZE, sFileName.c_str());
	_stscanf_s(szBuff, _T("%lu"), &dwProcessId );

	GetPrivateProfileString(_T("Crash"), _T("ThreadId"), _T("0"), szBuff, BUFF_SIZE, sFileName.c_str());
	_stscanf_s(szBuff, _T("%lu"), &dwThreadId );

	GetPrivateProfileString(_T("Crash"), _T("ExcPtrs"), _T("0"), szBuff, BUFF_SIZE, sFileName.c_str());
	_stscanf_s(szBuff, _T("%p"), &pExcPtrs );

	IMiniDumpWriter* pDmpWriter = NULL;
	CMiniDumpWriterFactory::CreateInstance(&pDmpWriter);
	std::wstring sOutFile = strconv::a2w(szOutFile);
	pDmpWriter->Write(dwProcessId, dwThreadId, pExcPtrs, sOutFile.c_str());
	delete pDmpWriter;

    m_sErrorMsg = "Success";

	return 0;
}

int CCommandProcessor::WriteDumpDbgHelp(LPCSTR szOutFile)
{
    m_sErrorMsg = "Unspecified error";
	m_pLog->write(0, "Reading config information from crash.ini file...\n");

	DWORD dwProcessId = 0;
	DWORD dwThreadId = 0;
	EXCEPTION_POINTERS* pExcPtrs = NULL;
	std::wstring sFileName = GetModulePath(NULL)+_T("\\crash.ini");
	const int BUFF_SIZE=1024;
	TCHAR szBuff[BUFF_SIZE] = _T("");

	GetPrivateProfileString(_T("Crash"), _T("ProcessId"), _T("0"), szBuff, BUFF_SIZE, sFileName.c_str());
	_stscanf_s(szBuff, _T("%lu"), &dwProcessId );

	GetPrivateProfileString(_T("Crash"), _T("ThreadId"), _T("0"), szBuff, BUFF_SIZE, sFileName.c_str());
	_stscanf_s(szBuff, _T("%lu"), &dwThreadId );

	GetPrivateProfileString(_T("Crash"), _T("ExcPtrs"), _T("0"), szBuff, BUFF_SIZE, sFileName.c_str());
	_stscanf_s(szBuff, _T("%p"), &pExcPtrs );

	_tprintf(_T("Writing minidump file '%s' using dbghelp MiniDumpWriteDump API...\n"), szOutFile);

	CDbgHelpDumpWriter dmp;
	std::wstring sOutFile = strconv::a2w(szOutFile);
	BOOL bWrite = dmp.Write(dwProcessId, dwThreadId, pExcPtrs, sOutFile.c_str());
	if(bWrite)
	{
		_tprintf(_T("Finished writing minidump file... OK.\n"));
	}
	else
	{
		_tprintf(_T("Finished writing minidump file... Failed.\n"));
		_tprintf(_T("Error message: %s\n"), dmp.GetErrorMsg());
	}

    m_sErrorMsg = "Success";
	return 0;
}

#endif

int CCommandProcessor::ReadPdb(LPCSTR szFileName, LPCSTR szOutFile)
{
    m_sErrorMsg = "Unspecified error";
	FILE* f = NULL;

	if(szOutFile==NULL)
		f = stdout;
	else
	{
#ifdef _WIN32
		fopen_s(&f, szOutFile, "wt");
#else
		f = fopen(szOutFile, "wt");
#endif

	}
	if(f==NULL)
		return 1;

	CMsfFile MsfFile;
	std::wstring sFileName = strconv::a2w(szFileName);
	BOOL bRead = MsfFile.Init(sFileName.c_str());
	if(!bRead)
	{
		m_sErrorMsg = "Couldn't read PDB file.";
        m_pLog->write(0, "Couldn't read PDB file!\n");
		return 1;
	}

    m_sErrorMsg = "Success";
	return 0;
}

int CCommandProcessor::ExtractPdbStream(LPCSTR szPdbFileName, int nStreamId, LPCSTR szOutFile)
{
    m_sErrorMsg = "Unspecified error";
	int nStatus = 1;
	FILE* f = NULL;
	BYTE buf[1024];
	DWORD dwBytesRead = 0;
	CMsfFile PdbFile;
	CMsfStream* pStream = NULL;
	BOOL bRead = FALSE;
	std::wstring sPdbFileName = strconv::a2w(szPdbFileName);
	std::wstring sOutFile = strconv::a2w(szOutFile);

	if(szOutFile==NULL)
		goto cleanup; // Invalid output file name

	bRead = PdbFile.Init(sPdbFileName.c_str());
	if(!bRead)
	{
		m_pLog->write(0, "Couldn't read PDB file!\n");
        m_sErrorMsg = "Couldn't read PDB file";
		goto cleanup;
	}

	pStream = PdbFile.GetStream(nStreamId);
	if(pStream==NULL)
	{
		m_pLog->write(0, "Couldn't get PDB stream %d!\n", nStreamId);
        m_sErrorMsg = "Couldn't get PDB stream";
		goto cleanup;
	}

#ifdef _WIN32
	fopen_s(&f, szOutFile, "wb");
#else
	f = fopen(szOutFile, "wb");
#endif
	if(f==NULL)
		goto cleanup; // Out file opening error

	pStream->SetStreamPos(0);
	for(;;)
	{
		BOOL bReadData = pStream->ReadData(buf, 1024, &dwBytesRead, TRUE);
		if(!bReadData || dwBytesRead==0)
			break;

		if(!fwrite(buf, dwBytesRead, 1, f)==1)
		{
			goto cleanup;
		}
	}

	m_pLog->write(0, "%d bytes copied.\n", pStream->GetStreamLen());
    m_sErrorMsg = "Success";
	nStatus = 0;

cleanup:

	if(f!=NULL)
	{
		fclose(f);
		f=NULL;
	}

    m_sErrorMsg = "Success";
	return nStatus;
}

int CCommandProcessor::ExtractPdbStreams(LPCSTR szPdbFileName, LPCSTR szOutDir)
{
    m_sErrorMsg = "Unspecified error";
	int nStatus = 1;
	FILE* f = NULL;
	BYTE buf[1024];
	DWORD dwBytesRead = 0;
	CMsfFile MsfFile;
	CMsfStream* pStream = NULL;
	BOOL bRead = FALSE;
	int nStreamCount = 0;
	int i;
	std::wstring sPdbFileName = strconv::a2w(szPdbFileName);

	if(szOutDir==NULL)
	{
		m_pLog->write(0, "Invalid output directory name specified!\n");
        m_sErrorMsg = "Invalid output directory name specified";
		goto cleanup;
	}

#ifdef _WIN32
	_mkdir(szOutDir);
#else
	if(0!=mkdir(szOutDir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) && errno!=EEXIST)
	{
	    m_pLog->write(0, "Error creating output directory '%s'\n", szOutDir);
        m_sErrorMsg = "Error creating output directory";
	    perror(NULL);
        goto cleanup;
	}
#endif

	bRead = MsfFile.Init(sPdbFileName.c_str());
	if(!bRead)
	{
		m_pLog->write(0, "Couldn't read PDB file!\n");
        m_sErrorMsg = "Couldn't read PDB file";
		goto cleanup;
	}

	nStreamCount = MsfFile.GetStreamCount();

	for(i=0; i<nStreamCount; i++)
	{
		pStream = MsfFile.GetStream(i);
		if(pStream==NULL)
		{
			m_pLog->write(0, "Couldn't get PDB stream %d!\n", i);
            m_sErrorMsg = "Couldn't get PDB stream";
			goto cleanup;
		}

		char szBuf[MAX_PATH]="";
#ifdef _WIN32
		sprintf_s(szBuf, MAX_PATH, "%s\\stream%03d.dat", szOutDir, i);
#else
		sprintf(szBuf, "%s/stream%03d.dat", szOutDir, i);
#endif
		std::string sOutFile = szBuf;

#ifdef _WIN32
		fopen_s(&f, sOutFile.c_str(), "wb");
#else
		f = fopen(sOutFile.c_str(), "wb");
#endif
		if(f==NULL)
		{
			m_pLog->write(0, "Error opening file for writing: %s!\n", sOutFile.c_str());
            m_sErrorMsg = "Error opening file for writing";
			goto cleanup; // Out file opening error
		}

		pStream->SetStreamPos(0);
		for(;;)
		{
			BOOL bReadData = pStream->ReadData(buf, 1024, &dwBytesRead, TRUE);
			if(!bReadData || dwBytesRead==0)
				break;

			if(!fwrite(buf, dwBytesRead, 1, f)==1)
			{
				m_pLog->write(0, "Error writing to file: %s!\n", sOutFile.c_str());
                m_sErrorMsg = "Error writing to file";
				goto cleanup;
			}
		}

		m_pLog->write(0, "%d bytes copied to file %s.\n", pStream->GetStreamLen(), sOutFile.c_str());
        m_sErrorMsg = "Success";

		fclose(f);
		f=NULL;

	}

    m_sErrorMsg = "Success";
	nStatus = 0;

cleanup:

	if(f!=NULL)
	{
		fclose(f);
		f=NULL;
	}

	return nStatus;
}

int CCommandProcessor::Dia2Dump(LPCSTR szPdbFileName, LPCSTR szOutFile)
{
    m_sErrorMsg = "Unspecified error";
	int nStatus = 1;
	FILE* f = NULL;
	CPdbReader PdbReader;
	BOOL bRead = FALSE;
	int i;
	std::wstring sPdbFileName = strconv::a2w(szPdbFileName);
	std::wstring sOutFile = strconv::a2w(szOutFile);
	int nSymCount = -1;
	CPdbSymbolStream* pSymStream = NULL;
	CPdbPSGSIStream* pPSI = NULL;
	int nModuleCount = -1;
	CPdbDebugInfoStream* pDBI = NULL;

	if(szOutFile==NULL)
	{
		m_pLog->write(0, "Invalid output file name specified!\n");
        m_sErrorMsg = "Invalid output file name specified";
		goto cleanup;
	}

	bRead = PdbReader.Init(sPdbFileName.c_str());
	if(!bRead)
	{
		m_pLog->write(0, "Couldn't read PDB file!\n");
        m_sErrorMsg = "Couldn't read PDB file";
		goto cleanup;
	}

#ifdef _WIN32
	fopen_s(&f, szOutFile, "wt");
#else
	f = fopen(szOutFile, "wt");
#endif
	if(f==NULL)
	{
		m_pLog->write(0, "Couldn't open output file for writing!\n");
        m_sErrorMsg = "Couldn't open output file for writing";
		goto cleanup;
	}

	/* MODULES */

	fprintf(f, "*** MODULES\n\n");

	pDBI = PdbReader.GetDebugInfoStream();
	if(!pDBI)
	{
		m_pLog->write(0, "Couldn't read DBI stream!\n");
        m_sErrorMsg = "Couldn't read DBI stream";
		goto cleanup;
	}

	nModuleCount = pDBI->GetModuleCount();
	for(i=0; i<nModuleCount; i++)
	{
		DBI_ModuleInfo* pModuleInfo = pDBI->GetModuleInfo(i);
		if(!pModuleInfo)
		{
			m_pLog->write(0, "Couldn't read module info!\n");
            m_sErrorMsg = "Couldn't read module info";
		}

		std::wstring sModuleName = strconv::a2w(pModuleInfo->m_sSrcModuleName);
		fprintf(f, "%03X  %04u  %08x  %s\n", i+1, pModuleInfo->m_Info.ifileMac, pModuleInfo->m_Info.mpifileichFile, strconv::w2a(sModuleName).c_str());
	}

	/* PUBLICS */

	fprintf(f, "\n\n*** PUBLICS\n\n");

	pPSI = PdbReader.GetPSIStream();

	pSymStream = PdbReader.GetSymbolStream();
	nSymCount = pPSI->GetAddrEntryCount();
	for(i=0; i<nSymCount; i++)
	{
		DWORD dwOffs = 0;
		if(!pPSI->GetAddrEntry(i, &dwOffs))
		{
			assert(0);
			continue;
		}

		int nSymIndex = pSymStream->GetSymbolIndexByOffsInStream(dwOffs);
		if(nSymIndex<0)
		{
			assert(nSymIndex>=0);
			continue;
		}
		CPdbSymbol* pSym = pSymStream->GetSymbol(i);

		std::string sType = pSym->GetTypeStr();
		std::string sName = pSym->GetName();

		CPdbSectionMapStream* pSectionStream = PdbReader.GetSectionMapStream();

		DWORD64 dwRVA = 0;

		int nSection = (int)pSym->GetSegment()-1;
		if(nSection>=0)
		{
			IMAGE_SECTION_HEADER* pSection = pSectionStream->GetSection(nSection);
			dwRVA = pSection->VirtualAddress+pSym->GetOffset();
		}

		fprintf(f, "%s: [%d][%d][%04x:%08x] %s\n", sType.c_str(), dwRVA,
			dwRVA, pSym->GetSegment(), pSym->GetOffset(), sName.c_str());
	}

	fprintf(f, "\n\n*** COMPILANDS\n\n");

	for(i=0; i<nModuleCount; i++)
	{
		DBI_ModuleInfo* pModuleInfo = pDBI->GetModuleInfo(i);

		fprintf(f, "\nCompiland num=%d name = %s\n\n", pModuleInfo->m_Info.wStreamNumber, pModuleInfo->m_sSrcModuleName.c_str());

		CPdbCompilandStream* pStream = PdbReader.GetCompilandStream(pModuleInfo);

		if(pModuleInfo->m_Info.wStreamNumber==65535)
			continue;

		int nCount = pStream->GetSrcFileCheckSumCount();
		int j;
		for(j=0; j<nCount; j++)
		{
			SrcChecksum* pChecksum = pStream->GetSrcFileCheckSum(j);

			const wchar_t* szFileName = NULL;
			int nFileNameIndex = pDBI->FindSrcFileNameByOffs(pChecksum->m_Info.dwNameOffset);
			if(nFileNameIndex>=0)
				szFileName = pDBI->GetSrcFileName(nFileNameIndex);

			fprintf(f, "%d. offs = %d checksum = %s srcfile=%s\n", i+1, pChecksum->m_Info.dwNameOffset,
				pChecksum->m_sCheckSum.c_str(), szFileName?strconv::w2a(szFileName).c_str():"(notfound)");
		}
	}



	fclose(f);
	f=NULL;

    m_sErrorMsg = "Success";
	nStatus = 0;

cleanup:

	if(f!=NULL)
	{
		fclose(f);
		f=NULL;
	}

	return nStatus;
}

int CCommandProcessor::DumpCrashReport(LPCWSTR szCrashRptFileName, LPCWSTR szOutFile, LPCWSTR szSymbolSearchDir, bool bExactMatchBuildAge)
{
    m_sErrorMsg = "Unspecified error";
	int nStatus = 1;
	FILE* f = NULL;
	CCrashReportReader CrashRptReader;
	BOOL bRead = FALSE;
	std::wstring sCrashRptFileName;
	std::wstring sOutFile;
	CCrashDescReader* pCrashDesc = NULL;
	CMiniDumpReader* pMiniDump = NULL;
	char szBuffer[1024]="";
	COutputter doc;
	std::string sUtf8OutFile;
    MiniDumpExceptionInfo* pExcInfo = NULL;
    MiniDumpSystemInfo* pSysInfo = NULL;
	std::wstring sStackTrace;

	if(m_pLog==NULL)
	{		
        m_sErrorMsg = "Log is not specified";
		goto cleanup;
	}

	if(szCrashRptFileName==NULL)
	{
		m_pLog->write(0, "Invalid src file name specified!\n");
        m_sErrorMsg = "Invalid src file name specified";
		goto cleanup;
	}

	if(szOutFile==NULL)
	{
		m_pLog->write(0, "Invalid output file name specified!\n");
        m_sErrorMsg = "Invalid output file name specified";
		goto cleanup;
	}

	if(szSymbolSearchDir!=NULL)
	{
		std::wstring sSearchDir(szSymbolSearchDir);
		m_pPdbCache->AddPdbSearchDir(sSearchDir, PDB_USUAL_DIR, true);
	}

	sUtf8OutFile = strconv::w2a(szOutFile);
	sCrashRptFileName = szCrashRptFileName;
	sOutFile = szOutFile;

	if(m_pPdbCache==NULL)
	{
		m_pLog->write(0, "Debug info cache is not specified!\n");
        m_sErrorMsg = "Debug info cache is not specified";
		goto cleanup;
	}

	bRead = CrashRptReader.Init(sCrashRptFileName);
	if(!bRead)
	{
		m_pLog->write(0, "Couldn't read crash report file!\n");
        m_sErrorMsg = "Couldn't read crash report file: ";
		m_sErrorMsg += strconv::w2a(CrashRptReader.GetErrorMsg());
		goto cleanup;
	}

	pCrashDesc = CrashRptReader.GetCrashDescReader();
	pMiniDump = CrashRptReader.GetMiniDumpReader();
	if(pMiniDump!=NULL)
	{
		pExcInfo = pMiniDump->GetExceptionInfo();
		pSysInfo = pMiniDump->GetSystemInfo();
	}

	if(pSysInfo && pSysInfo->m_uProcessorArchitecture != PROCESSOR_ARCHITECTURE_INTEL)
	{
		m_pLog->write(0, "Not supported CPU architecture of crash report file!\n");
        m_sErrorMsg = "Unsupported CPU architecture";
		goto cleanup;
	}

#ifdef _WIN32
	fopen_s(&f, strconv::w2a(szOutFile).c_str(), "wt");
#else
	f = fopen(sUtf8OutFile.c_str(), "wt");
#endif
	if(f==NULL)
	{
		m_pLog->write(0, "Couldn't open output file for writing!\n");
        m_sErrorMsg = "Couldn't open output file for writing";
		goto cleanup;
	}

	doc.Init(f, OUTPUT_XML);

	doc.BeginDocument("ErrorReport");

	doc. BeginSection("Summary");
	
	// Print CrashRpt version
#ifdef _WIN32
	sprintf_s(szBuffer, 1024, "%d", pCrashDesc->GetGeneratorVersion());
#else
    sprintf(szBuffer, "%d", pCrashDesc->GetGeneratorVersion());
#endif
	doc.PutRecord("GeneratorVersion", "%s", szBuffer);

	{
		// Print CrashGUID
		std::wstring sCrashGUID = pCrashDesc->GetCrashGUID();
		doc.PutRecord("CrashGUID", "%s", strconv::w2a(sCrashGUID).c_str());
	}

	// Print app name
	doc.PutRecord("ApplicationName", "%s", strconv::w2a(pCrashDesc->GetAppName()).c_str());

	// Print image name
	doc.PutRecord("ExecutableImage", "%s", strconv::w2a(pCrashDesc->GetImageName()).c_str());

	// Print app version
	doc.PutRecord("ApplicationVersion", "%s", strconv::w2a(pCrashDesc->GetAppVersion()).c_str());

	// Print creation date
	doc.PutRecord("DateCreatedUTC", "%s", strconv::w2a(pCrashDesc->GetCreationDate()).c_str());

	// Print OS friendly name
	doc.PutRecord("OSNameReg", "%s", strconv::w2a(pCrashDesc->GetOSFriendlyName()).c_str());

	if(pSysInfo)
	{
		// Print OS version
		doc.PutRecord("OSVersionMinidump", "%lu.%lu.%lu %s",
				pSysInfo->m_ulVerMajor,
				pSysInfo->m_ulVerMinor,
				pSysInfo->m_ulVerBuild,
				strconv::w2a(pSysInfo->m_sServicePack).c_str());
	}

	// Print OS 64-bit
	doc.PutRecord("OSIs64Bit", "%d", pCrashDesc->IsOS64Bit()?1:0);

	// Print geo location
	doc.PutRecord("GeographicLocation", "%s", strconv::w2a(pCrashDesc->GetGeoLocation()).c_str());

	if(pSysInfo)
	{
		// Print product type
		doc.PutRecord("ProductType", "%u %s", pSysInfo->m_uchProductType,
			strconv::w2a(pMiniDump->ProductTypeToStr(pSysInfo->m_uchProductType)).c_str());

		// Print CPU architecture
		doc.PutRecord("CPUArchitecture", "%u %s", pSysInfo->m_uProcessorArchitecture,
			strconv::w2a(pMiniDump->ProcessorArchitectureToStr(pSysInfo->m_uProcessorArchitecture)).c_str());

		doc.PutRecord("CPUCount", "%d", pSysInfo->m_uchNumberOfProcessors);

	}
	
	doc.PutRecord("GUIResourceCount", "%s", strconv::w2a(pCrashDesc->GetGUIResourceCount()).c_str());

	doc.PutRecord("OpenHandleCount", "%s", strconv::w2a(pCrashDesc->GetOpenHandleCount()).c_str());

	doc.PutRecord("MemoryUsageKbytes", "%s", strconv::w2a(pCrashDesc->GetMemoryUsage()).c_str());

	doc.PutRecord("ExceptionType", "%u %s", pCrashDesc->GetExceptionType(),
		strconv::w2a(pCrashDesc->ExceptionTypeToStr(pCrashDesc->GetExceptionType())).c_str());

	if(pExcInfo)
	{
		doc.PutRecord("ExceptionAddress", "%llu", pExcInfo->m_uExceptionAddress);

		doc.PutRecord("SEHExceptionCode", "%lu", pExcInfo->m_uExceptionCode);

		doc.PutRecord("ExceptionThreadID", "%u", pExcInfo->m_uThreadId);

		int nModule = pMiniDump->FindModuleIndexByAddr(pExcInfo->m_uExceptionAddress);
		MiniDumpModuleInfo* pmi = pMiniDump->GetModuleInfo(nModule);
		if(pmi)
		{
			std::wstring sModuleName = pmi!=NULL?pmi->m_sShortModuleName:L"";
			doc.PutRecord("ExceptionModuleName", "%s", strconv::w2a(sModuleName).c_str());
			doc.PutRecord("ExceptionModuleBase", "%llu", pmi->m_dwBaseAddr);
		}
	}

	doc.PutRecord("UserEmail", "%s", strconv::w2a(pCrashDesc->GetUserEmail()).c_str());

	doc.PutRecord("ProblemDescription", "%s", strconv::w2a(pCrashDesc->GetProblemDescription()).c_str());

	doc.EndSection();

	doc.BeginSection("ApplicationDefinedProperties");

	// Print custom property list
	doc.BeginTableRow();
	doc.PutTableCell(2, false, "%s", "No");
	doc.PutTableCell(16, false, "%s", "Name");
	doc.PutTableCell(32, true, "%s", "Value");
	doc.EndTableRow();

	int i;
	for(i=0; i<pCrashDesc->GetCustomPropCount(); i++)
	{
		CustomProp* pcp = pCrashDesc->GetCustomProp(i);
		doc.BeginTableRow();
		doc.PutTableCell(2, false, "%d", i+1);
		doc.PutTableCell(16, false, "%s",  strconv::w2a(pcp->m_sPropName).c_str());
		doc.PutTableCell(32, true, "%s",  strconv::w2a(pcp->m_sPropVal).c_str());
		doc.EndTableRow();
	}

	doc.EndSection();

	doc.BeginSection("FileList");

	// Print file list
	doc.BeginTableRow();
	doc.PutTableCell(2, false, "%s", "No");
	doc.PutTableCell(16, false, "%s", "Name");
	doc.PutTableCell(32, true, "%s", "Description");
	doc.EndTableRow();

	for(i=0; i<pCrashDesc->GetFileItemCount(); i++)
	{
		FileItem* pfi = pCrashDesc->GetFileItem(i);
		if(pfi==NULL)
			break;

		doc.BeginTableRow();
		doc.PutTableCell(2, false, "%d", i+1);
		doc.PutTableCell(16, false, "%s",  strconv::w2a(pfi->m_sFileName).c_str());
		doc.PutTableCell(32, true, "%s",  strconv::w2a(pfi->m_sFileDesc).c_str());
		doc.EndTableRow();
	}

	doc.EndSection();

	if(pMiniDump)
	{
		for(i=0; i<pMiniDump->GetThreadCount(); i++)
		{
			MiniDumpThreadInfo* pThread = pMiniDump->GetThreadInfo(i);
			if(!pThread)
				break;

			doc.BeginSection("StackTrace");
			doc.PutRecord("ThreadID", "%d", pThread->m_uThreadId);
			doc.BeginTableRow();
			doc.PutTableCell(32, false, "%s", "Title");
			doc.PutTableCell(12, false, "%s", "AddrPC");
			doc.PutTableCell(32, false, "%s", "ModuleName");
			doc.PutTableCell(12, false, "%s", "OffsInModule");
			doc.PutTableCell(32, false, "%s", "SymbolName");
			doc.PutTableCell(32, false, "%s", "UndecoratedSymbolName");
			doc.PutTableCell(32, false, "%s", "OffsInSymbol");
			doc.PutTableCell(24, false, "%s", "SrcFile");
			doc.PutTableCell(5, false, "%s", "SrcLine");
			doc.PutTableCell(5, true, "%s", "OffsInLine");
			doc.EndTableRow();

			BOOL bUnwindNotAvail = FALSE;

			CStackWalker StackWalker;
			bool bInit = StackWalker.Init(pMiniDump, m_pPdbCache, pThread->m_uThreadId, bExactMatchBuildAge);
			if(bInit)
			{
				sStackTrace.clear();
				BOOL bGetFrame = StackWalker.FirstStackFrame();
				while(bGetFrame)
				{
					CStackFrame* pStackFrame = StackWalker.GetStackFrame();
					if(!pStackFrame)
						break;

					std::wstring sFrameTitle;

					if(!pStackFrame->m_sModuleName.empty())
					{
						sFrameTitle += pStackFrame->m_sModuleName;
						sFrameTitle += L"!";
						sprintf(szBuffer, "%u ", pStackFrame->m_dwOffsInModule);
						sFrameTitle += strconv::utf82w(szBuffer);
					}
					else
					{
						sprintf(szBuffer, "0x%d ", pStackFrame->m_dwAddrPC);
						sFrameTitle += strconv::a2w(szBuffer);
					}

					if(!pStackFrame->m_sSymbolName.empty())
					{
						sFrameTitle += strconv::a2w(pStackFrame->m_sSymbolName);
					}

					if(!pStackFrame->m_sSrcFileName.empty())
					{
						sprintf(szBuffer, "[%s %d]", strconv::w2a(pStackFrame->m_sSrcFileName).c_str(), pStackFrame->m_nSrcLineNumber);
						sFrameTitle += strconv::a2w(szBuffer);
					}

					if(!pStackFrame->m_sSymbolName.empty())
					{
						sStackTrace += sFrameTitle;
						sStackTrace += L"\n";
					}

					doc.BeginTableRow();
					doc.PutTableCell(32, false, "%s", strconv::w2utf8(sFrameTitle).c_str());
					doc.PutTableCell(12, false, "%llu", pStackFrame->m_dwAddrPC);
					doc.PutTableCell(32, false, "%s", strconv::w2utf8(pStackFrame->m_sModuleName).c_str());
					doc.PutTableCell(32, false, "%u", pStackFrame->m_dwOffsInModule);
					doc.PutTableCell(32, false, "%s", pStackFrame->m_sSymbolName.c_str());
					doc.PutTableCell(32, false, "%s", pStackFrame->m_sUndecoratedSymbolName.c_str());
					doc.PutTableCell(5, true, "%u", pStackFrame->m_dwOffsInSymbol);
					doc.PutTableCell(24, false, "%s", strconv::w2utf8(pStackFrame->m_sSrcFileName).c_str());
					doc.PutTableCell(5, false, "%d", pStackFrame->m_nSrcLineNumber);
					doc.PutTableCell(5, true, "%u", pStackFrame->m_dwOffsInLine);
					doc.EndTableRow();

					if(pStackFrame->m_sSymbolName.empty() && !bUnwindNotAvail)
					{
						bUnwindNotAvail = TRUE;

						doc.BeginTableRow();
						doc.PutTableCell(32, false, "%s", "[UnwindInfoNotAvailable]");
						doc.PutTableCell(12, false, "%s",  "0");
						doc.PutTableCell(32, false, "%s",  "");
						doc.PutTableCell(32, false, "%s",  "0");
						doc.PutTableCell(32, false, "%s",  "");
						doc.PutTableCell(32, false, "%s",  "");
						doc.PutTableCell(5, true, "%s",  "0");
						doc.PutTableCell(24, false, "%s",  "");
						doc.PutTableCell(5, false, "%s",  "0");
						doc.PutTableCell(5, true, "%s",  "0");
						doc.EndTableRow();
					}

					bGetFrame = StackWalker.NextStackFrame();
				}

				if(!sStackTrace.empty())
				{
					std::string sStackTraceA = strconv::w2utf8(sStackTrace);
					MD5 md5;
					MD5_CTX md5_ctx;
					unsigned char md5_hash[16];
					md5.MD5Init(&md5_ctx);
					md5.MD5Update(&md5_ctx, (unsigned char*)sStackTraceA.c_str(), (unsigned int)sStackTrace.size());
					md5.MD5Final(md5_hash, &md5_ctx);

					std::string sStackTraceMD5;
					int i;
					for(i=0; i<16; i++)
					{
						sprintf(szBuffer, "%02x", md5_hash[i]);
						sStackTraceMD5 += szBuffer;
					}
					doc.PutRecord("StackTraceMD5", "%s", sStackTraceMD5.c_str());
				}
			}

			doc.EndSection();
		}

		doc.BeginSection("ModuleList");

		doc.BeginTableRow();
		doc.PutTableCell(2, false, "%s", "No");
		doc.PutTableCell(32, false, "%s",  "Name");
		doc.PutTableCell(32, false, "%s",  "SymLoadStatus");
		doc.PutTableCell(48, false, "%s",  "LoadedPDBName");
		doc.PutTableCell(48, false, "%s",  "LoadedPDBGUIDnAge");
		doc.PutTableCell(48, false, "%s",  "FileVersion");
		doc.PutTableCell(48, false, "%s",  "TimeStamp");
		doc.PutTableCell(48, true, "%s",  "GUIDnAge");
		doc.EndTableRow();

		// Walk through minidump modules
		for(i=0; i<pMiniDump->GetModuleCount(); i++)
		{
			// Get current module info
			MiniDumpModuleInfo* pmi = pMiniDump->GetModuleInfo(i);
			if(!pmi)
				break;

			std::wstring sPdbGUIDnAge;
			CPdbReader* pPdbReader = NULL;
			CPeReader* pPeReader = NULL;
			int hEntry = -1;

			// Try to load PDB file for this module
			bool bFind = m_pPdbCache->FindPdb(pmi->GUIDnAge(), pmi->m_sPdbFileName, pmi->m_sModuleName, 
				&pPdbReader, &pPeReader, &hEntry, NULL, bExactMatchBuildAge);

			if(bFind && pPdbReader!=NULL)
			{
				// Get PDB headers stream
				CPdbHeadersStream* pHeaders = pPdbReader->GetHeadersStream();
				
				// Read GUID+Age
				if(pHeaders)
					sPdbGUIDnAge = pHeaders->GetGUIDnAge();

				//Release cache entry
				m_pPdbCache->ReleaseCacheEntry(hEntry);
			}

			doc.BeginTableRow();
			doc.PutTableCell(2, false, "%d", i+1);
			doc.PutTableCell(32, false, "%s",  strconv::w2a(pmi->m_sShortModuleName).c_str());
			doc.PutTableCell(32, false, "%s",  pPdbReader!=NULL?"1":"0");
			doc.PutTableCell(48, false, "%s",  pPdbReader!=NULL?strconv::w2utf8(pPdbReader->GetFileName()).c_str():"");
			doc.PutTableCell(48, false, "%s",  pPdbReader!=NULL?strconv::w2utf8(sPdbGUIDnAge).c_str():"");
			doc.PutTableCell(48, false, "%s",  strconv::w2utf8(pmi->m_sVersion).c_str());
			doc.PutTableCell(48, false, "%u", pmi->m_dwTimeDateStamp);
			doc.PutTableCell(48, true, "%s",  strconv::w2utf8(pmi->GUIDnAge()).c_str());
			doc.EndTableRow();
		}

		doc.EndSection();
	}

	doc.EndDocument();

    m_sErrorMsg = "Success";
	nStatus = 0;

cleanup:

	if(f!=NULL)
	{
		fclose(f);
		f=NULL;
	}

	return nStatus;
}

int CCommandProcessor::ExtractFile(LPCWSTR szCrashRptFileName, LPCWSTR szFileItemName, LPCWSTR szOutFile)
{
	m_sErrorMsg = "Unspecified error";
	int nStatus = 1;
	CCrashReportReader CrashRptReader;
	BOOL bRead = FALSE;
	std::wstring sCrashRptFileName;
	std::wstring sFileItemName;
	std::wstring sOutFile;
	BOOL bExtract = FALSE;

	if(szCrashRptFileName==NULL)
	{
		m_pLog->write(0, "Invalid src file name specified!\n");
        m_sErrorMsg = "Invalid src file name specified";
		goto cleanup;
	}

	if(szFileItemName==NULL)
	{
		m_pLog->write(0, "Invalid file item name specified!\n");
        m_sErrorMsg = "Invalid file item name specified";
		goto cleanup;
	}

	if(szOutFile==NULL)
	{
		m_pLog->write(0, "Invalid output file name specified!\n");
        m_sErrorMsg = "Invalid output file name specified";
		goto cleanup;
	}

	sCrashRptFileName = szCrashRptFileName;
	sFileItemName = szFileItemName;
	sOutFile = szOutFile;

	bRead = CrashRptReader.Init(sCrashRptFileName);
	if(!bRead)
	{
		m_pLog->write(0, "Couldn't read crash report file!\n");
        m_sErrorMsg = "Couldn't read crash report file";
		goto cleanup;
	}

	bExtract = CrashRptReader.ExtractFile(szFileItemName, szOutFile, TRUE);
	if(!bExtract)
	{
		m_pLog->write(0, "Couldn't extract an item from zipped crash report file!\n");
		m_sErrorMsg = strconv::w2a(CrashRptReader.GetErrorMsg());
		goto cleanup;
	}

	nStatus = 0;
	m_sErrorMsg = "Success.";

cleanup:

	return nStatus;
}

int CCommandProcessor::ImportPdb(LPCWSTR szPdbFileName, LPCWSTR szSymDir, LPCWSTR szOutFile)
{
	int nStatus = 1;
    m_sErrorMsg = "Unspecified error.";
	std::wstring sDstFileName;
	std::wstring sPdbFileName;
	std::wstring sOutDir;
	char slash;
	wchar_t wslash;
	CPdbReader PdbReader;
	BOOL bInit = FALSE;
	CPdbHeadersStream* pHeaders = NULL;
	std::wstring sGUID;
	DWORD dwAge = 0;
	wchar_t szBuffer[256] = L"";
	std::wstring sDir;
    std::wstring sFile;
    std::wstring sBaseFileName;
    std::wstring sExtension;
	std::wstring sSubDirName;
	int nRes = -1;

    // Validate input
    if(szPdbFileName==NULL)
    {
		nStatus = 1;
        m_sErrorMsg = "Input file name is missing.";
        goto exit;
    }

    sPdbFileName = szPdbFileName;
	sOutDir = szSymDir;

#ifdef _WIN32
    slash = '\\';
    wslash = L'\\';
    std::replace(sPdbFileName.begin(), sPdbFileName.end(), '/', '\\');
	std::replace(sOutDir.begin(), sOutDir.end(), '/', '\\');
#else
    slash = '/';
    wslash = L'/';
#endif

    if(!sOutDir.empty() && sOutDir[sOutDir.length()-1]!=slash)
        sOutDir += slash;

    // Check if PDB cache exists
    if(m_pPdbCache==NULL)
    {
        m_sErrorMsg = "Symbol cache is not initialized.";
        goto exit;
    }

    // Read PDB file
	bInit = PdbReader.Init(sPdbFileName.c_str());
    if(!bInit)
    {
		nStatus = 2;
        m_sErrorMsg = "Input file is not a valid PDB file.";
        goto exit;
    }

    // Get headers stream
    pHeaders = PdbReader.GetHeadersStream();
    if(pHeaders==NULL)
    {
		nStatus = 2;
        m_sErrorMsg = "Error retrieving headers stream from PDB file.";
        goto exit;
    }

	if(PdbReader.IsAMD64())
	{
		nStatus = 2;
        m_sErrorMsg = "Unsupported architecture.";
        goto exit;
	}

    // Get GUID
    sGUID = pHeaders->GetGUID();
	// Get age
	dwAge = pHeaders->GetAge();

    SplitFileName(szPdbFileName, sDir, sFile, sBaseFileName, sExtension);

    // Format subdir name
    sSubDirName += sOutDir;
    sSubDirName += sBaseFileName;
    sSubDirName += L".";
    sSubDirName += sExtension;
    sSubDirName += wslash;

	// Create output directory
    nRes = CreateDir(sOutDir);
	if(0!=nRes && errno!=EEXIST)
	{
		m_pLog->write(2, "Error creating directory '%s'\n", strconv::w2a(sOutDir).c_str());
		m_pLog->log_last_error(2, "CreateDirectory error");
        m_sErrorMsg = "Error creating directory";
		nStatus = 3;
		goto exit;
	}

	// Create subdirectory
    nRes = CreateDir(sSubDirName);
	if(0!=nRes && errno!=EEXIST)
	{
		m_pLog->write(2, "Error creating directory '%s'\n", strconv::w2a(sSubDirName).c_str());
		m_pLog->log_last_error(2, "CreateDirectory error");
        m_sErrorMsg = "Error creating directory";
		nStatus = 3;
		goto exit;
	}

	// Format second-level subdir name
	sSubDirName += sGUID;
	swprintf(szBuffer, 256, L"%u", dwAge);
	sSubDirName += szBuffer;
    sSubDirName += wslash;

    // Create second-level subdirectory
    nRes = CreateDir(sSubDirName);
	if(0!=nRes && errno!=EEXIST)
	{
		m_pLog->write(2, "Error creating directory '%s'\n", strconv::w2utf8(sSubDirName).c_str());
		m_pLog->log_last_error(2, "CreateDirectory error");
        m_sErrorMsg = "Error creating directory";
		nStatus = 3;
		goto exit;
	}

    // Copy file to its destination directory
    sDstFileName = sSubDirName + sFile;
    if(!copy_file(sPdbFileName, sDstFileName, true))
    {
		m_pLog->write(0, "Error copying file '%s' to '%s'\n", strconv::w2a(sPdbFileName).c_str(), strconv::w2a(sDstFileName).c_str());
		m_pLog->log_last_error(0, "CopyFile error");
        m_sErrorMsg = "Error copying file";
		nStatus = 4;
		goto exit;
    }
		
    // Done
    m_sErrorMsg = "File imported successfully.";
	nStatus = 0;

exit:

	// If needed, write operation info to an XML file.
	if(szOutFile!=NULL)
	{
		FILE* f = NULL;
		COutputter doc;
		char szBuffer[1024];

#ifdef _WIN32
		fopen_s(&f, strconv::w2a(szOutFile).c_str(), "wt");
#else
        std::string sUtf8OutFile = strconv::w2utf8(szOutFile);
		f = fopen(sUtf8OutFile.c_str(), "wt");
#endif
		if(f==NULL)
		{
			m_pLog->write(0, "Couldn't open output file for writing: %s\n", szOutFile);
			m_sErrorMsg = "Couldn't open output file for writing";
			return 4;
		}

		doc.Init(f, OUTPUT_XML);

		doc.BeginDocument("PdbInfo");
		doc.BeginSection("Summary");

		// Write operation status and error message
		std::ostringstream sMsg;
		sMsg << nStatus << " " << m_sErrorMsg;
		doc.PutRecord("Status",  "%s", sMsg.str().c_str());

		// Write source PDB filename
		doc.PutRecord("FileName",  "%s", strconv::w2utf8(szPdbFileName).c_str());

		// Write destination PDB filename
		doc.PutRecord("ImportedFileName",  "%s", strconv::w2utf8(sDstFileName).c_str());

		if(pHeaders!=NULL)
		{
			// Write GUID
			doc.PutRecord("GUID",  "%s", strconv::w2utf8(sGUID).c_str());

			// Write Age
			sprintf(szBuffer, "%d", pHeaders->GetAge());
			doc.PutRecord("Age",  "%s", szBuffer);
		}

		doc.EndSection();

		doc.EndDocument();

		fclose(f);
	}

    return nStatus;
}

int CCommandProcessor::DeleteDebugInfo(LPCWSTR szPdbFileName)
{
	int nStatus = 1;
    m_sErrorMsg = "Unspecified error.";
	std::wstring sPdbFileName = szPdbFileName;
	bool bDelete = false;

    // Validate input
    if(szPdbFileName==NULL)
    {
		nStatus = 1;
        m_sErrorMsg = "Input file name is missing.";
        goto exit;
    }

    sPdbFileName = szPdbFileName;

	if(m_pPdbCache==NULL)
	{
		m_pLog->write(0, "Debug info cache is not specified!\n");
        m_sErrorMsg = "Debug info cache is not specified";
		goto exit;
	}

	bDelete = m_pPdbCache->DeleteCachedFile(sPdbFileName, true);
	if(!bDelete)
	{
		m_sErrorMsg = "File could not be deleted or it is not in PDB cache search path.";
		nStatus = 1;
		goto exit;
	}

	m_sErrorMsg = "Success";
	nStatus = 0;

exit:

	return nStatus;
}

