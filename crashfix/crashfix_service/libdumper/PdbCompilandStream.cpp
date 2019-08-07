//! \file PdbCompilandStream.cpp
//! \brief PDB compiland stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbCompilandStream.h"
#include "PdbDebugInfoStream.h"
#include "Buffer.h"
#include "strconv.h"
#include "PdbStreamStruct.h"
#include "PdbReader.h"

CPdbCompilandStream::CPdbCompilandStream()
	: CPdbSymbolStream()
{
    m_bInitialized = FALSE;
    m_nModuleIndex = -1;
    m_dwCompilandType = 0;
}

CPdbCompilandStream::CPdbCompilandStream(CPdbReader* pPdbReader, CMsfStream* pStream, DBI_ModuleInfo* pModuleInfo, BOOL* pbResult)	
{
    *pbResult = Init(pPdbReader, pStream, pModuleInfo);
}

CPdbCompilandStream::~CPdbCompilandStream()
{	
	Destroy();
}

BOOL CPdbCompilandStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream, DBI_ModuleInfo* pModuleInfo)
{
    //! PDB compiland stream (stream number >=10)
    //! The stream has the following structure:
    //! 1. Header including compiland name (variable length).
    //! 2. Subheader followed by array of zero-terminated string pairs.
    //!    The first string in a pair defines property name, the second one defines property value.
    //! 3. Symbol section follows (array of symbol records, as in stream #8). The size of this section can be determined from appropriate DBI stream's MODI record.
    //! 4. C13 Line numbers section follows. Size of this section can be determined from MODI record.
    //! 4.1. DWORD - maybe count or signature, or segment?
    //! 4.2. [DWORD - 16 byte] sequences (DWORD+16 byte checksum)
    //! 5. Some section follows. Leading 4 bytes is the size (size doesn't include 4 bytes).

    BOOL bResult=FALSE;
    DWORD dwOffs = 0;
    std::string str;
    std::string sParamName;
    int nCount = -1;
    DWORD dwLen = 0;
    DWORD dwLineSectionStart = 0;
    BOOL bBlocks = TRUE;
	
	// Reset stream pos
	pStream->SetStreamPos(0);

    // Allocate buffer for the entire stream
    DWORD dwStreamLen = pStream->GetStreamLen();
    CBuffer buf(dwStreamLen);
    DWORD dwBytesRead = 0;	
    BOOL bRead = pStream->ReadData(buf, dwStreamLen, &dwBytesRead, FALSE);
    if(!bRead || dwBytesRead!=dwStreamLen)
        goto cleanup;

    // Save module index
    m_nModuleIndex = pModuleInfo->m_nModuleIndex;

    // Read compiland type
    m_dwCompilandType = *(LPDWORD)buf.GetPtr();
    if(m_dwCompilandType == CT_RESFILE)
    {
        // Resource file
        bResult = true;
        goto cleanup;
    }
    else if(m_dwCompilandType != CT_OBJMODULE)
    {
        // Invalid compiland type
        goto cleanup;
    }

    //
    // Read symbols following the header.
    //

    if((LONG)dwOffs<pModuleInfo->m_Info.cbSymbols)
    {
        if(!CPdbSymbolStream::Init(pPdbReader, pStream, 4, pModuleInfo->m_Info.cbSymbols-4, &dwOffs))
            goto cleanup;
    }

    //
    // Read source file checksums and line numbers
    //

    // Read source checksum header
    //dwOffs = pModuleInfo->m_Info.cbSymbols;
    pStream->SetStreamPos(dwOffs);

    while(bBlocks)
    {
        BLOCK_HEADER_32 BlockHeader;
        bRead = pStream->ReadData((LPBYTE)&BlockHeader, sizeof(BLOCK_HEADER_32), &dwBytesRead, FALSE);
        if(!bRead || dwBytesRead!=sizeof(BLOCK_HEADER_32))
        {
            // End of file reached
            bResult = true;
            goto cleanup;
        }

        switch(BlockHeader.dwBlockType)
        {
        case 0xF4: // Source file checksums
           {
                dwLineSectionStart = pStream->GetStreamPos();

                BLOCK_HEADER_32 SrcChecksumHeader;
                bRead = pStream->ReadData((LPBYTE)&SrcChecksumHeader, sizeof(BLOCK_HEADER_32), &dwBytesRead, TRUE);
                if(!bRead || dwBytesRead!=sizeof(BLOCK_HEADER_32))
                {
                    // It seems there is no source checsums
                    bResult = true;
                    goto cleanup;
                }

                assert(SrcChecksumHeader.dwBlockType==0xF4);

                dwOffs = 0;

                // Read source file checksums
                int i;
                for(i=0; dwOffs<SrcChecksumHeader.dwLength; i++)
                {
                    SOURCE_CHECKSUM sc;
                    bRead = pStream->ReadData((LPBYTE)&sc, sizeof(SOURCE_CHECKSUM), &dwBytesRead, TRUE);
                    if(!bRead || dwBytesRead!=sizeof(SOURCE_CHECKSUM))
                    {
                        goto cleanup;
                    }

                    DWORD dwChecksumOffs = dwOffs;

                    dwOffs += sizeof(SOURCE_CHECKSUM);

                    SrcChecksum src_checksum;
                    src_checksum.m_Info = sc;

                    if(sc.wCheckSumType==CHECKSUM_MD5)
                    {
                        src_checksum.m_CheckSum.Allocate(16);
                        bRead = pStream->ReadData((LPBYTE)src_checksum.m_CheckSum, 16, &dwBytesRead, TRUE);
                        if(!bRead || dwBytesRead!=16)
                            goto cleanup;

                        LPBYTE pCheckSum = src_checksum.m_CheckSum.GetPtr();
                        char szCheckSum[128] = "";
#ifdef _WIN32
                        sprintf_s( szCheckSum, 128, "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                            pCheckSum[0],  pCheckSum[1],  pCheckSum[2],  pCheckSum[3],
                            pCheckSum[4],  pCheckSum[5],  pCheckSum[6],  pCheckSum[7],
                            pCheckSum[8],  pCheckSum[9],  pCheckSum[10], pCheckSum[11],
                            pCheckSum[12], pCheckSum[13], pCheckSum[14], pCheckSum[15]);
#else
                        sprintf( szCheckSum, "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x",
                            pCheckSum[0],  pCheckSum[1],  pCheckSum[2],  pCheckSum[3],
                            pCheckSum[4],  pCheckSum[5],  pCheckSum[6],  pCheckSum[7],
                            pCheckSum[8],  pCheckSum[9],  pCheckSum[10], pCheckSum[11],
                            pCheckSum[12], pCheckSum[13], pCheckSum[14], pCheckSum[15]);
#endif

                        src_checksum.m_sCheckSum = szCheckSum;

                        dwOffs += 16;
                    }

                    dwOffs += 2; // skip padding bytes
                    pStream->SetStreamPosRel(2);

                    m_aSrcChecksums.push_back(src_checksum);
                    m_aSrcChecksumOffsets[dwChecksumOffs] = m_aSrcChecksums.size()-1;
                }
            }
            break;
        case 0xF2: // Line numbers
            {
                SymbolLines SymLines;

                // Read symbol lines header
                SYMBOL_LINES sl;
                bRead = pStream->ReadData((LPBYTE)&sl, sizeof(SYMBOL_LINES), &dwBytesRead, TRUE);
                if(!bRead || dwBytesRead!=sizeof(SYMBOL_LINES))
                    goto cleanup;

                SymLines.m_Header = sl;

                dwOffs = sizeof(SYMBOL_LINES)-8;

                // Read line addresses following the header
                for(;dwOffs<sl.Header.dwLength;)
                {
                    LINE_ADDRESS la;
                    bRead = pStream->ReadData((LPBYTE)&la, sizeof(LINE_ADDRESS), &dwBytesRead, TRUE);
                    if(!bRead || dwBytesRead!=sizeof(LINE_ADDRESS))
                        goto cleanup;

                    SymLines.m_Lines.push_back(la);

                    dwOffs += sizeof(LINE_ADDRESS);
                }

                m_aSrcLines.push_back(SymLines);
            }
            break;
        default:
            bBlocks = FALSE;
            break;
        }
    }

    // Read the last strange section (possibly relocations or OMAP)

    // Read section length DWORD
    bRead = pStream->ReadData((LPBYTE)&dwLen, sizeof(DWORD), &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=sizeof(DWORD))
        goto cleanup;

    // Read DWORDs following the header
    nCount = dwLen/sizeof(DWORD);
    int i;
    for(i=0; i<nCount; i++)
    {
        DWORD dwNumber = 0;
        bRead = pStream->ReadData((LPBYTE)&dwNumber, sizeof(DWORD), &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=sizeof(DWORD))
            goto cleanup;

        m_aNumbers.push_back(dwNumber);
    }

    bResult=TRUE;

cleanup:

    m_bInitialized = bResult;

    return bResult;
}

