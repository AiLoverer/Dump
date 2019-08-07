//! \file PdbSymbol.h
//! \brief Debugging symbol.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "PdbStreamStruct.h"
#include "PdbSymbolStream.h"

class CPdbSymLexicalScope;
class CPdbSymbolStream;

//! Function call convention.
enum CALL_CONVENTION
{
	CALL_CONVENTION_UNDEFINED, //!< Unkown call convention
	CALL_CONVENTION_STDCALL,   //!< __stdcall convention
	CALL_CONVENTION_FASTCALL,  //!< __fastcall convention
	CALL_CONVENTION_CDECL,     //!< __cdecl convention
	CALL_CONVENTION_THISCALL   //!< __thiscall convention
};

enum NAME_TYPE
{
	NAME_TYPE_NORMAL,
	NAME_TYPE_CONSTRUCTOR,
	NAME_TYPE_DESTRUCTOR,
    NAME_TYPE_OPERATOR_DELETE,
	NAME_TYPE_OPERATOR_NEW,
	NAME_TYPE_OPERATOR_NEW_ARRAY,
    NAME_TYPE_OPERATOR_ARRAY,
	NAME_TYPE_OPERATOR_ARROW,
	NAME_TYPE_OPERATOR_TYPE_CONV,
	NAME_TYPE_OPERATOR_EQ,
	NAME_TYPE_OPERATOR_EQEQ,
	NAME_TYPE_OPERATOR_PLUS,
	NAME_TYPE_OPERATOR_PLUS_EQUAL,
	NAME_TYPE_OPERATOR_REF,
	NAME_TYPE_OPERATOR_BRACES,
	NAME_TYPE_VFTABLE,
	NAME_TYPE_SCALAR_DELETING_DESTRUCTOR,
	NAME_TYPE_VECTOR_DELETING_DESTRUCTOR,
	NAME_TYPE_RTTI_BASE_CLASS_ARRAY
};

// Class member access type
enum ACCESS_TYPE
{
	ACCESS_TYPE_UNDEFINED, // Undefined
	ACCESS_TYPE_PUBLIC,	   // Public
	ACCESS_TYPE_PROTECTED, // Protected
	ACCESS_TYPE_PRIVATE    // Private
};

//! \class CNameUndecorator
//! \brief Class that can be used to undecorate a symbol name.
class CNameUndecorator
{
public:

	//! Takes a decorated symbol name and produces undecorated symbol name.
	//! @param[in] sDecoratedName Decorated name.
	//! @param[out] sUndecoratedName On output, receives undecorated name.
	//! @param[in] bBrief If set, produces shortened version of undecorated name.
    bool UndecorateName(
        std::string sDecoratedName, 		
        std::string& sUndecoratedName,
		bool bBrief);

private:
    	
	//! Reads symbol name without args
    bool ReadName(std::string& sName, NAME_TYPE& NameType, std::vector<std::string>& sScopeTypeNames, bool bTemplate=false);

	//! Reads a part of the name
	bool ReadNamePart(std::string& sNamePart, NAME_TYPE& NameType, std::vector<std::string>& sScopeTypeNames);

	//! Reads an identifier
	bool ReadIdentifier(std::string& sName);

	//! Parses the list of arguments
    bool ReadArgs(std::vector<std::string>& asArgTypes, std::vector<std::string>& sScopeTypeNames, bool bTemplate=false);

	//! Checks if n characters are available for reading
	bool Avail(int n);

	// Internal variables
	bool m_bBriefMode;                       //!< If set, produces shortened undecorated name
	std::string m_sDecoratedName;            //!< Decorated symbol name
    size_t m_nAtPos;    	                 //!< Current position withing decorated name
    std::string m_sSymName;                  //!< Base undecorated symbol name
    std::vector<std::string> m_asArgTypes;   //!< Argument list     
    NAME_TYPE m_NameType;                    //!< Type of the symbol name
	ACCESS_TYPE m_AccessType;                //!< Class member access type
	BOOL m_bVirtual;                         //!< Virtual function or not
	BOOL m_bFuncPtr;                         //!< Function pointer or not
	CALL_CONVENTION m_CallConv;              //!< Function call convention
    bool m_bImport;                          //!< Import thunk flag
	bool m_bStatic;	                         //!< Static function or variable flag
	bool m_bConst;                           //!< Const function or vairable flag
	bool m_bRightConst;                      //!< Const func return value
	bool m_bPtr64;                           //!< 64-bit function
	bool m_bTrailingByteArg;                 // 
};

