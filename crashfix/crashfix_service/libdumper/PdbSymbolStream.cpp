//! \file PdbSymbolStream.cpp
//! \brief PDB stream containing symbol records.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbSymbolStream.h"
#include "PdbSectionMapStream.h"
#include "PdbSymLexicalScope.h"
#include "PdbReader.h"
#include "PdbSymbol.h"
#include "Buffer.h"
#include "Log.h"
#include "strconv.h"

//---------------------------------------------------------------
// CPdbSymbolStream implementation
//---------------------------------------------------------------

UINT CPdbSymbolStream::m_uLexicalScopeIdSeed = 0;

CPdbSymbolStream::CPdbSymbolStream()
	: CBasePdbStream()
{
    m_pLexicalScope = NULL;
}

CPdbSymbolStream::CPdbSymbolStream(
	CPdbReader* pPdbReader, 
	CMsfStream* pStream, 
	DWORD dwOffsetInStream, 
	UINT uSize, 
	BOOL* pbResult,
	DWORD dwFlags) 
	: CBasePdbStream()
{
    *pbResult = Init(pPdbReader, pStream, dwOffsetInStream, uSize, 0, dwFlags);
}

BOOL CPdbSymbolStream::Init(
	CPdbReader* pPdbReader, 
	CMsfStream* pStream, 
	DWORD dwOffsetInStream, 
	UINT uSize, 
	LPDWORD pdwNewOffsInStream,
	DWORD dwFlags)
{
    BOOL bResult = FALSE;
    DWORD64 dwRVA = 0;

    uSize; // unused parameter

    // Init output
    if(pdwNewOffsInStream)
        *pdwNewOffsInStream = 0;

	CAutoLock lock(&pPdbReader->m_AccessLock);
	
	// Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;
		
	// Get section map stream (we need it to determine symbol RVAs).
    CPdbSectionMapStream* pSectionStream = pPdbReader->GetSectionMapStream();
    assert(pSectionStream!=NULL);

	if((dwFlags&PSS_NO_LEXICAL_SCOPE_TREE)==0)
	{
		// Create module (top-level) lexical symbol scope
		m_pLexicalScope = new CPdbSymLexicalScope(GetUniqueLexicalScopeId(), NULL);
	}

	// Set current scope to be module scope
    //CPdbSymLexicalScope* pCurLexicalScope = m_pLexicalScope;

    DWORD dwOffs = dwOffsetInStream; // Offset in stream
    pStream->SetStreamPos(dwOffs);
    SYM_ALIGN as;
    int nSymCount = 0;
    for(;;)
    {
        // Read block header
        DWORD dwBytesRead = 0;
        BOOL bRead = pStream->ReadData((LPBYTE)(&as), sizeof(SYM_ALIGN), &dwBytesRead, FALSE);
        if(!bRead || dwBytesRead!=sizeof(SYM_ALIGN))
        {
            if(dwBytesRead==0)
                break; // End of stream reached
            else
                goto cleanup; // Error
        }

        // Check symbol record length
        if(as.wSymLength==0 || as.wSymType==0)
        {
            break; // End of symbols reached
        }

        // Allocate buffer for symbol record data
        CBuffer buf(as.wSymLength+2);

        // Read symbol
        bRead = pStream->ReadData(buf, as.wSymLength+2, &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=(DWORD)as.wSymLength+2)
            goto cleanup;

        // Increment symbol count
        nSymCount++;

        PSYM_VARIANT pSym = (PSYM_VARIANT)(LPBYTE)buf;
        //CPdbSymbol* pSymRec = NULL;
		DWORD wSegment = 0;
		DWORD wOffs = 0;
		
        switch(as.wSymType)
        {
        case S_PUB32: // Public symbol
            {
                SYM_PUB32* pPub32 = &pSym->Pub32;
                wSegment = pPub32->wSegment;
                wOffs = pPub32->dwOffset;

                break;
            }
        case S_LDATA32: // Local variable
        case S_GDATA32: // Global variable
            {
                // Set symbol properties
                SYM_DATA32* pData = &pSym->Data32;
                wSegment = pData->wSegment;
                wOffs = pData->dwOffset;
                
				break;
            }
        case S_LPROC32: // Local procedure
        case S_GPROC32: // Global procedures
            {
                // Set symbol properties
                SYM_PROC32* pProc = &pSym->Proc32;
                wSegment = pProc->wSegment;
                wOffs = pProc->dwOffset;

                break;
            }
        }
		        
        // Add a entry to list
		m_aSymOffsets.push_back(dwOffs);

		// Determine section index by segment index
		if(wSegment!=0)
		{
			int nSection = (int)wSegment-1;
			if(nSection>=0 && pSectionStream!=NULL)
			{
				// Get section info
				IMAGE_SECTION_HEADER* pSection = pSectionStream->GetSection(nSection);
				if(pSection!=NULL)
				{
					// Determine symbol RVA as section virtual address + sym offset
					dwRVA = pSection->VirtualAddress+wOffs;
					// Add an entry to RVA index
					m_aSymRvas[dwRVA] = m_aSymOffsets.size()-1;
				}            
			}
		}

        // Increment offset in stream
        dwOffs += as.wSymLength+2;
    }

	// Success
    bResult = TRUE;

cleanup:

	// Return current offset in stream
    if(pdwNewOffsInStream!=NULL)
    {
        *pdwNewOffsInStream = dwOffs;
    }

    return bResult;
}

