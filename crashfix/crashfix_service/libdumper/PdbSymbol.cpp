//! \file PdbSymbol.cpp
//! \brief Debugging symbol.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbSymbol.h"
#include "PdbReader.h"

//---------------------------------------------------------------
// CPdbSymbol implementation
//---------------------------------------------------------------

CPdbSymbol::CPdbSymbol(CPdbSymbolStream* pStream)
{
    m_pStream = pStream;
    m_Type = (_SYM_ENUM_e)0;
    m_CallConvention = CALL_CONVENTION_UNDEFINED;
    m_pLexicalScope = NULL;
    m_dwSegment = 0;
    m_lOffset = 0;
    m_dwLength = 0;
    m_dwTypeIndex = 0;
}


CPdbSymbol::CPdbSymbol(CPdbSymbolStream* pStream, _SYM_ENUM_e Type)
{
    m_pStream = pStream;
    m_Type = Type;
    m_CallConvention = CALL_CONVENTION_UNDEFINED;
    m_pLexicalScope = NULL;
    m_dwSegment = 0;
    m_lOffset = 0;
    m_dwLength = 0;
    m_dwTypeIndex = 0;
}

CPdbSymbol::~CPdbSymbol()
{
}

bool CPdbSymbol::QueryInterface(std::string sInterfaceId, void** ppInterface)
{
    *ppInterface = NULL;

    if(sInterfaceId.compare("PdbSymbol")==0)
    {
        *ppInterface = this;
        return true;
    }

    return false;
}

_SYM_ENUM_e CPdbSymbol::GetType()
{
    return m_Type;
}

void CPdbSymbol::SetType(_SYM_ENUM_e Type)
{
    m_Type = Type;
}

std::string CPdbSymbol::GetTypeStr()
{
    std::string sType = "UnknownType";
    switch(m_Type)
    {
    case S_PUB32:
        sType = "PublicSymbol";
        break;
    case S_CONSTANT:
        sType = "Constant";
        break;
    case S_LDATA32:
        sType = "LocalVar";
        break;
    case S_GDATA32:
        sType = "GlobalVar";
        break;
    case S_LPROC32:
        sType = "LocalRroc";
        break;
    case S_GPROC32:
        sType = "GlobalRroc";
        break;
    case S_LPROCREF:
        sType = "LocalRrocRef";
        break;
    case S_PROCREF:
        sType = "GlobalPprocRef";
    case S_LABEL32:
        sType = "CodeLabel";
        break;
    }

    return sType;
}

std::string CPdbSymbol::GetName()
{
    return m_sName;
}

void CPdbSymbol::SetName(std::string sName)
{
    m_sName = sName;    	
}

std::string CPdbSymbol::GetUndecoratedName()
{
	CAutoLock lock(&m_pStream->m_pPdbReader->m_AccessLock);

	if(m_sUndecoratedName.empty())
	{
		/*bool bUndecorate = */UndecorateSymbolName(m_sName, m_sUndecoratedName, true);
		//assert(bUndecorate);
	}
    return m_sUndecoratedName;
}

CPdbSymLexicalScope* CPdbSymbol::GetLexicalScope()
{
    return m_pLexicalScope;
}

void CPdbSymbol::SetLexicalScope(CPdbSymLexicalScope* pLexicalScope)
{
    m_pLexicalScope = pLexicalScope;
}

DWORD CPdbSymbol::GetSegment()
{
    return m_dwSegment;
}

void CPdbSymbol::SetSegment(DWORD dwSegment)
{
    m_dwSegment = dwSegment;
}

LONG CPdbSymbol::GetOffset()
{
    return m_lOffset;
}

void CPdbSymbol::SetOffset(LONG lOffset)
{
    m_lOffset = lOffset;
}

DWORD CPdbSymbol::GetLength()
{
    return m_dwLength;
}

void CPdbSymbol::SetLength(DWORD dwLength)
{
    m_dwLength = dwLength;
}

DWORD CPdbSymbol::GetTypeIndex()
{
    return m_dwTypeIndex;
}

void CPdbSymbol::SetTypeIndex(DWORD dwTypeIndex)
{
    m_dwTypeIndex = dwTypeIndex;
}

DWORD64 CPdbSymbol::GetRelativeVirtualAddr()
{
    return m_dwRVA;
}

void CPdbSymbol::SetRelativeVirtualAddr(DWORD64 dwRVA)
{
    m_dwRVA = dwRVA;
}

bool CPdbSymbol::UndecorateSymbolName(
                                      std::string sDecoratedName,
                                      std::string& sUndecoratedName,
									  bool bBrief)
{
    CNameUndecorator und;
    return und.UndecorateName(sDecoratedName, sUndecoratedName, bBrief);
}

//------------------------------------------------------
// CPdbObjNameSymbol impl
//------------------------------------------------------

