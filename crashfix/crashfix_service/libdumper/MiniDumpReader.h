//! \file MiniDumpReader.h
//! \brief Minidump reader.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "FileMapping.h"

//! Minidump header information
struct MiniDumpHeaderInfo
{
    ULONG m_uVerImplSpecific; //!< Implementation version
    ULONG m_uVersion;         //!< Version 
    ULONG m_uChecksum;        //!< Integrity checksum
    ULONG m_uStreamCount;     //!< Number of minidump streams
    time_t m_TimeDateStamp;   //!< Creation timestamp
    ULONG64 m_uMinidumpType;  //!< Type of the minidump
};

//! Minidump stream information
struct MiniDumpStreamInfo
{
    ULONG m_uStreamType;      //!< Stream type
    ULONG m_uDataSize;        //!< Size of the stream
    ULONG m_uDataRva;         //!< Data relative virtual address (RVA)
};

//! Client system information
struct MiniDumpSystemInfo
{
    UINT   m_uProcessorArchitecture;  //!< Processor architecture
    UINT   m_uProcessorLevel;         //!< Processor level
    UINT   m_uProcessorRevision;      //!< Processor revision
    UINT   m_uchNumberOfProcessors;   //!< Count of processors
    UINT   m_uchProductType;          //!< Windows type
    ULONG  m_ulVerMajor;              //!< Windows version's major number
    ULONG  m_ulVerMinor;              //!< Windows version's minor number
    ULONG  m_ulVerBuild;              //!< Windows version's build number
    std::wstring m_sServicePack;      //!< Windows version's service pack
    UINT   m_uPlatformId;             //!< Platform ID
    std::wstring m_sX86ProcessorVendorId; //!< Processor's vendor (applicable to x86 only)
    UINT   m_uX86ProcessorStepping;   //!< Processor's stepping (applicable to x86 only)
    UINT   m_uX86ProcessorModel;      //!< Processor's model (applicable to x86 only)
    UINT   m_uX86ProcessorFamily;     //!< Processor's family (applicable to x86 only)
    UINT   m_uX86ProcessorType;       //!< Processor's type (applicable to x86 only)

    struct _X86ProcessorFeatures      //!< Processor's features (applicable to x86 only)
    {
        BOOL m_bFPU;
        BOOL m_bVME;
        BOOL m_bDE;
        BOOL m_bPSEs;
        BOOL m_bTSC;
        BOOL m_bMSR;
        BOOL m_bPAE;
        BOOL m_bMCE;
        BOOL m_bCX8;
        BOOL m_bAPIC;
        BOOL m_bSEP;
        BOOL m_bMTRR;
        BOOL m_bPGE;
        BOOL m_bMCA;
        BOOL m_bCMOV;
        BOOL m_bPAT;
        BOOL m_bPSE;
        BOOL m_bPSN;
        BOOL m_bCLFSH;
        BOOL m_bDS;
        BOOL m_bACPI;
        BOOL m_bMMX;
        BOOL m_bFXSR;
        BOOL m_bSSE;
        BOOL m_bSSE2;
        BOOL m_bSS;
        BOOL m_bHTT;
        BOOL m_bTM;
        BOOL m_bPBE;        
    } m_X86ProcessorFeatures;

    struct _OtherCpuFeatures
    {
        BOOL m_bFPErrata;       // 0
        BOOL m_bFPEmulated;     // 1
        BOOL m_bCmpExchDouble;  // 2
        BOOL m_bMMX;            // 3
        BOOL m_bXMMI;           // 6
        BOOL m_b3DNow;          // 7
        BOOL m_bRDTCS;          // 8
        BOOL m_bPAE;            // 9
        BOOL m_bXMMI64;         // 10
        BOOL m_bNX;             // 12
        BOOL m_bSSE3;           // 13
        BOOL m_bCmpExch128;     // 14
        BOOL m_bCmp64Exch128;   // 15
        BOOL m_bChannels;       // 16
        BOOL m_bXSAVE;          // 17

    } m_NonX86ProcessorFeatures;
};

//! Minidump miscellaneous info
struct MiniDumpMiscInfo
{
	//! Constructor
	MiniDumpMiscInfo()
	{
		m_bProcessIdValid = FALSE;
		m_uProcessId = 0;
		m_bTimesValid = FALSE;
		m_ProcessCreateTime = 0;
		m_uProcessUserTime = 0;
		m_uProcessKernelTime = 0;
		m_bPowerInfoValid = 0;
		m_uProcessorMaxMhz = 0;
		m_uProcessorCurrentMhz = 0;
		m_uProcessorMhzLimit = 0;
		m_uProcessorMaxIdleState = 0;
		m_uProcessorCurrentIdleState = 0;
	}

