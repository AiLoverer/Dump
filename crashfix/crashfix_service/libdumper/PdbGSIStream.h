//! \file PdbGSIStream.h
//! \brief PDB global symbol info (GSI) stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStreamStruct.h"
#include "BasePdbStream.h"

//! \class CPdbGSIStream
//! \brief Implements the GSI stream of a PDB file.
class CPdbGSIStream : public CBasePdbStream
{
public:

    //! Constructor
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    CPdbGSIStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult);

    //! Destructor
    virtual ~CPdbGSIStream();

    //! Initializes the stream
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream);

private:

    std::vector<GSI_HASH_RECORD> m_aHashRecs; //!< List of hash records
    std::vector<DWORD> m_aHashBuckets;        //!< List of hash buckets

};

