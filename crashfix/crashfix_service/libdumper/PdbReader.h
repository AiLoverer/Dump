//! \file PdbReader.h
//! \brief PDB file reading functionality.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbHeadersStream.h"
#include "PdbTypeInfoStream.h"
#include "PdbDebugInfoStream.h"
#include "PdbLinkInfoStream.h"
#include "PdbFPOStream.h"
#include "PdbGSIStream.h"
#include "PdbPSGSIStream.h"
#include "PdbSymbolStream.h"
#include "PdbCompilandStream.h"
#include "PdbSectionMapStream.h"

//! \class CPdbReader
//! \brief Opens a PDB file and provides access to its contents.
//! PDB file is a file used to store symbolic information about the compiled application.
class CPdbReader
{	
	friend class CPdbSymbolStream;
	friend class CPdbSymbol;

public:

    //! Constructor.
    CPdbReader();

	//! Destructor.
    virtual ~CPdbReader();

    //! Loads PDB file.
	//! @param[in] sFileName File name to load.
	//! @return TRUE on success; otherwise FALSE.
    BOOL Init(std::wstring sFileName);

    //! Returns TRUE if initialized.
    BOOL IsInitialized();

    //! Frees all used data.
    void Destroy();

    //! Returns file name.
    std::wstring GetFileName();

	//! Calculates MD5 hash for the file.
	std::wstring GetMD5Hash();

	//! Returns file size.
	DWORD GetFileSize();

	//! Returns TRUE if this file is an x64 PDB file; FALSE if x86 file.
	BOOL IsAMD64();

    //! Returns PDB headers stream.
    CPdbHeadersStream* GetHeadersStream();

    //! Returns type info stream
    CPdbTypeInfoStream* GetTypeInfoStream();

    //! Returns debug info stream
    CPdbDebugInfoStream* GetDebugInfoStream();

    //! Returns link info stream
    CPdbLinkInfoStream* GetLinkInfoStream();

    //! Returns FPO stream.
    CPdbFPOStream* GetFPOStream();

    //! Returns new FPO stream.
    CPdbFPOStream* GetNewFPOStream();

    //! Returns GSI stream.
    CPdbGSIStream* GetGSIStream();

    //! Returns PSI stream.
    CPdbPSGSIStream* GetPSIStream();

    //! Returns symbol records stream and initializes it if required.	
    CPdbSymbolStream* GetSymbolStream();

    //! Returns section map stream
    CPdbSectionMapStream* GetSectionMapStream();

    //! Returns a compiland stream.
    CPdbCompilandStream* GetCompilandStream(DBI_ModuleInfo* pModuleInfo);

	//! Releases all unnecessary resources.
	BOOL ReduceMemoryConsumption();
	
private:
		
	CCritSec m_AccessLock;                 //!< Sync object
    BOOL m_bInitialized;                   //!< TRUE if this object is initialized.
    std::wstring m_sFileName;              //!< File name.
    BOOL m_bAMD64;                         //!< Is this PDB file for x64?
    CMsfFile m_MsfFile;                    //!< MSF format file parser.
    CPdbHeadersStream* m_pHeadersStream;   //!< PDB headers stream.
    CPdbTypeInfoStream* m_pTypeInfoStream; //!< PDB type info stream.
    CPdbDebugInfoStream* m_pDebugInfoStream; //!< PDB debug info stream.
    CPdbLinkInfoStream* m_pLinkInfoStream; //!< PDB link info stream.
    CPdbFPOStream* m_pFPOStream;           //!< PDB stream containing frame pointer omission (FPO) records.
    CPdbFPOStream* m_pNewFPOStream;        //!< PDB stream containing frame pointer omission (FPO) records (new format).
    CPdbGSIStream* m_pGSIStream;           //!< PDB stream containing references to global symbols.
    CPdbPSGSIStream* m_pPSIStream;           //!< PDB stream containing references to public symbols.
    CPdbSymbolStream* m_pSymbolStream;     //!< PDB stream containing symbol records.
    CPdbSectionMapStream* m_pSectionMapStream; //!< PDB section map stream.
    std::map<int, CPdbCompilandStream*> m_apCompilandStreams; //!< Referenced PDB compiland streams.
};

