//! \file BasePdbStream.cpp
//! \brief Base PDB stream.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "BasePdbStream.h"

//-----------------------------------------------
// CBasePdbStream impl
//-----------------------------------------------

CBasePdbStream::CBasePdbStream()
{
    m_pPdbReader = NULL;
    m_pRawStream = NULL;
    m_bInitialized = FALSE;
}

CBasePdbStream::~CBasePdbStream()
{

}

BOOL CBasePdbStream::Init(CPdbReader* pPdbReader, CMsfStream* pRawStream)
{
    if(pPdbReader==NULL ||
       pRawStream==NULL)
       return FALSE;

    m_pPdbReader = pPdbReader;
    m_pRawStream = pRawStream;
    m_bInitialized = TRUE;

    return TRUE;
}

BOOL CBasePdbStream::IsInitialized()
{
    return m_bInitialized;
}