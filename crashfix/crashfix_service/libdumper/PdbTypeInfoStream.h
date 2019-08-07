//! \file PdbTypeInfoStream.h
//! \brief PDB stream containing type info.
//! \author Oleg Krivtsov
//! \date 2011

#pragma once
#include "stdafx.h"
#include "MsfFile.h"
#include "PdbStreamStruct.h"
#include "BasePdbStream.h"

class CPdbTypeGenericRecord;

//! \class CPdbTypeInfoStream
//! \brief Implements the TPI stream of a PDB file.
class CPdbTypeInfoStream : public CBasePdbStream
{
public:

    // Construction/destruction
    CPdbTypeInfoStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult);
    virtual ~CPdbTypeInfoStream();

    //! Initializes the stream
    //! @param[in] pPdbReader  PDB reader this stream belongs to.    
    //! @param[in] pStream     Stream to read from.    
    BOOL Init(CPdbReader* pPdbReader, CMsfStream* pStream);

    //! Returns type declaration
    bool GetTypeName(DWORD dwTypeIndex, std::string& sTypeName);

    //! Dumps stream contents to file
    bool DumpTypes(FILE* f);

private:  

    //! Returns name of a simple type
    bool GetSimpleTypeName(DWORD dwTypeIndex, std::string& sTypeName);

    //! Returns name of a compound type
    bool GetCompoundTypeName(DWORD dwTypeIndex, std::string& sTypeName);

    DWORD m_dwMinTypeIndex;  //!< Min type index.
    DWORD m_dwMaxTypeIndex;  //!< Max type index.
    std::map<DWORD, CPdbTypeGenericRecord*> m_aTypes; //!< The list of types.
};

//! \class CPdbTypeGenericRecord
//! \brief Generic TPI record.
class CPdbTypeGenericRecord
{
public:

    // Construction/destruction
    CPdbTypeGenericRecord(DWORD dwType);

    //! Returns record value
    LPBYTE GetRecordValue(PBYTE pbData, UINT cbData, PDWORD pdValue);

    //! Returns method value
    PBYTE GetMethodValue (CV_fldattr_t attr, PDWORD pdData,	PDWORD pdValue);

    //! Returns type identifier
    DWORD GetType();

private:

    DWORD m_dwType; //!< Record type.
};

class CPdbTypeArray : public CPdbTypeGenericRecord
{
public:

    CPdbTypeArray(PlfArray ptr);

    DWORD m_dwElemType;
};

class CPdbTypeBitField : public CPdbTypeGenericRecord
{
public:

    CPdbTypeBitField(PlfBitfield ptr, DWORD dwSize);

    DWORD m_dwElemType;
    BYTE m_uchPos;
    BYTE m_uchLen;
};

class CPdbTypeClass : public CPdbTypeGenericRecord
{
public:

    CPdbTypeClass(PlfClass ptr, DWORD dwSize);

    std::string m_sName;
};

class CPdbTypeFieldList : public CPdbTypeGenericRecord
{
public:

    CPdbTypeFieldList(PlfFieldList ptr, DWORD dwSize);

    std::map<WORD, std::string> m_aValues;
};

class CPdbTypeEnum : public CPdbTypeGenericRecord
{
public:

    CPdbTypeEnum(PlfEnum ptr, DWORD dwSize);

    std::string m_sName;
    DWORD m_dwField;
};

class CPdbTypeStructure : public CPdbTypeGenericRecord
{
public:

    CPdbTypeStructure(PlfClass ptr, DWORD dwSize);

    std::string m_sName;
};

class CPdbTypeUnion : public CPdbTypeGenericRecord
{
public:

    CPdbTypeUnion(PlfUnion ptr, DWORD dwSize);

    std::string m_sName;
};

class CPdbTypePointer : public CPdbTypeGenericRecord
{
public:

    CPdbTypePointer(PlfPointer ptr, DWORD dwSize);

    DWORD m_dwElemType;
};

class CPdbTypeProcedure : public CPdbTypeGenericRecord
{
public:

    CPdbTypeProcedure(PlfProc ptr, DWORD dwSize);

    DWORD m_dwRetVal;
    DWORD m_dwCallingConvention;
    DWORD m_dwArgList;
};

class CPdbTypeMemberFunc : public CPdbTypeGenericRecord
{
public:

    CPdbTypeMemberFunc(PlfMFunc ptr, DWORD dwSize);

    DWORD m_dwRetVal;
    DWORD m_dwCallingConvention;
};

class CPdbTypeArgList : public CPdbTypeGenericRecord
{
public:

    CPdbTypeArgList(PlfArgList ptr, DWORD dwSize);

    std::string m_sName;
    std::vector<DWORD> m_aArgs;
};

class CPdbTypeVTShape : public CPdbTypeGenericRecord
{
public:

    CPdbTypeVTShape(PlfVTShape ptr, DWORD dwSize);

    std::string m_sName;
};