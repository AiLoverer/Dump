//! \file MiniDumpReader.cpp
//! \brief Minidump reader.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "DumpStruct.h"
#include "MiniDumpReader.h"
#include "strconv.h"
#include "PeStruct.h"

CMiniDumpReader::CMiniDumpReader()
{
    m_bInitialized = false;

}

CMiniDumpReader::~CMiniDumpReader()
{
    Destroy();
}

BOOL CMiniDumpReader::Init(std::wstring sFileName)
{
    ULONG32 uOffs = 0;
    ULONG i = 0;

    if(m_bInitialized)
    {
        Destroy();
    }

    BOOL bInit = m_fm.Init(sFileName);
    if(!bInit)
        return FALSE;

	// Read header
    MINIDUMP_HEADER* pHeader = (MINIDUMP_HEADER*)
        m_fm.CreateView(0, sizeof(MINIDUMP_HEADER));

	// Check header read
	if(!pHeader)
		return FALSE;

    // Check the signature "MDMP"
    if(memcmp(&pHeader->Signature, MINIDUMP_SIGNATURE, 4)!=0)
        return FALSE;

    // This value is internal and implementation-specific
    m_HeaderInfo.m_uVerImplSpecific = HIWORD(pHeader->Version);

    // This is MINIDUMP_VERSION
    m_HeaderInfo.m_uVersion = LOWORD(pHeader->Version);

    // Read timestamp
    m_HeaderInfo.m_TimeDateStamp = pHeader->TimeDateStamp;

    m_HeaderInfo.m_uChecksum = pHeader->CheckSum;

    // Read minidump type
    m_HeaderInfo.m_uMinidumpType = pHeader->Flags;

    m_HeaderInfo.m_uStreamCount = pHeader->NumberOfStreams;
    uOffs += pHeader->StreamDirectoryRva;

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_HEADER";
    bi.m_uRVA = 0;
    bi.m_uSize = pHeader->StreamDirectoryRva;
    AddBlockInfo(bi);

    bi.m_sId = L"EndOfFile";
    bi.m_uRVA = (ULONG)m_fm.GetSize();
    bi.m_uSize = 0;
    AddBlockInfo(bi);

    // Read stream directory
    for(i=0; i<m_HeaderInfo.m_uStreamCount; i++)
    {
        MINIDUMP_DIRECTORY* pDir = (MINIDUMP_DIRECTORY*)
            m_fm.CreateView(uOffs, sizeof(MINIDUMP_DIRECTORY));

        ULONG32 uStreamType = pDir->StreamType;

        MiniDumpStreamInfo si;
        si.m_uStreamType = uStreamType;
        si.m_uDataSize = pDir->Location.DataSize;
        si.m_uDataRva = pDir->Location.Rva;
        m_aStreams.push_back(si);

        MiniDumpBlockInfo bi;
        bi.m_sId = L"MINIDUMP_DIRECTORY";
        bi.m_uRVA = uOffs;
        bi.m_uSize = sizeof(MINIDUMP_DIRECTORY);
        AddBlockInfo(bi);

        uOffs += sizeof(MINIDUMP_DIRECTORY);
    }

    // Read streams

    for(i=0; i<m_HeaderInfo.m_uStreamCount; i++)
    {
        MiniDumpStreamInfo& si = m_aStreams[i];

        switch(si.m_uStreamType)
        {
        case ThreadListStream:
            {
                if(!ReadThreadListStream(si.m_uDataRva, si.m_uDataSize))
					goto exit;
            }
            break;
        case ModuleListStream:
            {
                if(!ReadModuleListStream(si.m_uDataRva, si.m_uDataSize))
					goto exit;
            }
            break;
        case MemoryListStream:
            {
                if(!ReadMemoryListStream(si.m_uDataRva, si.m_uDataSize))
					goto exit;
            }
            break;
		case Memory64ListStream:
            {
                if(!ReadMemory64ListStream(si.m_uDataRva, si.m_uDataSize))
					goto exit;
            }
            break;
        case ExceptionStream:
            {
                if(!ReadExceptionStream(si.m_uDataRva, si.m_uDataSize))
					goto exit;
            }
            break;
        case SystemInfoStream:
            {
                if(!ReadSystemInfoStream(si.m_uDataRva, si.m_uDataSize))
					goto exit;
            }
            break;
        case MiscInfoStream:
            {
                if(!ReadMiscInfoStream(si.m_uDataRva, si.m_uDataSize))
					goto exit;
            }
            break;
        case UnusedStream:
            {
                // Do nothing
            }
            break;
        default:
            {
				// Unknown stream
                //assert(0);
            }
            break;
        }
    }

    m_bInitialized = TRUE;

exit:

    return m_bInitialized;
}