//! \class CPdbSymbol
//! \brief Generic symbol record.
//! For the compiler, any name used to identify coding constructs (such as a variable, 
//! function, structure, enumeration, constant, class, method, property, and so on) and 
//! memory locations within an application.
class CPdbSymbol
{
public:

    //! Default constructor
    CPdbSymbol(CPdbSymbolStream* pStream);

	//! Constructor
    CPdbSymbol(CPdbSymbolStream* pStream, _SYM_ENUM_e Type);

	//! Destructor
    virtual ~CPdbSymbol();

    //! Determines if the specified interface is supported or not.
    virtual bool QueryInterface(std::string sInterfaceId, void** ppInterface);

    //! Returns symbol type (S_-prefixed enumeration
    _SYM_ENUM_e GetType();

	//! Sets symbol type
    void SetType(_SYM_ENUM_e Type);

    //! Returns symbol type as string
    std::string GetTypeStr();

    //! Returns symbol name (decorated)
    std::string GetName();

    //! Sets symbol name
    void SetName(std::string sName);

    //! Returns undecorated symbol name
    std::string GetUndecoratedName();

    //! Returns lexical scope of the symbol.
    CPdbSymLexicalScope* GetLexicalScope();

    //! Sets lexical scope for the symbol
    void SetLexicalScope(CPdbSymLexicalScope* pLexicalScope);

    //! Gets segment part of symbol address.
    DWORD GetSegment();

    //! Sets segment part of symbol address.
    void SetSegment(DWORD dwSegment);

    //! Gets offset part of symbol address.
    LONG GetOffset();

    //! Sets offset part of symbol address.
    void SetOffset(LONG lOffset);

    //! Returns symbol length in bytes.
    DWORD GetLength();

    //! Sets symbol length in bytes.
    void SetLength(DWORD dwLength);

    //! Returns symbol's type index.
    DWORD GetTypeIndex();

    //! Sets symbol's type index.
    void SetTypeIndex(DWORD dwTypeIndex);

    //! Returns RVA
    DWORD64 GetRelativeVirtualAddr();

    //! Sets RVA
    void SetRelativeVirtualAddr(DWORD64 dwRVA);

    //! Undecorates symbol name.
    static bool UndecorateSymbolName(std::string sDecoratedName, 
        std::string& sUndecoratedName, bool bBrief);

private:

    CPdbSymbolStream* m_pStream;    //!< Owner symbol stream.
    _SYM_ENUM_e m_Type;             //!< Symbol type
    std::string m_sUndecoratedName; //!< Undecorated symbol name
    std::string m_sName;            //!< Symbol name (decorated)  
    CALL_CONVENTION m_CallConvention; //!< Call convention.
    CPdbSymLexicalScope* m_pLexicalScope; //!< Lexical scope of the symbol.
    DWORD m_dwSegment;              //!< Segment part of symbol address.
    LONG m_lOffset;                 //!< Offset part of symbol address.
    DWORD m_dwLength;               //!< Symbol length (in bytes).
    DWORD64 m_dwRVA;                //!< Address of the symbol relative to image's start.
    DWORD m_dwTypeIndex;            //!< Type index of the symbol.
};



class CPdbObjNameSymbol : public CPdbSymbol
{
public:
    CPdbObjNameSymbol(CPdbSymbolStream* pStream, SYM_VARIANT* pSymRec);

    bool QueryInterface(std::string sInterfaceId, void** ppInterface);

    DWORD GetSignature();

private:

    DWORD m_dwSignature;
};

class CPdbCompileSymbol : public CPdbSymbol
{
public:
    CPdbCompileSymbol(CPdbSymbolStream* pStream, SYM_VARIANT* pSymRec);

    bool QueryInterface(std::string sInterfaceId, void** ppInterface);

    //! Returns compiler name.
    std::string GetCompilerName();

    //! Return count of compiland enviroment params.
    int GetCompilandEnvCount();

    //! Get compiland environment param by index.
    BOOL GetCompilandEnv(int nIndex, std::string& sName, std::string& sValue);

    //! Returns compiland environment param by its name.
    std::string GetCompilandEnv(std::string sParamName);

private:

    struct _CompilandEnv
    {
        std::string m_sName;  // Param name
        std::string m_sValue; // Param value
    };

    std::string m_sCompilerName;                  //!< Compiler name.
    std::vector<_CompilandEnv> m_aCompilandEnv;   //!< Compiler parameters.
    std::map<std::string, int> m_aCompilandEnvIndex; //!< Pairs <SYM_COMPILAND_ENV_name, index>.
};