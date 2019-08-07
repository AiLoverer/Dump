#include "stdafx.h"
#include "MiniDumpWriterImpl.h"
#include "DumpStruct.h"
#include <memory>
#include "PeReader.h"
#include "PeStruct.h"

#define _interlockedbittestandset fk_m$_set
#define _interlockedbittestandreset fk_m$_reset
#include <intrin.h>
#undef _interlockedbittestandset
#undef _interlockedbittestandreset

BOOL CMiniDumpWriterFactory::CreateInstance(IMiniDumpWriter** ppWriter)
{
    *ppWriter = new CMiniDumpWriterImpl();
    return TRUE;
}

CMiniDumpWriterImpl::CMiniDumpWriterImpl()
{
    m_uProcessID = 0;
    m_uThreadID = 0;
    m_pExcPtrs = NULL;
    m_fp = NULL;
    m_hProcess = NULL;
    m_hSnapshot = INVALID_HANDLE_VALUE;
    m_dwProcessPriorityClass = 0;
}

CMiniDumpWriterImpl::~CMiniDumpWriterImpl()
{
}

BOOL CMiniDumpWriterImpl::Write(ULONG uProcessID, ULONG uThreadID, void* pExcPtrs, LPCWSTR szFileName)
{
    BOOL bStatus = FALSE;
    m_uProcessID = uProcessID;
    m_uThreadID = uThreadID;
    m_pExcPtrs = (PEXCEPTION_POINTERS)pExcPtrs;
    m_sFileName = szFileName;

    _wfopen_s(&m_fp, szFileName, L"wb");
    if(m_fp==NULL)
        goto cleanup;

    if(!GetProcessSnapshot())
        goto cleanup;

    if(!SuspendProcessThreads(TRUE))
        goto cleanup;

    if(!GetExceptionInfo())
        goto cleanup;

    if(!GetProcessThreadList())
        goto cleanup;

    if(!GetProcessModuleList())
        goto cleanup;

    if(!CalcVariableRVAs())
        goto cleanup;

    if(!WriteHeader())
        goto cleanup;

    if(!WriteStreamDirectory(ThreadListStream, 0, 0))
        goto cleanup;

    if(!WriteStreamDirectory(ModuleListStream, 0, 0))
        goto cleanup;

    if(!WriteStreamDirectory(MemoryListStream, 0, 0))
        goto cleanup;

    if(!WriteStreamDirectory(ExceptionStream, 0, 0))
        goto cleanup;

    if(!WriteStreamDirectory(SystemInfoStream, 0, 0))
        goto cleanup;

    if(!WriteStreamDirectory(UnusedStream, 0, 0))
        goto cleanup;

    if(!WriteStreamDirectory(UnusedStream, 0, 0))
        goto cleanup;

    if(!WriteSystemInfoStream())
        goto cleanup;

    if(!WriteMiscInfoStream())
        goto cleanup;

    if(!WriteExceptionStream())
        goto cleanup;

    if(!WriteThreadList())
        goto cleanup;

    if(!WriteModuleList())
        goto cleanup;

    if(!WriteMemoryList())
        goto cleanup;

    if(!SuspendProcessThreads(FALSE))
        goto cleanup;

    bStatus = TRUE;

cleanup:

    CleanUp();

    return bStatus;
}

std::wstring CMiniDumpWriterImpl::GetErrorMsg()
{
    return m_sErrorMsg;
}

void CMiniDumpWriterImpl::CleanUp()
{
    if(m_fp!=NULL)
    {
        fclose(m_fp);
        m_fp = NULL;
    }
}