BOOL CMiniDumpReader::IsInitialized()
{
    return m_bInitialized;
}

void CMiniDumpReader::Destroy()
{
    m_bInitialized = FALSE;
    m_fm.Destroy();
    m_aStreams.clear();
    m_aModules.clear();
    m_aThreads.clear();
    m_ThreadIndex.clear();
    m_aMemRanges.clear();
    m_Layout.clear();
}

BOOL CMiniDumpReader::AddBlockInfo(MiniDumpBlockInfo& bi)
{
    if(bi.m_uSize==0)
        return FALSE;

    m_Layout.insert(std::pair<ULONG, MiniDumpBlockInfo>(bi.m_uRVA, bi));

    return TRUE;
}

BOOL CMiniDumpReader::ReadSystemInfoStream(ULONG uRva, ULONG uSize)
{
    //LPBYTE pInfo = (LPBYTE)m_fm.CreateView(684, 1024);

    MINIDUMP_SYSTEM_INFO* pSysInfo = (MINIDUMP_SYSTEM_INFO*)m_fm.CreateView(uRva, uSize);

    m_SysInfo.m_uProcessorArchitecture = pSysInfo->ProcessorArchitecture;
    m_SysInfo.m_uProcessorLevel = pSysInfo->ProcessorLevel;
    m_SysInfo.m_uProcessorRevision = pSysInfo->ProcessorRevision;
    m_SysInfo.m_uchNumberOfProcessors = pSysInfo->NumberOfProcessors;
    m_SysInfo.m_uchProductType = pSysInfo->ProductType;
    m_SysInfo.m_ulVerMajor = pSysInfo->MajorVersion;
    m_SysInfo.m_ulVerMinor = pSysInfo->MinorVersion;
    m_SysInfo.m_ulVerBuild = pSysInfo->BuildNumber;
    m_SysInfo.m_uPlatformId = pSysInfo->PlatformId;

    if(pSysInfo->ProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL)
    {
        std::string sVendorId = std::string((char*)pSysInfo->Cpu.X86CpuInfo.VendorId, 12);
        m_SysInfo.m_sX86ProcessorVendorId = strconv::a2w(sVendorId);
        m_SysInfo.m_uX86ProcessorStepping = pSysInfo->Cpu.X86CpuInfo.VersionInformation&0x0F;
        m_SysInfo.m_uX86ProcessorModel =
            ((pSysInfo->Cpu.X86CpuInfo.VersionInformation&0xF0)>>4) +
            ((pSysInfo->Cpu.X86CpuInfo.VersionInformation&0xF0000)>>16); // extended model
        m_SysInfo.m_uX86ProcessorFamily =
            ((pSysInfo->Cpu.X86CpuInfo.VersionInformation&0x0F00)>>8) +
            ((pSysInfo->Cpu.X86CpuInfo.VersionInformation&0xF0000)>>20); // extended family
        m_SysInfo.m_uX86ProcessorType = (pSysInfo->Cpu.X86CpuInfo.VersionInformation&0x3000)>>12;

        m_SysInfo.m_X86ProcessorFeatures.m_bFPU = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<0))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bVME = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<1))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bDE  = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<2))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bPSEs = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<3))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bTSC = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<4))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bMSR = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<5))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bPAE = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<6))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bMCE = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<7))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bCX8 = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<8))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bAPIC = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<9))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bSEP = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<10))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bMTRR = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<11))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bPGE = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<12))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bMCA = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<13))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bCMOV = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<14))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bPAT = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<15))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bPSE = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<16))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bPSN = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<17))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bCLFSH = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<18))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bDS = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<20))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bACPI = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<21))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bMMX = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<22))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bFXSR = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<23))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bSSE = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<24))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bSSE2 = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<25))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bSS = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<26))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bHTT = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<27))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bTM = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<28))!=0;
        m_SysInfo.m_X86ProcessorFeatures.m_bPBE = (pSysInfo->Cpu.X86CpuInfo.FeatureInformation&(1<<30))!=0;
    }
    else
    {
		m_SysInfo.m_NonX86ProcessorFeatures.m_bFPErrata = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>0)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bFPEmulated = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>1)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bCmpExchDouble = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>2)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bMMX = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>3)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bXMMI  = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>6)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_b3DNow = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>7)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bRDTCS = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>8)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bPAE = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>9)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bXMMI64 = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>10)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bNX = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>12)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bSSE3 = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>13)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bCmpExch128 = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>14)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bCmp64Exch128 = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>15)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bChannels = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>16)&0x1)!=0;
        m_SysInfo.m_NonX86ProcessorFeatures.m_bXSAVE = ((pSysInfo->Cpu.OtherCpuInfo.ProcessorFeatures[0]>>17)&0x1)!=0;
    }

    ULONG32 uVersionRva = pSysInfo->CSDVersionRva;
    GetString(uVersionRva, m_SysInfo.m_sServicePack);

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_SYSTEM_INFO";
    bi.m_uRVA = uRva;
    bi.m_uSize = uSize;
    AddBlockInfo(bi);

    return TRUE;
}