void CPdbCompilandStream::Destroy()
{
	// Destroy parent
	CPdbSymbolStream::Destroy();

	m_aNumbers.clear();
	m_aSrcLines.clear();
	m_aSrcChecksumOffsets.clear();
	m_aSrcChecksums.clear();
}

DWORD CPdbCompilandStream::GetCompilandType()
{
    return m_dwCompilandType;
}

int CPdbCompilandStream::GetSrcFileCheckSumCount()
{
    return (int)m_aSrcChecksums.size();
}

SrcChecksum* CPdbCompilandStream::GetSrcFileCheckSum(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_aSrcChecksums.size())
        return NULL;

    return &m_aSrcChecksums[nIndex];
}

int CPdbCompilandStream::GetSrcFileCheckSumIndexByOffs(DWORD dwOffs)
{
    std::map<DWORD, size_t>::iterator it = m_aSrcChecksumOffsets.find(dwOffs);
    if(it==m_aSrcChecksumOffsets.end())
        return -1; // Not found such offset

    return (int)it->second;
}

int CPdbCompilandStream::GetSymbolLineCount()
{
    return (int)m_aSrcLines.size();
}

SymbolLines* CPdbCompilandStream::GetSymbolLines(int nIndex)
{
    return &m_aSrcLines[nIndex];
}

