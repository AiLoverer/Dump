//! \file PdbLinkInfoStream.h
//! \brief PDB link info stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStreamStruct.h"
#include "BasePdbStream.h"

#pragma pack(1)
typedef struct tagLINK_INFO_HEADER
{
    DWORD m_dwDataLength;   //!< Length of data, including this header length
    DWORD m_dwAge;          //!< Build age.
    DWORD m_dwUnknown1;     //!< Unknown purpose
    DWORD m_dwUnknown2;     //!< Unknown purpose
    DWORD m_dwUnknown3;     //!< Unknown purpose
    DWORD m_dwUnknown4;     //!< Unknown purpose
    BYTE  m_chStrings[1];   //!< Sequence of zero-separated strings representing linker command line.
}
LINK_INFO_HEADER, *PLINK_INFO_HEADER;
#pragma pack()

//! \class CPdbLinkInfoStream
//! \brief Implements the link info stream of a PDB file.
class CPdbLinkInfoStream : public CBasePdbStream
{
public:

    //! Constructor.
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    CPdbLinkInfoStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult);

    //! Destructor
    virtual ~CPdbLinkInfoStream();

    //! Initializes the stream
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream);

    //! Returns length of data
    DWORD GetDataLength();

    //! Returns age
    DWORD GetAge();

    //! Returns count of strings
    UINT GetStringCount();

    //! Returns string by index
    std::string GetString(UINT i);

private:

    DWORD m_dwDataLength;                 //!< Length of data
    DWORD m_dwAge;                        //!< Age
    std::vector<std::string> m_aStrings;  //!< Array of strings

};