CPdbObjNameSymbol::CPdbObjNameSymbol(CPdbSymbolStream* pStream, SYM_VARIANT* pSym)
: CPdbSymbol(pStream)
{
    SYM_OBJNAME* pObjName = &pSym->ObjFileName;

    SetType((_SYM_ENUM_e)pObjName->Header.wRecordType);
    SetName((char*)pObjName->uchName);

    m_dwSignature = pObjName->dwSignature;
}

bool CPdbObjNameSymbol::QueryInterface(std::string sInterfaceId, void** ppInterface)
{
    if(!CPdbSymbol::QueryInterface(sInterfaceId, ppInterface ))
    {
        if(sInterfaceId.compare("PdbObjNameSymbol")==0)
        {
            *ppInterface = this;
            return true;
        }
    }

    return false;
}

DWORD CPdbObjNameSymbol::GetSignature()
{
    return m_dwSignature;
}

//------------------------------------------------------
// CPdbCompileSymbol impl
//------------------------------------------------------

CPdbCompileSymbol::CPdbCompileSymbol(CPdbSymbolStream* pStream, SYM_VARIANT* pSym)
: CPdbSymbol(pStream)
{
    SYM_COMPILE* pSymRec = &pSym->Compile;

    SetType((_SYM_ENUM_e)pSymRec->Header.wRecordType);

    int nString = 0;
    std::string str;
    std::string sParamName;

    // Read strings.
    int i;
    for(i=0;;i++)
    {
        char c = (char)pSymRec->uchVerStr[i];

        if(c!=0)
            str += c;
        else
        {
            if(str.length()==0)
                break;

            if(nString==0)
            {
                m_sCompilerName = str;
            }
            else
            {
                if(nString%2!=0)
                {
                    sParamName = str;
                }
                else
                {
                    _CompilandEnv ce;
                    ce.m_sName = sParamName;
                    ce.m_sValue = str;
                    m_aCompilandEnv.push_back(ce);

                    m_aCompilandEnvIndex[sParamName] = (int)m_aCompilandEnv.size()-1;
                }
            }

            str.clear();
            nString++;
        }
    }
}

bool CPdbCompileSymbol::QueryInterface(std::string sInterfaceId, void** ppInterface)
{
    if(!CPdbSymbol::QueryInterface(sInterfaceId, ppInterface ))
    {
        if(sInterfaceId.compare("PdbCompileSymbol")==0)
        {
            *ppInterface = this;
            return true;
        }
    }

    return false;
}

int CPdbCompileSymbol::GetCompilandEnvCount()
{
    return (int)m_aCompilandEnv.size();
}

BOOL CPdbCompileSymbol::GetCompilandEnv(int nIndex, std::string& sName, std::string& sValue)
{
    if(nIndex<0 || nIndex>(int)m_aCompilandEnv.size())
        return FALSE;

    sName = m_aCompilandEnv[nIndex].m_sName;
    sValue = m_aCompilandEnv[nIndex].m_sValue;

    return TRUE;
}

std::string CPdbCompileSymbol::GetCompilerName()
{
    return m_sCompilerName;
}

std::string CPdbCompileSymbol::GetCompilandEnv(std::string sParamName)
{
    // Find compiler param with such name
    std::map<std::string, int>::iterator it = m_aCompilandEnvIndex.find(sParamName);
    if(it==m_aCompilandEnvIndex.end())
        return std::string(); // Not found

    // Found
    return m_aCompilandEnv[it->second].m_sValue;
}


//-----------------------------------------------------
// CNameUndecorator impl
//-----------------------------------------------------

