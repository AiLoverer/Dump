//! \file PdbSymLexicalScope.h
//! \brief Symbol lexical scope.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"

class CPdbSymbol;

//! \brief Lexical scope. The region of source code in which a symbol (or identifier) is considered valid; that is, a namespace, file, function, block of code, or function prototype.
//! \details Programs have nested scopes. The
//!   outermost scope is module scope which encompasses all of the symbols not defined within any
//!   inner (lexical) scope. Symbols and types defined at one scoping level are visible to all scopes
//!   nested within it. Symbols and types defined at module scope are visible to all inner scopes.
class CPdbSymLexicalScope
{
public:

    // Construction/destruction.
    CPdbSymLexicalScope(DWORD dwScopeId, CPdbSymLexicalScope* pParentScope);
    virtual ~CPdbSymLexicalScope();

    //! Returns unique Id of this lexical scope.
    DWORD GetLexicalScopeId();

    //! Returns Id of the parent scope. Module scope has zero Id.
    CPdbSymLexicalScope* GetParentLexicalScope();

    //! Returns count of child lexical scopes.
    UINT GetChildScopeCount();

    //! Returns child scope by its zero-based index in the list
    CPdbSymLexicalScope* GetChildScope(int nIndex);

    //! Adds child lexical scope
    void AddChildScope(CPdbSymLexicalScope* pChild);

    //! Returns count of symbols in this scope.
    UINT GetSymbolCount();

    //! Returns symbol in this scope by symbol's index.
    CPdbSymbol* GetSymbol(int nIndex);

    //! Adds symbol to this scope
    void AddSymbol(CPdbSymbol* pSymbol);

private:

    DWORD m_dwScopeId;  //!< Unque ID of this lexical scope.
    CPdbSymLexicalScope* m_pParentScope; //!< Parent lexical scope.  
    std::map<DWORD, CPdbSymLexicalScope*> m_apChildScopes; //!< Child scopes.
    std::vector<CPdbSymbol*> m_apSymbols; //!< Owned symbols.
};

