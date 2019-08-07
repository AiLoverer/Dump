#pragma once
#include "stdafx.h"

// http://msdn.microsoft.com/en-us/windows/hardware/gg463119.aspx
// http://jpassing.com/category/debugging/
// http://www.debuginfo.com/articles/debuginfomatch.html
// http://www.debuginfo.com/articles/gendebuginfo.html

#define PtrFromRva( base, rva ) ( ( ( PUCHAR ) base ) + rva )

/**
* PDB v2.0 in image debug info.
* The URL is constructed from the timestamp and the %02x age?
*/
typedef struct CV_INFO_PDB20
{
    ULONG    Signature;              /**< CV_SIGNATURE_PDB70. */
    ULONG     Offset;                 /**< Always 0. Used to be the offset to the real debug info. */
    ULONG    TimeDateStamp;
    ULONG    Age;
    CHAR     PdbFilename[4];
} CV_INFO_PDB20;

#define ANYSIZE_ARRAY 1

typedef struct _CV_INFO_PDB70
{
    ULONG CvSignature;
    GUID Signature;
    ULONG Age;
    UCHAR PdbFileName[ ANYSIZE_ARRAY ];
} CV_INFO_PDB70, *PCV_INFO_PDB70;

#define CV_SIGNATURE_PDB70 "NB10"

typedef struct {
    WORD             wLength;
    WORD             wValueLength;
    WORD             wType;
    WCHAR            szKey[15];
    WORD             Padding1;
    VS_FIXEDFILEINFO Value;
    WORD             Padding2;
    WORD             Children;
} VS_VERSIONINFO;

//----------------------------------------
// x64 unwind-related definitions
//----------------------------------------

typedef enum _UNWIND_OP_CODES {
    UWOP_PUSH_NONVOL = 0, /* info == register number */
    UWOP_ALLOC_LARGE,     /* no info, alloc size in next 2 slots */
    UWOP_ALLOC_SMALL,     /* info == size of allocation / 8 - 1 */
    UWOP_SET_FPREG,       /* no info, FP = RSP + UNWIND_INFO.FPRegOffset*16 */
    UWOP_SAVE_NONVOL,     /* info == register number, offset in next slot */
    UWOP_SAVE_NONVOL_FAR, /* info == register number, offset in next 2 slots */
    UWOP_SAVE_XMM128,     /* info == XMM reg number, offset in next slot */
    UWOP_SAVE_XMM128_FAR, /* info == XMM reg number, offset in next 2 slots */
    UWOP_PUSH_MACHFRAME   /* info == 0: no error-code, 1: error-code */
} UNWIND_CODE_OPS;

typedef unsigned char UBYTE;

typedef union _UNWIND_CODE {
    struct {
        UBYTE CodeOffset;
        UBYTE UnwindOp : 4;
        UBYTE OpInfo   : 4;
    };
    USHORT FrameOffset;
} UNWIND_CODE, *PUNWIND_CODE;

#define UNW_FLAG_EHANDLER  0x01
#define UNW_FLAG_UHANDLER  0x02
#define UNW_FLAG_CHAININFO 0x04

typedef struct _UNWIND_INFO {
    UBYTE Version       : 3;
    UBYTE Flags         : 5;
    UBYTE SizeOfProlog;
    UBYTE CountOfCodes;
    UBYTE FrameRegister : 4;
    UBYTE FrameOffset   : 4;
    UNWIND_CODE UnwindCode[1];
/*  UNWIND_CODE MoreUnwindCode[((CountOfCodes + 1) & ~1) - 1];
*   union {
*       OPTIONAL ULONG ExceptionHandler;
*       OPTIONAL ULONG FunctionEntry;
*   };
*   OPTIONAL ULONG ExceptionData[]; */
} UNWIND_INFO, *PUNWIND_INFO;

typedef struct _RUNTIME_FUNCTION {
    ULONG BeginAddress;
    ULONG EndAddress;
    ULONG UnwindData;
} RUNTIME_FUNCTION, *PRUNTIME_FUNCTION;

#define GetUnwindCodeEntry(info, index) \
    ((info)->UnwindCode[index])

#define GetLanguageSpecificDataPtr(info) \
    ((PVOID)&GetUnwindCodeEntry((info),((info)->CountOfCodes + 1) & ~1))

#define GetExceptionHandler(base, info) \
    ((PEXCEPTION_HANDLER)((base) + *(PULONG)GetLanguageSpecificDataPtr(info)))

#define GetChainedFunctionEntry(base, info) \
    ((PRUNTIME_FUNCTION)((base) + *(PULONG)GetLanguageSpecificDataPtr(info)))

#define GetExceptionDataPtr(info) \
    ((PVOID)((PULONG)GetLanguageSpecificData(info) + 1)

#pragma pack(1)
struct EXCEPTION_REGISTRATION
{
   EXCEPTION_REGISTRATION *prev;
   DWORD handler;
   int   id;
   DWORD ebp;
};
#pragma pack()