BOOL CMiniDumpWriterImpl::GetImageInfo(ULONG_PTR ModuleBaseAddress, ULONG32* pTimeDateStamp, ULONG32* pCheckSum,
                                       LPBYTE* ppCvRecord, LPDWORD pdwCvRecSize, LPBYTE* ppMiscRecord, LPDWORD pdwMiscRecSize,
                                       VS_FIXEDFILEINFO** ppVersionInfo)
{
    *pTimeDateStamp = 0;
    *pCheckSum = 0;
    *ppCvRecord = NULL;
    *pdwCvRecSize = 0;
    *ppMiscRecord = NULL;
    *pdwMiscRecSize = 0;
    *ppVersionInfo = NULL;

    IMAGE_DOS_HEADER DosHeader;
    IMAGE_NT_HEADERS NtHeaders;
    PIMAGE_DATA_DIRECTORY pDebugDataDirectory;
    IMAGE_DEBUG_DIRECTORY DebugDirectory;
    ULONG uIndex = 0;
    ULONG uNumberOfDebugDirs = 0;
    SIZE_T uBytesRead = 0;

    // Read image DOS header
    if(!ReadProcessMemory(m_hProcess, (LPCVOID)ModuleBaseAddress, &DosHeader, sizeof(IMAGE_DOS_HEADER), &uBytesRead) ||
        uBytesRead!=sizeof(IMAGE_DOS_HEADER))
        return FALSE; // Couldn't read image DOS header

    // Check signature
    if(IMAGE_DOS_SIGNATURE!=DosHeader.e_magic)
        return FALSE; // Invalid signature

    // Read image NT headers
    ULONG_PTR NtHeadersRva = ModuleBaseAddress+DosHeader.e_lfanew;
    if(!ReadProcessMemory(m_hProcess, (LPCVOID)NtHeadersRva,
        &NtHeaders, sizeof(IMAGE_NT_HEADERS), &uBytesRead) ||
        uBytesRead!=sizeof(IMAGE_NT_HEADERS))
        return FALSE; // Couldn't read image NT headers

    // Check signature
    if(IMAGE_NT_SIGNATURE!=NtHeaders.Signature)
        return FALSE; // Invalid signature

    *pTimeDateStamp = NtHeaders.FileHeader.TimeDateStamp;
    *pCheckSum = NtHeaders.OptionalHeader.CheckSum;

    // Get debug data directory
    pDebugDataDirectory = &NtHeaders.OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    uNumberOfDebugDirs = pDebugDataDirectory->Size/sizeof(IMAGE_DEBUG_DIRECTORY);

    // Check debug directory size
    if((pDebugDataDirectory->Size % sizeof(IMAGE_DEBUG_DIRECTORY))!= 0)
        return FALSE;

    // Look for CodeView record.
    for (uIndex = 0; uIndex < uNumberOfDebugDirs; uIndex++ )
    {
        // Read debug directory
        ULONG_PTR uDebugDirRva = ModuleBaseAddress+pDebugDataDirectory->VirtualAddress+uIndex*sizeof(IMAGE_DEBUG_DIRECTORY);
        if(!ReadProcessMemory(m_hProcess, (LPCVOID)uDebugDirRva,
            &DebugDirectory, sizeof(IMAGE_DEBUG_DIRECTORY), &uBytesRead) ||
            uBytesRead!=sizeof(IMAGE_DEBUG_DIRECTORY))
            return FALSE; // Couldn't read debug directory

        if(DebugDirectory.Type==IMAGE_DEBUG_TYPE_CODEVIEW)
        {
            *ppCvRecord = new BYTE[DebugDirectory.SizeOfData];
            *pdwCvRecSize = DebugDirectory.SizeOfData;

            // Read CodeView data
            if(!ReadProcessMemory(m_hProcess, PtrFromRva(ModuleBaseAddress, DebugDirectory.AddressOfRawData),
                *ppCvRecord, *pdwCvRecSize, &uBytesRead) ||
                uBytesRead!=*pdwCvRecSize)
                return FALSE; // Couldn't read CodeView data
        }
        else if(DebugDirectory.Type==IMAGE_DEBUG_TYPE_MISC)
        {
            *ppCvRecord = new BYTE[DebugDirectory.SizeOfData];
            *pdwMiscRecSize = DebugDirectory.SizeOfData;

            // Read misc data
            if(!ReadProcessMemory(m_hProcess, PtrFromRva(ModuleBaseAddress, DebugDirectory.AddressOfRawData),
                *ppMiscRecord, *pdwMiscRecSize, &uBytesRead) ||
                uBytesRead!=*pdwMiscRecSize)
                return FALSE; // Couldn't read CodeView data
        }
    }

    // Walk through section headers
    ULONG_PTR uSectionHeaderRva = NtHeadersRva + offsetof(IMAGE_NT_HEADERS, OptionalHeader) + NtHeaders.FileHeader.SizeOfOptionalHeader;
    ULONG uSectionCount = NtHeaders.FileHeader.NumberOfSections;

    for(uIndex=0; uIndex<uSectionCount; uIndex++)
    {
        IMAGE_SECTION_HEADER SectionHeader;

        // Read section header
        if(!ReadProcessMemory(m_hProcess, (LPCVOID)uSectionHeaderRva,
            &SectionHeader, sizeof(IMAGE_SECTION_HEADER), &uBytesRead) ||
            uBytesRead!=sizeof(IMAGE_SECTION_HEADER))
            return FALSE; // Couldn't read section header

        uSectionHeaderRva += sizeof(IMAGE_SECTION_HEADER);

        // Look for ".rcsrc" section containing resources
        if(memcmp(SectionHeader.Name, ".rsrc", 5)!=0)
            continue;

        ULONG_PTR uResourceDirRva = ModuleBaseAddress + SectionHeader.VirtualAddress;
        ULONG uResourceDirSize = SectionHeader.SizeOfRawData;

        BrowseResourceDirForVersionInfo(0, ModuleBaseAddress, uResourceDirRva,
            uResourceDirRva, uResourceDirSize, ppVersionInfo);

        break;
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::BrowseResourceDirForVersionInfo(
    int nLevel,
    ULONG_PTR uResourceDataSectionOffs,
    ULONG_PTR uResourceDirTableOffs,
    ULONG_PTR uResourceDirRva,
    ULONG /*uResourceDirSize*/,
    VS_FIXEDFILEINFO** ppFixedFileInfo)
{
    *ppFixedFileInfo = NULL;
    IMAGE_RESOURCE_DIRECTORY ResourceDir;
    ULONG_PTR uBytesRead = 0;

    // Read resource dir
    if(!ReadProcessMemory(m_hProcess, (LPCVOID)uResourceDirRva,
        &ResourceDir, sizeof(IMAGE_RESOURCE_DIRECTORY), &uBytesRead) ||
        uBytesRead!=sizeof(IMAGE_RESOURCE_DIRECTORY))
        return FALSE; // Couldn't read resource dir

    ULONG uIdEntryCount = ResourceDir.NumberOfIdEntries;
    UINT i;
    for(i=0; i<uIdEntryCount; i++)
    {
        IMAGE_RESOURCE_DIRECTORY_ENTRY DirEntry;

        // Read resource entry
        if(!ReadProcessMemory(m_hProcess, (LPCVOID)(uResourceDirRva+sizeof(IMAGE_RESOURCE_DIRECTORY)+i*sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY)),
            &DirEntry, sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY), &uBytesRead) ||
            uBytesRead!=sizeof(IMAGE_RESOURCE_DIRECTORY_ENTRY))
            return FALSE; // Couldn't read resource entry

        if(nLevel<2)
        {
            if(nLevel==1 || MAKEINTRESOURCE(DirEntry.Id)==RT_VERSION && DirEntry.DataIsDirectory==1)
            {
                BrowseResourceDirForVersionInfo(nLevel+1, uResourceDataSectionOffs, uResourceDirTableOffs,
                    uResourceDirTableOffs+DirEntry.OffsetToDirectory, sizeof(IMAGE_RESOURCE_DIRECTORY), ppFixedFileInfo);
            }
        }
        else
        {
            IMAGE_RESOURCE_DATA_ENTRY DataEntry;
            ULONG_PTR uResourceDataEntryRva = uResourceDirTableOffs+DirEntry.OffsetToData;

            // Read resource data entry
            if(!ReadProcessMemory(m_hProcess, (LPCVOID)(uResourceDataEntryRva),
                &DataEntry, sizeof(IMAGE_RESOURCE_DATA_ENTRY), &uBytesRead) ||
                uBytesRead!=sizeof(IMAGE_RESOURCE_DATA_ENTRY))
                return FALSE;

            // Read resource data
            ULONG_PTR uResourceDataRva = uResourceDataSectionOffs+DataEntry.OffsetToData;
            LPBYTE pBuffer = new BYTE[DataEntry.Size];
            if(!ReadProcessMemory(m_hProcess, (LPCVOID)(uResourceDataRva),
                pBuffer, DataEntry.Size, &uBytesRead) ||
                uBytesRead!=DataEntry.Size)
                return FALSE;

            VS_VERSIONINFO* pVersionInfo = (VS_VERSIONINFO*)pBuffer;

            *ppFixedFileInfo = new VS_FIXEDFILEINFO;
            memcpy(*ppFixedFileInfo, &pVersionInfo->Value, sizeof(VS_FIXEDFILEINFO));

            delete [] pBuffer;

            return TRUE;
        }
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::GetProcessSnapshot()
{
    int nTrial = 0;

    // Open process to access its virtual memory
    m_hProcess = OpenProcess(PROCESS_QUERY_INFORMATION|PROCESS_SUSPEND_RESUME|PROCESS_VM_READ, FALSE, m_uProcessID);
    if(m_hProcess==NULL)
    {
        return FALSE;
    }

    m_hSnapshot = INVALID_HANDLE_VALUE;

    // Try to create a snapshot of the process
    for(nTrial = 0; nTrial<50; nTrial++)
    {
        m_hSnapshot = CreateToolhelp32Snapshot(
            TH32CS_SNAPMODULE|
            TH32CS_SNAPMODULE32|
            TH32CS_SNAPTHREAD,
            m_uProcessID
            );

        if(m_hSnapshot!=INVALID_HANDLE_VALUE)
            break;
    }

    if(m_hSnapshot==INVALID_HANDLE_VALUE)
    {
        return FALSE;
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::SuspendProcessThreads(BOOL bSuspend)
{
    BOOL bNextThread = FALSE;

    THREADENTRY32 te;
    memset(&te, 0, sizeof(THREADENTRY32));
    te.dwSize = sizeof(THREADENTRY32);

    bNextThread = Thread32First(m_hSnapshot, &te);
    while(bNextThread)
    {
        if(te.th32OwnerProcessID == m_uProcessID)
        {
            HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, te.th32ThreadID);
            if(hThread)
            {
                if(bSuspend)
                {
                    /*DWORD dwSuspendCount = */SuspendThread(hThread);
                }
                else
                {
                    ResumeThread(hThread);
                }

                CloseHandle(hThread);
                hThread = NULL;
            }
        }
        bNextThread = Thread32Next(m_hSnapshot, &te);
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::GetExceptionInfo()
{
    if(m_pExcPtrs==NULL)
        return FALSE;

    SIZE_T uBytesRead = 0;
    BYTE buff[1024];
    memset(&buff, 0, 1024);

    if(!ReadProcessMemory(m_hProcess, m_pExcPtrs, &buff, sizeof(EXCEPTION_POINTERS), &uBytesRead) ||
        uBytesRead!=sizeof(EXCEPTION_POINTERS))
        return FALSE;

    EXCEPTION_POINTERS* pExcPtrs = (EXCEPTION_POINTERS*)buff;

    memset(&m_ExcStream, 0, sizeof(MINIDUMP_EXCEPTION_STREAM));
    m_ExcStream.ThreadId = m_uThreadID;
    m_ExcStream.ExceptionRecord.ExceptionAddress = (ULONG64)pExcPtrs->ExceptionRecord->ExceptionAddress;
    m_ExcStream.ExceptionRecord.ExceptionCode = pExcPtrs->ExceptionRecord->ExceptionCode;
    m_ExcStream.ExceptionRecord.ExceptionFlags = pExcPtrs->ExceptionRecord->ExceptionFlags;

    int i;
    for(i=0; i<15; i++)
    {
        m_ExcStream.ExceptionRecord.ExceptionInformation[i] = pExcPtrs->ExceptionRecord->ExceptionInformation[i];
    }

    m_ExcStream.ExceptionRecord.ExceptionRecord = (ULONG64)pExcPtrs->ExceptionRecord->ExceptionRecord;
    m_ExcStream.ExceptionRecord.NumberParameters = pExcPtrs->ExceptionRecord->NumberParameters;

    return TRUE;
}

BOOL CMiniDumpWriterImpl::GetProcessThreadList()
{
    BOOL bNextThread = FALSE;

    THREADENTRY32 te;
    memset(&te, 0, sizeof(THREADENTRY32));
    te.dwSize = sizeof(THREADENTRY32);

    ULONG32 uPriorityClass = GetPriorityClass(m_hProcess);

    bNextThread = Thread32First(m_hSnapshot, &te);
    while(bNextThread)
    {
        if(te.th32OwnerProcessID == m_uProcessID)
        {
            MINIDUMP_THREAD mt;
            memset(&mt, 0, sizeof(MINIDUMP_THREAD));
            mt.ThreadId = te.th32ThreadID;
            mt.SuspendCount = 0;

            HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION|THREAD_GET_CONTEXT, FALSE, te.th32ThreadID);
            if(hThread!=NULL)
            {
#ifdef _X86

                CONTEXT_x86 ctx;
                memset(&ctx, 0, sizeof(CONTEXT_x86));
                ctx.ContextFlags = CONTEXT_ALL;
                BOOL bGetContext = GetThreadContext(hThread, &ctx);
                //Wow64GetThreadContext

                if(bGetContext)
                {
                    mt.ThreadContext.Rva = 0;
                    mt.ThreadContext.DataSize = 0;

                    if(mt.ThreadId==m_uThreadID)
                    {
                        m_ExcStream.ThreadContext.DataSize = sizeof(CONTEXT);
                    }
                }

                // Convert segment-related address to flat address (works for x86 only)
                LDT_ENTRY ldt_entry;
                BOOL bGetThreadSelector = GetThreadSelectorEntry(hThread, ctx.SegFs, &ldt_entry);
                if(bGetThreadSelector)
                {
                    ULONG64 uTebAddr = (ldt_entry.BaseLow) | (ldt_entry.HighWord.Bytes.BaseHi << 24) | (ldt_entry.HighWord.Bytes.BaseMid << 16);
                    mt.Teb = uTebAddr;

                    NT_TIB Tib;

                    ULONG_PTR uBytesRead = 0;
                    if(ReadProcessMemory(m_hProcess, (LPCVOID)(uTebAddr), &Tib, sizeof(NT_TIB), &uBytesRead) &&
                        uBytesRead==sizeof(NT_TIB))
                    {
                        //mt.Stack.Memory.DataSize = (ULONG64)Tib.StackBase-(ULONG64)Tib.StackLimit;
                    }
                }

                mt.Stack.StartOfMemoryRange = ctx.Esp;

                ULONG64 offs;
                for(offs=0; ; offs++)
                {
                    ULONG u;
                    ULONG uBytesRead=0;
                    if(!ReadProcessMemory(m_hProcess, (LPCVOID)(ctx.Esp+offs), &u, sizeof(ULONG), &uBytesRead) ||
                        uBytesRead!=sizeof(ULONG))
                    {
                        break;
                    }
                }

                mt.Stack.Memory.DataSize = ctx.Esi-ctx.Esp;

                mt.ThreadContext.DataSize = sizeof(CONTEXT);

#endif

                ULONG32 uPriority = GetThreadPriority(hThread);

                mt.PriorityClass = uPriorityClass;
                mt.Priority = uPriority;

                CloseHandle(hThread);
            }

            m_aThreads.push_back(mt);
        }
        bNextThread = Thread32Next(m_hSnapshot, &te);
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::GetProcessModuleList()
{
    BOOL bNextModule = FALSE;

    MODULEENTRY32W me;
    memset(&me, 0, sizeof(MODULEENTRY32W));
    me.dwSize = sizeof(MODULEENTRY32W);

    bNextModule = Module32FirstW(m_hSnapshot, &me);
    while(bNextModule)
    {
        if(me.th32ProcessID == m_uProcessID)
        {
            MDW_ModuleInfo mi;
            memset(&mi.m_Module, 0, sizeof(MINIDUMP_MODULE));

            mi.m_sModuleName = me.szModule;
            mi.m_sImagePath = me.szExePath;
            mi.m_Module.BaseOfImage = (ULONG64)me.modBaseAddr;
            mi.m_Module.SizeOfImage = me.modBaseSize;

            ULONG_PTR uBaseAddr = (ULONG_PTR)me.modBaseAddr;

            /*BOOL bGetInfo = */GetImageInfo(
                uBaseAddr,
                &mi.m_uTimeDateStamp,
                &mi.m_uCheckSum,
                &mi.m_pCvRecord,
                &mi.m_uCvRecSize,
                &mi.m_pMiscRecord,
                &mi.m_uMiscRecSize,
                &mi.m_pVersionInfo);

            if(mi.m_pVersionInfo)
            {
                memcpy(&mi.m_Module.VersionInfo, mi.m_pVersionInfo, sizeof(VS_FIXEDFILEINFO));
            }

            m_aModules.push_back(mi);
        }

        bNextModule = Module32NextW(m_hSnapshot, &me);
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::CalcVariableRVAs()
{
    size_t uCurOffs = 0;
    uCurOffs += sizeof(MINIDUMP_HEADER);
    uCurOffs += 8*sizeof(MINIDUMP_DIRECTORY);
    uCurOffs += sizeof(MINIDUMP_SYSTEM_INFO);
    uCurOffs += sizeof(MINIDUMP_MISC_INFO);
    uCurOffs += sizeof(MINIDUMP_EXCEPTION_STREAM);
    uCurOffs += 4+m_aThreads.size()*sizeof(MINIDUMP_THREAD);
    uCurOffs += 4+m_aModules.size()*sizeof(MINIDUMP_MODULE);

    // Calculate module name RVAs

    size_t i;
    for(i=0; i<m_aModules.size(); i++)
    {
        MDW_ModuleInfo& mi = m_aModules[i];
        mi.m_Module.ModuleNameRva = (ULONG32)uCurOffs;
        uCurOffs += 4 + (mi.m_sImagePath.length()+1)*sizeof(TCHAR);
    }

    // Calculate exception thread context RVA

    m_ExcStream.ThreadContext.Rva = (ULONG32)uCurOffs;
    uCurOffs += m_ExcStream.ThreadContext.DataSize;

    // Calculate other thread context RVAs

    for(i=0; i<m_aThreads.size(); i++)
    {
        MINIDUMP_THREAD& mt = m_aThreads[i];
        mt.ThreadContext.Rva = (ULONG32)uCurOffs;
        uCurOffs += mt.ThreadContext.DataSize;
    }

    // Calculate module CodeView record RVAs

    for(i=0; i<m_aModules.size(); i++)
    {
        MDW_ModuleInfo& mi = m_aModules[i];

        if(mi.m_pCvRecord==NULL)
            continue;

        mi.m_Module.CvRecord.Rva = (ULONG32)uCurOffs;
        uCurOffs += mi.m_uCvRecSize;
    }

    // Calculate

    return TRUE;
}

BOOL CMiniDumpWriterImpl::WriteHeader()
{
    BOOL bStatus = FALSE;
    MINIDUMP_HEADER header;
    memset(&header, 0, sizeof(MINIDUMP_HEADER));
    memcpy(&header.Signature, MINIDUMP_SIGNATURE, 4);
    header.Version = MINIDUMP_VERSION;
    header.NumberOfStreams = 8;
    header.StreamDirectoryRva = sizeof(MINIDUMP_HEADER);
    header.CheckSum = 0;
    time((time_t*)&header.TimeDateStamp);
    header.Flags = 0; //MinidumpNormal;

    if(1!=fwrite(&header, sizeof(MINIDUMP_HEADER), 1, m_fp))
        goto cleanup;

    bStatus = TRUE;

cleanup:

    return bStatus;
}

BOOL CMiniDumpWriterImpl::WriteStreamDirectory(ULONG uStreamType, RVA uDirectoryRva, ULONG uDataSize)
{
    MINIDUMP_DIRECTORY md;
    memset(&md, 0, sizeof(MINIDUMP_DIRECTORY));
    md.StreamType = uStreamType;
    md.Location.Rva = uDirectoryRva;
    md.Location.DataSize = uDataSize;

    if(1!=fwrite(&md, sizeof(MINIDUMP_DIRECTORY), 1, m_fp))
        return FALSE;

    return TRUE;
}

BOOL CMiniDumpWriterImpl::WriteSystemInfoStream()
{
    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);

    OSVERSIONINFOEX osvi;
    memset(&osvi, 0, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);
    GetVersionEx((LPOSVERSIONINFO)&osvi);

    MINIDUMP_SYSTEM_INFO si;
    memset(&si, 0, sizeof(MINIDUMP_SYSTEM_INFO));
    si.ProcessorArchitecture = SystemInfo.wProcessorArchitecture;
    si.ProcessorLevel = SystemInfo.wProcessorLevel;
    si.ProcessorRevision = SystemInfo.wProcessorRevision;
    si.NumberOfProcessors = (UCHAR)SystemInfo.dwNumberOfProcessors;
    si.ProductType = osvi.wProductType;
    si.MajorVersion = osvi.dwMajorVersion;
    si.MinorVersion = osvi.dwMinorVersion;
    si.BuildNumber = osvi.dwBuildNumber;
    si.PlatformId = osvi.dwPlatformId;
    si.CSDVersionRva = 0; //???
    si.SuiteMask = osvi.wSuiteMask;
    if(SystemInfo.wProcessorArchitecture==PROCESSOR_ARCHITECTURE_INTEL)
    {
        int CPUInfo[4]={0,0,0,0};
        __cpuid(&CPUInfo[0], 0);

        si.Cpu.X86CpuInfo.VendorId[0] = CPUInfo[0];
        si.Cpu.X86CpuInfo.VendorId[1] = CPUInfo[1];
        si.Cpu.X86CpuInfo.VendorId[2] = CPUInfo[2];

        __cpuid(&CPUInfo[0], 1);
        si.Cpu.X86CpuInfo.VersionInformation = CPUInfo[0];
        si.Cpu.X86CpuInfo.FeatureInformation = CPUInfo[3];

        __cpuid(&CPUInfo[0], 0x80000001);
        si.Cpu.X86CpuInfo.AMDExtendedCpuFeatures = CPUInfo[1];
    }
    else
    {
        DWORD dwFeatures = 0;
        DWORD dwFeature = 0;
        for(dwFeature=0; dwFeature<0xFFFFFFFF; dwFeature)
        {
            if(IsProcessorFeaturePresent(dwFeature))
                dwFeatures |= dwFeature;
        }
        si.Cpu.OtherCpuInfo.ProcessorFeatures[0] = (ULONG64)dwFeatures;
    }

    if(1!=fwrite(&si, sizeof(MINIDUMP_SYSTEM_INFO), 1, m_fp))
        return FALSE;

    return TRUE;
}

BOOL CMiniDumpWriterImpl::WriteMiscInfoStream()
{
    HANDLE hProcess = NULL;
    MINIDUMP_MISC_INFO mi;
    memset(&mi, 0, sizeof(MINIDUMP_MISC_INFO));
    mi.SizeOfInfo = sizeof(MINIDUMP_MISC_INFO);

    mi.Flags1 |= MINIDUMP_MISC1_PROCESS_ID;
    mi.ProcessId = m_uProcessID;

    // Open process to access its virtual memory
    hProcess = OpenProcess(PROCESS_SUSPEND_RESUME|PROCESS_VM_READ, FALSE, m_uProcessID);
    if(hProcess!=NULL)
    {
        FILETIME ftCreationTime;
        FILETIME ftExitTime;
        FILETIME ftKernelTime;
        FILETIME ftUserTime;

        BOOL bGet = GetProcessTimes(hProcess, &ftCreationTime, &ftExitTime, &ftKernelTime, &ftUserTime);

        CloseHandle(hProcess);
        hProcess = NULL;

        if(bGet)
        {
            mi.Flags1 |= MINIDUMP_MISC1_PROCESS_TIMES;
            memcpy(&mi.ProcessCreateTime, &ftCreationTime, sizeof(ULONG32));
            memcpy(&mi.ProcessKernelTime, &ftKernelTime, sizeof(ULONG32));
            memcpy(&mi.ProcessUserTime, &ftUserTime, sizeof(ULONG32));
        }
    }

    if(1!=fwrite(&mi, sizeof(MINIDUMP_MISC_INFO), 1, m_fp))
        return FALSE;

    return TRUE;
}

BOOL CMiniDumpWriterImpl::WriteExceptionStream()
{
    if(m_pExcPtrs==NULL)
        return FALSE;

    if(1!=fwrite(&m_ExcStream, sizeof(MINIDUMP_EXCEPTION_STREAM), 1, m_fp))
        return FALSE;

    return TRUE;
}

BOOL CMiniDumpWriterImpl::WriteThreadList()
{
    // Write thread count
    ULONG32 uThreadCount = (ULONG32)m_aThreads.size();
    fwrite(&uThreadCount, sizeof(ULONG32), 1, m_fp);

    size_t i;
    for(i=0; i<m_aThreads.size(); i++)
    {
        if(1!=fwrite(&m_aThreads[i], sizeof(MINIDUMP_THREAD), 1, m_fp))
            return FALSE;
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::WriteModuleList()
{
    ULONG32 uModuleCount = (ULONG32)m_aModules.size();

    // Write module count
    fwrite(&uModuleCount, sizeof(ULONG32), 1, m_fp);

    // Write MINIDUMP_MODULE entries
    size_t i;
    for(i=0; i<m_aModules.size(); i++)
    {
        if(1!=fwrite(&m_aModules[i].m_Module, sizeof(MINIDUMP_MODULE), 1, m_fp))
            return FALSE;
    }

    // Write module name list
    for(i=0; i<m_aModules.size(); i++)
    {
        ULONG32 uStrLen = ((ULONG32)m_aModules[i].m_sImagePath.length()+1)*sizeof(TCHAR);

        if(1!=fwrite(&uStrLen, sizeof(ULONG32), 1, m_fp))
            return FALSE;

        if(1!=fwrite(m_aModules[i].m_sImagePath.c_str(), uStrLen, 1, m_fp))
            return FALSE;
    }

    return TRUE;
}

BOOL CMiniDumpWriterImpl::WriteMemoryList()
{


    return TRUE;
}