BOOL CMiniDumpReader::ReadMiscInfoStream(ULONG uRva, ULONG uSize)
{
    m_MiscInfo.m_bProcessIdValid = FALSE;
    m_MiscInfo.m_bTimesValid = FALSE;
    m_MiscInfo.m_bPowerInfoValid = FALSE;


    MINIDUMP_MISC_INFO* pMiscInfo = (MINIDUMP_MISC_INFO*)m_fm.CreateView(uRva, uSize);

	if(pMiscInfo->SizeOfInfo>=sizeof(MINIDUMP_MISC_INFO))
    {
        m_MiscInfo.m_bProcessIdValid = (pMiscInfo->Flags1&MINIDUMP_MISC1_PROCESS_ID)!=0;
        m_MiscInfo.m_uProcessId = pMiscInfo->ProcessId;
        m_MiscInfo.m_bTimesValid = (pMiscInfo->Flags1&MINIDUMP_MISC1_PROCESS_TIMES)!=0;
        m_MiscInfo.m_ProcessCreateTime = pMiscInfo->ProcessCreateTime;
        m_MiscInfo.m_uProcessUserTime = pMiscInfo->ProcessUserTime;
        m_MiscInfo.m_uProcessKernelTime = pMiscInfo->ProcessKernelTime;
    }

    m_MiscInfo.m_bPowerInfoValid = FALSE;
    if(pMiscInfo->SizeOfInfo>=sizeof(MINIDUMP_MISC_INFO_2))
    {
        MINIDUMP_MISC_INFO_2* pMiscInfo2 = (MINIDUMP_MISC_INFO_2*)m_fm.CreateView(uRva, uSize);
        m_MiscInfo.m_bPowerInfoValid = (pMiscInfo2->Flags1&MINIDUMP_MISC1_PROCESSOR_POWER_INFO)!=0;
        m_MiscInfo.m_uProcessorMaxMhz = pMiscInfo2->ProcessorMaxMhz;
        m_MiscInfo.m_uProcessorCurrentMhz = pMiscInfo2->ProcessorCurrentMhz;
    }

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_MISC_INFO";
    bi.m_uRVA = uRva;
    bi.m_uSize = uSize;
    AddBlockInfo(bi);

    return TRUE;
}

BOOL CMiniDumpReader::ReadExceptionStream(ULONG uRva, ULONG uSize)
{
    MINIDUMP_EXCEPTION_STREAM* pExceptionStream =
        (MINIDUMP_EXCEPTION_STREAM*)m_fm.CreateView(uRva, uSize);

    if(pExceptionStream==NULL ||
        uSize<sizeof(MINIDUMP_EXCEPTION_STREAM))
        return FALSE;

    m_ExcInfo.m_uThreadId = pExceptionStream->ThreadId;
    m_ExcInfo.m_uExceptionCode = pExceptionStream->ExceptionRecord.ExceptionCode;
    m_ExcInfo.m_uExceptionAddress = pExceptionStream->ExceptionRecord.ExceptionAddress;
    m_ExcInfo.m_uExceptionThreadContextRva = pExceptionStream->ThreadContext.Rva;
    m_ExcInfo.m_uExceptionThreadContextSize = pExceptionStream->ThreadContext.DataSize;

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_EXCEPTION_STREAM";
    bi.m_uRVA = uRva;
    bi.m_uSize = uSize;
    AddBlockInfo(bi);

    bi.m_sId = L"MINIDUMP_LOCATION (ExceptionThreadContext)";
    bi.m_uRVA = pExceptionStream->ThreadContext.Rva;
    bi.m_uSize = pExceptionStream->ThreadContext.DataSize;
    AddBlockInfo(bi);

    return TRUE;
}

