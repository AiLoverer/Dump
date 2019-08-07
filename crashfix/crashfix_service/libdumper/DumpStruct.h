#pragma once
#include "stdafx.h"

#define RVA ULONG32

#define MINIDUMP_SIGNATURE "MDMP"
#define MINIDUMP_VERSION 0x5128A793

typedef struct _MINIDUMP_HEADER
{
    ULONG32 Signature;
    ULONG32 Version;
    ULONG32 NumberOfStreams;
    RVA StreamDirectoryRva;
    ULONG32 CheckSum;
    union
    {
        ULONG32 Reserved;
        ULONG32 TimeDateStamp;
    };
    ULONG64 Flags;
}
MINIDUMP_HEADER, *PMINIDUMP_HEADER;

typedef enum _MINIDUMP_TYPE {
    MiniDumpNormal                           = 0x00000000,
    MiniDumpWithDataSegs                     = 0x00000001,
    MiniDumpWithFullMemory                   = 0x00000002,
    MiniDumpWithHandleData                   = 0x00000004,
    MiniDumpFilterMemory                     = 0x00000008,
    MiniDumpScanMemory                       = 0x00000010,
    MiniDumpWithUnloadedModules              = 0x00000020,
    MiniDumpWithIndirectlyReferencedMemory   = 0x00000040,
    MiniDumpFilterModulePaths                = 0x00000080,
    MiniDumpWithProcessThreadData            = 0x00000100,
    MiniDumpWithPrivateReadWriteMemory       = 0x00000200,
    MiniDumpWithoutOptionalData              = 0x00000400,
    MiniDumpWithFullMemoryInfo               = 0x00000800,
    MiniDumpWithThreadInfo                   = 0x00001000,
    MiniDumpWithCodeSegs                     = 0x00002000,
    MiniDumpWithoutAuxiliaryState            = 0x00004000,
    MiniDumpWithFullAuxiliaryState           = 0x00008000,
    MiniDumpWithPrivateWriteCopyMemory       = 0x00010000,
    MiniDumpIgnoreInaccessibleMemory         = 0x00020000,
    MiniDumpWithTokenInformation             = 0x00040000
} MINIDUMP_TYPE;

typedef struct _MINIDUMP_LOCATION_DESCRIPTOR
{
    ULONG32 DataSize;
    RVA Rva;
}
MINIDUMP_LOCATION_DESCRIPTOR;

typedef enum _MINIDUMP_STREAM_TYPE {
    UnusedStream                = 0,
    ReservedStream0             = 1,
    ReservedStream1             = 2,
    ThreadListStream            = 3,
    ModuleListStream            = 4,
    MemoryListStream            = 5,
    ExceptionStream             = 6,
    SystemInfoStream            = 7,
    ThreadExListStream          = 8,
    Memory64ListStream          = 9,
    CommentStreamA              = 10,
    CommentStreamW              = 11,
    HandleDataStream            = 12,
    FunctionTableStream         = 13,
    UnloadedModuleListStream    = 14,
    MiscInfoStream              = 15,
    MemoryInfoListStream        = 16,
    ThreadInfoListStream        = 17,
    HandleOperationListStream   = 18,
    LastReservedStream          = 0xffff
} MINIDUMP_STREAM_TYPE;


typedef struct _MINIDUMP_DIRECTORY
{
    ULONG32 StreamType;
    MINIDUMP_LOCATION_DESCRIPTOR Location;
}
MINIDUMP_DIRECTORY, *PMINIDUMP_DIRECTORY;

#ifndef PROCESSOR_ARCHITECTURE_UNKNOWN
#define PROCESSOR_ARCHITECTURE_UNKNOWN 0xffff
#endif
#define PROCESSOR_ARCHITECTURE_INTEL   0
#define PROCESSOR_ARCHITECTURE_IA64    6
#define PROCESSOR_ARCHITECTURE_AMD64   9

#define VER_PLATFORM_WIN32s        0
#define VER_PLATFORM_WIN32_WINDOWS 1
#define VER_PLATFORM_WIN32_NT      2

