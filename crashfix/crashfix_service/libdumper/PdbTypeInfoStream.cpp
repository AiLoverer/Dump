//! \file PdbTypeInfoStream.cpp
//! \brief PDB stream containing type info.
//! \author Oleg Krivtsov
//! \date 2011

#include "stdafx.h"
#include "PdbTypeInfoStream.h"
#include "Buffer.h"
#include "Log.h"
#include "strconv.h"

CPdbTypeInfoStream::CPdbTypeInfoStream(CPdbReader* pPdbReader, CMsfStream* pStream, BOOL* pbResult)
{
    *pbResult = Init(pPdbReader, pStream);
}

BOOL CPdbTypeInfoStream::Init(CPdbReader* pPdbReader, CMsfStream* pStream)
{
    // Init base class
    if(!CBasePdbStream::Init(pPdbReader, pStream))
        return FALSE;

    CBuffer buf(1024);
    DWORD dwBytesRead = 0;
    BOOL bRead = pStream->ReadData(buf, sizeof(TPI_STREAM_HEADER), &dwBytesRead, TRUE);
    if(!bRead || dwBytesRead!=sizeof(TPI_STREAM_HEADER))
        return FALSE;

    TPI_STREAM_HEADER* pHeader = (TPI_STREAM_HEADER*)(LPBYTE)buf;

    m_dwMinTypeIndex = pHeader->dwMinTypeIndex;
    m_dwMaxTypeIndex = pHeader->dwMaxTypeIndex;

    int i;
    int nTypeCount = pHeader->dwMaxTypeIndex-pHeader->dwMinTypeIndex;
    for(i=0; i<nTypeCount; i++)
    {
        WORD wBlockSize = 0;

        bRead = pStream->ReadData((LPBYTE)&wBlockSize, sizeof(WORD), &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=sizeof(WORD))
            break;

        if(wBlockSize==0)
            break;

        CBuffer buf(wBlockSize);

        BOOL bRead = pStream->ReadData(buf, wBlockSize, &dwBytesRead, TRUE);
        if(!bRead || dwBytesRead!=wBlockSize)
            break;

        g_log.writeW(0, L"BLOCK length = %d\n", wBlockSize);

        g_log.write_raw(0, buf, wBlockSize);

        PlfRecord plr = (PlfRecord)(LPBYTE)buf;

        WORD wLeaf = plr->leaf;

        CPdbTypeGenericRecord* pRec = NULL;

        switch(wLeaf)
        {
        case LF_ARRAY:
            pRec = new CPdbTypeArray(&plr->Array);
            break;
        case LF_BITFIELD:
            pRec = new CPdbTypeBitField(&plr->Bitfield, wBlockSize);
            break;
        case LF_CLASS:
            pRec = new CPdbTypeClass(&plr->Class, wBlockSize);
            break;
        case LF_STRUCTURE:
            pRec = new CPdbTypeStructure(&plr->Structure, wBlockSize);
            break;
        case LF_UNION:
            pRec = new CPdbTypeUnion(&plr->Union, wBlockSize);
            break;
        case LF_ENUM:
            pRec = new CPdbTypeEnum(&plr->Enum, wBlockSize);
            break;
        case LF_POINTER:
            pRec = new CPdbTypePointer(&plr->Pointer, wBlockSize);
            break;
        case LF_PROCEDURE:
            pRec = new CPdbTypeProcedure(&plr->Proc, wBlockSize);
            break;
        case LF_MFUNCTION:
            pRec = new CPdbTypeMemberFunc(&plr->MFunc, wBlockSize);
            break;
        case LF_ARGLIST:
            pRec = new CPdbTypeArgList(&plr->ArgList, wBlockSize);
            break;
        case LF_VTSHAPE:
            pRec = new CPdbTypeVTShape(&plr->VTShape, wBlockSize);
            break;
        case LF_FIELDLIST:
            pRec = new CPdbTypeFieldList(&plr->FieldList, wBlockSize);
            break;
        default:
            pRec = new CPdbTypeGenericRecord(i);
            break;
        }

        if(pRec!=NULL)
            m_aTypes[m_dwMinTypeIndex+i] = pRec;
    }

    if(nTypeCount==i)
        return TRUE;

    return FALSE;
}

