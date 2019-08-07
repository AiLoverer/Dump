//! \file MsfStream.h
//! \brief A stream of a multi-stream (MSF) file.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"

class CMsfFile;

//! \class CMsfStream
//! \brief Implements a stream of a PDB file.
class CMsfStream
{
    friend class CMsfFile;
public:

    // Construction/destruction
    CMsfStream(DWORD dwStreamId, DWORD dwStreamLen, CMsfFile* pFile);
    virtual ~CMsfStream();

    //! Returns stream length in bytes.
    DWORD GetStreamLen();

    //! Gets current stream position relative to stream's start.
    BOOL GetStreamPos();

    //! Sets current stream position relative to stream's start.
    BOOL SetStreamPos(DWORD dwPos);

    //! Sets stream position relative to current position.
    BOOL SetStreamPosRel(LONG lDeltaPos);

    //! Reads data from current position.
    //! @param[in] pBuffer Buffer to read data to, required.
    //! @param[in] cbBuffer Size of buffer in bytes, required.
    //! @param[out] pdwBytesCopied Optional. Count of bytes copied to the buffer.
    //! @param[in] bShiftStreamPos If set, shifts stream position after reading data.
    BOOL ReadData(LPBYTE pBuffer, DWORD cbBuffer, LPDWORD pdwBytesCopied, BOOL bShiftStreamPos);

private:

    //! Adds a page to the stream
    //! @param[in] dwPage Page number.
    void AddPage(DWORD dwPage);

    CMsfFile* m_pFile;           //!< Pointer to owner MSF file object.
    DWORD m_dwStreamId;          //!< Stream ID.
    DWORD m_dwStreamLen;         //!< Stream length in bytes.
    DWORD m_dwStreamPos;         //!< Stream position.
    std::vector<DWORD> m_aPages; //!< The list of pages in stream.
};