BOOL CMiniDumpReader::ReadModuleListStream(ULONG uRva, ULONG uSize)
{
    MINIDUMP_MODULE_LIST* pModuleStream =
        (MINIDUMP_MODULE_LIST*)m_fm.CreateView(uRva, uSize);
    if(pModuleStream==NULL)
        return FALSE;

    ULONG32 uNumberOfModules = pModuleStream->NumberOfModules;
    ULONG32 i;
    for(i=0; i<uNumberOfModules; i++)
    {
        ULONG uModuleOffs = uRva+4+i*(sizeof(MINIDUMP_MODULE));
        MINIDUMP_MODULE* pModule =
            (MINIDUMP_MODULE*)m_fm.CreateView(uModuleOffs, sizeof(MINIDUMP_MODULE));

        std::wstring sPdbGuid;
		DWORD dwAge = 0;
        std::wstring sPdbFileName;
        if(pModule->CvRecord.DataSize>0)
        {
            // Get CV record
            CV_INFO_PDB70* pCvRecord =
                (CV_INFO_PDB70*)m_fm.CreateView(pModule->CvRecord.Rva, pModule->CvRecord.DataSize);

            GUID Guid = pCvRecord->Signature;

            // Format string GUID
            wchar_t szBuf[120];
            swprintf(szBuf,
                120,
                L"%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x",
                Guid.Data1,
                Guid.Data2,
                Guid.Data3,
                UINT(Guid.Data4[0]),
                UINT(Guid.Data4[1]),
                UINT(Guid.Data4[2]),
                UINT(Guid.Data4[3]),
                UINT(Guid.Data4[4]),
                UINT(Guid.Data4[5]),
                UINT(Guid.Data4[6]),
                UINT(Guid.Data4[7]));
            sPdbGuid = std::wstring(szBuf);

			dwAge = pCvRecord->Age;

            char* szName = (char*)&pCvRecord->PdbFileName[0];

            sPdbFileName = strconv::a2w(std::string(szName));

            // Restore module view
            pModule =
                (MINIDUMP_MODULE*)m_fm.CreateView(uModuleOffs, sizeof(MINIDUMP_MODULE));
        }

        MiniDumpBlockInfo bi;
        bi.m_sId = L"MINIDUMP_LOCATION (ModuleMiscRecord)";
        bi.m_uRVA = pModule->MiscRecord.Rva;
        bi.m_uSize = pModule->MiscRecord.DataSize;
        AddBlockInfo(bi);

        bi.m_sId = L"MINIDUMP_LOCATION (ModuleCvRecord)";
        bi.m_uRVA = pModule->CvRecord.Rva;
        bi.m_uSize = pModule->CvRecord.DataSize;
        AddBlockInfo(bi);

        MiniDumpModuleInfo m;
		m.m_dwTimeDateStamp = pModule->TimeDateStamp;

		WORD dwVerMajor = HIWORD(pModule->VersionInfo.dwProductVersionMS);
        WORD dwVerMinor = LOWORD(pModule->VersionInfo.dwProductVersionMS);
        WORD dwPatchLevel = HIWORD(pModule->VersionInfo.dwProductVersionLS);
        WORD dwVerBuild = LOWORD(pModule->VersionInfo.dwProductVersionLS);

		wchar_t szVer[32]=L"";
        swprintf(szVer, 32, L"%u.%u.%u.%u",
            dwVerMajor, dwVerMinor, dwPatchLevel, dwVerBuild);
		m.m_sVersion = szVer;

        m.m_dwBaseAddr = pModule->BaseOfImage;
        m.m_dwImageSize = pModule->SizeOfImage;
        m.m_sPdbGuid = sPdbGuid;
		m.m_dwPdbAge = dwAge;
        m.m_sPdbFileName = sPdbFileName;
        GetString(pModule->ModuleNameRva, m.m_sModuleName);

        std::wstring sShortModuleName = m.m_sModuleName;
        size_t pos = m.m_sModuleName.rfind(L'\\');
        if(pos!=m.m_sModuleName.npos)
            sShortModuleName = sShortModuleName.substr(pos+1, std::string::npos);
        m.m_sShortModuleName = sShortModuleName;

        m_aModules.push_back(m);
    }

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_MODULE_LIST";
    bi.m_uRVA = uRva;
    bi.m_uSize = uSize;
    AddBlockInfo(bi);

    return TRUE;
}