    BOOL m_bProcessIdValid;              //!<
    ULONG m_uProcessId;                  //!< Process ID
    BOOL m_bTimesValid;                  //!<
    time_t m_ProcessCreateTime;          //!< Process creation time
    ULONG m_uProcessUserTime;            //!< Process time in user mode
    ULONG m_uProcessKernelTime;          //!< Process time in kenel mode
    BOOL m_bPowerInfoValid;              //!<
    ULONG m_uProcessorMaxMhz;            //!<
    ULONG m_uProcessorCurrentMhz;        //!<
    ULONG m_uProcessorMhzLimit;          //!<
    ULONG m_uProcessorMaxIdleState;      //!<
    ULONG m_uProcessorCurrentIdleState;  //!<
};

//! Minidump exception info
struct MiniDumpExceptionInfo
{
	//! Constructor
	MiniDumpExceptionInfo()
	{
		m_uThreadId = 0;
		m_uExceptionCode = 0;
		m_uExceptionAddress = 0;
		m_uExceptionThreadContextRva = 0;
		m_uExceptionThreadContextSize = 0;
	}	

    ULONG    m_uThreadId;                    //!< Exception thread ID
    ULONG    m_uExceptionCode;               //!< Exception code
    ULONG64  m_uExceptionAddress;            //!< Exception address
    ULONG    m_uExceptionThreadContextRva;   //!< Exception thread context RVA
    ULONG    m_uExceptionThreadContextSize;  //!< Exception thread context size
};

//! Minidump module info
struct MiniDumpModuleInfo
{
	//! Constructor.
	MiniDumpModuleInfo()
	{
		m_dwBaseAddr = 0;
		m_dwImageSize = 0;
		m_dwPdbAge = 0;
	}

    std::wstring m_sModuleName;              //!< Module name.
    std::wstring m_sShortModuleName;         //!< Short module name.
    DWORD64 m_dwBaseAddr;                    //!< Module's starting address.
    DWORD64 m_dwImageSize;                   //!< Size of the module.
    std::wstring m_sPdbGuid;                 //!< Program database GUID.
	DWORD m_dwPdbAge;                        //!< PDB build age.
    std::wstring m_sPdbFileName;             //!< PDB file name.
	DWORD m_dwTimeDateStamp;                 //!< Timestamp.
	std::wstring m_sVersion;                 //!< Module version.

	//! Returns concatenated GUID and Age.
	std::wstring GUIDnAge()
	{
		std::wstringstream wstream;
		wstream << m_sPdbGuid;
		wstream << m_dwPdbAge;
		return wstream.str();
	}
};

//! Describes minidump thread info
struct MiniDumpThreadInfo
{
	//! Constructor.
	MiniDumpThreadInfo()
	{
		m_uThreadId = 0;
		m_uSuspendCount = 0;
		m_uPriorityClass = 0;
		m_uPriority = 0;
		m_u64Teb = 0;
		m_uStackStart = 0;
		m_uStackSize = 0;
		m_uThreadContextRva = 0;
		m_uThreadContextSize = 0;
	}

    ULONG m_uThreadId;             //!< Thread ID  
    ULONG m_uSuspendCount;         //!< Thread suspend count
    ULONG m_uPriorityClass;        //!< Thread priority class
    ULONG m_uPriority;             //!< Thread priority
    ULONG64 m_u64Teb;              //!< Thread environment block address ???
    ULONG64 m_uStackStart;         //!< Stack start address
    ULONG m_uStackSize;            //!< Size of the stack
    ULONG m_uThreadContextRva;     //!< Thread context RVA
    ULONG m_uThreadContextSize;    //!< Size of thread context
};

//! Describes minidump memory range
struct MiniDumpMemRange
{
	//! Constructor.
	MiniDumpMemRange()
	{
		m_uStart = 0;
		m_uSize = 0;
		m_uRva = 0;
	}

    ULONG64 m_uStart;     //!< Memory range start
    ULONG64 m_uSize;        //!< Memory range size
    ULONG64 m_uRva;         //!< Memory range RVA
};

//! Minidump block internal layout
struct MiniDumpBlockInfo
{
	//! Constructor
	MiniDumpBlockInfo()
	{
		m_uRVA = 0;
		m_uSize = 0;
	}
		

    std::wstring m_sId;   //!< Block ID
    ULONG64 m_uRVA;         //!< Block RVA
    ULONG64 m_uSize;        //!< Block size
};

//! \class CMiniDumpReader
//! \brief Provides access to minidump file contents.
class CMiniDumpReader
{
public:

    //! Constructor.
    CMiniDumpReader();
    ~CMiniDumpReader();