#define VER_SUITE_BACKOFFICE       0x00000004
#define VER_SUITE_BLADE            0x00000400
#define VER_SUITE_COMPUTE_SERVER   0x00004000
#define VER_SUITE_DATACENTER       0x00000080
#define VER_SUITE_ENTERPRISE       0x00000002
#define VER_SUITE_EMBEDDEDNT       0x00000040
#define VER_SUITE_PERSONAL         0x00000200
#define VER_SUITE_SINGLEUSERTS     0x00000100
#define VER_SUITE_SMALLBUSINESS    0x00000001
#define VER_SUITE_SMALLBUSINESS_RESTRICTED 0x00000020
#define VER_SUITE_STORAGE_SERVER   0x00002000
#define VER_SUITE_TERMINAL         0x00000010

#pragma pack(1)
typedef struct _MINIDUMP_STRING {
    ULONG32 Length;
    WCHAR   Buffer[1];
} MINIDUMP_STRING, *PMINIDUMP_STRING;
#pragma pack()


typedef struct _MINIDUMP_SYSTEM_INFO {
    USHORT  ProcessorArchitecture;
    USHORT  ProcessorLevel;
    USHORT  ProcessorRevision;
    union {
        USHORT Reserved0;
        struct {
            UCHAR NumberOfProcessors;
            UCHAR ProductType;
        } ;
    } ;
    ULONG32 MajorVersion;
    ULONG32 MinorVersion;
    ULONG32 BuildNumber;
    ULONG32 PlatformId;
    RVA     CSDVersionRva;
    union {
        ULONG32 Reserved1;
        struct {
            USHORT SuiteMask;
            USHORT Reserved2;
        } ;
    } ;
    union {
        struct {
            ULONG32 VendorId[3];
            ULONG32 VersionInformation;
            ULONG32 FeatureInformation;
            ULONG32 AMDExtendedCpuFeatures;
        } X86CpuInfo;
        struct {
            ULONG64 ProcessorFeatures[2];
        } OtherCpuInfo;
    } Cpu;
} MINIDUMP_SYSTEM_INFO, *PMINIDUMP_SYSTEM_INFO;

#define MINIDUMP_MISC1_PROCESS_ID    0x00000001
#define MINIDUMP_MISC1_PROCESS_TIMES 0x00000002
#define MINIDUMP_MISC1_PROCESSOR_POWER_INFO 0x00000004

typedef struct _MINIDUMP_MISC_INFO {
    ULONG32 SizeOfInfo;
    ULONG32 Flags1;
    ULONG32 ProcessId;
    ULONG32 ProcessCreateTime;
    ULONG32 ProcessUserTime;
    ULONG32 ProcessKernelTime;
} MINIDUMP_MISC_INFO, *PMINIDUMP_MISC_INFO;

typedef struct _MINIDUMP_MISC_INFO_2 {
    ULONG32 SizeOfInfo;
    ULONG32 Flags1;
    ULONG32 ProcessId;
    ULONG32 ProcessCreateTime;
    ULONG32 ProcessUserTime;
    ULONG32 ProcessKernelTime;
    ULONG32 ProcessorMaxMhz;
    ULONG32 ProcessorCurrentMhz;
    ULONG32 ProcessorMhzLimit;
    ULONG32 ProcessorMaxIdleState;
    ULONG32 ProcessorCurrentIdleState;
} MINIDUMP_MISC_INFO_2, *PMINIDUMP_MISC_INFO_2;

#ifndef _WIN32
#define EXCEPTION_MAXIMUM_PARAMETERS 15 // maximum number of exception parameters
#endif

typedef struct _MINIDUMP_EXCEPTION {
    ULONG32 ExceptionCode;
    ULONG32 ExceptionFlags;
    ULONG64 ExceptionRecord;
    ULONG64 ExceptionAddress;
    ULONG32 NumberParameters;
    ULONG32 __unusedAlignment;
    ULONG64 ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
} MINIDUMP_EXCEPTION, *PMINIDUMP_EXCEPTION;

typedef struct MINIDUMP_EXCEPTION_STREAM {
    ULONG32                      ThreadId;
    ULONG32                      __alignment;
    MINIDUMP_EXCEPTION           ExceptionRecord;
    MINIDUMP_LOCATION_DESCRIPTOR ThreadContext;
} MINIDUMP_EXCEPTION_STREAM, *PMINIDUMP_EXCEPTION_STREAM;