BOOL CMiniDumpReader::ReadThreadListStream(ULONG uRva, ULONG uSize)
{
    MINIDUMP_THREAD_LIST* pThreadStream =
        (MINIDUMP_THREAD_LIST*)m_fm.CreateView(uRva, sizeof(MINIDUMP_THREAD_LIST));
    if(pThreadStream==NULL)
        return FALSE;

    ULONG32 uNumberOfThreads = pThreadStream->NumberOfThreads;
    ULONG32 i;
    for(i=0; i<uNumberOfThreads; i++)
    {
        ULONG uThreadOffs = uRva+4+i*(sizeof(MINIDUMP_THREAD));
        MINIDUMP_THREAD* pThread =
            (MINIDUMP_THREAD*)m_fm.CreateView(uThreadOffs, sizeof(MINIDUMP_THREAD));

        MiniDumpThreadInfo ti;
        ti.m_uThreadId = pThread->ThreadId;
        ti.m_uSuspendCount = pThread->SuspendCount;
        ti.m_uPriorityClass = pThread->PriorityClass;
        ti.m_uPriority = pThread->Priority;
        ti.m_u64Teb = pThread->Teb;
        ti.m_uStackStart = pThread->Stack.StartOfMemoryRange;
        ti.m_uStackSize = pThread->Stack.Memory.DataSize;
        ti.m_uThreadContextRva = pThread->ThreadContext.Rva;
        ti.m_uThreadContextSize = pThread->ThreadContext.DataSize;

        m_aThreads.push_back(ti);

        m_ThreadIndex[ti.m_uThreadId] = (int)m_aThreads.size()-1;

        /*_BlockInfo bi;
        bi.m_sId = _T("MINIDUMP_LOCATION (ThreadStack)");
        bi.m_uRVA = pThread->Stack.Memory.Rva;
        bi.m_uSize = pThread->Stack.Memory.DataSize;
        AddBlockInfo(bi);*/

        MiniDumpBlockInfo bi;
        bi.m_sId = L"MINIDUMP_LOCATION (ThreadContext)";
        bi.m_uRVA = pThread->ThreadContext.Rva;
        bi.m_uSize = pThread->ThreadContext.DataSize;
        AddBlockInfo(bi);
    }

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_THREAD_LIST";
    bi.m_uRVA = uRva;
    bi.m_uSize = uSize;
    AddBlockInfo(bi);

    return TRUE;
}

BOOL CMiniDumpReader::ReadMemoryListStream(ULONG uRva, ULONG uSize)
{
    MINIDUMP_MEMORY_LIST* pMemStream =
        (MINIDUMP_MEMORY_LIST*)m_fm.CreateView(uRva, sizeof(MINIDUMP_MEMORY_LIST));
    if(pMemStream==NULL)
        return FALSE;

    UINT uCount = pMemStream->NumberOfMemoryRanges;

    UINT i;
    for(i=0; i<uCount; i++)
    {
        ULONG uOffs = uRva + 4 + i*sizeof(MINIDUMP_MEMORY_DESCRIPTOR);
        MINIDUMP_MEMORY_DESCRIPTOR* pMemDesc =
            (MINIDUMP_MEMORY_DESCRIPTOR*)m_fm.CreateView(uOffs, sizeof(MINIDUMP_MEMORY_DESCRIPTOR));

        MiniDumpMemRange mr;
        mr.m_uStart = pMemDesc->StartOfMemoryRange;
        mr.m_uSize = pMemDesc->Memory.DataSize;
        mr.m_uRva = pMemDesc->Memory.Rva;

        m_aMemRanges.push_back(mr);

        MiniDumpBlockInfo bi;
        bi.m_sId = L"MINIDUMP_MEMORY_RANGE";
        bi.m_uRVA = pMemDesc->Memory.Rva;
        bi.m_uSize = pMemDesc->Memory.DataSize;
        AddBlockInfo(bi);
    }

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_MEMORY_LIST";
    bi.m_uRVA = uRva;
    bi.m_uSize = uSize;
    AddBlockInfo(bi);

    return TRUE;
}

BOOL CMiniDumpReader::ReadMemory64ListStream(ULONG uRva, ULONG uSize)
{
    _MINIDUMP_MEMORY64_LIST* pMemStream =
        (_MINIDUMP_MEMORY64_LIST*)m_fm.CreateView(uRva, sizeof(MINIDUMP_MEMORY_LIST));
    if(pMemStream==NULL)
        return FALSE;

    ULONG64 uCount = pMemStream->NumberOfMemoryRanges;

	ULONG64 uBlockRva = pMemStream->BaseRva;

    ULONG64 i;
    for(i=0; i<uCount; i++)
    {
        ULONG64 uOffs = uRva + 16 + i*sizeof(_MINIDUMP_MEMORY_DESCRIPTOR64);
        _MINIDUMP_MEMORY_DESCRIPTOR64* pMemDesc =
            (_MINIDUMP_MEMORY_DESCRIPTOR64*)m_fm.CreateView((DWORD)uOffs, sizeof(MINIDUMP_MEMORY_DESCRIPTOR));
		if(pMemDesc==NULL)
			return FALSE;

        MiniDumpMemRange mr;
        mr.m_uStart = pMemDesc->StartOfMemoryRange;
		mr.m_uSize = pMemDesc->DataSize;
		mr.m_uRva = uBlockRva;

        m_aMemRanges.push_back(mr);

        MiniDumpBlockInfo bi;
        bi.m_sId = L"MINIDUMP_MEMORY_RANGE";
        bi.m_uRVA = pMemDesc->DataSize;
        bi.m_uSize = uBlockRva;
        AddBlockInfo(bi);

		uBlockRva += pMemDesc->DataSize;
    }

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_MEMORY_LIST";
    bi.m_uRVA = uBlockRva;
    bi.m_uSize = uSize;
    AddBlockInfo(bi);

    return TRUE;
}

