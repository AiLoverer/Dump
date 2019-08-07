//! \file PdbCompilandStream.h
//! \brief PDB compiland stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbSymbolStream.h"
#include "Buffer.h"

// Compiland types
#define CT_OBJMODULE    0x4        // Object module
#define CT_RESFILE      0xEFFEEFFE // Compiled resources

// Checksum types
#define CHECKSUM_MD5        0x110 // MD5 hash


#pragma pack(1)
struct BLOCK_HEADER_32
{
    DWORD  dwBlockType;    // Signature.
    DWORD  dwLength;       // Length of the block (not including length of this header).  
};
#pragma pack()

#pragma pack(1)
struct SOURCE_CHECKSUM
{
    DWORD dwNameOffset;     // Offset of source file name
    WORD  wCheckSumType;    // Type of checksum 
    // Checksum follows this header.
    // Padding bytes follow the checksum.
};
#pragma pack()

#pragma pack(1)
struct SYMBOL_LINES
{
    BLOCK_HEADER_32 Header;    // Block header
    DWORD dwOffset;            // Line offset relative to segment start.
    DWORD dwSegment;           // Segment (section) index.
    DWORD dwSectionContribLen; // Length of section contribution for this code, in bytes.
    DWORD dwSrcCheckSumOffs;   // Offset of source file checksum record
    DWORD dwLineCount;         // Count of line records following this record
    DWORD dwLineEnd;           // Ending line number
    // Line addresses follow this header.
};
#pragma pack()

#pragma pack(1)
struct LINE_ADDRESS  
{
    DWORD dwOffset;      // Line offset from beginning of symbol.
    WORD  wNumber;       // Line number
    WORD  wFlags;        // Flags???
};
#pragma pack()


struct DBI_ModuleInfo;

struct SrcChecksum
{
    SOURCE_CHECKSUM m_Info;
    CBuffer m_CheckSum;      // Source file checksum (16 bytes).
    std::string m_sCheckSum; // Source file checksum string
};

struct SymbolLines
{
    SYMBOL_LINES m_Header;
    std::vector<LINE_ADDRESS> m_Lines;
};

//! \class CPdbCompilandStream
//! \brief Implements a compiland stream of a PDB file.
class CPdbCompilandStream : public CPdbSymbolStream
{
public:

    //! Constructor.
    CPdbCompilandStream();

    //! Constructor.
    //! @param[in] pStream     Stream to read from.
    //! @param[in] pModuleInfo Compiland info taken from DBI stream.
    //! @param[out] pbResult   Initialization result.
    CPdbCompilandStream(CPdbReader* pPdbReader, CMsfStream* pStream, DBI_ModuleInfo* pModuleInfo, BOOL* pbResult);

    //! Destructor.
    virtual ~CPdbCompilandStream();

    //! Initializes the compiland stream.
    //! @param[in] pStream     Stream to read from.
    //! @param[in] pModuleInfo Compiland info taken from DBI stream.
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream, DBI_ModuleInfo* pModuleInfo);

	//! Destroys the stream.
	virtual void Destroy();

    //! Returns compiland type
    DWORD GetCompilandType();

    //! Returns count of source files that contributed to this compiland.
    int GetSrcFileCheckSumCount();

    //! Returns source file info 
    SrcChecksum* GetSrcFileCheckSum(int nIndex);

    //! Returns checskum's index by offset
    int GetSrcFileCheckSumIndexByOffs(DWORD dwOffs);

    //! Returns count of src line blocks
    int GetSymbolLineCount();

    //! Returns symbol lines
    SymbolLines* GetSymbolLines(int nIndex);

    //! Finds src file and line by address.
    bool FindSrcFileAndLineByAddr(DWORD64 dwAddr, std::wstring& sFileName, int& nLineNumber, DWORD& dwOffsInLine);

private:

    int m_nModuleIndex;                           //!< Module index.
    DWORD m_dwCompilandType;                      //!< Compiland type.
    std::vector<SrcChecksum> m_aSrcChecksums;     //!< Source file checksums.
    std::map<DWORD, size_t> m_aSrcChecksumOffsets; //!< Offsets of src file checksums
    std::vector<SymbolLines> m_aSrcLines;         //!< Source file lines.
    std::vector<DWORD> m_aNumbers;                //!< ???
};

