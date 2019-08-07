//! \file Buffer.cpp
//! \brief Self-deallocating memory buffer.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "Buffer.h"

CBuffer::CBuffer()
{
    m_pBuffer = NULL;
    m_dwSize = 0;
}

CBuffer::CBuffer(const CBuffer& anotherBuffer)
{
    m_pBuffer = NULL;
    m_dwSize = 0;

    DWORD dwSize = anotherBuffer.GetSize();
    if(dwSize==0)
        return;

    Allocate(dwSize);
    anotherBuffer.CopyTo(this);
}

CBuffer::CBuffer(DWORD dwSize)
{
    m_pBuffer = NULL;
    m_dwSize = 0;
    Allocate(dwSize);
}

CBuffer::~CBuffer()
{
    Free();
}

bool CBuffer::Allocate(DWORD dwSize)
{
    if(m_pBuffer!=NULL)
        Free();

    if(dwSize==0)
        return true;

    m_pBuffer = new BYTE[dwSize];
    m_dwSize = dwSize;
    if(m_pBuffer!=NULL)
        return true;


    return false;

}

bool CBuffer::Reallocate(DWORD dwSize)
{
    if(dwSize==0)
    {
        // Free buffer
        Free();
        return true;
    }

    // Allocate new buffer size
    LPBYTE pNewBuf =  new BYTE[dwSize];
    if(pNewBuf==NULL)
        return false; // Couldn't allocate new size

    // Copy old buffer contents
    if(m_pBuffer!=NULL)
    {
        memcpy(pNewBuf, m_pBuffer, min(m_dwSize, dwSize));
        Free();
    }

    // Substitute new data
    m_pBuffer = pNewBuf;
    m_dwSize = dwSize;

    return true;
}

DWORD CBuffer::GetSize() const
{
    //
    return m_dwSize;
}

void CBuffer::Free()
{
    if(m_pBuffer!=NULL)
        delete [] m_pBuffer;
    m_pBuffer = NULL;
    m_dwSize = 0;
}

bool CBuffer::CopyTo(CBuffer* pAnotherBuffer) const
{
    if(pAnotherBuffer==NULL)
        return false;

    DWORD dwSize = pAnotherBuffer->GetSize();
    DWORD dwMySize = GetSize();
    if(dwMySize>dwSize)
        return false;

    memcpy(pAnotherBuffer->GetPtr(), m_pBuffer, dwSize);

    return true;
}

bool CBuffer::IsEmpty()
{
    if(m_pBuffer==NULL)
        return true;
    else return false;
}

LPBYTE CBuffer::GetPtr()
{
    return m_pBuffer;
}

CBuffer::operator LPBYTE() const
{
    //ATLASSERT(m_pBuffer!=NULL);
    return m_pBuffer;
}