    //! Reads minidump file
    BOOL Init(std::wstring sFileName);

    //! Returns TRUE if file open
    BOOL IsInitialized();

    //! Closes the minidump
    void Destroy();

    //! Returns minidump header info
    MiniDumpHeaderInfo* GetHeaderInfo();

    //! Returns client system information.
    MiniDumpSystemInfo* GetSystemInfo();

    //! Returns minidump misc info.
    MiniDumpMiscInfo* GetMiscInfo();

    //! Returns exception info
    MiniDumpExceptionInfo* GetExceptionInfo();

    //! Returns number of threads
    int GetThreadCount();

    //! Returns thread info
    MiniDumpThreadInfo* GetThreadInfo(int nThread);

    //! Returns thread context
    BOOL GetThreadContext(DWORD dwThreadId, LPBYTE* ppContext, PUINT pcbContext);

    //! Returns the zero-based index by thread ID.
    int FindThreadIndexByThreadId(DWORD dwThreadId);

    //! Returns index of module the address belongs to, or -1 if such module not found.
    //! @param[in] Address Memory address.
    int FindModuleIndexByAddr(DWORD64 Address);

    //! Returns count of modules.
    int GetModuleCount();

    //! Returns module info.
    //! @param[in] nModuleIndex Zero-based index of the module.
    MiniDumpModuleInfo* GetModuleInfo(int nModuleIndex);

    //! Returns count of memory ranges
    int GetMemRangeCount();

    //! Returns memory range info
    MiniDumpMemRange* GetMemRangeInfo(int nMemRange);

    //! Reads memory ranges from minidump
    BOOL ReadMemory(DWORD64 lpBaseAddress, PVOID lpBuffer, DWORD nSize, LPDWORD lpNumberOfBytesRead);
    
    //! Converts stream type to string
    static std::wstring StreamTypeToStr(ULONG uStreamType);

    //! Converts minidump type constant to string
    static std::wstring MiniDumpTypeToStr(ULONG uMiniDumpType);

    //! Converts processor architecture number to string
    static std::wstring ProcessorArchitectureToStr(ULONG uProcessorArchitecture);

    //! Converts product (OS) type to string 
    static std::wstring ProductTypeToStr(ULONG uProductType);

    //! Returns number of minidump streams
    int GetStreamCount();

    //! Returns minidump stream info.
    MiniDumpStreamInfo* GetStreamInfo(int nStream);

    //! Returns number of internal layout blocks
    int GetLayoutBlockCount();

    //! Returns block by its zero-based index
    MiniDumpBlockInfo* GetLayoutBlock(int nBlock);

private:

    //! Reads system info stream
    BOOL ReadSystemInfoStream(ULONG uRva, ULONG uSize);

    //! Reads misc info stream
    BOOL ReadMiscInfoStream(ULONG uRva, ULONG uSize);

    //! Reads exception stream
    BOOL ReadExceptionStream(ULONG uRva, ULONG uSize);

    //! Reads module list stream
    BOOL ReadModuleListStream(ULONG uRva, ULONG uSize);

    //! Reads thread list stream
    BOOL ReadThreadListStream(ULONG uRva, ULONG uSize);

    //! Reads memory list stream
    BOOL ReadMemoryListStream(ULONG uRva, ULONG uSize);

	//! Reads full memory list stream
    BOOL ReadMemory64ListStream(ULONG uRva, ULONG uSize);

    //! Retrieves a string by its RVA
    BOOL GetString(ULONG uRva, std::wstring& sString);

    //! Adds a block info to minidump layout
    BOOL AddBlockInfo(MiniDumpBlockInfo& bi);

    BOOL m_bInitialized;       //!< TRUE if initialized
    CFileMemoryMapping m_fm;   //!< File mapping    
    MiniDumpHeaderInfo m_HeaderInfo;              //!< Minidump header info
    std::vector<MiniDumpStreamInfo> m_aStreams;   //!< List of minidump streams
    MiniDumpSystemInfo m_SysInfo;                 //!< System info
    MiniDumpMiscInfo m_MiscInfo;                  //!< Misc info
    MiniDumpExceptionInfo m_ExcInfo;              //!< Exception info
    std::vector<MiniDumpModuleInfo> m_aModules;   //!< The list of modules
    std::vector<MiniDumpThreadInfo> m_aThreads;   //!< The list of threads
    std::map<DWORD, int> m_ThreadIndex;           //!< Index for fast accessing threads by thread ID
    std::vector<MiniDumpMemRange> m_aMemRanges;   //!< Memory ranges
    std::multimap<ULONG, MiniDumpBlockInfo> m_Layout;    //!< Minidump internal layout

};