bool CNameUndecorator::UndecorateName(
         std::string sDecoratedName,
         std::string& sUndecoratedName, 
		 bool bBrief)
{
    //!  Microsoft symbol files store the names of symbols in their so-called decorated form,
    //!  which means that the symbol name might be prefixed and postfixed by additional
    //!  character sequences that carry information about the type and usage of the symbol.

    //!  EXAMPLE              DESCRIPTION
    //!  symbol               Undecorated symbol (might have been declared in an ASM module)
    //!  _symbol              __cdecl function or global variable
    //!  _symbol@N            __stdcall function with N argument bytes
    //!  @symbol@N            __fastcall function with N argument bytes
    //!  __imp__symbol        import thunk of a __cdecl function or variable
    //!  __imp__symbol@N      import thunk of a __stdcall function with N argument bytes
    //!  __imp_@symbol@N      import thunk of a __fastcall function with N argument bytes
    //!  ?symbol              C++ symbol with embedded argument type information
    //!  ___@@_PchSym_symbol  PCH symbol

	m_bBriefMode = bBrief;
    m_sDecoratedName = sDecoratedName;
    m_nAtPos = 0;
    m_NameType = NAME_TYPE_NORMAL;
	m_AccessType = ACCESS_TYPE_UNDEFINED;
	m_bVirtual = FALSE;
	m_CallConv = CALL_CONVENTION_UNDEFINED;
    m_bImport = false;
	m_bStatic = false;	
	m_bConst = false;
	m_bFuncPtr = FALSE;
	m_bRightConst = false;
	m_bPtr64 = false;
	m_bTrailingByteArg = false;

    if(Avail(6) && m_sDecoratedName.substr(m_nAtPos, 6).compare("__imp_")==0)
    {
        // import thunk
        m_bImport = true;
        m_nAtPos+=6;
        m_bImport = true;
    }
	else if(Avail(8) && m_sDecoratedName.substr(m_nAtPos, 8).compare("__catch$")==0)
    {
        // SEH handler        
        m_nAtPos+=12; 
		sUndecoratedName = sDecoratedName;
		return true;
    }
	else if(Avail(12) && m_sDecoratedName.substr(m_nAtPos, 12).compare("__ehhandler$")==0)
    {
        // SEH handler        
        m_nAtPos+=12; 
		sUndecoratedName = sDecoratedName;
		return true;
    }
	else if(Avail(17) && m_sDecoratedName.substr(m_nAtPos, 17).compare("?__unwindfunclet$")==0)
    {
        // Unwind funclet???
        m_nAtPos+=17;  
		sUndecoratedName = sDecoratedName;
		return true;
    }
	else if(Avail(16) && m_sDecoratedName.substr(m_nAtPos, 16).compare("__unwindfunclet$")==0)
    {
        // Unwind funclet???
        m_nAtPos+=16;  
		sUndecoratedName = sDecoratedName;
		return true;
    }
	else if(Avail(4) && m_sDecoratedName.substr(m_nAtPos, 4).compare("??_C")==0)
    {
        // ???        
		sUndecoratedName = sDecoratedName;
		return true;
    }
	else if(Avail(1) && m_sDecoratedName.substr(m_nAtPos, 1).compare("@")==0)
    {
        // ???        
		sUndecoratedName = sDecoratedName;
		return true;
    }
	
    if(Avail(1) && sDecoratedName.substr(m_nAtPos, 1).compare("?")==0)
    {
        //!  ?symbol C++ symbol with embedded argument type information        
        m_nAtPos ++;     		

		std::vector<std::string> asTypeNames;

        // Extract symbol name
        if(!ReadName(m_sSymName, m_NameType, asTypeNames))
			return false;
        
        // Extact argument info
        if(!ReadArgs(m_asArgTypes, asTypeNames))
			return false;

        // Extract postfix
        for(;;)
        {
            if(m_nAtPos>=m_sDecoratedName.length())
                break;

            if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='A')
            {
                //m_AccessType = ACCESS_TYPE_UNDEFINED;
                m_nAtPos++;
            }
            else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='B')
            {
                m_bConst = true;
				m_nAtPos++;
            }
            else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='Z')
            {
                m_bPtr64 = true;
                m_nAtPos++;
            }
			else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='@')
            {   
				if(m_nAtPos!=m_sDecoratedName.length()-1)
					return false;               
				m_nAtPos++;			    
            }
            else
            {
                return false;
            }
        }
    }       	
    else 
    {    
		if(Avail(1) && sDecoratedName.substr(m_nAtPos, 1).compare("_")==0)
		{		
			//!  _symbol              __cdecl function or global variable
			//!  _symbol@N            __stdcall function with N argument bytes
		
			//m_CallConv = CALL_CONVENTION_CDECL;

			m_nAtPos ++; 
		}

        // Read symbol name
		for(;;)
		{
			if(m_nAtPos>=m_sDecoratedName.length())
				break;

			char c = m_sDecoratedName.at(m_nAtPos);

			if(c=='@')
			{
				m_CallConv = CALL_CONVENTION_STDCALL;
				m_nAtPos++;
				break;
			}
			else if(c>0 && (isalnum(c) || c=='_')) 
			{
				m_sSymName += m_sDecoratedName.at(m_nAtPos);
				m_nAtPos++;
			}
			else
			{
				// Not an alpha-numberic character
				sUndecoratedName = sDecoratedName;
				return true;
			}
		}

		// Read trailing number (hex)
		for(;;)
		{
			if(m_nAtPos>=m_sDecoratedName.length())
				break;

			char c = m_sDecoratedName.at(m_nAtPos);						
			if(!isdigit(c) && !(c>='a' && c<='f'))
			{
				assert(0);
				return false;
			}
			m_nAtPos++;
			m_bTrailingByteArg = true;
		}			
	}    

    // Format undecorated symbol name

	if(!m_bBriefMode)
	{
		if(m_bImport)
			sUndecoratedName += "import: ";

		if(m_AccessType==ACCESS_TYPE_PUBLIC)
			sUndecoratedName += "public: ";
		else if(m_AccessType==ACCESS_TYPE_PROTECTED)
			sUndecoratedName += "protected: ";
		else if(m_AccessType==ACCESS_TYPE_PRIVATE)
			sUndecoratedName += "private: ";

		if(m_bVirtual)
			sUndecoratedName += "virtual ";

		if(m_bStatic)
			sUndecoratedName += "static ";
	}

	BOOL bConstructorOrDestructor = m_NameType==NAME_TYPE_CONSTRUCTOR || m_NameType==NAME_TYPE_DESTRUCTOR;
    BOOL bPrependRetType = !(m_NameType==NAME_TYPE_CONSTRUCTOR || m_NameType==NAME_TYPE_DESTRUCTOR || m_NameType==NAME_TYPE_OPERATOR_TYPE_CONV);

    // Prepend return type
    if(bPrependRetType && m_asArgTypes.size()!=0)
    {
        sUndecoratedName += m_asArgTypes[0];
        sUndecoratedName += " ";
    }
    
	if(m_bFuncPtr)
		sUndecoratedName +=	'(';
    
	if(!m_bBriefMode)
	{
		// Prepend calling convention
		if(m_CallConv == CALL_CONVENTION_CDECL)
			sUndecoratedName += "__cdecl ";        
		else if(m_CallConv == CALL_CONVENTION_STDCALL)
			sUndecoratedName += "__stdcall ";        
		else if(m_CallConv == CALL_CONVENTION_THISCALL)
			sUndecoratedName += "__thiscall ";        		
	}	

	if(m_bConst)
			sUndecoratedName += "const ";        	
	
	if(m_bFuncPtr)
		sUndecoratedName +=	"* ";
    
    // Append symbol name
	sUndecoratedName += m_sSymName;   
	if(m_bFuncPtr)
		sUndecoratedName +=	')';

	if(m_NameType==NAME_TYPE_OPERATOR_TYPE_CONV && m_asArgTypes.size()!=0)
	{
		sUndecoratedName.replace( sUndecoratedName.find("{type}"), 6, m_asArgTypes[0]);
	}

    if((int)m_asArgTypes.size()>(m_NameType==NAME_TYPE_NORMAL?1:0) || m_bTrailingByteArg)
    {
        // Append argument type list
        sUndecoratedName += "(";
        size_t i;
	    for(i=!bConstructorOrDestructor?1:0; i<m_asArgTypes.size(); i++)
	    {
		    sUndecoratedName += m_asArgTypes[i];
		    if(i!=m_asArgTypes.size()-1)
		    sUndecoratedName += ",";
	    }		
        sUndecoratedName += ")";
    }

    // Append postfix
	if(m_bRightConst)
        sUndecoratedName += "const";        

    //if(m_bPtr64)
    //    sUndecoratedName += " __ptr64";        
    
    return true;
}

