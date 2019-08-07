//! \file Buffer.h
//! \brief Self-deallocating memory buffer.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"

class CBuffer
{
public:

    //! Default constructor.
    CBuffer();

    //! Copy constructor.
    //! @param[in] anotherBuffer Source buffer.
    CBuffer(const CBuffer& anotherBuffer);  

    //! Constructor.
    //! @param[in] dwSize Buffer size in bytes.
    CBuffer(DWORD dwSize);

    //! Destructor.
    virtual ~CBuffer();	

    //! Allocates buffer. If buffer already contained data, they will be lost.
    //! @param[in] dwSize Buffer size in bytes.
    bool Allocate(DWORD dwSize);

    //! Reallocates buffer (modifies buffer size without loosing buffer contents).	
    //! @param[in] dwSize New buffer size in bytes.
    bool Reallocate(DWORD dwSize);

    //! Gets buffer size in bytes.
    DWORD GetSize() const;

    //! Deallocates memory used by buffer.
    void Free();	

    //! Copies itself to another buffer (equal or greater size).
    //! @param[in] pAnotherBuffer Pointer to source buffer.
    bool CopyTo(CBuffer* pAnotherBuffer) const;

    //! Returns pointer to buffer data.
    LPBYTE GetPtr();

    //! Operator returning pointer to buffer data.
    operator LPBYTE() const;

    //! Determines if buffer is empty.
    bool IsEmpty();	

private:

    LPBYTE m_pBuffer; //! Pointer to buffer data.
    DWORD m_dwSize;   //! Buffer size.
};

