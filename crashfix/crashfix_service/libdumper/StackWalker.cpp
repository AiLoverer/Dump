//! \file StackWalker.cpp
//! \brief Stack trace functionality.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "StackWalker.h"
#include "DumpStruct.h"
#include "PeStruct.h"

CStackWalker::CStackWalker()
{
    m_pMdmpReader = NULL;
    m_pThreadContext = NULL;
    m_uThreadContextSize = 0;
    m_pPdbCache = NULL;
	m_bExactMatchBuildAge = false;
}

CStackWalker::~CStackWalker()
{
    Destroy();
}

bool CStackWalker::Init(CMiniDumpReader* pMiniDump, CPdbCache* pPdbCache, DWORD dwThreadId, bool bExactMatchBuildAge)
{
	bool bResult = false;
	m_sErrorMsg = L"Unspecified error.";
	BOOL bGetContext = FALSE;
	DWORD dwMachineType = 0;
	MiniDumpSystemInfo* pSysInfo = NULL;

    // Validate input params
    if(pMiniDump==NULL ||
        pPdbCache==NULL)
	{
		m_sErrorMsg = L"Stack walker couldn't initialize itself (invalid argument passed).";
		goto cleanup;
	}

	// Free resources (in case previously initialized)
	Destroy();

    // Save input vars for later use
    m_pMdmpReader = pMiniDump;
    m_pPdbCache = pPdbCache;
	m_bExactMatchBuildAge = bExactMatchBuildAge;

	// Determine machine type (x86 or x64)
	pSysInfo = pMiniDump->GetSystemInfo();
	dwMachineType = pSysInfo->m_uProcessorArchitecture;

	// Get thread context
	bGetContext = pMiniDump->GetThreadContext(dwThreadId, &m_pThreadContext, &m_uThreadContextSize);
	if(!bGetContext || m_pThreadContext==NULL || m_uThreadContextSize==0)
	{
		m_sErrorMsg = L"Error getting thread context from minidump.";
		goto cleanup;
	}

    // Init stack frame with correct initial values
    // See this:
    // http://www.codeproject.com/KB/threads/StackWalker.aspx
    //
    //  1. Always set AddrPC to the current instruction pointer (Eip on x86, Rip on x64 and StIIP on IA64)
    //  2. Always set AddrStack to the current stack pointer (Esp on x86, Rsp on x64 and IntSp on IA64)
    //  3. Set AddrFrame to the current frame pointer when meaningful. On x86 this is Ebp, on x64 you
    //     can use Rbp (but is not used by VC2005B2; instead it uses Rdi!) and on IA64 you can use RsBSP.
    //     StackWalk64 will ignore the value when it isn't needed for unwinding.
    //  4. Set AddrBStore to RsBSP for IA64.

    switch(dwMachineType)
    {
    case PROCESSOR_ARCHITECTURE_INTEL:
        {
			// x86 architecture
			PCONTEXT_x86 pThreadContextX86 = (PCONTEXT_x86)m_pThreadContext;
            m_StackFrame.m_dwAddrPC = pThreadContextX86->Eip;
            m_StackFrame.m_dwAddrStack = pThreadContextX86->Esp;
            m_StackFrame.m_dwAddrFrame = pThreadContextX86->Ebp;
            m_StackFrame.m_dwAddrBStore = 0;
        }
        break;
    case PROCESSOR_ARCHITECTURE_AMD64:
        {
			// x64 architecture
			PCONTEXT_x64 pThreadContextX64 = (PCONTEXT_x64)m_pThreadContext;
            m_StackFrame.m_dwAddrPC = pThreadContextX64->Rip;
            m_StackFrame.m_dwAddrStack = pThreadContextX64->Rsp;
            m_StackFrame.m_dwAddrFrame = pThreadContextX64->Rbp;
            m_StackFrame.m_dwAddrBStore = 0;
        }
        break;
    default:
        {
            // Unsupported architecture
            m_sErrorMsg = L"Couldn't walk the stack, because of unsupported processor architecture.";
            return false;
        }
    }

    // Done
	m_sErrorMsg = L"Success";
	bResult = true;

cleanup:

    return bResult;
}

void CStackWalker::Destroy()
{
	// Free thread context memory
	if(m_pThreadContext)
	{
		delete [] m_pThreadContext;		
	}

	m_pThreadContext = NULL;
    m_uThreadContextSize = 0;	
	m_pMdmpReader = NULL;
    m_pPdbCache = NULL;
}