BOOL CMiniDumpReader::GetString(ULONG uRva, std::wstring& sString)
{
    sString.clear();

    if(uRva>=m_fm.GetSize())
        return FALSE;

    MINIDUMP_STRING* pStr = (MINIDUMP_STRING*)m_fm.CreateView(uRva, sizeof(MINIDUMP_STRING));
    if(pStr==NULL ||
        uRva+pStr->Length>=m_fm.GetSize())
        return FALSE;

    pStr = (MINIDUMP_STRING*)m_fm.CreateView(uRva, sizeof(MINIDUMP_STRING)+pStr->Length);

    size_t i;
    for(i=0; i<pStr->Length/2; i++)
    {
        wchar_t c = pStr->Buffer[i];
        sString += c;
    }
    //sString += L'\0'; // zero-terminate string

    MiniDumpBlockInfo bi;
    bi.m_sId = L"MINIDUMP_STRING";
    bi.m_uRVA = uRva;
    bi.m_uSize = pStr->Length+4/*length*/+2/*null char*/;
    AddBlockInfo(bi);

    return TRUE;
}

std::wstring CMiniDumpReader::StreamTypeToStr(ULONG uStreamType)
{
    std::wstring str;

    switch(uStreamType)
    {
    case UnusedStream: str = L"UnusedStream"; break;
    case ReservedStream0: str = L"ReservedStream0"; break;
    case ReservedStream1: str = L"ReservedStream1"; break;
    case ThreadListStream: str = L"ThreadListStream"; break;
    case ModuleListStream: str = L"ModuleListStream"; break;
    case MemoryListStream: str = L"MemoryListStream"; break;
    case ExceptionStream: str = L"ExceptionStream"; break;
    case SystemInfoStream: str = L"SystemInfoStream"; break;
    case ThreadExListStream: str = L"ThreadExListStream"; break;
    case Memory64ListStream: str = L"Memory64ListStream"; break;
    case CommentStreamA: str = L"CommentStreamA"; break;
    case CommentStreamW: str = L"CommentStreamW"; break;
    case HandleDataStream: str = L"HandleDataStream"; break;
    case FunctionTableStream: str = L"FunctionTableStream"; break;
    case UnloadedModuleListStream: str = L"UnloadedModuleListStream"; break;
    case MiscInfoStream: str = L"MiscInfoStream"; break;
    case MemoryInfoListStream: str = L"MemoryInfoListStream"; break;
    case ThreadInfoListStream: str = L"ThreadInfoListStream"; break;
    case HandleOperationListStream: str = L"HandleOperationListStream"; break;
    case LastReservedStream: str = L"LastReservedStream"; break;
    default:
        {
            wchar_t szBuf[256]=L"";
            swprintf(szBuf, 256, L"Unknown stream (%lu)", uStreamType);
            str = szBuf;
        }
        break;
    };

    return str;
}

