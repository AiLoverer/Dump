//! \file PdbSectionMapStream.h
//! \brief PDB section map stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "BasePdbStream.h"

//! \class CPdbSectionMapStream
//! \brief Implements the section map stream of a PDB file.
class CPdbSectionMapStream : public CBasePdbStream
{
public:

    // Construction/destruction
    CPdbSectionMapStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult);
    virtual ~CPdbSectionMapStream();

    //! Initializes the stream
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream);

    //! Returns count of sections.
    int GetSectionCount();

    //! Returns n-th section.
    IMAGE_SECTION_HEADER* GetSection(int nIndex);

private:

    std::vector<IMAGE_SECTION_HEADER> m_aSections;
};