CPdbSymbolStream::~CPdbSymbolStream()
{
	Destroy();
}

void CPdbSymbolStream::Destroy()
{
	//CAutoLock lock(&m_pPdbReader->m_AccessLock);

	// Free symbols
	size_t i;
	for(i=0; i<m_aSymbols.size(); i++)
	{
		delete m_aSymbols[i];
	}
	m_aSymbols.clear();
	m_aSymOffsets.clear();
	m_aSymRvas.clear();

	// Free lexical scopes
	if(m_pLexicalScope)
	{
		delete m_pLexicalScope;
		m_pLexicalScope = NULL;
	}

	// Reset lexical scope ID seed
	m_uLexicalScopeIdSeed = 0;
}

int CPdbSymbolStream::GetSymbolCount()
{
	CAutoLock lock(&m_pPdbReader->m_AccessLock);
    return (int)m_aSymOffsets.size();
}

CPdbSymbol* CPdbSymbolStream::GetSymbol(int nIndex)
{	
	CAutoLock lock(&m_pPdbReader->m_AccessLock);

	// Check if the symbol with such an index has already been instantiated
	std::map<size_t, CPdbSymbol*>::iterator it = m_aSymbols.find(nIndex);
    if(it!=m_aSymbols.end())
		return it->second; // This symbol was already instantiated, return it

	// We have to create new symbol instance and return it.

	if(nIndex<0 || nIndex>=(int)m_aSymOffsets.size())
		return NULL; // No such symbol index

	// Get symbol's offset in stream	
	DWORD dwOffs = m_aSymOffsets[nIndex]; // Offset in stream
    m_pRawStream->SetStreamPos(dwOffs);
    
	SYM_ALIGN as;
    
	// Read block header
    DWORD dwBytesRead = 0;
    BOOL bRead = m_pRawStream->ReadData((LPBYTE)(&as), sizeof(SYM_ALIGN), &dwBytesRead, FALSE);
    if(!bRead || dwBytesRead!=sizeof(SYM_ALIGN))
    {
        if(dwBytesRead==0)
            return NULL; // End of stream reached
        else
            return NULL; // Error
    }

    // Check symbol record length
    if(as.wSymLength==0 || as.wSymType==0)
    {
        return NULL; // End of symbols reached
    }

    // Allocate buffer for symbol record data
    CBuffer buf(as.wSymLength+2);

    // Read symbol
    bRead = m_pRawStream->ReadData(buf, as.wSymLength+2, &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=(DWORD)as.wSymLength+2)
        return NULL;
	        
    PSYM_VARIANT pSym = (PSYM_VARIANT)(LPBYTE)buf;
    CPdbSymbol* pSymRec = NULL;

    switch(as.wSymType)
    {
    case S_PUB32: // Public symbol
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_PUB32* pPub32 = &pSym->Pub32;
            pSymRec->SetName(std::string((const char*)pPub32->uchName));
            pSymRec->SetSegment(pPub32->wSegment);
            pSymRec->SetOffset(pPub32->dwOffset);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
            break;
        }
    case S_CONSTANT: // Constant
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_CONSTANT* pConst = &pSym->Constant;
            pSymRec->SetName(std::string((const char*)pConst->uchName));

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            break;
        }
    case S_LDATA32: // Local variable
    case S_GDATA32: // Global variable
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_DATA32* pData = &pSym->Data32;
            pSymRec->SetSegment(pData->wSegment);
            pSymRec->SetOffset(pData->dwOffset);
            pSymRec->SetName(std::string((const char*)pData->uchName));

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
            break;
        }
    case S_UDT: // User defined type
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_UDT* pUdt = &pSym->Udt;
            pSymRec->SetName(std::string((const char*)pUdt->uchName));

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
            break;
        }
    case S_LPROC32: // Local procedure
    case S_GPROC32: // Global procedures
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_PROC32* pProc = &pSym->Proc32;
            pSymRec->SetName(std::string((const char*)pProc->uchName));
            pSymRec->SetSegment(pProc->wSegment);
            pSymRec->SetOffset(pProc->dwOffset);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            // Create nested procedure scope
            //CPdbSymLexicalScope* pProcScope = new CPdbSymLexicalScope(GetUniqueLexicalScopeId(), pCurLexicalScope);
            //pCurLexicalScope->AddChildScope(pProcScope);
            // Make procedure scope current one
            //pCurLexicalScope = pProcScope;

            break;
        }
    case S_LPROCREF: // Reference to local procedure
    case S_PROCREF:  // Reference to global procedure
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            //SYM_PROCREF* pProcRef = &pSym->ProcRef;
            //pSymRec->SetName(std::string((const char*)pRef->sumName));

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            break;
        }
    case S_LABEL32:  // Code label
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_LABEL32* pLabel = &pSym->Label32;
            pSymRec->SetName(std::string((const char*)pLabel->uchName));
            pSymRec->SetSegment(pLabel->wSegment);
            pSymRec->SetOffset(pLabel->dwOffset);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            break;
        }
    case S_FRAMEPROC:  // Frame procedure
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            //SYM_FRAMEPROC* pFrameProc = &pSym->FrameProc;

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            break;
        }
    case S_UNKNOWN113A: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            //SYM_FRAMEPROC* pFrameProc = &pSym->FrameProc;
            //pSymRec->SetName(std::string((const char*)pFrameProc->uchName));

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_BPREL32: // BP relative
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_BPREL_32* pBPRel = &pSym->BpRelative32;
            pSymRec->SetName(std::string((const char*)pBPRel->uchName));
            pSymRec->SetOffset(pBPRel->lOffset);
            pSymRec->SetTypeIndex(pBPRel->dwTypeIndex);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_END: // End of lexical scope
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            // Make parent lexical scope current
            //pCurLexicalScope = pCurLexicalScope->GetParentLexicalScope();

            //assert(pCurLexicalScope!=NULL);

            break;
        }
    case S_BLOCK32: // Block start
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            SYM_BLOCK32* pBlock = &pSym->Block32;
            pSymRec->SetName(std::string((const char*)pBlock->uchName));
            pSymRec->SetOffset(pBlock->dwOffset);
            pSymRec->SetSegment(pBlock->wSegment);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            // Create nested block scope
            //CPdbSymLexicalScope* pBlockScope = new CPdbSymLexicalScope(GetUniqueLexicalScopeId(), pCurLexicalScope);
            //pCurLexicalScope->AddChildScope(pBlockScope);
            // Make block scope current one
            //pCurLexicalScope = pBlockScope;
        }
        break;
    case S_OBJNAME: // Object file name
        {
            pSymRec = new CPdbObjNameSymbol(this, pSym);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_COMPILE2: // Compile
        {
            pSymRec = new CPdbCompileSymbol(this, pSym);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_THUNK32:
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            SYM_THUNK32* pThunk = &pSym->Thunk32;
            pSymRec->SetName((char*)pThunk->uchName);
            pSymRec->SetSegment(pThunk->wSegment);
            pSymRec->SetOffset(pThunk->dwOffset);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

            // Create nested thunk scope
            //CPdbSymLexicalScope* pThunkScope = new CPdbSymLexicalScope(GetUniqueLexicalScopeId(), pCurLexicalScope);
            //pCurLexicalScope->AddChildScope(pThunkScope);
            // Make thunk scope current one
            //pCurLexicalScope = pThunkScope;
        }
        break;
    case S_REGISTER: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            SYM_REGISTER* pReg = &pSym->Register;
            pSymRec->SetName((char*)pReg->uchName);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_TRAMPOLINE: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            //SYM_TRAMPOLINE* pTramp = &pSym->Trampoline;

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_UNKNOWN1136: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_UNKNOWN1137: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_UNKNOWN1138: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
    case S_UNKNOWN1139: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);

            // Set symbol properties
            //SYM_FRAMEPROC* pFrameProc = &pSym->FrameProc;
            //pSymRec->SetName(std::string((const char*)pFrameProc->uchName));

            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

        }
        break;
    case S_REGREL32:
        {                   
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);
                
            // Set symbol properties
            REGREL32* pRegRel32 = &pSym->RegRel32;
            pSymRec->SetName(std::string((const char*)pRegRel32->name));
                
            //pCurLexicalScope->AddSymbol(pSymRec);
        }
        break;
	case S_UNKNOWN113C: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);
        
            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

        }
		break;
	case S_UNKNOWN113D: //
        {
            pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);
        
            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);

        }
        break;
    default:
		{
			//assert(0);
			pSymRec = new CPdbSymbol(this, (_SYM_ENUM_e)as.wSymType);
        
            // Add this symbol to current lexical scope
            //pCurLexicalScope->AddSymbol(pSymRec);				
		}
        break;
    }

    assert(pSymRec!=NULL);
    //assert(pSymRec->GetLexicalScope()!=NULL);

    // Add a entry to list
    m_aSymbols[nIndex] = pSymRec;
    
	// Get section map stream (we need it to determine symbol RVAs).
    CPdbSectionMapStream* pSectionStream = m_pPdbReader->GetSectionMapStream();
    assert(pSectionStream!=NULL);

	// Determine section index by segment index
    int nSection = (int)pSymRec->GetSegment()-1;
    if(nSection>=0 && pSectionStream!=NULL)
    {
		// Get section info
        IMAGE_SECTION_HEADER* pSection = pSectionStream->GetSection(nSection);
        if(pSection!=NULL)
        {
			// Determine symbol RVA as section virtual address + sym offset
            DWORD dwRVA = pSection->VirtualAddress+pSymRec->GetOffset();
			// Save RVA
            pSymRec->SetRelativeVirtualAddr(dwRVA);			
        }            
    }
	
    return pSymRec;
}

