//! \file PdbFPOStream.h
//! \brief PDB stream containing FPO records.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStreamStruct.h"
#include "BasePdbStream.h"

#pragma pack(1)
struct _FPO_DATA_V2
{
    DWORD       ulOffStart;             // offset 1st byte of function code
    DWORD       cbProcSize;             // # bytes in function
    DWORD       cdwLocals;              // # bytes in locals/4
    DWORD       cbParams;               //
    DWORD       dwUnknown1;             // # always 0
    DWORD       dwProgramStringOffset;  //
    WORD        cbProlog;               // size of prolog
    WORD        cbSavedRegs;            // size of saved registers
    DWORD       dwUnknown2;             // # some kind of flags. 0,1,4,5
};
#pragma pack()

union FPO_DATA_RECORD
{
    _FPO_DATA fpo;
    _FPO_DATA_V2 fpo2;
};

class CPdbReader;

//! \class CPdbFPOStream
//! \brief Implements the FPO stream of a PDB file.
class CPdbFPOStream : public CBasePdbStream
{
public:

    //! Default constructor
    CPdbFPOStream();

    //! Constructor
    //! @param[in] pStream Pointer to raw MSF stream.
    //! @param[in] bNewFPO If set, assume the stream contains new FPO records.
    //! @param[out] pbResult Initialization result.
    CPdbFPOStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL bNewFPO, BOOL* pbResult);

    //! Destructor.
    virtual ~CPdbFPOStream();

    //! Initializes the compiland stream.
    //! @param[in] pStream     Stream to read from.
    //! @param[in] pModuleInfo Compiland info taken from DBI stream.
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL bNewFPO);

    //! Returns TRUE if the stream contains new FPO records.
    BOOL IsNewFPO();

    //! Returns count of records.
    int GetRecordCount();

    //! Returns an FPO record by its zero-based index.
    //! @param[in] nIndex Zero-based record index.
    FPO_DATA_RECORD* GetFPORecord(int nIndex);

    //! Returns an FPO record index that owns the address, or -1 if there is no such record.
    int FindFPORecordByAddr(DWORD dwAddr);

private:

    BOOL m_bNewFPO; //!< TRUE if new FPO format.
    std::vector<FPO_DATA_RECORD> m_FPORecords; //!< The list of records.
    std::map<DWORD, int> m_RVAs; //!< FPO records' addresses.
};


