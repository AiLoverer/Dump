//! \file PdbPSGSIStream.h
//! \brief PDB globally compacted public symbol info (PSGSI) stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStreamStruct.h"
#include "BasePdbStream.h"

//! \class CPdbPSGSIStream
//! \brief Implements the PSGSI stream of a PDB file.
class CPdbPSGSIStream : public CBasePdbStream
{
public:

    // Constructor.    
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    CPdbPSGSIStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult);

    //! Destructor
    virtual ~CPdbPSGSIStream();

    //! Initializes the stream
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream);

    /* Public symbol access by address. */

    //! Returns count of address entries.
    int GetAddrEntryCount();

    //! Returns offset of a public symbol record in symbol stream.
    BOOL GetAddrEntry(int nIndex, LPDWORD pdwAddrEntry);

private:

    std::vector<GSI_HASH_RECORD> m_aHashRecs; //!< List of hash records
    std::vector<DWORD> m_aHashBuckets;        //!< List of hash buckets.
    std::vector<DWORD> m_aAddrEntries;        //!< List of addr entries.
};
