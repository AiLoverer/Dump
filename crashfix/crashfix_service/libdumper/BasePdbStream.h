//! \file BasePdbStream.h
//! \brief Base PDB stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"

class CMsfStream;
class CPdbReader;

//! \class CBasePdbStream
//! \brief Base PDB stream.
//!
//! All other PDB streams are derived from base PDB stream.
//! This class provides stream initialization functionality.
class CBasePdbStream
{
	friend class CPdbSymbol;
public:

    //! Constructor.
    CBasePdbStream();

    //! Destructor.
    virtual ~CBasePdbStream();

    //! Initializes internal variables
    //! @param[in] pPdbReader Pointer to PDB reader object this stream belongs to.
    //! @param[in] pRawStream Pointer to raw MSF stream object.
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pRawStream);

    //! Checks if stream is initialized.
    BOOL IsInitialized();

protected:

    BOOL m_bInitialized;      //!< Initialized or not?
    CPdbReader* m_pPdbReader; //!< Owner PDB reader.
    CMsfStream* m_pRawStream; //!< Pointer to raw MSF stream.
};

