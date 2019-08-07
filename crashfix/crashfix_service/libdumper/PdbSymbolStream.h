//! \file PdbSymbolStream.h
//! \brief PDB stream containing symbol records.
//! \author Oleg Krivtsov
//! \date 2011

#ifndef _PDBSYMBOLSTREAM_H_
#define _PDBSYMBOLSTREAM_H_

#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStreamStruct.h"
#include "PdbSymbol.h"
#include "PdbSymLexicalScope.h"
#include "BasePdbStream.h"

class CPdbReader;

// Initialization flags
#define PSS_NO_LEXICAL_SCOPE_TREE   0x1  // Do not init lexical scope tree.

//! \class CPdbSymbolStream
//! \brief Implements the symbol stream of a PDB file.
class CPdbSymbolStream : public CBasePdbStream
{
public:

    //! Default constructor.
    CPdbSymbolStream();

	//! Constructor.
	//! @param[in] pPdbReader Owner PDB reader object.
	//! @param[in] pStream Raw MSF stream object.
	//! @param[in] dwOffsetInStream Starting offset within the raw stream.
	//! @param[in] uSize Raw stream size.
	//! @param[out] pbResult Optional. On output, receives initialization status.
	//! @param[in] dwFlags Initialization flags.
    CPdbSymbolStream(
		CPdbReader* pPdbReader, 
		CMsfStream* pStream, 
		DWORD dwOffsetInStream, 
		UINT uSize, 
		BOOL* pbResult, 
		DWORD dwFlags=0);

	//! Destructor.
    virtual ~CPdbSymbolStream();

    //! Initializes the stream.
	//! @param[in] pPdbReader Owner PDB reader object.
	//! @param[in] pStream Raw MSF stream object.
	//! @param[in] dwOffsetInStream Starting offset within the raw stream.
	//! @param[in] uSize Raw stream size.
	//! @param[out] pbResult Optional. On output, receives initialization status.
	//! @param[in] dwFlags Initialization flags.
    BOOL Init(
		CPdbReader* pPdbReader, 
		CMsfStream* pStream, 
		DWORD dwOffsetInStream, 
		UINT uSize, 
		LPDWORD pdwNewOffsInStream=NULL, 
		DWORD dwFlags=0);

	//! Frees the resources used by the stream.
	virtual void Destroy();

    //! Returns count of symbol records.
    int GetSymbolCount();

    //! Returns symbol by its zero-based index; or NULL if there is no such a symbol.
    CPdbSymbol* GetSymbol(int nIndex);

    //! Gets symbol by its offset in stream.
    //! \return Pointer to symbol or NULL if there is no symbol by such offset.
    int GetSymbolIndexByOffsInStream(DWORD dwStartOffs);

    //! Gets symbol that contains the specified virtual address (RVA).
    int GetSymbolIndexByAddr(DWORD64 dwAddr);

    //! Returns lexical scope of the module this PDB file corresponds to.
    //! The module's lexical scope usually has nested scopes.
    CPdbSymLexicalScope* GetModuleLexicalScope();

private:  

    //! Returns unique id for new lexical scope.
    UINT GetUniqueLexicalScopeId();

    //! Adds symbol's address to internal index.
    bool AddSymRva(DWORD64 dwRva, size_t nSymIndex);
	    
	// Internal vars
    std::vector<DWORD> m_aSymOffsets;         //!< List of symbol offsets within stream
    std::map<DWORD64, size_t> m_aSymRvas;     //!< Pairs <sym_address_rva, sym_index>
	std::map<size_t, CPdbSymbol*> m_aSymbols; //!< Pairs <sym_index, CPdbSymbol*>
    CPdbSymLexicalScope* m_pLexicalScope;     //!< Lexical scope tree.
    static UINT m_uLexicalScopeIdSeed;        //!< Used for lexical scope id generation
};

#endif //!_PDBSYMBOLSTREAM_H_