std::wstring CMiniDumpReader::MiniDumpTypeToStr(ULONG uMiniDumpType)
{
    std::wstring str;

    if(uMiniDumpType&MiniDumpWithDataSegs)
        str += L"MiniDumpWithDataSegs ";

    if(uMiniDumpType&MiniDumpWithFullMemory)
        str += L"MiniDumpWithFullMemory ";

    if(uMiniDumpType&MiniDumpWithHandleData)
        str += L"MiniDumpWithHandleData ";

    if(uMiniDumpType&MiniDumpFilterMemory)
        str += L"MiniDumpFilterMemory ";

    if(uMiniDumpType&MiniDumpScanMemory)
        str += L"MiniDumpScanMemory ";

    if(uMiniDumpType&MiniDumpWithUnloadedModules)
        str += L"MiniDumpWithUnloadedModules ";

    if(uMiniDumpType&MiniDumpWithIndirectlyReferencedMemory)
        str += L"MiniDumpWithIndirectlyReferencedMemory ";

    if(uMiniDumpType&MiniDumpFilterModulePaths)
        str += L"MiniDumpFilterModulePaths ";

    if(uMiniDumpType&MiniDumpWithProcessThreadData)
        str += L"MiniDumpWithProcessThreadData ";

    if(uMiniDumpType&MiniDumpWithPrivateReadWriteMemory)
        str += L"MiniDumpWithPrivateReadWriteMemory ";

    if(uMiniDumpType&MiniDumpWithoutOptionalData)
        str += L"MiniDumpWithoutOptionalData ";

    if(uMiniDumpType&MiniDumpWithFullMemoryInfo)
        str += L"MiniDumpWithFullMemoryInfo ";

    if(uMiniDumpType&MiniDumpWithThreadInfo)
        str += L"MiniDumpWithThreadInfo ";

    if(uMiniDumpType&MiniDumpWithCodeSegs)
        str += L"MiniDumpWithCodeSegs ";

    if(uMiniDumpType&MiniDumpWithoutAuxiliaryState)
        str += L"MiniDumpWithoutAuxiliaryState ";

    if(uMiniDumpType&MiniDumpWithFullAuxiliaryState)
        str += L"MiniDumpWithFullAuxiliaryState ";

    if(uMiniDumpType&MiniDumpWithPrivateWriteCopyMemory)
        str += L"MiniDumpWithPrivateWriteCopyMemory ";

    if(uMiniDumpType&MiniDumpIgnoreInaccessibleMemory)
        str += L"MiniDumpIgnoreInaccessibleMemory ";

    if(uMiniDumpType&MiniDumpWithTokenInformation)
        str += L"MiniDumpWithTokenInformation ";

    if(str.empty())
        str =  L"MiniDumpNormal";

    return str;
}

std::wstring CMiniDumpReader::ProcessorArchitectureToStr(ULONG uProcessorArchitecture)
{
    std::wstring str;

    switch(uProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_AMD64: str = L"x64 (AMD or Intel)"; break;
    case PROCESSOR_ARCHITECTURE_IA64:  str = L"Intel Itanium"; break;
    case PROCESSOR_ARCHITECTURE_INTEL: str = L"x86"; break;
    default: str = L"Unknown architecture"; break;
    }

    return str;
}

std::wstring CMiniDumpReader::ProductTypeToStr(ULONG uProductType)
{
    std::wstring str;

    switch(uProductType)
    {
    case VER_NT_DOMAIN_CONTROLLER: str = L"Domain controller"; break;
    case VER_NT_SERVER:  str = L"Server"; break;
    case VER_NT_WORKSTATION: str = L"Workstation"; break;
    default: str = L"Unknown product type"; break;
    }

    return str;
}

int CMiniDumpReader::FindThreadIndexByThreadId(DWORD dwThreadId)
{
    std::map<DWORD, int>::iterator it = m_ThreadIndex.find(dwThreadId);
    if(it==m_ThreadIndex.end())
        return -1;

    return it->second;
}

BOOL CMiniDumpReader::GetThreadContext(DWORD dwThreadId, LPBYTE* ppContext, PUINT pcbContext)
{
	if(ppContext==NULL || pcbContext==NULL)
		return FALSE; // Invalid param

	// Init output
	*ppContext = NULL;
	*pcbContext = 0;

	DWORD uThreadContextRva = 0;
    ULONG uSize = 0;

    if(dwThreadId==m_ExcInfo.m_uThreadId)
    {
        uThreadContextRva = m_ExcInfo.m_uExceptionThreadContextRva;
        uSize = m_ExcInfo.m_uExceptionThreadContextSize;
    }
    else
    {
        int nThreadIndex = FindThreadIndexByThreadId(dwThreadId);
        uThreadContextRva = m_aThreads[nThreadIndex].m_uThreadContextRva;
        uSize = m_aThreads[nThreadIndex].m_uThreadContextSize;
    }
	
	if(uSize==0)
		return FALSE;

	LPBYTE pContext = new BYTE[uSize];

    LPBYTE pPtr = m_fm.CreateView(uThreadContextRva, uSize);
    if(pPtr==NULL)
	{
		delete [] pContext;		
        return FALSE;
	}

    memcpy(pContext, pPtr, uSize);
	
	*ppContext = pContext;
	*pcbContext = uSize;

    return TRUE;
}