CStackFrame* CStackWalker::GetStackFrame()
{
    return &m_StackFrame;
}

BOOL CStackWalker::FirstStackFrame()
{
    return NextStackFrame(TRUE);
}

BOOL CStackWalker::NextStackFrame(BOOL bFirstFrame)
{
    DWORD dwValue = 0;
    DWORD dwBytesRead = 0;

    if(!bFirstFrame)
    {
        // Check infinite loop
        if(m_StackFrame.m_dwAddrPC == m_StackFrame.m_dwAddrReturn)
            return FALSE;

        // Update program counter
        m_StackFrame.m_dwAddrPC = m_StackFrame.m_dwAddrReturn;
    }

    // Determine if we have AMD64 stack
    BOOL bAMD64 = m_pMdmpReader->GetSystemInfo()->m_uProcessorArchitecture==PROCESSOR_ARCHITECTURE_AMD64;

    // Find module the instruction pointer register address belongs to
    int nModuleIndex = m_pMdmpReader->FindModuleIndexByAddr(m_StackFrame.m_dwAddrPC);
    MiniDumpModuleInfo* pModuleInfo = m_pMdmpReader->GetModuleInfo(nModuleIndex);
    if(pModuleInfo)
    {
        // Module found, now find matching PDB/image files for the module
        CPdbReader* pPdbReader = NULL;
        CPeReader* pPeReader = NULL;
		int nEntry = -1;
		bool bFind = m_pPdbCache->FindPdb(pModuleInfo->GUIDnAge(), pModuleInfo->m_sPdbFileName,
            pModuleInfo->m_sModuleName, &pPdbReader, &pPeReader, &nEntry, NULL, m_bExactMatchBuildAge);
        if(bFind)
        {
            // Matching PDB/PE found

            if(!bAMD64) // x86
            {
                // Find FPO record that the address belongs to

				//CPdbFPOStream* pFPOStream = pPdbReader->GetFPOStream();
				//CPdbFPOStream* pNewFPOStream = pPdbReader->GetNewFPOStream();
				//if(pNewFPOStream)
				//{
					//DWORD dwOffsInModule = (DWORD)(m_StackFrame.m_dwAddrPC-pModuleInfo->m_dwBaseAddr);
				    //int nFPORec = pNewFPOStream->FindFPORecordByAddr(dwOffsInModule);
					//if(nFPORec>=0)
					//{
					//	FPO_DATA_RECORD* pFPORec = pNewFPOStream->GetFPORecord(nFPORec);
					//	
					//	if((pFPORec->fpo2.dwUnknown2&0x4)!=0) // EBP allocated
					//	{
					//		// Ommit frame using FPO data						
					//		m_StackFrame.m_dwAddrStack = m_StackFrame.m_dwAddrFrame+pFPORec->fpo2.cbParams + 4/*ret*/;

					//		m_StackFrame.m_dwAddrStack += 4; /*EBP*/

					//		if((pFPORec->fpo2.dwUnknown2&0x1)!=0) // SEH handler allocated
					//		{	
					//			m_StackFrame.m_dwAddrStack += 12;
					//		}							
					//	}
					//	else // No EBP
					//	{
					//		// Ommit frame using FPO data						
					//		m_StackFrame.m_dwAddrFrame = m_StackFrame.m_dwAddrStack+
					//			pFPORec->fpo2.cbSavedRegs + pFPORec->fpo2.cdwLocals;

					//		m_StackFrame.m_dwAddrStack = m_StackFrame.m_dwAddrStack+
					//			pFPORec->fpo2.cbSavedRegs + pFPORec->fpo2.cdwLocals+
					//			pFPORec->fpo2.cbParams+4/*ret*/;
					//	}                        												
     //               }
     //           }				
            }
            else // AMD64
            {
                if(pPeReader==NULL)
                    return FALSE;

                // Determine instruction address relative to module base
                DWORD64 dwInstructionAddr = m_StackFrame.m_dwAddrPC-pModuleInfo->m_dwBaseAddr;

                // Get unwind info for the address
                DWORD dwUnwindInfoRVA = 0;
                DWORD dwOffsInFunc = 0;
                if(!GetAMD64UnwindInfo(pPeReader, dwInstructionAddr, dwUnwindInfoRVA, dwOffsInFunc))
                {
                    // If no function table entry is found, then it is in a leaf function,
                    // and RSP will directly address the return pointer. The return pointer
                    // at [RSP] is stored in the updated context, the simulated RSP is incremented
                    // by 8, and step 1 is repeated.

                    // Read return address
                    DWORD64 dwRetAddr64 = 0;
                    DWORD dwBytesRead = 0;
                    BOOL bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrStack, &dwRetAddr64, sizeof(DWORD64), &dwBytesRead);
                    if(!bRead || dwBytesRead!=sizeof(DWORD64))
                        return FALSE;

                    // Get pointer to AMD64 thread context
                    PCONTEXT_x64 pThreadContextX64 = (PCONTEXT_x64)m_pThreadContext;

                    pThreadContextX64->Rip = dwRetAddr64;
                }
                else
                {
                    // If a function table entry is found, RIP can lie within three regions
                    // a) in an epilog, b) in the prolog, or c) in code that may be covered
                    // by an exception handler.

                    // Read 128-byte fragment of code
                    BYTE uchProgram[128];
                    BOOL bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrPC, &uchProgram, 128, &dwBytesRead);
                    if(!bRead || dwBytesRead!=128)
                        return FALSE;

                    // Check if instruction pointer is within prolog or epilog.
                    CStackFrame StackFrame = m_StackFrame;
                    if(CheckAMD64Epilog(uchProgram, dwBytesRead, &StackFrame))
                    {
                        // The effects of the epilog must be continued to compute the
                        // context of the caller function.
                        m_StackFrame = StackFrame;
                    }
                    else
                    {
                        // The effects of the prolog must be undone to compute the
                        // context of the caller function.

                        if(!UndoAMD64Prolog(pPeReader, dwUnwindInfoRVA, dwOffsInFunc))
                            return FALSE;
                    }
                }
            }

			// Release PDB cache entry
			m_pPdbCache->ReleaseCacheEntry(nEntry);
        }
    }

    if(!bAMD64) // x86
    {
		if(!bFirstFrame)
        {
            // Read prev base pointer
            BOOL bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrFrame, &dwValue, sizeof(DWORD), &dwBytesRead);
            if(!bRead || dwBytesRead!=sizeof(DWORD))
                return FALSE;

			m_StackFrame.m_dwAddrFrame = dwValue;			
        }

        // Read return address
        BOOL bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrFrame+4, &dwValue, sizeof(DWORD), &dwBytesRead);
        if(!bRead || dwBytesRead!=sizeof(DWORD))
            return FALSE;
    }

    // Set stack frame fields

    m_StackFrame.m_dwAddrReturn = dwValue;
    m_StackFrame.m_pFuncTableEntry = FALSE;
    m_StackFrame.m_bFar = FALSE;
    m_StackFrame.m_bVirtual = FALSE;

    // Get symbol information for stack frame
    GetSymbolInfoForCurStackFrame();

    return TRUE;
}

