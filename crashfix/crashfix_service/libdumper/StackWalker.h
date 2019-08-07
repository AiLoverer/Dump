//! \file StackWalker.h
//! \brief Stack trace functionality.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MiniDumpReader.h"
#include "PdbCache.h"

//! \class CStackFrame
//! \brief Stack frame.
class CStackFrame 
{
public:

	//! Constructor.
	CStackFrame()
	{
		m_dwAddrPC = 0;
		m_dwAddrReturn = 0;
		m_dwAddrFrame = 0;
		m_dwAddrStack = 0;
		m_dwAddrBStore = 0;
		m_pFuncTableEntry = NULL;
		m_bFar = FALSE;
		m_bVirtual = FALSE;
		m_dwSymStartAddr = 0;
		m_dwOffsInModule = 0;
		m_dwOffsInSymbol = 0;
		m_nSrcLineNumber = 0;
		m_dwOffsInLine = 0;
	}
	
    DWORD64  m_dwAddrPC;               //!< program counter
    DWORD64  m_dwAddrReturn;           //!< return address
    DWORD64  m_dwAddrFrame;            //!< frame pointer
    DWORD64  m_dwAddrStack;            //!< stack pointer
    DWORD64  m_dwAddrBStore;           //!< backing store pointer
    PVOID    m_pFuncTableEntry;        //!< pointer to pdata/fpo or NULL
    BOOL     m_bFar;                   //!< WOW far call
    BOOL     m_bVirtual;               //!< is this a virtual frame?  

    std::wstring m_sModuleName;      //!< Module name
	DWORD m_dwOffsInModule;          //!< Offset from the beginning of the module.
    std::wstring m_sPdbFileName;     //!< Matching PDB file name
    std::string m_sSymbolName;       //!< Symbol name
	std::string m_sUndecoratedSymbolName; //!< Undecorated symbol name
    DWORD64 m_dwSymStartAddr;        //!< Symbol starting address
	DWORD m_dwOffsInSymbol;          //!< Offset from the beginning of symbol.
    std::wstring m_sSrcFileName;     //!< Source file name
    int m_nSrcLineNumber;            //!< Source line number
	DWORD m_dwOffsInLine;            //!< Offset from the beginning of source line.
};

//! \class CStackWalker
//! \brief Produces a stack trace for a specified minidump and thread.
class CStackWalker
{
public:

    //! Constructor
    CStackWalker();

	//! Destructor
    virtual ~CStackWalker();

    //! Initializes the stack walker for a specified thread.
    //! @param[in] pMiniDump Pointer to minidump reader
    //! @param[in] pPdbCache PDB symbol cache
    //! @param[in] dwThreadId Thread ID
	//! @param[in] bExactMatchBuildAge If to require build age match.
    bool Init(CMiniDumpReader* pMiniDump, CPdbCache* pPdbCache, DWORD dwThreadId, bool bExactMatchBuildAge = TRUE);

	//! Frees all used resources.
	void Destroy();

    //! Returns current stack frame, or NULL if there is not current stack frame
    CStackFrame* GetStackFrame();

    //! Loads first stack frame.
    BOOL FirstStackFrame();    

    //! Loads next stack frame.
    BOOL NextStackFrame(BOOL bFirstFrame = FALSE);

    //! Returns last error message
    std::wstring GetErrorMsg();

private:

	//! Retrieves information for current stack frame
    bool GetSymbolInfoForCurStackFrame();
    
    //! Retrieves x64 unwind info by instruction RVA.
    BOOL GetAMD64UnwindInfo(CPeReader* pPeReader, DWORD64 dwAddr, DWORD& dwUnwindInfoRVA, DWORD& dwOffsInFunc);

    //! Checks if the provided program fragment can be an AMD64 epilog code
    //! and continues the effects of epilog on the specified stack frame
    BOOL CheckAMD64Epilog(LPBYTE uchProgram, UINT cbProgram, CStackFrame* pStackFrame);

    //! Returns pointer to a general purpose register in thread context by register's index.
    PDWORD64 GetGPRegByIndex(UINT uRegIndex);

    //! Returns pointer to an XML register in thread context by register's index.
    LPBYTE GetXMMRegByIndex(UINT uRegIndex);

    //! Undoes effects of prolog.
    BOOL UndoAMD64Prolog(CPeReader* pPeReader, DWORD dwUnwindInfoRVA, DWORD dwOffsInFunc);
        
    CMiniDumpReader* m_pMdmpReader; //!< Pointer to minidump reader
    LPBYTE m_pThreadContext;        //!< Thread context.
	UINT m_uThreadContextSize;      //!< Thread context size.
    CPdbCache* m_pPdbCache;         //!< PDB cache
	bool m_bExactMatchBuildAge;       //!< If to require exact match of PDB build age.
    CStackFrame m_StackFrame;       //!< Current stack frame
    std::wstring m_sErrorMsg;       //!< Last error message
};

