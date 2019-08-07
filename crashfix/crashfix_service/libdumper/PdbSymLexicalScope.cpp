//! \file PdbSymLexicalScope.cpp
//! \brief Symbol lexical scope.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbSymLexicalScope.h"
#include "PdbSymbol.h"

//---------------------------------------------------------------
// CPdbSymLexicalScope implementation
//---------------------------------------------------------------

CPdbSymLexicalScope::CPdbSymLexicalScope(DWORD dwScopeId, CPdbSymLexicalScope* pParentScope)
{
    m_dwScopeId = dwScopeId;
    m_pParentScope = pParentScope;
}

CPdbSymLexicalScope::~CPdbSymLexicalScope()
{
    // Remove child scopes
    std::map<DWORD, CPdbSymLexicalScope*>::iterator it;
    for(it=m_apChildScopes.begin(); it!=m_apChildScopes.end(); it++)
    {
        CPdbSymLexicalScope* pChild = it->second;
        delete pChild;
    }
}

DWORD CPdbSymLexicalScope::GetLexicalScopeId()
{
    return m_dwScopeId;
}

CPdbSymLexicalScope* CPdbSymLexicalScope::GetParentLexicalScope()
{
    return m_pParentScope;
}

UINT CPdbSymLexicalScope::GetChildScopeCount()
{
    // Get children count
    return (UINT)m_apChildScopes.size();
}

CPdbSymLexicalScope* CPdbSymLexicalScope::GetChildScope(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_apChildScopes.size())
        return NULL; // Invalid parameter

    // Get lexical scope by its index  
    return m_apChildScopes[nIndex];
}

void CPdbSymLexicalScope::AddChildScope(CPdbSymLexicalScope* pChild)
{
    m_apChildScopes[pChild->GetLexicalScopeId()] = pChild;
}

UINT CPdbSymLexicalScope::GetSymbolCount()
{
    return (UINT)m_apSymbols.size();
}

CPdbSymbol* CPdbSymLexicalScope::GetSymbol(int nIndex)
{
    if(nIndex<0 || nIndex>=(int)m_apSymbols.size())
        return NULL; // Invalid index

    return m_apSymbols[nIndex];
}

void CPdbSymLexicalScope::AddSymbol(CPdbSymbol* pSymbol)
{
    pSymbol->SetLexicalScope(this);
    m_apSymbols.push_back(pSymbol);
}