int CPdbSymbolStream::GetSymbolIndexByOffsInStream(DWORD dwStartOffs)
{
	CAutoLock lock(&m_pPdbReader->m_AccessLock);

	// Look for symbol whose offset in stream equals to the given offset
    std::vector<DWORD>::iterator it = 
		std::find(m_aSymOffsets.begin(), m_aSymOffsets.end(), dwStartOffs);
    if(it==m_aSymOffsets.end())
        return -1; // Not found

	// Return found symbol's index
    return (int)(*it);
}

int CPdbSymbolStream::GetSymbolIndexByAddr(DWORD64 dwAddr)
{
	CAutoLock lock(&m_pPdbReader->m_AccessLock);

	// Look for symbol to whome the provided address belongs
    if(m_aSymRvas.size()==0)
        return -1; // No symbols

    std::map<DWORD64, size_t>::iterator upper = m_aSymRvas.upper_bound(dwAddr);
    upper--;
    if(upper!=m_aSymRvas.end())
	{
		// Found
        return (int)upper->second;
	}

    return -1; // Not found
}

CPdbSymLexicalScope* CPdbSymbolStream::GetModuleLexicalScope()
{
	CAutoLock lock(&m_pPdbReader->m_AccessLock);

	// Return module (top-level) lexical scope
    return m_pLexicalScope;
}

UINT CPdbSymbolStream::GetUniqueLexicalScopeId()
{
	CAutoLock lock(&m_pPdbReader->m_AccessLock);

	// Create unique lexical scope ID
    return ++m_uLexicalScopeIdSeed;
}

bool CPdbSymbolStream::AddSymRva(DWORD64 dwRva, size_t nSymIndex)
{
	CAutoLock lock(&m_pPdbReader->m_AccessLock);

	// Update sym RVA index
    std::map<DWORD64, size_t>::iterator it = m_aSymRvas.find(dwRva);
    if(it!=m_aSymRvas.end())
    {
		// Such an RVA is already in the list!
        assert(0);
        return false;
    }

	// Insert new RVA 
    m_aSymRvas[dwRva] = nSymIndex;
    return true;
}