CPdbTypeInfoStream::~CPdbTypeInfoStream()
{
}

bool CPdbTypeInfoStream::DumpTypes(FILE* f)
{
    std::map<DWORD, CPdbTypeGenericRecord*>::iterator it;
    for(it=m_aTypes.begin(); it!=m_aTypes.end(); it++)
    {
        DWORD dwTypeIndex = it->first;

        std::string sTypeName;
        bool bGetTypeName = GetTypeName(dwTypeIndex, sTypeName);
        if(bGetTypeName)
        {
            fprintf(f, "%u %s\n", dwTypeIndex, sTypeName.c_str());
        }
    }

    return true;
}

bool CPdbTypeInfoStream::GetTypeName(DWORD dwTypeIndex, std::string& sTypeName)
{
    if(dwTypeIndex<m_dwMinTypeIndex)
        return GetSimpleTypeName(dwTypeIndex, sTypeName);
    else
        return GetCompoundTypeName(dwTypeIndex, sTypeName);
}

bool CPdbTypeInfoStream::GetSimpleTypeName(DWORD dwTypeIndex, std::string& sTypeName)
{
    switch (dwTypeIndex)
    {
    case T_HRESULT:
        sTypeName = "HRESULT";
        return true;
    case T_32PHRESULT:
        sTypeName = "PHRESULT";
        return true;
    case T_VOID:
        sTypeName = "VOID";
        return true;
    case T_32PVOID:
    case T_PVOID:
        sTypeName = "PVOID";
        return true;
    case T_RCHAR:
    case T_CHAR:
        sTypeName = "CHAR";
        return true;
    case T_32PRCHAR:
    case T_32PCHAR:
    case T_PRCHAR:
    case T_PCHAR:
        sTypeName = "PCHAR";
        return true;
    case T_UCHAR:
        sTypeName = "UCHAR";
        return true;
    case T_32PUCHAR:
    case T_PUCHAR:
        sTypeName = "PUCHAR";
        return true;
    case T_WCHAR:
        sTypeName = "WCHAR";
        return true;
    case T_32PWCHAR:
    case T_PWCHAR:
        sTypeName = "PWCHAR";
        return true;
    case T_SHORT:
        sTypeName = "SHORT";
        return true;
    case T_32PSHORT:
    case T_PSHORT:
        sTypeName = "PSHORT";
        return true;
    case T_USHORT:
        sTypeName = "USHORT";
        return true;
    case T_32PUSHORT:
    case T_PUSHORT:
        sTypeName = "PUSHORT";
        return true;
    case T_LONG:
        sTypeName = "LONG";
        return true;
    case T_32PLONG:
    case T_PLONG:
        sTypeName = "PLONG";
        return true;
    case T_ULONG:
        sTypeName = "ULONG";
        return true;
    case T_32PULONG:
    case T_PULONG:
        sTypeName = "PULONG";
        return true;
    case T_REAL32:
        sTypeName = "FLOAT";
        return true;
    case T_32PREAL32:
    case T_PREAL32:
        sTypeName = "PFLOAT";
        return true;
    case T_REAL64:
        sTypeName = "DOUBLE";
        return true;
    case T_32PREAL64:
    case T_PREAL64:
        sTypeName = "PDOUBLE";
        return true;
    case T_QUAD:
        sTypeName = "LONGLONG";
        return true;
    case T_32PQUAD:
    case T_PQUAD:
        sTypeName = "PLONGLONG";
        return true;
    case T_UQUAD:
        sTypeName = "ULONGLONG";
        return true;
    case T_32PUQUAD:
    case T_PUQUAD:
        sTypeName = "PULONGLONG";
        return true;
    case T_INT4:
        sTypeName = "INT";
        return true;
    case T_32PINT4:
    case T_PINT4:
        sTypeName = "PINT";
        return true;
    case T_UINT4:
        sTypeName = "UINT";
        return true;
    case T_32PUINT4:
    case T_PUINT4:
        sTypeName = "PUINT";
        return true;
    }

    return false;
}