bool CNameUndecorator::ReadName(std::string& sName, NAME_TYPE& NameType, std::vector<std::string>& asScopeTypeNames, bool bTemplate)
{
    // Read name    
	// Name is a sequence of name parts separated with @ and ended with @@	
	std::string sNamePart;
    int nCount = 0;
    
    for(;;)
	{
		if(m_nAtPos>=m_sDecoratedName.length())
			break;

		if(m_sDecoratedName.at(m_nAtPos)=='@')
		{
			// "@@" encountered				
			m_nAtPos++;
			break;
		}
		
		NAME_TYPE NameType2 = NAME_TYPE_NORMAL;
		if(!ReadNamePart(sNamePart, NameType2, asScopeTypeNames))
			return false;

		if(nCount==0)
			NameType = NameType2;

		if(NameType2==NAME_TYPE_NORMAL || 
		   NameType2==NAME_TYPE_CONSTRUCTOR ||
		   NameType2==NAME_TYPE_DESTRUCTOR)
		{
			// Add this name part to the global list, if such a name is not 
			// in the list yet

			bool bFound = false;
			size_t j;
			for(j=0; j<asScopeTypeNames.size(); j++)
			{
				if(asScopeTypeNames[j]==sNamePart)
				{
					bFound = true;
					break;
				}
			}
			
			if(!bFound)
				asScopeTypeNames.push_back(sNamePart);
		}

		if(bTemplate)
		{
			std::vector<std::string> asTemplateParams;
			if(!ReadArgs(asTemplateParams, asScopeTypeNames, true))
				return false;
			sNamePart += "<";
			size_t k;
			for(k=0; k<asTemplateParams.size(); k++)
			{                    
				sNamePart+=asTemplateParams[k];

				if(k<asTemplateParams.size()-1)
					sNamePart += ",";
			}
			sNamePart += ">";			
		}
		
		if(NameType2==NAME_TYPE_CONSTRUCTOR)
			sNamePart = sNamePart + "::" + sNamePart;		
		else if(NameType2==NAME_TYPE_DESTRUCTOR)
			sNamePart = sNamePart + "::~" + sNamePart;		
		
		if(nCount>0)
			sName = sNamePart + "::" + sName; // Prepend symbol name with "::"
		else 
			sName = sNamePart;

		nCount++;

		if(bTemplate)
			break;
	}

    return true;
}