BOOL CStackWalker::GetAMD64UnwindInfo(CPeReader* pPeReader, DWORD64 dwAddr, DWORD& dwUnwindInfoRVA, DWORD& dwOffsInFunc)
{
    // Init output variable
    dwUnwindInfoRVA = 0;
    dwOffsInFunc = 0;

    // Find ".pdata" image section
    int nSection = pPeReader->GetSectionIndexByName(".pdata");
    if(nSection>=0)
    {
        // Get section header
        IMAGE_SECTION_HEADER* pSection = pPeReader->GetSectionHeader(nSection);
        if(pSection!=NULL && pSection->SizeOfRawData%sizeof(RUNTIME_FUNCTION)==0)
        {
            // Walk through runtime function table
            int nRecCount = pSection->SizeOfRawData/sizeof(RUNTIME_FUNCTION);
            int nRec;
            for(nRec=0; nRec<nRecCount; nRec++)
            {
                RUNTIME_FUNCTION RFunc;

                // Get runtime function record
                DWORD dwRFuncRVA = pSection->PointerToRawData+nRec*sizeof(RUNTIME_FUNCTION);
                DWORD dwBytesRead = 0;
                BOOL bRead = pPeReader->ReadImageMemory(dwRFuncRVA, &RFunc, sizeof(RUNTIME_FUNCTION), &dwBytesRead);
                if(!bRead || dwBytesRead!=sizeof(RUNTIME_FUNCTION))
                    break;

                // Check if address is inside
                if(dwAddr>=RFunc.BeginAddress && dwAddr<=RFunc.EndAddress)
                {
                    // Unwind info found
                    pPeReader->GetRvaByVA(RFunc.UnwindData, dwUnwindInfoRVA);
                    dwOffsInFunc = (DWORD)dwAddr - RFunc.BeginAddress;
                    return TRUE;
                }
            }
        }
    }

    return FALSE;
}