bool CPdbTypeInfoStream::GetCompoundTypeName(DWORD dwTypeIndex, std::string& sTypeName)
{
    sTypeName = "";

    std::map<DWORD, CPdbTypeGenericRecord*>::iterator it = m_aTypes.find(dwTypeIndex);
    if(it==m_aTypes.end())
        return false; // No such type index found

    CPdbTypeGenericRecord* pRec = it->second;

    switch (pRec->GetType())
    {
    case LF_ARRAY:
        {
            CPdbTypeArray* pArray = (CPdbTypeArray*)pRec;
            return GetTypeName(pArray->m_dwElemType, sTypeName);
        }
    case LF_BITFIELD:
        {
            CPdbTypeBitField* pBitField = (CPdbTypeBitField*)pRec;
            std::string sBitFieldName;
            if(!GetTypeName(pBitField->m_dwElemType, sBitFieldName))
                return false;

            char szBuf[256]="";
            sprintf(szBuf, "%s pos=%u len=%u", sBitFieldName.c_str(),
                pBitField->m_uchPos, pBitField->m_uchLen);
            sTypeName = szBuf;
            return true;
        }
    case LF_CLASS:
        {
            CPdbTypeClass* pClass = (CPdbTypeClass*)pRec;
            sTypeName = pClass->m_sName;
            return true;
        }
    case LF_STRUCTURE:
        {
            CPdbTypeStructure* pStruct = (CPdbTypeStructure*)pRec;
            sTypeName = pStruct->m_sName;
            return true;
        }
    case LF_UNION:
        {
            CPdbTypeUnion* pUnion = (CPdbTypeUnion*)pRec;
            sTypeName = pUnion->m_sName;
            return true;
        }
    case LF_ENUM:
        {
            CPdbTypeEnum* pEnum = (CPdbTypeEnum*)pRec;
            sTypeName = pEnum->m_sName;
            return true;
        }
    case LF_POINTER:
        {
            CPdbTypePointer* pPtr = (CPdbTypePointer*)pRec;
            std::string sBodyType;
            GetTypeName(pPtr->m_dwElemType, sBodyType);

            sTypeName = sBodyType;
            return TRUE;
        }
    case LF_PROCEDURE:
        {
            CPdbTypeProcedure* pProc = (CPdbTypeProcedure*)pRec;

            std::string sRetType;
            GetTypeName(pProc->m_dwRetVal, sRetType);

            std::string sArgList;
            GetTypeName(pProc->m_dwArgList, sArgList);

            std::string sCallingConvention;
            GetTypeName(pProc->m_dwCallingConvention, sCallingConvention);

            char szBuf[1024];
#ifdef _WIN32
            sprintf_s(szBuf, 1024, "%s %s * (%s)", sRetType.c_str(), sCallingConvention.c_str(), sArgList.c_str());
#else
            sprintf(szBuf, "%s %s * (%s)", sRetType.c_str(), sCallingConvention.c_str(), sArgList.c_str());
#endif

            sTypeName = szBuf;
            return true;
        }
    case LF_MFUNCTION:
        {
            return true;
        }
    case LF_ARGLIST:
        {
            std::string sArgList;
            CPdbTypeArgList* pArgList = (CPdbTypeArgList*)pRec;

            size_t i;
            for(i=0; i<pArgList->m_aArgs.size(); i++)
            {
                if (i > 0)
                    sArgList.append(", ");

                std::string sArg;
                GetTypeName(pArgList->m_aArgs[i], sArg);
                sArgList.append(sArg);
            }

            return true;
        }
    case LF_VTSHAPE:
        return true;
    case LF_FIELDLIST:
        return true;
    default:
        return true;
    }
}

//------------------------------------------------
// CPdbTypeGenericRecord
//------------------------------------------------

CPdbTypeGenericRecord::CPdbTypeGenericRecord(DWORD dwType)
{
    m_dwType = dwType;
}

DWORD CPdbTypeGenericRecord::GetType()
{
    return m_dwType;
}