bool CNameUndecorator::ReadNamePart(std::string& sNamePart, NAME_TYPE& NameType, std::vector<std::string>& asScopeTypeNames)
{
	// Init output variables	
	sNamePart.clear();
	NameType = NAME_TYPE_NORMAL;

	bool bStatus = false;
    
	if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?0")
	{
		NameType = NAME_TYPE_CONSTRUCTOR;
		m_nAtPos+=2;
		
		NAME_TYPE NameType2;
		if(!ReadNamePart(sNamePart, NameType2, asScopeTypeNames))
			goto exit;
    }
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?1")
	{
		NameType = NAME_TYPE_DESTRUCTOR;		
		m_nAtPos+=2;

		NAME_TYPE NameType2;
		if(!ReadNamePart(sNamePart, NameType2, asScopeTypeNames))
			goto exit;        
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?2")
	{
		NameType = NAME_TYPE_OPERATOR_NEW;
		sNamePart = "operator new";
		m_nAtPos+=2;
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?3")
	{
		NameType = NAME_TYPE_OPERATOR_DELETE;
		sNamePart = "operator delete";
		m_nAtPos+=2;
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?4")
	{
		NameType = NAME_TYPE_OPERATOR_EQ;
		sNamePart = "operator=";
		m_nAtPos+=2;
	}
	else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3)=="?_7")
	{
		NameType = NAME_TYPE_VFTABLE;
		sNamePart = "`vftable'";
		m_nAtPos+=3;
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?8")
	{
		NameType = NAME_TYPE_OPERATOR_EQEQ;
		sNamePart = "operator==";
		m_nAtPos+=2;		
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?A")
	{
		NameType = NAME_TYPE_OPERATOR_ARRAY;
		sNamePart = "operator[]";
		m_nAtPos+=2;		
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?B")
	{
		NameType = NAME_TYPE_OPERATOR_TYPE_CONV;
		sNamePart = "operator {type}";
		m_nAtPos+=2;		
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?C")
	{
		NameType = NAME_TYPE_OPERATOR_ARROW;
		sNamePart = "operator->";
		m_nAtPos+=2;		
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?H")
	{
		NameType = NAME_TYPE_OPERATOR_PLUS;
		sNamePart = "operator+";
		m_nAtPos+=2;		
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?Y")
	{
		NameType = NAME_TYPE_OPERATOR_PLUS_EQUAL;
		sNamePart = "operator+=";
		m_nAtPos+=2;		
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?I")
	{
		NameType = NAME_TYPE_OPERATOR_REF;
		sNamePart = "operator&";
		m_nAtPos+=2;		
	}
	else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?R")
	{
		NameType = NAME_TYPE_OPERATOR_BRACES;
		sNamePart = "operator()";
		m_nAtPos+=2;		
	}
	else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3)=="?_U")
	{
		NameType = NAME_TYPE_OPERATOR_NEW_ARRAY;        
		sNamePart = "operator new[]";
		m_nAtPos+=3;		
	}
	else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3)=="?_V")
	{
		NameType = NAME_TYPE_OPERATOR_DELETE;        
		sNamePart = "operator delete[]";
		m_nAtPos+=3;		
	}
	else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3)=="?_G")
	{
		NameType = NAME_TYPE_SCALAR_DELETING_DESTRUCTOR;        
		sNamePart = "`scalar deleting destructor'";
		m_nAtPos+=3;		
	}
	else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3)=="?_E")
	{
		NameType = NAME_TYPE_VECTOR_DELETING_DESTRUCTOR;        
		sNamePart = "`vector deleting destructor'";
		m_nAtPos+=3;		
	}
	else if(Avail(4) && m_sDecoratedName.substr(m_nAtPos, 4)=="?_R2")
	{
		NameType = NAME_TYPE_RTTI_BASE_CLASS_ARRAY;        
		sNamePart = "`RTTI Base Class Array'";
		m_nAtPos+=4;		
	}
	else if(sNamePart.empty() && Avail(1) && m_sDecoratedName.at(m_nAtPos)>='0' && m_sDecoratedName.at(m_nAtPos)<='9')
	{
		int nIndex = m_sDecoratedName[m_nAtPos] - '0';
		if(nIndex>=(int)asScopeTypeNames.size())
			goto exit;
		sNamePart = asScopeTypeNames[nIndex];		
		m_nAtPos++;		
	}
    else if(sNamePart.empty() && Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="?$")
    {		
	    m_nAtPos+=2;        
		NAME_TYPE NameType = NAME_TYPE_NORMAL;
		std::vector<std::string> asChildScopeTypeNames;
		if(!ReadName(sNamePart, NameType, asChildScopeTypeNames, true))
			goto exit;
    }
	else
	{
		if(!ReadIdentifier(sNamePart))
			goto exit;		
	}	

	bStatus = true;

exit:
	
	return bStatus;
}