#pragma pack(1)
typedef struct _MINIDUMP_MODULE {
    ULONG64                      BaseOfImage;
    ULONG32                      SizeOfImage;
    ULONG32                      CheckSum;
    ULONG32                      TimeDateStamp;
    RVA                          ModuleNameRva;
    VS_FIXEDFILEINFO             VersionInfo;
    MINIDUMP_LOCATION_DESCRIPTOR CvRecord;
    MINIDUMP_LOCATION_DESCRIPTOR MiscRecord;
    ULONG64                      Reserved0;
    ULONG64                      Reserved1;
} MINIDUMP_MODULE, *PMINIDUMP_MODULE;
#pragma pack()

typedef struct _MINIDUMP_MODULE_LIST {
    ULONG32         NumberOfModules;
    MINIDUMP_MODULE Modules[1];
} MINIDUMP_MODULE_LIST, *PMINIDUMP_MODULE_LIST;

#pragma pack(1)
typedef struct _MINIDUMP_MEMORY_DESCRIPTOR {
    ULONG64                      StartOfMemoryRange;
    MINIDUMP_LOCATION_DESCRIPTOR Memory;
} MINIDUMP_MEMORY_DESCRIPTOR, *PMINIDUMP_MEMORY_DESCRIPTOR;
#pragma pack()

#pragma pack(1)
typedef struct _MINIDUMP_THREAD {
    ULONG32                      ThreadId;
    ULONG32                      SuspendCount;
    ULONG32                      PriorityClass;
    ULONG32                      Priority;
    ULONG64                      Teb;
    MINIDUMP_MEMORY_DESCRIPTOR   Stack;
    MINIDUMP_LOCATION_DESCRIPTOR ThreadContext;
} MINIDUMP_THREAD, *PMINIDUMP_THREAD;
#pragma pack()

typedef struct _MINIDUMP_THREAD_LIST {
    ULONG32         NumberOfThreads;
    MINIDUMP_THREAD Threads[1];
} MINIDUMP_THREAD_LIST, *PMINIDUMP_THREAD_LIST;

typedef struct _MINIDUMP_MEMORY_LIST {
    ULONG32                    NumberOfMemoryRanges;
    MINIDUMP_MEMORY_DESCRIPTOR MemoryRanges[1];
} MINIDUMP_MEMORY_LIST, *PMINIDUMP_MEMORY_LIST;

typedef struct _MINIDUMP_MEMORY_DESCRIPTOR64 {
    ULONG64 StartOfMemoryRange;
    ULONG64 DataSize;
} MINIDUMP_MEMORY_DESCRIPTOR64, *PMINIDUMP_MEMORY_DESCRIPTOR64;

typedef ULONG64 RVA64;

typedef struct _MINIDUMP_MEMORY64_LIST {
    ULONG64 NumberOfMemoryRanges;
    RVA64 BaseRva;
    MINIDUMP_MEMORY_DESCRIPTOR64 MemoryRanges [1];
} MINIDUMP_MEMORY64_LIST, *PMINIDUMP_MEMORY64_LIST;

//-------------------------------------------------------------------------
// Excerpt from winnt.h
//-------------------------------------------------------------------------

//
//  Define the size of the 80387 save area, which is in the context frame.
//

#define SIZE_OF_80387_REGISTERS      80

#define MAXIMUM_SUPPORTED_EXTENSION     512

typedef struct _FLOATING_SAVE_AREA_x86 {
    DWORD   ControlWord;
    DWORD   StatusWord;
    DWORD   TagWord;
    DWORD   ErrorOffset;
    DWORD   ErrorSelector;
    DWORD   DataOffset;
    DWORD   DataSelector;
    BYTE    RegisterArea[SIZE_OF_80387_REGISTERS];
    DWORD   Cr0NpxState;
} FLOATING_SAVE_AREA_x86;

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//  The layout of the record conforms to a standard call frame.
//