LPBYTE CPdbTypeGenericRecord::GetRecordValue(PBYTE pbData, UINT /*cbData*/, PDWORD pdValue)
{
    WORD wValue = 0;
    DWORD dValue = (DWORD)-1;
    PBYTE pbText = NULL;

    if (pbData != NULL)
    {
        if ((wValue = *(PWORD)pbData) < LF_NUMERIC)
        {
            dValue = wValue;
            pbText = pbData + sizeof(WORD);
        }
        else
        {
            switch (wValue)
            {
            case LF_CHAR:
                dValue = (LONG) (*(PCHAR)(pbData + sizeof(WORD)));
                pbText = pbData + sizeof(WORD) + sizeof(CHAR);
                break;

            case LF_SHORT:
                dValue = (LONG) (*(PSHORT)(pbData + sizeof(WORD)));
                pbText = pbData + sizeof(WORD) + sizeof(SHORT);
                break;

            case LF_USHORT:
                dValue = (LONG) (*(PUSHORT)(pbData + sizeof(WORD)));
                pbText = pbData + sizeof(WORD) + sizeof(USHORT);
                break;

            case LF_LONG:
                dValue = (LONG) (*(PLONG)(pbData + sizeof(WORD)));
                pbText = pbData + sizeof(WORD) + sizeof(LONG);
                break;

            case LF_ULONG:
                dValue = (LONG) (*(PULONG)(pbData + sizeof(WORD)));
                pbText = pbData + sizeof(WORD) + sizeof(ULONG);
                break;
            }
        }
    }

    if (pdValue != NULL) *pdValue = dValue;
    return pbText;
}

PBYTE CPdbTypeGenericRecord::GetMethodValue (CV_fldattr_t attr, PDWORD pdData,	PDWORD pdValue)
{
    DWORD dValue = (DWORD)-1;
    PBYTE pbText = NULL;

    if (pdData != NULL)
    {
        switch (attr.mprop)
        {
        case CV_MTintro:
        case CV_MTpureintro:
            {
                dValue = *pdData;
                pbText = (PBYTE) (pdData + 1);
                break;
            }
        default:
            {
                pbText = (PBYTE) pdData;
                break;
            }
        }
    }
    if (pdValue != NULL)
        *pdValue = dValue;
    return pbText;
}

//------------------------------------------------
// CPdbTypeArray
//------------------------------------------------

CPdbTypeArray::CPdbTypeArray(PlfArray pla)
: CPdbTypeGenericRecord(LF_ARRAY)
{
    m_dwElemType = pla->elemtype;
}

//------------------------------------------------
// CPdbTypeFieldList
//------------------------------------------------

CPdbTypeFieldList::CPdbTypeFieldList(PlfFieldList plfl, DWORD dwSize)
: CPdbTypeGenericRecord(LF_FIELDLIST)
{
    DWORD dwOffs = 2; //offsetof(lfFieldList, SubRecord);

    int i;
    for(i=0; ; i++)
    {
        PlfSubRecord plsr = (PlfSubRecord)((LPBYTE)plfl + dwOffs);
        switch(plsr->leaf)
        {
        case LF_ENUMERATE:
            {
                DWORD dwValue = 0;
                LPCSTR szName = (LPCSTR)GetRecordValue (plsr->Enumerate.value,	dwSize, &dwValue);

                m_aValues[(WORD)dwValue] = szName;

                DWORD n = (DWORD)(4 + 2 + strlen(szName) + 1);
                dwOffs += n;
                if((dwOffs-2)%4!=0)
                    dwOffs += 4-(dwOffs-2)%4; // dword padding
            }
            break;
        case LF_MEMBER:
            goto cleanup;
            break;
        case LF_BCLASS:
            goto cleanup;
            break;
        case LF_VFUNCTAB:
            goto cleanup;
            break;
        case LF_ONEMETHOD:
            goto cleanup;
            break;
        case LF_METHOD:
            goto cleanup;
            break;
        case LF_NESTTYPE:
            goto cleanup;
            break;
        default:
            goto cleanup;
            break;
        }
    }

cleanup:

    return;

}