bool CNameUndecorator::ReadIdentifier(std::string& sId)
{
	// Read identifier
	// An identifier is a sequence of alphanumeric characters or underscores
	// ended by @ charater

	sId.clear();

	for(;;)
	{
		if(m_nAtPos>=m_sDecoratedName.length())
			break;
		
		if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='@')
		{			
			m_nAtPos++;
			break; // End of identifier
		}
		else if(Avail(1))
		{
			char c = m_sDecoratedName.at(m_nAtPos);
			if(!isalnum(c) && c!='_')
				return false;
			
			sId += m_sDecoratedName.at(m_nAtPos);
			m_nAtPos++;
		}
	}

	return true;
}

bool CNameUndecorator::Avail(int n)
{
	if(m_nAtPos+n<=m_sDecoratedName.length())
		return true;
	return false;
}

bool CNameUndecorator::ReadArgs(std::vector<std::string>& asArgTypes, std::vector<std::string>& asScopeTypeNames, bool bTemplate)
{
	asArgTypes.clear();
	std::vector<std::string> asCompoundArgTypes;

    std::string sArgType;
    std::string sArgPrefix;
    std::string sArgPostfix;
    bool bModifier = false;
	bool bCompoundArg = false;

    if(!bTemplate)
    {
        // Extract access type and calling convention    

		if(Avail(1) && m_sDecoratedName.at(m_nAtPos)>='0' && m_sDecoratedName.at(m_nAtPos)<='2')
        {		
			m_AccessType = ACCESS_TYPE_PUBLIC;

			if(m_sDecoratedName.at(m_nAtPos)=='1')
				m_AccessType = ACCESS_TYPE_PROTECTED;

            m_bStatic = true;
            m_nAtPos++;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)>='3' && m_sDecoratedName.at(m_nAtPos)<='9')
        {	
            m_nAtPos++;
        }

		if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("AAE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PRIVATE;
            m_CallConv = CALL_CONVENTION_THISCALL;
            m_nAtPos+=3;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("CA")==0)
        {               
            m_AccessType = ACCESS_TYPE_PRIVATE;
			m_bStatic = TRUE;
            m_CallConv = CALL_CONVENTION_CDECL;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("CG")==0)
        {   
			m_AccessType = ACCESS_TYPE_PRIVATE; 
			m_CallConv = CALL_CONVENTION_STDCALL;
			m_bStatic = true;
            m_nAtPos+=2;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("EAE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PRIVATE;
            m_CallConv = CALL_CONVENTION_THISCALL;
			m_bVirtual = true;
            m_nAtPos+=3;
        }
        else if(Avail(4) && m_sDecoratedName.substr(m_nAtPos, 4).compare("QEAA")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_CDECL;
            m_nAtPos+=4;
        }	
		else if(Avail(4) && m_sDecoratedName.substr(m_nAtPos, 4).compare("IEAA")==0)
        {               
            m_AccessType = ACCESS_TYPE_PROTECTED;
            m_CallConv = CALL_CONVENTION_CDECL;
            m_nAtPos+=4;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("IAE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PROTECTED;
            m_CallConv = CALL_CONVENTION_THISCALL;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("IBE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PROTECTED;
            m_CallConv = CALL_CONVENTION_THISCALL;
			m_bRightConst = true;
            m_nAtPos+=3;
        }
		else if(Avail(4) && m_sDecoratedName.substr(m_nAtPos, 4).compare("IEBA")==0)
        {               
            m_AccessType = ACCESS_TYPE_PROTECTED;
			m_CallConv = CALL_CONVENTION_CDECL;
            m_bRightConst = true;
            m_nAtPos+=4;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("QAA")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_CDECL;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("QAE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_THISCALL;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("QAG")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_STDCALL;			
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("QBA")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_CDECL;
			m_bRightConst = true;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("QBE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_THISCALL;
			m_bRightConst = true;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("MAE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PROTECTED;
            m_CallConv = CALL_CONVENTION_THISCALL;
			m_bVirtual = TRUE;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("MBE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PROTECTED;
            m_CallConv = CALL_CONVENTION_THISCALL;
			m_bVirtual = TRUE;
			m_bRightConst=true;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("UAE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_THISCALL;
			m_bVirtual = TRUE;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("UBE")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_THISCALL;
			m_bVirtual = TRUE;
			m_bRightConst = true;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("UAG")==0)
        {               
            m_AccessType = ACCESS_TYPE_PUBLIC;
            m_CallConv = CALL_CONVENTION_STDCALL;
			m_bVirtual = TRUE;
            m_nAtPos+=3;
        }		
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("P6A")==0)
        {               
            m_AccessType = ACCESS_TYPE_PRIVATE;
			m_bFuncPtr = TRUE;
            m_CallConv = CALL_CONVENTION_CDECL;
            m_nAtPos+=3;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("KA")==0)
        {   
			m_AccessType = ACCESS_TYPE_PROTECTED; 
			m_CallConv = CALL_CONVENTION_CDECL;
			m_bStatic = true;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("KG")==0)
        {   
			m_AccessType = ACCESS_TYPE_PROTECTED; 
			m_CallConv = CALL_CONVENTION_STDCALL;
			m_bStatic = true;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("SA")==0)
        {   
			m_AccessType = ACCESS_TYPE_PUBLIC; 
			m_CallConv = CALL_CONVENTION_CDECL;
			m_bStatic = true;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("SG")==0)
        {   
			m_AccessType = ACCESS_TYPE_PUBLIC; 
			m_CallConv = CALL_CONVENTION_STDCALL;
			m_bStatic = true;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("QE")==0)
        {   
			m_AccessType = ACCESS_TYPE_PUBLIC;            
            m_nAtPos+=2;
        }
        else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("YA")==0)
        {   
			m_AccessType = ACCESS_TYPE_UNDEFINED;
            m_CallConv = CALL_CONVENTION_CDECL;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("YG")==0)
        {   
			m_AccessType = ACCESS_TYPE_UNDEFINED;
            m_CallConv = CALL_CONVENTION_STDCALL;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("QB")==0)
        {   
			m_AccessType = ACCESS_TYPE_PROTECTED;
            m_nAtPos+=2;
        }               

        if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("?A")==0)
        {           
            m_nAtPos+=2;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='@')
        {           
            m_nAtPos+=1;
        }
    }    

    // Extact argument info
    for(;;)
    {
        if(m_nAtPos>=m_sDecoratedName.length())
            break; // End of string encountered

        bModifier = false;

        if(m_sDecoratedName.at(m_nAtPos)=='@')
        {                
            m_nAtPos++;            
            
            if(bTemplate)
                break;
        }				
		else if(m_sDecoratedName.at(m_nAtPos)>='0' && m_sDecoratedName.at(m_nAtPos)<='9')
        {
			int nIndex = m_sDecoratedName.at(m_nAtPos)-'0';
			if(nIndex>=(int)asCompoundArgTypes.size())
			{
				if(nIndex>=(int)asArgTypes.size())
					return false;

				sArgType = asArgTypes[asArgTypes.size()-1];
				m_nAtPos++;
			}
			else
			{
				sArgType = asCompoundArgTypes[nIndex];
				m_nAtPos++;
			}
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("AA")==0)
        {
            sArgPostfix = " &" + sArgPostfix;
            bModifier = true;
            m_nAtPos+=2;
        }
		else if(Avail(4) && m_sDecoratedName.substr(m_nAtPos, 4).compare("ABQA")==0)
        {
            sArgPostfix += " * const &";
            bModifier = true;
            m_nAtPos+=4;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("AB")==0)
        {
            sArgPostfix += " const &";
            bModifier = true;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("CB")==0)
        {
            sArgPostfix += " const";
            bModifier = true;
            m_nAtPos+=2;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("AEB")==0)
        {
            sArgPostfix += " const &";
            bModifier = true;
            m_nAtPos+=3;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='A')
		{
			// 'A' at the end of symbol
			m_nAtPos++;
			if(m_nAtPos!=m_sDecoratedName.length())
				return false;

			break;
		}
        else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='D')
        {
            sArgType = "char";
            m_nAtPos++;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='E')
        {
            sArgType = "unsigned char";
            m_nAtPos++;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='F')
        {
            sArgType = "short";
            m_nAtPos++;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='G')
        {
            sArgType = "unsigned short";
            m_nAtPos++;
        }
        else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='H')
        {
            sArgType = "int";
            m_nAtPos++;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="IB")
        {
            sArgType = "unsigned int const";
            m_nAtPos+=2;
        }
        else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='I')
        {
            sArgType = "unsigned int";
            m_nAtPos++;
        }
        else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='J')
        {
            sArgType += "long";            
            m_nAtPos++;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("_J")==0)
        {
            sArgType += "__int64";            
            m_nAtPos+=2;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='K')
        {
            sArgType += "unsigned long";            
            m_nAtPos++;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="_K")
        {
            sArgType += "unsigned __int64";            
            m_nAtPos+=2;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='M')
        {
            sArgType += "float";            
            m_nAtPos++;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='N')
        {
            sArgType += "double";            
            m_nAtPos++;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2)=="_N")
        {
            sArgType += "bool";            
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("PA")==0)
        {
            sArgPostfix = " *" + sArgPostfix;
            bModifier = true;
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("PB")==0)
        {
            sArgPostfix += " const *";
            bModifier = true;
            m_nAtPos+=2;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("PEA")==0)
        {
            sArgPostfix += " *";
            bModifier = true;
            m_nAtPos+=3;
        }		
        else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("PEB")==0)
        {
            sArgPostfix += " const *";
            bModifier = true;
            m_nAtPos+=3;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("P6A")==0)
        {   
            m_nAtPos+=3;
			bCompoundArg = true;
						
			CALL_CONVENTION CallConv = CALL_CONVENTION_CDECL;
			std::vector<std::string> asArgTypes;
			if(!ReadArgs(asArgTypes, asScopeTypeNames))
				return false;
			
			// Prepend return type
			sArgType = asArgTypes[0];

			sArgType +=	" (";
    
			// Prepend calling convention
			if(CallConv == CALL_CONVENTION_CDECL)
				sArgType += "__cdecl ";        
			else if(CallConv == CALL_CONVENTION_STDCALL)
				sArgType += "__stdcall ";        
			
			sArgType +=	"*)";
						
			// Append argument type list
			sArgType += "(";
			size_t i;
			for(i=1; i<asArgTypes.size(); i++)
			{
				sArgType += asArgTypes[i];
				if(i!=asArgTypes.size()-1)
					sArgType += ",";
			}		
			sArgType += ")";			
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='T')
        {
            m_nAtPos++;           
			std::string sName;
			NAME_TYPE NameType;
			if(!ReadName(sName, NameType, asScopeTypeNames))
				return false;
            sArgType = "union " + sName;            
			bCompoundArg = true;
        }
		else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='U')
        {
            m_nAtPos++;           
			std::string sName;
			NAME_TYPE NameType;
			if(!ReadName(sName, NameType, asScopeTypeNames))
				return false;
            sArgType = "struct " + sName;            
			bCompoundArg = true;
        }
        else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='V')
        {
            m_nAtPos++;           
			std::string sName;
			NAME_TYPE NameType;
			if(!ReadName(sName, NameType, asScopeTypeNames))
				return false;
            sArgType = "class " + sName;            
			bCompoundArg = true;
        }
        else if(Avail(1) && m_sDecoratedName.at(m_nAtPos)=='X')
        {
            sArgType = "void";
            m_nAtPos++;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("ZZ")==0)
        {
            sArgType = "...";
            m_nAtPos+=2;
        }
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("W4")==0)
        {
            m_nAtPos+=2;           
			std::string sName;			
			NAME_TYPE NameType;
			if(!ReadName(sName, NameType, asScopeTypeNames))
				return false;
            sArgType = "enum " + sName; 
			bCompoundArg = true;
        }
        else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("_W")==0)
        {
            sArgType = "wchar_t";
            m_nAtPos+=2;
        }	
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("QA")==0)
        {
            sArgPostfix = " * const" + sArgPostfix;
            bModifier = true;
            m_nAtPos+=2;
        }
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("QEB")==0)
        {
            sArgPostfix += " const * const *";
            bModifier = true;
            m_nAtPos+=3;
        }		
		else if(Avail(3) && m_sDecoratedName.substr(m_nAtPos, 3).compare("$00")==0)
		{
			m_nAtPos+=3;
			sArgType = "1";
		}
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("$0")==0)
		{
			m_nAtPos+=2;

			// An argument is a encoded number
			DWORD dwNumber = 0;			
			for(;;)
			{
				if(!Avail(1))
					break;				
				char c = m_sDecoratedName.at(m_nAtPos);
				if(c=='@')
				{
					// End of number
					m_nAtPos ++;				
					break;
				}
				dwNumber*=16;
				dwNumber += (c-'A');
				m_nAtPos ++;				
			}
			char szBuffer[256]="";
			sprintf(szBuffer, "%d", dwNumber);
			sArgType = szBuffer;
		}
		else if(Avail(2) && m_sDecoratedName.substr(m_nAtPos, 2).compare("$$")==0)
		{
			m_nAtPos+=2;
		}
        else
        {
            // Unknown arg type code
            break;
        }

        if(!bModifier && !sArgType.empty())
        {			
            if(!sArgPrefix.empty())
                sArgType = sArgPrefix +  sArgType;

            if(!sArgPostfix.empty())
                sArgType = sArgType + sArgPostfix;

            asArgTypes.push_back(sArgType);  

			if(asArgTypes.size()>1 && (!sArgPrefix.empty() || !sArgPostfix.empty() || bCompoundArg))
			{
				asCompoundArgTypes.push_back(sArgType);  
				bCompoundArg = false;
			}

            sArgType.clear();
            sArgPrefix.clear();
            sArgPostfix.clear();
        }
    }

    return true;
}