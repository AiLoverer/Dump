//! \file PdbHeadersStream.h
//! \brief PDB headers stream.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "BasePdbStream.h"

// Headers stream header (stream #1)
//

#pragma pack(1)
typedef struct tagPDB_HEADERS_STREAM
{
    DWORD m_dwUnused;    // Unknown
    DWORD m_dwTimeStamp; // Time stamp (time_t)
    DWORD m_dwAge;       // Build age
    GUID  m_Guid;        // GUID
    DWORD m_cbNames;     // Summary size of name strings following this header
} PDB_HEADERS_STREAM, *PPDB_HEADERS_STREAM;
#pragma pack()

//! \class CPdbHeadersStream
//! \brief Implements the headers stream of a PDB file.
class CPdbHeadersStream : public CBasePdbStream
{
public:

    //! Constructor
    //! @param[in] pPdbReader  PDB reader.
    //! @param[in] pStream     Stream to read from.    
    //! @param[out] pbResult   Initialization result.
    CPdbHeadersStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult);

    //! Destructor
    virtual ~CPdbHeadersStream();

    //! Initializes the stream
    //! @param[in] pPdbReader  PDB reader.
    //! @param[in] pStream     Stream to read from.   
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream);

    //! Returns time stamp
    std::wstring GetTimeStamp();

    //! Returns PDB age
    DWORD GetAge();

    //! Returns GUID
    std::wstring GetGUID();

	//! Returns GUID concatenated with Age
    std::wstring GetGUIDnAge();

private:

    time_t m_TimeStamp;                 //!< Time stamp
    std::wstring m_sTimeStamp;          //!< String time stamp
    DWORD m_dwAge;                      //!< Build age
    GUID m_GUID;                        //!< GUID
    std::wstring m_sGUID;               //!< String GUID
    std::vector<std::string> m_asNames; //!< Name list
    std::vector<DWORD>m_adwNumbers;     //!< DWORD list
};