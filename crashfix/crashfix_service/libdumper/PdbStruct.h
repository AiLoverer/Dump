#pragma once
#include "stdafx.h"

#define PDB_UNUSED_16       (( WORD) -1) // Unused stream (16-bit).
#define PDB_UNUSED_32       ((DWORD) -1) // Unused stream (32-bit).

#define PDB_TI_MIN          0x00001000   // Type index base.
#define PDB_TI_MAX          0x00FFFFFF   // Type index limit.

// PDB stream IDs
#define PDB_STREAM_ROOT     0 // PDB root directory stream
#define PDB_STREAM_PDB      1 // PDB info stream
#define PDB_STREAM_TPI      2 // Type info
#define PDB_STREAM_DBI      3 // Debug info stream
#define PDB_STREAM_LNK      4 // Link info stream
#define PDB_STREAM_FPO      5 // FPO records stream
#define PDB_STREAM_NEW_FPO  13 // New FPO stream (or 11 ???)

#define PDB_VERSION_200     200        // binary version number
#define PDB_SIGNATURE_200   "Microsoft C/C++ program database 2.00\r\n\032JG\0"
#define PDB_SIGNATURE_200_SIZE  0x2C   // signature size (bytes)

#define PDB_VERSION_700         700    // binary version number
#define PDB_SIGNATURE_700   "Microsoft C/C++ MSF 7.00\r\n\032DS\0\0"
#define PDB_SIGNATURE_700_SIZE  0x20   // signature size (bytes)

typedef struct _PDB_STREAM_200
{
    DWORD dwStreamBytes;            // stream size (-1 = unused)
    PVOID pReserved;                // implementation dependent
}
PDB_STREAM_200, *PPDB_STREAM_200;

typedef struct _PDB_HEADER_200
{
    BYTE           achSignature [PDB_SIGNATURE_200_SIZE]; // version ID
    DWORD          dwPageSize;     // 0x0400, 0x0800, 0x1000
    WORD           wStartPage;     // 0x0009, 0x0005, 0x0002
    WORD           wPageCount;     // file size / dPageBytes
    PDB_STREAM_200 RootStream;     // stream directory
    WORD           awRootPages[1]; // pages containing PDB_ROOT_200
}
PDB_HEADER_200, *PPDB_HEADER_200, **PPPDB_HEADER_200;

typedef struct _PDB_ROOT_200
{
    WORD           wStreams;       // number of streams
    WORD           wReserved;      // not used
    PDB_STREAM_200 aStreams[1];    // stream size list
}
PDB_ROOT_200, *PPDB_ROOT_200;

typedef struct _PDB_HEADER_700
{
    BYTE  achSignature[PDB_SIGNATURE_700_SIZE]; // version ID
    DWORD dwPageSize;              // 0x0400
    DWORD dwPageFlags;              // 0x0002
    DWORD dwPageCount;              // file size / dPageBytes
    DWORD dwRootStreamSize;         // stream directory size
    DWORD dwReserved;               // 0
    DWORD adwRootStreamPages[1];    // root page index pages
}
PDB_HEADER_700, *PPDB_HEADER_700;

typedef struct _PDB_ROOT_700
{
    DWORD dwStreams;                // number of streams
    DWORD adwStreamBytes[1];        // stream size list
}
PDB_ROOT_700, *PPDB_ROOT_700;

typedef struct _PDB_VERSION
{
    DWORD dwVersion;                // version number
    DWORD dwHeader;                 // header size
    PBYTE pchSignature;             // version ID
}
PDB_VERSION, *PPDB_VERSION;