BOOL CMiniDumpReader::ReadMemory(
                                 DWORD64 lpBaseAddress,
                                 PVOID lpBuffer,
                                 DWORD nSize,
                                 LPDWORD lpNumberOfBytesRead)
{
    *lpNumberOfBytesRead = 0;

    // Validate input parameters
    if(lpBaseAddress==0 ||
        lpBuffer==NULL ||
        nSize==0)
    {
        // Invalid parameter
        return FALSE;
    }

    ULONG i;
    for(i=0; i<m_aMemRanges.size(); i++)
    {
        MiniDumpMemRange& mr = m_aMemRanges[i];
        if(lpBaseAddress>=mr.m_uStart &&
            lpBaseAddress<mr.m_uStart+mr.m_uSize)
        {
            DWORD64 dwOffs = lpBaseAddress-mr.m_uStart;

            LONG64 lBytesRead = 0;

            if(mr.m_uSize-dwOffs>nSize)
                lBytesRead = nSize;
            else
                lBytesRead = mr.m_uSize-dwOffs;

            if(lBytesRead<=0 || nSize<lBytesRead)
                return FALSE;

            LPBYTE pStartPtr = m_fm.CreateView((DWORD)(mr.m_uRva+dwOffs), (DWORD)lBytesRead);
            if(pStartPtr==NULL)
                return FALSE;

            *lpNumberOfBytesRead = (DWORD)lBytesRead;
            memcpy(lpBuffer, pStartPtr, (size_t)lBytesRead);

            return TRUE;
        }
    }

    return FALSE;
}

int CMiniDumpReader::FindModuleIndexByAddr(DWORD64 Address)
{
    // Find a module that owns the provided memory address
    int i;
    for(i=0; i<(int)m_aModules.size(); i++)
    {
        MiniDumpModuleInfo& mi = m_aModules[i];
        if(mi.m_dwBaseAddr<=Address && Address<mi.m_dwBaseAddr+mi.m_dwImageSize)
        {
            return i;
        }
    }

    // Not found!
    return -1;
}

int CMiniDumpReader::GetModuleCount()
{
    return (int)m_aModules.size();
}

MiniDumpModuleInfo* CMiniDumpReader::GetModuleInfo(int nModuleIndex)
{
    // Validate input
    if(nModuleIndex<0 || nModuleIndex>=(int)m_aModules.size())
        return NULL; // Invalid index

    return &m_aModules[nModuleIndex];
}

MiniDumpHeaderInfo* CMiniDumpReader::GetHeaderInfo()
{
    if(!m_bInitialized)
        return NULL;

    return &m_HeaderInfo;
}

MiniDumpSystemInfo* CMiniDumpReader::GetSystemInfo()
{
    if(!m_bInitialized)
        return NULL;

    return &m_SysInfo;
}

MiniDumpExceptionInfo* CMiniDumpReader::GetExceptionInfo()
{
    if(!m_bInitialized)
        return NULL;

    return &m_ExcInfo;
}

int CMiniDumpReader::GetStreamCount()
{
    return (int)m_aStreams.size();
}

MiniDumpStreamInfo* CMiniDumpReader::GetStreamInfo(int nStream)
{
    if(nStream<0 || nStream>=(int)m_aStreams.size())
        return NULL;

    return &m_aStreams[nStream];
}

int CMiniDumpReader::GetLayoutBlockCount()
{
    return (int)m_Layout.size();
}

MiniDumpBlockInfo* CMiniDumpReader::GetLayoutBlock(int nBlock)
{
    if(nBlock<0 || nBlock>=(int)m_Layout.size())
        return NULL;

    std::multimap<ULONG, MiniDumpBlockInfo>::iterator it = m_Layout.begin();

    int i;
    for(i=0; i<nBlock; i++) it++;

    return &it->second;
}

MiniDumpMiscInfo* CMiniDumpReader::GetMiscInfo()
{
    if(!m_bInitialized)
        return NULL;

    return &m_MiscInfo;
}

int CMiniDumpReader::GetThreadCount()
{
    return (int)m_aThreads.size();
}

MiniDumpThreadInfo* CMiniDumpReader::GetThreadInfo(int nThread)
{
    if(nThread<0 || nThread>=(int)m_aThreads.size())
        return NULL;

    return &m_aThreads[nThread];
}

int CMiniDumpReader::GetMemRangeCount()
{
    return (int)m_aMemRanges.size();
}

 MiniDumpMemRange* CMiniDumpReader::GetMemRangeInfo(int nMemRange)
 {
     if(nMemRange<0 || nMemRange>=(int)m_aMemRanges.size())
         return NULL;

     return &m_aMemRanges[nMemRange];
 }