typedef struct _CONTEXT_x86 {

    //
    // The flags values within this flag control the contents of
    // a CONTEXT record.
    //
    // If the context record is used as an input parameter, then
    // for each portion of the context record controlled by a flag
    // whose value is set, it is assumed that that portion of the
    // context record contains valid context. If the context record
    // is being used to modify a threads context, then only that
    // portion of the threads context will be modified.
    //
    // If the context record is used as an IN OUT parameter to capture
    // the context of a thread, then only those portions of the thread's
    // context corresponding to set flags will be returned.
    //
    // The context record is never used as an OUT only parameter.
    //

    DWORD ContextFlags;

    //
    // This section is specified/returned if CONTEXT_DEBUG_REGISTERS is
    // set in ContextFlags.  Note that CONTEXT_DEBUG_REGISTERS is NOT
    // included in CONTEXT_FULL.
    //

    DWORD   Dr0;
    DWORD   Dr1;
    DWORD   Dr2;
    DWORD   Dr3;
    DWORD   Dr6;
    DWORD   Dr7;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_FLOATING_POINT.
    //

    FLOATING_SAVE_AREA_x86 FloatSave;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_SEGMENTS.
    //

    DWORD   SegGs;
    DWORD   SegFs;
    DWORD   SegEs;
    DWORD   SegDs;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_INTEGER.
    //

    DWORD   Edi;
    DWORD   Esi;
    DWORD   Ebx;
    DWORD   Edx;
    DWORD   Ecx;
    DWORD   Eax;

    //
    // This section is specified/returned if the
    // ContextFlags word contians the flag CONTEXT_CONTROL.
    //

    DWORD   Ebp;
    DWORD   Eip;
    DWORD   SegCs;              // MUST BE SANITIZED
    DWORD   EFlags;             // MUST BE SANITIZED
    DWORD   Esp;
    DWORD   SegSs;

    //
    // This section is specified/returned if the ContextFlags word
    // contains the flag CONTEXT_EXTENDED_REGISTERS.
    // The format and contexts are processor specific
    //

    BYTE    ExtendedRegisters[MAXIMUM_SUPPORTED_EXTENSION];

} CONTEXT_x86;

typedef CONTEXT_x86 *PCONTEXT_x86;

//
// Define initial MxCsr and FpCsr control.
//

#define INITIAL_MXCSR 0x1f80            // initial MXCSR value
#define INITIAL_FPCSR 0x027f            // initial FPCSR value

//
// Define 128-bit 16-byte aligned xmm register type.
//

typedef struct DECLSPEC_ALIGN(16) _M128A_my {
    ULONGLONG Low;
    LONGLONG High;
} M128A_my, *PM128A_my;

//
// Format of data for 32-bit fxsave/fxrstor instructions.
//

typedef struct _XMM_SAVE_AREA32 {
    WORD   ControlWord;
    WORD   StatusWord;
    BYTE  TagWord;
    BYTE  Reserved1;
    WORD   ErrorOpcode;
    DWORD ErrorOffset;
    WORD   ErrorSelector;
    WORD   Reserved2;
    DWORD DataOffset;
    WORD   DataSelector;
    WORD   Reserved3;
    DWORD MxCsr;
    DWORD MxCsr_Mask;
    M128A_my FloatRegisters[8];
    M128A_my XmmRegisters[16];
    BYTE  Reserved4[96];
} XMM_SAVE_AREA32, *PXMM_SAVE_AREA32;

#define LEGACY_SAVE_AREA_LENGTH sizeof(XMM_SAVE_AREA32)

//
// Context Frame
//
//  This frame has a several purposes: 1) it is used as an argument to
//  NtContinue, 2) is is used to constuct a call frame for APC delivery,
//  and 3) it is used in the user level thread creation routines.
//
//
// The flags field within this record controls the contents of a CONTEXT
// record.
//
// If the context record is used as an input parameter, then for each
// portion of the context record controlled by a flag whose value is
// set, it is assumed that that portion of the context record contains
// valid context. If the context record is being used to modify a threads
// context, then only that portion of the threads context is modified.
//
// If the context record is used as an output parameter to capture the
// context of a thread, then only those portions of the thread's context
// corresponding to set flags will be returned.
//
// CONTEXT_CONTROL specifies SegSs, Rsp, SegCs, Rip, and EFlags.
//
// CONTEXT_INTEGER specifies Rax, Rcx, Rdx, Rbx, Rbp, Rsi, Rdi, and R8-R15.
//
// CONTEXT_SEGMENTS specifies SegDs, SegEs, SegFs, and SegGs.
//
// CONTEXT_DEBUG_REGISTERS specifies Dr0-Dr3 and Dr6-Dr7.
//
// CONTEXT_MMX_REGISTERS specifies the floating point and extended registers
//     Mm0/St0-Mm7/St7 and Xmm0-Xmm15).
//