bool CPdbCompilandStream::FindSrcFileAndLineByAddr(DWORD64 dwAddr, std::wstring& sFileName, int& nLineNumber, DWORD& dwOffsInLine)
{
    sFileName = L"";
    nLineNumber = -1;

    CPdbDebugInfoStream* pDBI = m_pPdbReader->GetDebugInfoStream();
    CPdbSectionMapStream* pSectionMap = m_pPdbReader->GetSectionMapStream();

    size_t i;
    for(i=0; i<m_aSrcLines.size(); i++)
    {
        // Get section start address
        SymbolLines& lines = m_aSrcLines[i];
        IMAGE_SECTION_HEADER* pSecHdr = pSectionMap->GetSection(lines.m_Header.dwSegment-1);
        if(!pSecHdr)
            continue;

        // Get RVA and size of this section contribution
        DWORD dwStart = pSecHdr->VirtualAddress + lines.m_Header.dwOffset;
        DWORD dwSize = lines.m_Header.dwSectionContribLen;

        // Check if user-provided address belongs to this section contribution
        if(dwStart<=dwAddr && dwAddr<dwStart+dwSize)
        {
            int nCheckSumIndex = GetSrcFileCheckSumIndexByOffs(lines.m_Header.dwSrcCheckSumOffs);
            SrcChecksum* pChecksum = GetSrcFileCheckSum(nCheckSumIndex);
            assert(pChecksum);
            int nFileNameIndex = pDBI->FindSrcFileNameByModule(m_nModuleIndex, nCheckSumIndex);
            if(nFileNameIndex>=0)
            {
                sFileName = pDBI->GetSrcFileName(nFileNameIndex);
            }

            // Get line number
            DWORD dwOffs = (DWORD)dwAddr-dwStart;            
			int j;
            for(j=(int)lines.m_Lines.size()-1; j>=0; j--)
            {				
				LINE_ADDRESS& line = lines.m_Lines[j];
				if(dwOffs<line.dwOffset)
					continue;
				
				nLineNumber = line.wNumber;				
				dwOffsInLine = dwOffs-line.dwOffset;				
				break;
			}
			
            return true;
        }
    }

    // No source file information found
    return false;
}