//------------------------------------------------
// CPdbTypeEnum
//------------------------------------------------

CPdbTypeEnum::CPdbTypeEnum(PlfEnum ptr, DWORD /*dwSize*/)
: CPdbTypeGenericRecord(LF_ENUM)
{
    m_sName = (char*)ptr->Name;
    m_dwField = ptr->field;

    return;
}

//------------------------------------------------
// CPdbTypeBitField
//------------------------------------------------

CPdbTypeBitField::CPdbTypeBitField(PlfBitfield ptr, DWORD /*dwSize*/)
: CPdbTypeGenericRecord(LF_BITFIELD)
{
    m_dwElemType = ptr->type;
    m_uchPos = ptr->position;
    m_uchLen = ptr->length;

}

//------------------------------------------------
// CPdbTypeClass
//------------------------------------------------

CPdbTypeClass::CPdbTypeClass(PlfClass ptr, DWORD dwSize)
: CPdbTypeGenericRecord(LF_CLASS)
{
    DWORD dBytes;
    PBYTE pbName = GetRecordValue (ptr->data, dwSize, &dBytes);

    m_sName = std::string((char*)pbName);
}

//------------------------------------------------
// CPdbTypeVTShape
//------------------------------------------------

CPdbTypeVTShape::CPdbTypeVTShape(PlfVTShape ptr, DWORD /*dwSize*/)
: CPdbTypeGenericRecord(LF_VTSHAPE)
{
    DWORD i;
    BYTE  b;

    for (i = 0; i < ptr->count; i++)
    {
        b = ptr->desc [i/2];
        b = (i & 1 ? b & 0xF : b >> 4);
    }
}

//------------------------------------------------
// CPdbTypeArgList
//------------------------------------------------

CPdbTypeArgList::CPdbTypeArgList(PlfArgList ptr, DWORD /*dwSize*/)
: CPdbTypeGenericRecord(LF_ARGLIST)
{
    int nArgCount = ptr->count;
    int i;
    for(i=0; i<nArgCount; i++)
    {
        m_aArgs.push_back(ptr->arg[i]);
    }
}

//------------------------------------------------
// CPdbTypeMemberFunc
//------------------------------------------------

CPdbTypeMemberFunc::CPdbTypeMemberFunc(PlfMFunc ptr, DWORD /*dwSize*/)
: CPdbTypeGenericRecord(LF_MFUNCTION)
{
    m_dwRetVal = ptr->rvtype;
    m_dwCallingConvention = ptr->calltype;
}

//------------------------------------------------
// CPdbTypeProcedure
//------------------------------------------------

CPdbTypeProcedure::CPdbTypeProcedure(PlfProc ptr, DWORD /*dwSize*/)
: CPdbTypeGenericRecord(LF_PROCEDURE)
{
    m_dwRetVal = ptr->rvtype;
    m_dwCallingConvention = ptr->calltype;
    m_dwArgList = ptr->arglist;
}

//------------------------------------------------
// CPdbTypePointer
//------------------------------------------------

CPdbTypePointer::CPdbTypePointer(PlfPointer ptr, DWORD /*dwSize*/)
: CPdbTypeGenericRecord(LF_POINTER)
{
    m_dwElemType = ptr->pbase.btype.index;
}

//------------------------------------------------
// CPdbTypeUnion
//------------------------------------------------

CPdbTypeUnion::CPdbTypeUnion(PlfUnion ptr, DWORD dwSize)
: CPdbTypeGenericRecord(LF_UNION)
{
    DWORD dBytes;
    PBYTE pbName = GetRecordValue (ptr->data, dwSize, &dBytes);

    m_sName = std::string((char*)pbName);
}

//------------------------------------------------
// CPdbTypeStructure
//------------------------------------------------

CPdbTypeStructure::CPdbTypeStructure(PlfClass ptr, DWORD dwSize)
: CPdbTypeGenericRecord(LF_STRUCTURE)
{
    DWORD dBytes;
    PBYTE pbName = GetRecordValue (ptr->data, dwSize, &dBytes);

    m_sName = std::string((char*)pbName);
}