BOOL CStackWalker::UndoAMD64Prolog(CPeReader* pPeReader, DWORD dwUnwindInfoRVA, DWORD dwOffsInFunc)
{
    UNWIND_INFO UnwindInfo;

    // Read unwind info record
    DWORD dwBytesRead = 0;
    BOOL bRead = pPeReader->ReadImageMemory(dwUnwindInfoRVA, &UnwindInfo, sizeof(UNWIND_INFO), &dwBytesRead);
    if(!bRead || dwBytesRead!=sizeof(UNWIND_INFO))
        return FALSE;

    // Check unwind info version
    if(UnwindInfo.Version!=1)
        return FALSE; // Invalid version

    if((UnwindInfo.Flags&UNW_FLAG_CHAININFO)!=0)
    {
        // Chained unwind info

        DWORD dwRFuncRVA = dwUnwindInfoRVA+4+UnwindInfo.CountOfCodes*sizeof(UNWIND_CODE);

        RUNTIME_FUNCTION RFunc;

        // Get runtime function record
        DWORD dwBytesRead = 0;
        BOOL bRead = pPeReader->ReadImageMemory(dwRFuncRVA, &RFunc, sizeof(RUNTIME_FUNCTION), &dwBytesRead);
        if(!bRead || dwBytesRead!=sizeof(RUNTIME_FUNCTION))
            return FALSE;

        assert(0);
    }

    // Get count of unwind codes
    int nCodeCount = UnwindInfo.CountOfCodes;
    if(nCodeCount<=0)
        return FALSE; // Invalid code count

    // Get pointer to AMD64 thread context
    PCONTEXT_x64 pThreadContextX64 = (PCONTEXT_x64)m_pThreadContext;

    // Determine what register is used as frame pointer
    PDWORD64 pFPReg = NULL;
    if(UnwindInfo.FrameRegister!=0)
    {
        pFPReg = GetGPRegByIndex(UnwindInfo.FrameRegister);
    }

    // Walk through unwind codes in reverse order
    int i;
    for(i=nCodeCount-1; i>=0; i--)
    {
        UNWIND_CODE UnwindCode;

        DWORD dwUnwindCodeRVA = dwUnwindInfoRVA+4+i*sizeof(UNWIND_CODE);

        // Read unwind code
        DWORD dwBytesRead = 0;
        BOOL bRead = pPeReader->ReadImageMemory(dwUnwindCodeRVA, &UnwindCode, sizeof(UNWIND_CODE), &dwBytesRead);
        if(!bRead || dwBytesRead!=sizeof(UNWIND_CODE))
            return FALSE;

        // Check if we should undo the rest of prolog
        if(dwOffsInFunc<UnwindCode.CodeOffset)
            continue; // Skip this

        // Check unwind operation
        switch(UnwindCode.UnwindOp)
        {
        case UWOP_PUSH_NONVOL: // Push a nonvolatile integer register, decrementing RSP by 8.
            {
                // Determine what register to read
                PDWORD64 pReg = GetGPRegByIndex(UnwindCode.OpInfo);

                // Read register from stack top (RSP)
                DWORD64 dwVal = 0;
                bRead = m_pMdmpReader->ReadMemory(pThreadContextX64->Rsp, &dwVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;
                *pReg = dwVal;

                // Increment RSP by 8
                pThreadContextX64->Rsp+=8;
            }
            break;
        case UWOP_ALLOC_LARGE: // Allocate a large-sized area on the stack.
            {
                // There are two forms. If the operation info equals 0, then the size of the allocation
                // divided by 8 is recorded in the next slot, allowing an allocation up to 512K – 8. If
                // the operation info equals 1, then the unscaled size of the allocation is recorded in
                // the next two slots in little-endian format, allowing allocations up to 4GB – 8.

                if(UnwindCode.OpInfo==0)
                {
                    // Read the next unwind code slot
                    i++;
                    WORD wFixedStackAllocSize = 0;
                    bRead = pPeReader->ReadImageMemory(dwUnwindCodeRVA+4, &wFixedStackAllocSize, sizeof(WORD), &dwBytesRead);
                    if(!bRead || dwBytesRead!=sizeof(WORD))
                        return FALSE;

                    pThreadContextX64->Rsp -= 8*wFixedStackAllocSize;
                }
                else
                {
                    // Read the next two slots
                    i+=2;
                    DWORD dwFixedStackAllocSize = 0;
                    bRead = pPeReader->ReadImageMemory(dwUnwindCodeRVA+4, &dwFixedStackAllocSize, sizeof(DWORD), &dwBytesRead);
                    if(!bRead || dwBytesRead!=sizeof(DWORD))
                        return FALSE;

                    pThreadContextX64->Rsp -= dwFixedStackAllocSize;
                }
            }
            break;
        case UWOP_ALLOC_SMALL: // Allocate a small-sized area on the stack.
            {
                // The size of the allocation is the operation info field * 8 + 8,
                // allowing allocations from 8 to 128 bytes.

                pThreadContextX64->Rsp -= UnwindCode.OpInfo*8+8;
            }
            break;
        case UWOP_SET_FPREG: // Establish the frame pointer register
            {
                // The offset is equal to the Frame Register offset (scaled) field
                // in the UNWIND_INFO * 16, allowing offsets from 0 to 240. The use
                // of an offset permits establishing a frame pointer that points to
                // the middle of the fixed stack allocation, helping code density by
                // allowing more accesses to use short instruction forms. Note that
                // the operation info field is reserved and should not be used.

                // Determine what register is the frame register
                PDWORD64 pReg = GetGPRegByIndex(UnwindInfo.FrameRegister);

                pReg -= UnwindInfo.FrameOffset*16;
            }
            break;
        case UWOP_SAVE_NONVOL: // Save a nonvolatile integer register on the stack using a MOV instead of a PUSH
            {
                // This is primarily used for shrink-wrapping, where a nonvolatile
                // register is saved to the stack in a position that was previously allocated.
                // The operation info is the number of the register. The scaled-by-8 stack offset
                // is recorded in the next unwind operation code slot, as described in the note above.

                // Determine what register to read
                PDWORD64 pReg = GetGPRegByIndex(UnwindCode.OpInfo);

                // Read the next slot
                i++;
                WORD wRegOffsInStack = 0;
                bRead = pPeReader->ReadImageMemory(dwUnwindCodeRVA+4, &wRegOffsInStack, sizeof(WORD), &dwBytesRead);
                if(!bRead || dwBytesRead!=sizeof(WORD))
                    return FALSE;

                // Calculate offset of the saved register's value in stack
                DWORD64 dwRegValRVA = 0;
                if(UnwindInfo.FrameRegister==0)
                    dwRegValRVA = pThreadContextX64->Rsp+wRegOffsInStack*8;
                else
                    dwRegValRVA = *pFPReg + wRegOffsInStack*8;

                // Read register
                DWORD64 dwRegVal = 0;
                bRead = m_pMdmpReader->ReadMemory(dwRegValRVA, &dwRegVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;

                *pReg -= dwRegVal;
            }
            break;
        case UWOP_SAVE_NONVOL_FAR: // Save a nonvolatile integer register on the stack with a long offset, using a MOV instead of a PUSH.
            {
                // Save a nonvolatile integer register on the stack with a long offset,
                // using a MOV instead of a PUSH. This is primarily used for shrink-wrapping,
                // where a nonvolatile register is saved to the stack in a position that was
                // previously allocated. The operation info is the number of the register. The
                // unscaled stack offset is recorded in the next two unwind operation code slots,
                // as described in the note above.

                // Determine what register to read
                PDWORD64 pReg = GetGPRegByIndex(UnwindCode.OpInfo);

                // Read the next two slots
                i+=2;
                DWORD dwRegOffsInStack = 0;
                bRead = pPeReader->ReadImageMemory(dwUnwindCodeRVA+4, &dwRegOffsInStack, sizeof(DWORD), &dwBytesRead);
                if(!bRead || dwBytesRead!=sizeof(DWORD))
                    return FALSE;

                // Calculate offset of the saved register's value in stack
                DWORD64 dwRegValRVA = 0;
                if(UnwindInfo.FrameRegister==0)
                    dwRegValRVA = pThreadContextX64->Rsp+dwRegOffsInStack;
                else
                    dwRegValRVA = *pFPReg + dwRegOffsInStack;

                // Read register
                DWORD64 dwRegVal = 0;
                bRead = m_pMdmpReader->ReadMemory(dwRegValRVA, &dwRegVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;

                *pReg -= dwRegVal;
            }
            break;
        case UWOP_SAVE_XMM128: // Save all 128 bits of a nonvolatile XMM register on the stack.
            {
                // The operation info is the number of the register. The scaled-by-16 stack offset
                // is recorded in the next slot.

                // Determine what XMM register to read
                LPBYTE pReg = GetXMMRegByIndex(UnwindCode.OpInfo);

                // Read the next unwind code slot
                i++;
                WORD wOffsInFixedStackAlloc = 0;
                bRead = pPeReader->ReadImageMemory(dwUnwindCodeRVA+4, &wOffsInFixedStackAlloc, sizeof(WORD), &dwBytesRead);
                if(!bRead || dwBytesRead!=sizeof(WORD))
                    return FALSE;

                // Calculate offset of the saved register's value in stack
                DWORD64 dwRegValRVA = 0;
                if(UnwindInfo.FrameRegister==0)
                    dwRegValRVA = pThreadContextX64->Rsp + wOffsInFixedStackAlloc*16;
                else
                    dwRegValRVA = *pFPReg + wOffsInFixedStackAlloc*16;

                // Read register
                BYTE XmmReg[16];
                bRead = m_pMdmpReader->ReadMemory(dwRegValRVA, &XmmReg, 16, &dwBytesRead);
                if(!bRead || dwBytesRead!=16)
                    return FALSE;

                // Save reg value to thread context
                memcpy(pReg, &XmmReg, 16);

            }
            break;
        case UWOP_SAVE_XMM128_FAR: // Save all 128 bits of a nonvolatile XMM register on the stack with a long offset.
            {
                // The operation info is the number of the register. The unscaled stack offset is
                // recorded in the next two slots.

                // Determine what XMM register to read
                LPBYTE pReg = GetXMMRegByIndex(UnwindCode.OpInfo);

                // Read the next two slots
                i+=2;
                DWORD dwOffsInFixedStackAlloc = 0;
                bRead = pPeReader->ReadImageMemory(dwUnwindCodeRVA+4, &dwOffsInFixedStackAlloc, sizeof(DWORD), &dwBytesRead);
                if(!bRead || dwBytesRead!=sizeof(DWORD))
                    return FALSE;

                // Calculate offset of the saved register's value in stack
                DWORD64 dwRegValRVA = 0;
                if(UnwindInfo.FrameRegister==0)
                    dwRegValRVA = pThreadContextX64->Rsp + dwOffsInFixedStackAlloc;
                else
                    dwRegValRVA = *pFPReg + dwOffsInFixedStackAlloc;

                // Read register
                BYTE XmmReg[16];
                bRead = m_pMdmpReader->ReadMemory(pThreadContextX64->Rsp+dwOffsInFixedStackAlloc, &XmmReg, 16, &dwBytesRead);
                if(!bRead || dwBytesRead!=16)
                    return FALSE;

                // Save reg value to thread context
                memcpy(pReg, &XmmReg, 16);
            }
            break;
        case UWOP_PUSH_MACHFRAME: // Push a machine frame.
            {
                // This is used to record the effect of a hardware interrupt or exception.

                DWORD64 dwVal = 0;
                DWORD dwOffsInStack = 0;

                if(UnwindCode.OpInfo==1)
                {
                    // Read Error Code
                    dwOffsInStack += 8;
                }

                // Read RIP
                bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrStack+dwOffsInStack, &dwVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;
                pThreadContextX64->Rip = dwVal;
                dwOffsInStack += 8;

                // Read CS
                bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrStack+dwOffsInStack, &dwVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;
                pThreadContextX64->SegCs = (WORD)dwVal;
                dwOffsInStack += 8;

                // Read EFLAGS
                bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrStack+dwOffsInStack, &dwVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;
                pThreadContextX64->EFlags = (DWORD)dwVal;
                dwOffsInStack += 8;

                // Read old RSP
                bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrStack+dwOffsInStack, &dwVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;
                pThreadContextX64->Rsp = dwVal;
                dwOffsInStack += 8;

                // Read SS
                bRead = m_pMdmpReader->ReadMemory(m_StackFrame.m_dwAddrStack+dwOffsInStack, &dwVal, 8, &dwBytesRead);
                if(!bRead || dwBytesRead!=8)
                    return FALSE;
                pThreadContextX64->SegSs = (WORD)dwVal;
                dwOffsInStack += 8;
            }
            break;
        }

    }

    return TRUE;
}

PDWORD64 CStackWalker::GetGPRegByIndex(UINT uRegIndex)
{
    PCONTEXT_x64 pThreadContextX64 = (PCONTEXT_x64)m_pThreadContext;

    PDWORD64 pReg = NULL;
    switch(uRegIndex)
    {
    case 0: pReg = &pThreadContextX64->Rax;
    case 1: pReg = &pThreadContextX64->Rcx;
    case 2: pReg = &pThreadContextX64->Rdx;
    case 3: pReg = &pThreadContextX64->Rbx;
    case 4: pReg = &pThreadContextX64->Rsp;
    case 5: pReg = &pThreadContextX64->Rbx;
    case 6: pReg = &pThreadContextX64->Rsi;
    case 7: pReg = &pThreadContextX64->Rdi;
    case 8: pReg = &pThreadContextX64->R8;
    case 9: pReg = &pThreadContextX64->R9;
    case 10: pReg = &pThreadContextX64->R10;
    case 11: pReg = &pThreadContextX64->R11;
    case 12: pReg = &pThreadContextX64->R12;
    case 13: pReg = &pThreadContextX64->R13;
    case 14: pReg = &pThreadContextX64->R14;
    case 15: pReg = &pThreadContextX64->R15;
    }

    return pReg;
}

LPBYTE CStackWalker::GetXMMRegByIndex(UINT uRegIndex)
{
    PCONTEXT_x64 pThreadContextX64 = (PCONTEXT_x64)m_pThreadContext;

    LPBYTE pReg = NULL;
    switch(uRegIndex)
    {
    case 0: pReg = (LPBYTE)&pThreadContextX64->Xmm0;
    case 1: pReg = (LPBYTE)&pThreadContextX64->Xmm1;
    case 2: pReg = (LPBYTE)&pThreadContextX64->Xmm2;
    case 3: pReg = (LPBYTE)&pThreadContextX64->Xmm3;
    case 4: pReg = (LPBYTE)&pThreadContextX64->Xmm4;
    case 5: pReg = (LPBYTE)&pThreadContextX64->Xmm5;
    case 6: pReg = (LPBYTE)&pThreadContextX64->Xmm6;
    case 7: pReg = (LPBYTE)&pThreadContextX64->Xmm7;
    case 8: pReg = (LPBYTE)&pThreadContextX64->Xmm8;
    case 9: pReg = (LPBYTE)&pThreadContextX64->Xmm9;
    case 10: pReg = (LPBYTE)&pThreadContextX64->Xmm10;
    case 11: pReg = (LPBYTE)&pThreadContextX64->Xmm11;
    case 12: pReg = (LPBYTE)&pThreadContextX64->Xmm12;
    case 13: pReg = (LPBYTE)&pThreadContextX64->Xmm13;
    case 14: pReg = (LPBYTE)&pThreadContextX64->Xmm14;
    case 15: pReg = (LPBYTE)&pThreadContextX64->Xmm15;
    }

    return pReg;
}

BOOL CStackWalker::CheckAMD64Epilog(LPBYTE uchProgram, UINT cbProgram, CStackFrame* pStackFrame)
{
	pStackFrame; // Unreferensed param

    BOOL bLeaFound = FALSE;
    BOOL bAddFound = FALSE;
    BOOL bPopFound = FALSE;
	BYTE uchRexPrefix = 0;

    UINT i;
    for(i=0; i<cbProgram; i++)
    {
        BYTE uchOpCode = uchProgram[i];
		if(uchOpCode>=0x40 && uchOpCode<=0x4F) // REX prefix
		{
			uchRexPrefix = uchOpCode;
			i++;
		}
        else if(uchOpCode==0x8D) // LEA
        {
            bLeaFound = TRUE;
        }
        else if(uchOpCode==0x81) // ADD
        {
            bAddFound = TRUE;
        }
        else if(uchOpCode==0x58) // POP
        {
            bPopFound = TRUE;
        }
        else if(uchOpCode==0xC3) // RET
        {
            // This is an epilog
            if((bLeaFound||bAddFound) && bPopFound)
                return TRUE;

            // Not an epilog
            return FALSE;
        }
        else
        {
            // Not an epilog
            return FALSE;
        }
    }

    // Not an epilog
    return FALSE;
}

std::wstring CStackWalker::GetErrorMsg()
{
    return m_sErrorMsg;
}

bool CStackWalker::GetSymbolInfoForCurStackFrame()
{
    m_StackFrame.m_sModuleName.clear();
    m_StackFrame.m_sPdbFileName.clear();
    m_StackFrame.m_sSymbolName.clear();
	m_StackFrame.m_sUndecoratedSymbolName.clear();
    m_StackFrame.m_sSrcFileName.clear();
    m_StackFrame.m_nSrcLineNumber = -1;
    m_StackFrame.m_dwSymStartAddr = 0;
	m_StackFrame.m_dwOffsInModule = 0;
	m_StackFrame.m_dwOffsInSymbol = 0;
	m_StackFrame.m_dwOffsInLine = 0;

    int nModuleIndex = m_pMdmpReader->FindModuleIndexByAddr(m_StackFrame.m_dwAddrPC);
    MiniDumpModuleInfo* pModuleInfo = m_pMdmpReader->GetModuleInfo(nModuleIndex);
    if(pModuleInfo)
    {
        m_StackFrame.m_sModuleName = pModuleInfo->m_sShortModuleName;
        DWORD64 dwAddr = m_StackFrame.m_dwAddrPC - pModuleInfo->m_dwBaseAddr;
		m_StackFrame.m_dwOffsInModule = (DWORD)dwAddr;

        CPdbReader* pPdbReader = NULL;
        CPeReader* pPeReader = NULL;
		int nEntry = -1;
		bool bFind = m_pPdbCache->FindPdb(pModuleInfo->GUIDnAge(), pModuleInfo->m_sPdbFileName,
            pModuleInfo->m_sModuleName, &pPdbReader, &pPeReader, &nEntry, NULL, m_bExactMatchBuildAge);
        if(bFind)
        {
            m_StackFrame.m_sPdbFileName = pPdbReader->GetFileName();
            CPdbSymbolStream* pSymbolStream = pPdbReader->GetSymbolStream();
            if(pSymbolStream)
            {
                int nSymIndex = pSymbolStream->GetSymbolIndexByAddr(dwAddr);
                if(nSymIndex>=0)
                {
                    CPdbSymbol* pSymbol = pSymbolStream->GetSymbol(nSymIndex);
					if(pSymbol!=NULL)
					{
						std::string sSymName = pSymbol->GetName();
						m_StackFrame.m_sSymbolName = sSymName;
						m_StackFrame.m_sUndecoratedSymbolName = pSymbol->GetUndecoratedName();
						m_StackFrame.m_dwSymStartAddr = pSymbol->GetRelativeVirtualAddr();
						m_StackFrame.m_dwOffsInSymbol = (DWORD)(dwAddr-m_StackFrame.m_dwSymStartAddr);
					}
                }
            }

            CPdbDebugInfoStream* pDBI = pPdbReader->GetDebugInfoStream();
            int nSectionContrib = pDBI->FindSectionContribByAddr(dwAddr);
            if(nSectionContrib>=0)
            {
                DBI_SECTION_CONTRIB* pSC = pDBI->GetSectionContrib(nSectionContrib);
                int nModuleIndex = pSC->SectionContrib40.wModuleIndex;
                DBI_ModuleInfo* pModuleInfo = pDBI->GetModuleInfo(nModuleIndex);
                if(pModuleInfo)
                {
                    CPdbCompilandStream* pCompiland = pPdbReader->GetCompilandStream(pModuleInfo);
                    if(pCompiland)
                    {
                        int nSymIndex = pCompiland->GetSymbolIndexByAddr(dwAddr);
                        CPdbSymbol* pSym = pCompiland->GetSymbol(nSymIndex);
						std::string sName; 
						if(pSym!=NULL)
							sName = pSym->GetName();                        
						if(!sName.empty())
							m_StackFrame.m_sUndecoratedSymbolName = sName+"()";

						// Get source file, line number and offset in line
                        std::wstring sSrcFileName;
                        int nLine = -1;
						DWORD dwOffsInLine = 0;
												
                        bool bSrcInfo = pCompiland->FindSrcFileAndLineByAddr(dwAddr, sSrcFileName, nLine, dwOffsInLine);
                        if(bSrcInfo)
                        {
                            m_StackFrame.m_sSrcFileName = sSrcFileName;
                            m_StackFrame.m_nSrcLineNumber = nLine;
							m_StackFrame.m_dwOffsInLine = dwOffsInLine;
                        }						
                    }
                }
            }

			m_pPdbCache->ReleaseCacheEntry(nEntry);
        }
    }

    return true;
}