typedef struct DECLSPEC_ALIGN(16) _CONTEXT_x64 {

    //
    // Register parameter home addresses.
    //
    // N.B. These fields are for convience - they could be used to extend the
    //      context record in the future.
    //

    DWORD64 P1Home;
    DWORD64 P2Home;
    DWORD64 P3Home;
    DWORD64 P4Home;
    DWORD64 P5Home;
    DWORD64 P6Home;

    //
    // Control flags.
    //

    DWORD ContextFlags;
    DWORD MxCsr;

    //
    // Segment Registers and processor flags.
    //

    WORD   SegCs;
    WORD   SegDs;
    WORD   SegEs;
    WORD   SegFs;
    WORD   SegGs;
    WORD   SegSs;
    DWORD EFlags;

    //
    // Debug registers
    //

    DWORD64 Dr0;
    DWORD64 Dr1;
    DWORD64 Dr2;
    DWORD64 Dr3;
    DWORD64 Dr6;
    DWORD64 Dr7;

    //
    // Integer registers.
    //

    DWORD64 Rax;
    DWORD64 Rcx;
    DWORD64 Rdx;
    DWORD64 Rbx;
    DWORD64 Rsp;
    DWORD64 Rbp;
    DWORD64 Rsi;
    DWORD64 Rdi;
    DWORD64 R8;
    DWORD64 R9;
    DWORD64 R10;
    DWORD64 R11;
    DWORD64 R12;
    DWORD64 R13;
    DWORD64 R14;
    DWORD64 R15;

    //
    // Program counter.
    //

    DWORD64 Rip;

    //
    // Floating point state.
    //

    union {
        XMM_SAVE_AREA32 FltSave;
        struct {
            M128A_my Header[2];
            M128A_my Legacy[8];
            M128A_my Xmm0;
            M128A_my Xmm1;
            M128A_my Xmm2;
            M128A_my Xmm3;
            M128A_my Xmm4;
            M128A_my Xmm5;
            M128A_my Xmm6;
            M128A_my Xmm7;
            M128A_my Xmm8;
            M128A_my Xmm9;
            M128A_my Xmm10;
            M128A_my Xmm11;
            M128A_my Xmm12;
            M128A_my Xmm13;
            M128A_my Xmm14;
            M128A_my Xmm15;
        };
    };

    //
    // Vector registers.
    //

    M128A_my VectorRegister[26];
    DWORD64 VectorControl;

    //
    // Special debug control registers.
    //

    DWORD64 DebugControl;
    DWORD64 LastBranchToRip;
    DWORD64 LastBranchFromRip;
    DWORD64 LastExceptionToRip;
    DWORD64 LastExceptionFromRip;
} CONTEXT_x64, *PCONTEXT_x64;

#ifndef _WIN32

typedef struct _EXCEPTION_RECORD {
    DWORD    ExceptionCode;
    DWORD ExceptionFlags;
    struct _EXCEPTION_RECORD *ExceptionRecord;
    PVOID ExceptionAddress;
    DWORD NumberParameters;
    ULONG_PTR ExceptionInformation[EXCEPTION_MAXIMUM_PARAMETERS];
    } EXCEPTION_RECORD;

typedef EXCEPTION_RECORD *PEXCEPTION_RECORD;

typedef struct _EXCEPTION_POINTERS {
    PEXCEPTION_RECORD ExceptionRecord;
    PCONTEXT_x86 ContextRecord;
} EXCEPTION_POINTERS, *PEXCEPTION_POINTERS;

#endif
