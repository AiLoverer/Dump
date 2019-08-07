#pragma once
// pdb_info.h
// 06-25-2006 Sven B. Schreiber
// sbs@orgon.com

// =================================================================
// PDB INTERFACE VERSIONS
// =================================================================

#define PDBIntv41         920924
#define PDBIntv50       19960502
#define PDBIntv50a      19970116
#define PDBIntv60       19970116
#define PDBIntv61       19980914
#define PDBIntv69       19990511
#define PDBIntv70Dep    20000406
#define PDBIntv70       20001102

#define PDBIntv         PDBIntv70

#define PDBIntvAlt      PDBIntv50
#define PDBIntvAlt2     PDBIntv60
#define PDBIntvAlt3     PDBIntv69

// =================================================================
// PDB IMPLEMENTATION VERSIONS
// =================================================================

#define PDBImpvVC2      19941610
#define PDBImpvVC4      19950623
#define PDBImpvVC41     19950814
#define PDBImpvVC50     19960307
#define PDBImpvVC98     19970604
#define PDBImpvVC70Dep  19990604
#define PDBImpvVC70     20000404

#define PDBImpv         PDBImpvVC70

// =================================================================
// DBI IMPLEMENTATION VERSIONS
// =================================================================

#define DBIImpvV41        930803
#define DBIImpvV50      19960307
#define DBIImpvV60      19970606
#define DBIImpvV70      19990903

#define DBIImpv         DBIImpvV70

// =================================================================
// BASIC TYPES
// =================================================================

typedef enum _TYPE_ENUM_e
{
    T_NOTYPE                = 0x00000000,
    T_ABS                   = 0x00000001,
    T_SEGMENT               = 0x00000002,
    T_VOID                  = 0x00000003,

    T_HRESULT               = 0x00000008,
    T_32PHRESULT            = 0x00000408,
    T_64PHRESULT            = 0x00000608,

    T_PVOID                 = 0x00000103,
    T_PFVOID                = 0x00000203,
    T_PHVOID                = 0x00000303,
    T_32PVOID               = 0x00000403,
    T_32PFVOID              = 0x00000503,
    T_64PVOID               = 0x00000603,

    T_CURRENCY              = 0x00000004,
    T_NBASICSTR             = 0x00000005,
    T_FBASICSTR             = 0x00000006,
    T_NOTTRANS              = 0x00000007,
    T_BIT                   = 0x00000060,
    T_PASCHAR               = 0x00000061,

    T_CHAR                  = 0x00000010,
    T_PCHAR                 = 0x00000110,
    T_PFCHAR                = 0x00000210,
    T_PHCHAR                = 0x00000310,
    T_32PCHAR               = 0x00000410,
    T_32PFCHAR              = 0x00000510,
    T_64PCHAR               = 0x00000610,

    T_UCHAR                 = 0x00000020,
    T_PUCHAR                = 0x00000120,
    T_PFUCHAR               = 0x00000220,
    T_PHUCHAR               = 0x00000320,
    T_32PUCHAR              = 0x00000420,
    T_32PFUCHAR             = 0x00000520,
    T_64PUCHAR              = 0x00000620,

    T_RCHAR                 = 0x00000070,
    T_PRCHAR                = 0x00000170,
    T_PFRCHAR               = 0x00000270,
    T_PHRCHAR               = 0x00000370,
    T_32PRCHAR              = 0x00000470,
    T_32PFRCHAR             = 0x00000570,
    T_64PRCHAR              = 0x00000670,

    T_WCHAR                 = 0x00000071,
    T_PWCHAR                = 0x00000171,
    T_PFWCHAR               = 0x00000271,
    T_PHWCHAR               = 0x00000371,
    T_32PWCHAR              = 0x00000471,
    T_32PFWCHAR             = 0x00000571,
    T_64PWCHAR              = 0x00000671,

    T_INT1                  = 0x00000068,
    T_PINT1                 = 0x00000168,
    T_PFINT1                = 0x00000268,
    T_PHINT1                = 0x00000368,
    T_32PINT1               = 0x00000468,
    T_32PFINT1              = 0x00000568,
    T_64PINT1               = 0x00000668,

    T_UINT1                 = 0x00000069,
    T_PUINT1                = 0x00000169,
    T_PFUINT1               = 0x00000269,
    T_PHUINT1               = 0x00000369,
    T_32PUINT1              = 0x00000469,
    T_32PFUINT1             = 0x00000569,
    T_64PUINT1              = 0x00000669,

    T_SHORT                 = 0x00000011,
    T_PSHORT                = 0x00000111,
    T_PFSHORT               = 0x00000211,
    T_PHSHORT               = 0x00000311,
    T_32PSHORT              = 0x00000411,
    T_32PFSHORT             = 0x00000511,
    T_64PSHORT              = 0x00000611,

    T_USHORT                = 0x00000021,
    T_PUSHORT               = 0x00000121,
    T_PFUSHORT              = 0x00000221,
    T_PHUSHORT              = 0x00000321,
    T_32PUSHORT             = 0x00000421,
    T_32PFUSHORT            = 0x00000521,
    T_64PUSHORT             = 0x00000621,

    T_INT2                  = 0x00000072,
    T_PINT2                 = 0x00000172,
    T_PFINT2                = 0x00000272,
    T_PHINT2                = 0x00000372,
    T_32PINT2               = 0x00000472,
    T_32PFINT2              = 0x00000572,
    T_64PINT2               = 0x00000672,

    T_UINT2                 = 0x00000073,
    T_PUINT2                = 0x00000173,
    T_PFUINT2               = 0x00000273,
    T_PHUINT2               = 0x00000373,
    T_32PUINT2              = 0x00000473,
    T_32PFUINT2             = 0x00000573,
    T_64PUINT2              = 0x00000673,

    T_LONG                  = 0x00000012,
    T_PLONG                 = 0x00000112,
    T_PFLONG                = 0x00000212,
    T_PHLONG                = 0x00000312,
    T_32PLONG               = 0x00000412,
    T_32PFLONG              = 0x00000512,
    T_64PLONG               = 0x00000612,

    T_ULONG                 = 0x00000022,
    T_PULONG                = 0x00000122,
    T_PFULONG               = 0x00000222,
    T_PHULONG               = 0x00000322,
    T_32PULONG              = 0x00000422,
    T_32PFULONG             = 0x00000522,
    T_64PULONG              = 0x00000622,

    T_INT4                  = 0x00000074,
    T_PINT4                 = 0x00000174,
    T_PFINT4                = 0x00000274,
    T_PHINT4                = 0x00000374,
    T_32PINT4               = 0x00000474,
    T_32PFINT4              = 0x00000574,
    T_64PINT4               = 0x00000674,

    T_UINT4                 = 0x00000075,
    T_PUINT4                = 0x00000175,
    T_PFUINT4               = 0x00000275,
    T_PHUINT4               = 0x00000375,
    T_32PUINT4              = 0x00000475,
    T_32PFUINT4             = 0x00000575,
    T_64PUINT4              = 0x00000675,

    T_QUAD                  = 0x00000013,
    T_PQUAD                 = 0x00000113,
    T_PFQUAD                = 0x00000213,
    T_PHQUAD                = 0x00000313,
    T_32PQUAD               = 0x00000413,
    T_32PFQUAD              = 0x00000513,
    T_64PQUAD               = 0x00000613,

    T_UQUAD                 = 0x00000023,
    T_PUQUAD                = 0x00000123,
    T_PFUQUAD               = 0x00000223,
    T_PHUQUAD               = 0x00000323,
    T_32PUQUAD              = 0x00000423,
    T_32PFUQUAD             = 0x00000523,
    T_64PUQUAD              = 0x00000623,

    T_INT8                  = 0x00000076,
    T_PINT8                 = 0x00000176,
    T_PFINT8                = 0x00000276,
    T_PHINT8                = 0x00000376,
    T_32PINT8               = 0x00000476,
    T_32PFINT8              = 0x00000576,
    T_64PINT8               = 0x00000676,

    T_UINT8                 = 0x00000077,
    T_PUINT8                = 0x00000177,
    T_PFUINT8               = 0x00000277,
    T_PHUINT8               = 0x00000377,
    T_32PUINT8              = 0x00000477,
    T_32PFUINT8             = 0x00000577,
    T_64PUINT8              = 0x00000677,

    T_OCT                   = 0x00000014,
    T_POCT                  = 0x00000114,
    T_PFOCT                 = 0x00000214,
    T_PHOCT                 = 0x00000314,
    T_32POCT                = 0x00000414,
    T_32PFOCT               = 0x00000514,
    T_64POCT                = 0x00000614,

    T_UOCT                  = 0x00000024,
    T_PUOCT                 = 0x00000124,
    T_PFUOCT                = 0x00000224,
    T_PHUOCT                = 0x00000324,
    T_32PUOCT               = 0x00000424,
    T_32PFUOCT              = 0x00000524,
    T_64PUOCT               = 0x00000624,

    T_INT16                 = 0x00000078,
    T_PINT16                = 0x00000178,
    T_PFINT16               = 0x00000278,
    T_PHINT16               = 0x00000378,
    T_32PINT16              = 0x00000478,
    T_32PFINT16             = 0x00000578,
    T_64PINT16              = 0x00000678,

    T_UINT16                = 0x00000079,
    T_PUINT16               = 0x00000179,
    T_PFUINT16              = 0x00000279,
    T_PHUINT16              = 0x00000379,
    T_32PUINT16             = 0x00000479,
    T_32PFUINT16            = 0x00000579,
    T_64PUINT16             = 0x00000679,

    T_REAL32                = 0x00000040,
    T_PREAL32               = 0x00000140,
    T_PFREAL32              = 0x00000240,
    T_PHREAL32              = 0x00000340,
    T_32PREAL32             = 0x00000440,
    T_32PFREAL32            = 0x00000540,
    T_64PREAL32             = 0x00000640,

    T_REAL48                = 0x00000044,
    T_PREAL48               = 0x00000144,
    T_PFREAL48              = 0x00000244,
    T_PHREAL48              = 0x00000344,
    T_32PREAL48             = 0x00000444,
    T_32PFREAL48            = 0x00000544,
    T_64PREAL48             = 0x00000644,

    T_REAL64                = 0x00000041,
    T_PREAL64               = 0x00000141,
    T_PFREAL64              = 0x00000241,
    T_PHREAL64              = 0x00000341,
    T_32PREAL64             = 0x00000441,
    T_32PFREAL64            = 0x00000541,
    T_64PREAL64             = 0x00000641,

    T_REAL80                = 0x00000042,
    T_PREAL80               = 0x00000142,
    T_PFREAL80              = 0x00000242,
    T_PHREAL80              = 0x00000342,
    T_32PREAL80             = 0x00000442,
    T_32PFREAL80            = 0x00000542,
    T_64PREAL80             = 0x00000642,

    T_REAL128               = 0x00000043,
    T_PREAL128              = 0x00000143,
    T_PFREAL128             = 0x00000243,
    T_PHREAL128             = 0x00000343,
    T_32PREAL128            = 0x00000443,
    T_32PFREAL128           = 0x00000543,
    T_64PREAL128            = 0x00000643,

    T_CPLX32                = 0x00000050,
    T_PCPLX32               = 0x00000150,
    T_PFCPLX32              = 0x00000250,
    T_PHCPLX32              = 0x00000350,
    T_32PCPLX32             = 0x00000450,
    T_32PFCPLX32            = 0x00000550,
    T_64PCPLX32             = 0x00000650,

    T_CPLX64                = 0x00000051,
    T_PCPLX64               = 0x00000151,
    T_PFCPLX64              = 0x00000251,
    T_PHCPLX64              = 0x00000351,
    T_32PCPLX64             = 0x00000451,
    T_32PFCPLX64            = 0x00000551,
    T_64PCPLX64             = 0x00000651,

    T_CPLX80                = 0x00000052,
    T_PCPLX80               = 0x00000152,
    T_PFCPLX80              = 0x00000252,
    T_PHCPLX80              = 0x00000352,
    T_32PCPLX80             = 0x00000452,
    T_32PFCPLX80            = 0x00000552,
    T_64PCPLX80             = 0x00000652,

    T_CPLX128               = 0x00000053,
    T_PCPLX128              = 0x00000153,
    T_PFCPLX128             = 0x00000253,
    T_PHCPLX128             = 0x00000353,
    T_32PCPLX128            = 0x00000453,
    T_32PFCPLX128           = 0x00000553,
    T_64PCPLX128            = 0x00000653,

    T_BOOL08                = 0x00000030,
    T_PBOOL08               = 0x00000130,
    T_PFBOOL08              = 0x00000230,
    T_PHBOOL08              = 0x00000330,
    T_32PBOOL08             = 0x00000430,
    T_32PFBOOL08            = 0x00000530,
    T_64PBOOL08             = 0x00000630,

    T_BOOL16                = 0x00000031,
    T_PBOOL16               = 0x00000131,
    T_PFBOOL16              = 0x00000231,
    T_PHBOOL16              = 0x00000331,
    T_32PBOOL16             = 0x00000431,
    T_32PFBOOL16            = 0x00000531,
    T_64PBOOL16             = 0x00000631,

    T_BOOL32                = 0x00000032,
    T_PBOOL32               = 0x00000132,
    T_PFBOOL32              = 0x00000232,
    T_PHBOOL32              = 0x00000332,
    T_32PBOOL32             = 0x00000432,
    T_32PFBOOL32            = 0x00000532,
    T_64PBOOL32             = 0x00000632,

    T_BOOL64                = 0x00000033,
    T_PBOOL64               = 0x00000133,
    T_PFBOOL64              = 0x00000233,
    T_PHBOOL64              = 0x00000333,
    T_32PBOOL64             = 0x00000433,
    T_32PFBOOL64            = 0x00000533,
    T_64PBOOL64             = 0x00000633,

    T_NCVPTR                = 0x000001F0,
    T_FCVPTR                = 0x000002F0,
    T_HCVPTR                = 0x000003F0,
    T_32NCVPTR              = 0x000004F0,
    T_32FCVPTR              = 0x000005F0,
    T_64NCVPTR              = 0x000006F0,
}
TYPE_ENUM_e, *PTYPE_ENUM_e, **PPTYPE_ENUM_e;

// =================================================================
// TYPE INFO RECORD TAGS
// =================================================================

typedef enum _LEAF_ENUM_e
{
    LF_MODIFIER_16t         = 0x00000001,
    LF_POINTER_16t          = 0x00000002,
    LF_ARRAY_16t            = 0x00000003,
    LF_CLASS_16t            = 0x00000004,
    LF_STRUCTURE_16t        = 0x00000005,
    LF_UNION_16t            = 0x00000006,
    LF_ENUM_16t             = 0x00000007,
    LF_PROCEDURE_16t        = 0x00000008,
    LF_MFUNCTION_16t        = 0x00000009,
    LF_VTSHAPE              = 0x0000000A,
    LF_COBOL0_16t           = 0x0000000B,
    LF_COBOL1               = 0x0000000C,
    LF_BARRAY_16t           = 0x0000000D,
    LF_LABEL                = 0x0000000E,
    LF_NULL                 = 0x0000000F,
    LF_NOTTRAN              = 0x00000010,
    LF_DIMARRAY_16t         = 0x00000011,
    LF_VFTPATH_16t          = 0x00000012,
    LF_PRECOMP_16t          = 0x00000013,
    LF_ENDPRECOMP           = 0x00000014,
    LF_OEM_16t              = 0x00000015,
    LF_TYPESERVER_ST        = 0x00000016,

    LF_SKIP_16t             = 0x00000200,
    LF_ARGLIST_16t          = 0x00000201,
    LF_DEFARG_16t           = 0x00000202,
    LF_LIST                 = 0x00000203,
    LF_FIELDLIST_16t        = 0x00000204,
    LF_DERIVED_16t          = 0x00000205,
    LF_BITFIELD_16t         = 0x00000206,
    LF_METHODLIST_16t       = 0x00000207,
    LF_DIMCONU_16t          = 0x00000208,
    LF_DIMCONLU_16t         = 0x00000209,
    LF_DIMVARU_16t          = 0x0000020A,
    LF_DIMVARLU_16t         = 0x0000020B,
    LF_REFSYM               = 0x0000020C,

    LF_BCLASS_16t           = 0x00000400,
    LF_VBCLASS_16t          = 0x00000401,
    LF_IVBCLASS_16t         = 0x00000402,
    LF_ENUMERATE_ST         = 0x00000403,
    LF_FRIENDFCN_16t        = 0x00000404,
    LF_INDEX_16t            = 0x00000405,
    LF_MEMBER_16t           = 0x00000406,
    LF_STMEMBER_16t         = 0x00000407,
    LF_METHOD_16t           = 0x00000408,
    LF_NESTTYPE_16t         = 0x00000409,
    LF_VFUNCTAB_16t         = 0x0000040A,
    LF_FRIENDCLS_16t        = 0x0000040B,
    LF_ONEMETHOD_16t        = 0x0000040C,
    LF_VFUNCOFF_16t         = 0x0000040D,

    LF_TI16_MAX             = 0x00001000,
    LF_MODIFIER             = 0x00001001,
    LF_POINTER              = 0x00001002,
    LF_ARRAY_ST             = 0x00001003,
    LF_CLASS_ST             = 0x00001004,
    LF_STRUCTURE_ST         = 0x00001005,
    LF_UNION_ST             = 0x00001006,
    LF_ENUM_ST              = 0x00001007,
    LF_PROCEDURE            = 0x00001008,
    LF_MFUNCTION            = 0x00001009,
    LF_COBOL0               = 0x0000100A,
    LF_BARRAY               = 0x0000100B,
    LF_DIMARRAY_ST          = 0x0000100C,
    LF_VFTPATH              = 0x0000100D,
    LF_PRECOMP_ST           = 0x0000100E,
    LF_OEM                  = 0x0000100F,
    LF_ALIAS_ST             = 0x00001010,
    LF_OEM2                 = 0x00001011,

    LF_SKIP                 = 0x00001200,
    LF_ARGLIST              = 0x00001201,
    LF_DEFARG_ST            = 0x00001202,
    LF_FIELDLIST            = 0x00001203,
    LF_DERIVED              = 0x00001204,
    LF_BITFIELD             = 0x00001205,
    LF_METHODLIST           = 0x00001206,
    LF_DIMCONU              = 0x00001207,
    LF_DIMCONLU             = 0x00001208,
    LF_DIMVARU              = 0x00001209,
    LF_DIMVARLU             = 0x0000120A,

    LF_BCLASS               = 0x00001400,
    LF_VBCLASS              = 0x00001401,
    LF_IVBCLASS             = 0x00001402,
    LF_FRIENDFCN_ST         = 0x00001403,
    LF_INDEX                = 0x00001404,
    LF_MEMBER_ST            = 0x00001405,
    LF_STMEMBER_ST          = 0x00001406,
    LF_METHOD_ST            = 0x00001407,
    LF_NESTTYPE_ST          = 0x00001408,
    LF_VFUNCTAB             = 0x00001409,
    LF_FRIENDCLS            = 0x0000140A,
    LF_ONEMETHOD_ST         = 0x0000140B,
    LF_VFUNCOFF             = 0x0000140C,
    LF_NESTTYPEEX_ST        = 0x0000140D,
    LF_MEMBERMODIFY_ST      = 0x0000140E,
    LF_MANAGED_ST           = 0x0000140F,

    LF_ST_MAX               = 0x00001500,
    LF_TYPESERVER           = 0x00001501,
    LF_ENUMERATE            = 0x00001502,
    LF_ARRAY                = 0x00001503,
    LF_CLASS                = 0x00001504,
    LF_STRUCTURE            = 0x00001505,
    LF_UNION                = 0x00001506,
    LF_ENUM                 = 0x00001507,
    LF_DIMARRAY             = 0x00001508,
    LF_PRECOMP              = 0x00001509,
    LF_ALIAS                = 0x0000150A,
    LF_DEFARG               = 0x0000150B,
    LF_FRIENDFCN            = 0x0000150C,
    LF_MEMBER               = 0x0000150D,
    LF_STMEMBER             = 0x0000150E,
    LF_METHOD               = 0x0000150F,
    LF_NESTTYPE             = 0x00001510,
    LF_ONEMETHOD            = 0x00001511,
    LF_NESTTYPEEX           = 0x00001512,
    LF_MEMBERMODIFY         = 0x00001513,
    LF_MANAGED              = 0x00001514,
    LF_TYPESERVER2          = 0x00001515,

    LF_NUMERIC              = 0x8000,
    LF_CHAR                 = 0x8000,
    LF_SHORT                = 0x8001,
    LF_USHORT               = 0x8002,
    LF_LONG                 = 0x8003,
    LF_ULONG                = 0x8004,
    LF_REAL32               = 0x8005,
    LF_REAL64               = 0x8006,
    LF_REAL80               = 0x8007,
    LF_REAL128              = 0x8008,
    LF_QUADWORD             = 0x8009,
    LF_UQUADWORD            = 0x800A,
    LF_REAL48               = 0x800B,
    LF_COMPLEX32            = 0x800C,
    LF_COMPLEX64            = 0x800D,
    LF_COMPLEX80            = 0x800E,
    LF_COMPLEX128           = 0x800F,
    LF_VARSTRING            = 0x8010,
    LF_OCTWORD              = 0x8017,
    LF_UOCTWORD             = 0x8018,
    LF_DECIMAL              = 0x8019,
    LF_DATE                 = 0x801A,
    LF_UTF8STRING           = 0x801B,

    LF_PAD0                 = 0x00F0,
    LF_PAD1                 = 0x00F1,
    LF_PAD2                 = 0x00F2,
    LF_PAD3                 = 0x00F3,
    LF_PAD4                 = 0x00F4,
    LF_PAD5                 = 0x00F5,
    LF_PAD6                 = 0x00F6,
    LF_PAD7                 = 0x00F7,
    LF_PAD8                 = 0x00F8,
    LF_PAD9                 = 0x00F9,
    LF_PAD10                = 0x00FA,
    LF_PAD11                = 0x00FB,
    LF_PAD12                = 0x00FC,
    LF_PAD13                = 0x00FD,
    LF_PAD14                = 0x00FE,
    LF_PAD15                = 0x00FF,
}
LEAF_ENUM_e, *PLEAF_ENUM_e, **PPLEAF_ENUM_e;

// =================================================================
// Symbol record indices.
//
// Symbol indices are broken into five ranges. The first range is for
// symbols whose format does not change with the compilation model of
// the program or the target machine. These include register symbols,
// user-defined type symbols, and so on. The second range of symbols
// are those that contain 16:16 segmented addresses. The third symbol
// range is for symbols that contain 16:32 addresses. Note that for
// flat model programs, the segment is replaced with the section number
// for PE format .exe files. The fourth symbol range is for symbols
// that are specific to the MIPS architecture/compiler. The fifth range
// is for Microsoft CodeView optimization.
// =================================================================

typedef enum _SYM_ENUM_e
{
    S_COMPILE               = 0x00000001,   // Compile flags symbol
    S_REGISTER_16t          = 0x00000002,   // Register variable (16-bit)
    S_CONSTANT_16t          = 0x00000003,   // Constant symbol   (16-bit)
    S_UDT_16t               = 0x00000004,   // User-defined Type (16-bit)
    S_SSEARCH               = 0x00000005,   // Start search
    S_END                   = 0x00000006,   // End of block, procedure, with, or thunk
    S_SKIP                  = 0x00000007,   // Skip - Reserve symbol space
    S_CVRESERVE             = 0x00000008,   // Reserved for internal use by the Microsoft debugger
    S_OBJNAME_ST            = 0x00000009,   // Specify name of object file
    S_ENDARG                = 0x0000000A,   // Specify end of arguments in function symbols
    S_COBOLUDT_16t          = 0x0000000B,   // Microfocus COBOL user-defined type
    S_MANYREG_16t           = 0x0000000C,   // Many register symbol
    S_RETURN                = 0x0000000D,   // Function return description
    S_ENTRYTHIS             = 0x0000000E,   // Description of this pointer at entry

    S_BPREL16               = 0x00000100,   // BP relative 16:16
    S_LDATA16               = 0x00000101,   // Local data 16:16
    S_GDATA16               = 0x00000102,   // Global data 16:16
    S_PUB16                 = 0x00000103,   // Public symbol 16:16
    S_LPROC16               = 0x00000104,   // Local procedure start 16:16
    S_GPROC16               = 0x00000105,   // Global procedure start 16:16
    S_THUNK16               = 0x00000106,   // Thunk start 16:16
    S_BLOCK16               = 0x00000107,   // Block start 16:16
    S_WITH16                = 0x00000108,   // With start 16:16
    S_LABEL16               = 0x00000109,   // Code label 16:16
    S_CEXMODEL16            = 0x0000010A,   // Change execution model 16:16
    S_VFTABLE16             = 0x0000010B,   // Virtual function table path descriptor 16:16
    S_REGREL16              = 0x0000010C,   // Specify 16:16 offset relative to arbitrary register

    S_BPREL32_16t           = 0x00000200,   // BP relative 16:32
    S_LDATA32_16t           = 0x00000201,   // Local data 16:32
    S_GDATA32_16t           = 0x00000202,   // Global data 16:32
    S_PUB32_16t             = 0x00000203,   // Public symbol 16:32
    S_LPROC32_16t           = 0x00000204,   // Local procedure start 16:32
    S_GPROC32_16t           = 0x00000205,   // Global procedure start 16:32
    S_THUNK32_ST            = 0x00000206,   // Thunk start 16:32
    S_BLOCK32_ST            = 0x00000207,   // Block start 16:32
    S_WITH32_ST             = 0x00000208,   // With block 16:32
    S_LABEL32_ST            = 0x00000209,   // Code label 16:32
    S_CEXMODEL32            = 0x0000020A,   // Change execution model 16:32
    S_VFTABLE32_16t         = 0x0000020B,   // Virtual function table path descriptor 16:32
    S_REGREL32_16t          = 0x0000020C,   // 16:32 offset relative to arbitrary register
    S_LTHREAD32_16t         = 0x0000020D,   // Local Thread Storage data
    S_GTHREAD32_16t         = 0x0000020E,   // Global Thread Storage data
    S_SLINK32               = 0x0000020F,   //

    S_LPROCMIPS_16t         = 0x00000300,   // Local procedure start MIPS
    S_GPROCMIPS_16t         = 0x00000301,   // Global procedure start MIPS

    S_PROCREF_ST            = 0x00000400,   // Reference to a procedure
    S_DATAREF_ST            = 0x00000401,   // Reference to data
    S_ALIGN                 = 0x00000402,   // Page align symbols
    S_LPROCREF_ST           = 0x00000403,   //
    S_OEM                   = 0x00000404,   //

    S_TI16_MAX              = 0x00001000,   //
    S_REGISTER_ST           = 0x00001001,   //
    S_CONSTANT_ST           = 0x00001002,   //
    S_UDT_ST                = 0x00001003,   //
    S_COBOLUDT_ST           = 0x00001004,   //
    S_MANYREG_ST            = 0x00001005,   //
    S_BPREL32_ST            = 0x00001006,   //
    S_LDATA32_ST            = 0x00001007,   //
    S_GDATA32_ST            = 0x00001008,   //
    S_PUB32_ST              = 0x00001009,   //
    S_LPROC32_ST            = 0x0000100A,   //
    S_GPROC32_ST            = 0x0000100B,   //
    S_VFTABLE32             = 0x0000100C,   //
    S_REGREL32_ST           = 0x0000100D,   //
    S_LTHREAD32_ST          = 0x0000100E,   //
    S_GTHREAD32_ST          = 0x0000100F,   //
    S_LPROCMIPS_ST          = 0x00001010,   //
    S_GPROCMIPS_ST          = 0x00001011,   //
    S_FRAMEPROC             = 0x00001012,   //
    S_COMPILE2_ST           = 0x00001013,   //
    S_MANYREG2_ST           = 0x00001014,   //
    S_LPROCIA64_ST          = 0x00001015,   //
    S_GPROCIA64_ST          = 0x00001016,   //
    S_LOCALSLOT_ST          = 0x00001017,   //
    S_PARAMSLOT_ST          = 0x00001018,   //
    S_ANNOTATION            = 0x00001019,   //
    S_GMANPROC_ST           = 0x0000101A,   //
    S_LMANPROC_ST           = 0x0000101B,   //
    S_RESERVED1             = 0x0000101C,   //
    S_RESERVED2             = 0x0000101D,   //
    S_RESERVED3             = 0x0000101E,   //
    S_RESERVED4             = 0x0000101F,   //
    S_LMANDATA_ST           = 0x00001020,   //
    S_GMANDATA_ST           = 0x00001021,   //
    S_MANFRAMEREL_ST        = 0x00001022,   //
    S_MANREGISTER_ST        = 0x00001023,   //
    S_MANSLOT_ST            = 0x00001024,   //
    S_MANMANYREG_ST         = 0x00001025,   //
    S_MANREGREL_ST          = 0x00001026,   //
    S_MANMANYREG2_ST        = 0x00001027,   //
    S_MANTYPREF             = 0x00001028,   //
    S_UNAMESPACE_ST         = 0x00001029,   //

    S_ST_MAX                = 0x00001100,   //
    S_OBJNAME               = 0x00001101,   // Specify name of object file
    S_THUNK32               = 0x00001102,   // Thunk start
    S_BLOCK32               = 0x00001103,   // Block start
    S_WITH32                = 0x00001104,   // With start
    S_LABEL32               = 0x00001105,   // Code label
    S_REGISTER              = 0x00001106,   //
    S_CONSTANT              = 0x00001107,   //
    S_UDT                   = 0x00001108,   //
    S_COBOLUDT              = 0x00001109,   //
    S_MANYREG               = 0x0000110A,   //
    S_BPREL32               = 0x0000110B,   // BP relative 32:32
    S_LDATA32               = 0x0000110C,   //
    S_GDATA32               = 0x0000110D,   //
    S_PUB32                 = 0x0000110E,   //
    S_LPROC32               = 0x0000110F,   //
    S_GPROC32               = 0x00001110,   //
    S_REGREL32              = 0x00001111,   //
    S_LTHREAD32             = 0x00001112,   //
    S_GTHREAD32             = 0x00001113,   //
    S_LPROCMIPS             = 0x00001114,   //
    S_GPROCMIPS             = 0x00001115,   //
    S_COMPILE2              = 0x00001116,   // Compile Flag
    S_MANYREG2              = 0x00001117,   //
    S_LPROCIA64             = 0x00001118,   //
    S_GPROCIA64             = 0x00001119,   //
    S_LOCALSLOT             = 0x0000111A,   //
    S_SLOT                  = 0x0000111A,   //
    S_PARAMSLOT             = 0x0000111B,   //
    S_LMANDATA              = 0x0000111C,   //
    S_GMANDATA              = 0x0000111D,   //
    S_MANFRAMEREL           = 0x0000111E,   //
    S_MANREGISTER           = 0x0000111F,   //
    S_MANSLOT               = 0x00001120,   //
    S_MANMANYREG            = 0x00001121,   //
    S_MANREGREL             = 0x00001122,   //
    S_MANMANYREG2           = 0x00001123,   //
    S_UNAMESPACE            = 0x00001124,   //
    S_PROCREF               = 0x00001125,   //
    S_DATAREF               = 0x00001126,   //
    S_LPROCREF              = 0x00001127,   //
    S_ANNOTATIONREF         = 0x00001128,   //
    S_TOKENREF              = 0x00001129,   //
    S_GMANPROC              = 0x0000112A,   //
    S_LMANPROC              = 0x0000112B,   //
    S_TRAMPOLINE            = 0x0000112C,   //
    S_MANCONSTANT           = 0x0000112D,   //
    S_RECTYPE_LAST          = 0x0000112D,   //
    S_RECTYPE_MAX           = 0x0000112E,   //
    S_UNKNOWN1136           = 0x00001136,   //
    S_UNKNOWN1137           = 0x00001137,   //
    S_UNKNOWN1138           = 0x00001138,   //
    S_UNKNOWN1139           = 0x00001139,   //
    S_UNKNOWN113A           = 0x0000113A,   //
	S_UNKNOWN113C           = 0x0000113C,   //
	S_UNKNOWN113D           = 0x0000113D,   //
}
SYM_ENUM_e, *PSYM_ENUM_e, **PPSYM_ENUM_e;

// =================================================================
// CALLING CONVENTIONS
// =================================================================

typedef enum _CV_call_e
{
    CV_CALL_NEAR_C          = 0x00000000,
    CV_CALL_FAR_C           = 0x00000001,
    CV_CALL_NEAR_PASCAL     = 0x00000002,
    CV_CALL_FAR_PASCAL      = 0x00000003,
    CV_CALL_NEAR_FAST       = 0x00000004,
    CV_CALL_FAR_FAST        = 0x00000005,
    CV_CALL_SKIPPED         = 0x00000006,
    CV_CALL_NEAR_STD        = 0x00000007,
    CV_CALL_FAR_STD         = 0x00000008,
    CV_CALL_NEAR_SYS        = 0x00000009,
    CV_CALL_FAR_SYS         = 0x0000000A,
    CV_CALL_THISCALL        = 0x0000000B,
    CV_CALL_MIPSCALL        = 0x0000000C,
    CV_CALL_GENERIC         = 0x0000000D,
    CV_CALL_ALPHACALL       = 0x0000000E,
    CV_CALL_PPCCALL         = 0x0000000F,
    CV_CALL_SHCALL          = 0x00000010,
    CV_CALL_ARMCALL         = 0x00000011,
    CV_CALL_AM33CALL        = 0x00000012,
    CV_CALL_TRICALL         = 0x00000013,
    CV_CALL_SH5CALL         = 0x00000014,
    CV_CALL_M32RCALL        = 0x00000015,
    CV_CALL_RESERVED        = 0x00000016,
}
CV_call_e, *PCV_call_e, **PPCV_call_e;

// =================================================================
// POINTER TYPES
// =================================================================

typedef enum _CV_ptrtype_e
{
    CV_PTR_NEAR             = 0x00000000,
    CV_PTR_FAR              = 0x00000001,
    CV_PTR_HUGE             = 0x00000002,
    CV_PTR_BASE_SEG         = 0x00000003,
    CV_PTR_BASE_VAL         = 0x00000004,
    CV_PTR_BASE_SEGVAL      = 0x00000005,
    CV_PTR_BASE_ADDR        = 0x00000006,
    CV_PTR_BASE_SEGADDR     = 0x00000007,
    CV_PTR_BASE_TYPE        = 0x00000008,
    CV_PTR_BASE_SELF        = 0x00000009,
    CV_PTR_NEAR32           = 0x0000000A,
    CV_PTR_FAR32            = 0x0000000B,
    CV_PTR_64               = 0x0000000C,
    CV_PTR_UNUSEDPTR        = 0x0000000D,
}
CV_ptrtype_e, *PCV_ptrtype_e, **PPCV_ptrtype_e;

// =================================================================
// POINTER MODES
// =================================================================

typedef enum _CV_ptrmode_e
{
    CV_PTR_MODE_PTR         = 0x00000000,
    CV_PTR_MODE_REF         = 0x00000001,
    CV_PTR_MODE_PMEM        = 0x00000002,
    CV_PTR_MODE_PMFUNC      = 0x00000003,
    CV_PTR_MODE_RESERVED    = 0x00000004,
}
CV_ptrmode_e, *PCV_ptrmode_e, **PPCV_ptrmode_e;

// =================================================================
// ACCESS PROTECTION MODES
// =================================================================

typedef enum _CV_access_e
{
    CV_private              = 0x00000001,
    CV_protected            = 0x00000002,
    CV_public               = 0x00000003,
}
CV_access_e, *PCV_access_e, **PPCV_access_e;

// =================================================================
// METHOD PROPERTIES
// =================================================================

typedef enum _CV_methodprop_e
{
    CV_MTvanilla            = 0x0000,
    CV_MTvirtual            = 0x0001,
    CV_MTstatic             = 0x0002,
    CV_MTfriend             = 0x0003,
    CV_MTintro              = 0x0004,
    CV_MTpurevirt           = 0x0005,
    CV_MTpureintro          = 0x0006,
}
CV_methodprop_e, *PCV_methodprop_e, **PPCV_methodprop_e;

// =================================================================
// CODEVIEW STRUCTURES
// =================================================================

#pragma pack (1)

// -----------------------------------------------------------------

typedef struct _NB10I
{
    /*000*/ DWORD dwSig;
    /*004*/ DWORD dwOffset;
    /*008*/ DWORD sig;
    /*00C*/ DWORD age;
    /*010*/ BYTE  szPdb [MAX_PATH]; // PDB file name
    /*114*/ }
NB10I, *PNB10I, **PPNB10I;

#define NB10I_ sizeof (NB10I)

// -----------------------------------------------------------------

typedef struct _RSDSI
{
    /*000*/ DWORD dwSig;
    /*004*/ GUID  guidSig;
    /*014*/ DWORD age;
    /*018*/ BYTE  szPdb [3*MAX_PATH];
    /*324*/ }
RSDSI, *PRSDSI, **PPRSDSI;

#define RSDSI_ sizeof (RSDSI)

// -----------------------------------------------------------------

typedef union _CV
{
    /*000*/ DWORD dwSig;
    /*000*/ NB10I nb10i;
    /*000*/ RSDSI rsdsi;
    /*324*/ }
CV, *PCV, **PPCV;

#define CV_ sizeof (CV)

// -----------------------------------------------------------------

#pragma pack ()

// =================================================================
// MSF STRUCTURES
// =================================================================

#pragma pack (1)

// -----------------------------------------------------------------

typedef struct _SI_PERSIST
{
    /*000*/ LONG cb;      // number of bytes
    /*004*/ LONG mpspnpn;
    /*008*/ }
SI_PERSIST, *PSI_PERSIST, **PPSI_PERSIST;

#define SI_PERSIST_ sizeof (SI_PERSIST)

// -----------------------------------------------------------------

typedef struct _PG
{
    /*0000*/ BYTE rgb [0x1000]; // page data
    /*1000*/ }
PG, *PPG, **PPPG;

#define PG_ sizeof (PG)

// -----------------------------------------------------------------

typedef union _MSF_HDR
{
    struct
    {
        /*0000*/     BYTE       szMagic [0x2C];
        /*002C*/     LONG       cbPg;
        /*0030*/     WORD       pnFpm;
        /*0032*/     WORD       pnMac;
        /*0034*/     SI_PERSIST siSt;
        /*003C*/     WORD       mpspnpn [0x141];
        /*02BE*/     };
    /*0000*/ PG pg;
    /*1000*/ }
MSF_HDR, *PMSF_HDR, **PPMSF_HDR;

#define MSF_HDR_ sizeof (MSF_HDR)

// -----------------------------------------------------------------

typedef union _BIGMSF_HDR
{
    struct
    {
        /*0000*/     BYTE       szMagic [0x1E];
        /*001E*/     WORD       reserved;
        /*0020*/     LONG       cbPg;
        /*0024*/     DWORD      pnFpm;
        /*0028*/     DWORD      pnMac;
        /*002C*/     SI_PERSIST siSt;
        /*0034*/     DWORD      mpspnpnSt [0x49];
        /*0158*/     };
    /*0000*/ PG pg;
    /*1000*/ }
BIGMSF_HDR, *PBIGMSF_HDR, **PPBIGMSF_HDR;

#define BIGMSF_HDR_ sizeof (BIGMSF_HDR)

// -----------------------------------------------------------------

typedef struct _FPM
{
    /*000*/ DWORD   iwMac;
    /*004*/ DWORD   iwRover;
    /*008*/ LONG    cbPg;
    /*00C*/ BOOL    fBigMsf;
    /*00D*/ BYTE    reserved1; // padding
    /*00E*/ WORD    reserved2; // padding
    /*010*/ struct
        /*010*/     {
            /*010*/     DWORD rgt;
            /*014*/     DWORD itMac;
            /*018*/     DWORD itMax;
            /*01C*/     } rgw;
    /*01C*/ DWORD   wFill;
    /*020*/ }
FPM, *PFPM, **PPFPM;

#define FPM_ sizeof (FPM)

// -----------------------------------------------------------------

#pragma pack ()

// =================================================================
// PDB STRUCTURES (STREAM #1)
// =================================================================

#pragma pack (1)

// -----------------------------------------------------------------

#define GUID_SIG70 \
{0x33DED1D8, 0x5D57, 0x40D0, \
{0xA5, 0xE2, 0xF1, 0x71, 0x96, 0x98, 0x07, 0x21}}

// -----------------------------------------------------------------

typedef struct _PDBStream
{
    /*000*/ DWORD impv;
    /*004*/ DWORD sig;
    /*008*/ DWORD age;
    /*00C*/ }
PDBStream, *PPDBStream, **PPPDBStream;

#define PDBStream_ sizeof (PDBStream)

// -----------------------------------------------------------------

typedef struct _PDBStream70
{
    /*000*/ PDBStream pdbstream;
    /*00C*/ GUID      sig70;
    /*01C*/ }
PDBStream70, *PPDBStream70, **PPPDBStream70;

#define PDBStream70_ sizeof (PDBStream70)

// -----------------------------------------------------------------

#pragma pack ()

// =================================================================
// TPI STRUCTURES (STREAM #2)
//
// Contains information about symbol typing.
// There are fields in the records contained in $$SYMBOLS that index into the records contained
// in $$TYPES. Records in $$TYPES can also index into the records contained in the $$TYPES
// table.
// =================================================================

#pragma pack (1)

// -----------------------------------------------------------------
// Data location

struct TPI_LOCATION
{
    LONG lOffset;  // Offset of the data
    LONG cbSize;   // Size of the data
};

// -----------------------------------------------------------------
// TPI hash

struct TpiHash
{
    WORD  sn;            // stream #
    WORD  snPad;         // padding
    LONG  cbHashKey;
    LONG  cHashBuckets;
    TPI_LOCATION offcbHashVals;
    TPI_LOCATION offcbTiOff;
    TPI_LOCATION offcbHashAdj;
};

// -----------------------------------------------------------------
// TPI stream header
//

struct TPI_STREAM_HEADER
{
    DWORD   dwVersion;        // Implementation version
    LONG    cbHeader;         // Header size
    DWORD   dwMinTypeIndex;   // Type index base  (0x1000..0xFFFFFF)
    DWORD   dwMaxTypeIndex;   // Type index limit (0x1000..0xFFFFFF)
    DWORD   cbTypeRecords;    // Size of follow-up data
    struct TpiHash TpiHash;
};

// -----------------------------------------------------------------
// ???

typedef struct _OHDR
{
    BYTE  szMagic[0x2C];  // Magic string
    DWORD dwVersion;      // Implementation version
    DWORD dwSignature;    // Signature
    DWORD dwAge;          // File age
    WORD  dwMinTypeIndex; // Type index base
    WORD  dwMaxTypeIndex; // Type index limit
    LONG  cbSize;         // Size
}
OHDR, *POHDR, **PPOHDR;

// -----------------------------------------------------------------

typedef struct _CV_prop_t
{
    /*000.0*/ WORD packed   : 1;
    /*000.1*/ WORD ctor     : 1;
    /*000.2*/ WORD ovlops   : 1;
    /*000.3*/ WORD isnested : 1;
    /*000.4*/ WORD cnested  : 1;
    /*000.5*/ WORD opassign : 1;
    /*000.6*/ WORD opcast   : 1;
    /*000.7*/ WORD fwdref   : 1;
    /*001.0*/ WORD scoped   : 1;
    /*001.1*/ WORD reserved : 7;
    /*002*/ }
CV_prop_t, *PCV_prop_t, **PPCV_prop_t;

#define CV_prop_t_ sizeof (CV_prop_t)

// -----------------------------------------------------------------

typedef struct _CV_fldattr_t
{
    /*000.0*/ WORD access      : 2; // CV_access_e
    /*000.2*/ WORD mprop       : 3; // CV_methodprop_e
    /*000.5*/ WORD pseudo      : 1;
    /*000.6*/ WORD noinherit   : 1;
    /*000.7*/ WORD noconstruct : 1;
    /*001.0*/ WORD compgenx    : 1;
    /*001.1*/ WORD unused      : 7;
    /*002*/ }
CV_fldattr_t, *PCV_fldattr_t, **PPCV_fldattr_t;

#define CV_fldattr_t_ sizeof (CV_fldattr_t)

// -----------------------------------------------------------------

typedef struct _lfArray         // array
{
    /*000*/ WORD  leaf;             // LF_ARRAY
    /*002*/ DWORD elemtype;         // element type index
    /*006*/ DWORD idxtype;          // indexing type index
    /*00A*/ BYTE  data [1];          // size in bytes
    /*00A*/ }
lfArray, *PlfArray, **PPlfArray;

#define lfArray_ sizeof (lfArray)

// -----------------------------------------------------------------

typedef struct _lfBitfield      // bitfield structure
{
    /*000*/ WORD  leaf;             // LF_BITFIELD
    /*002*/ DWORD type;             // base type index
    /*006*/ BYTE  length;           // number of bits
    /*007*/ BYTE  position;         // bit offset of bit 0
    /*008*/ }
lfBitfield, *PlfBitfield, **PPlfBitfield;

#define lfBitfield_ sizeof (lfBitfield)

// -----------------------------------------------------------------

typedef struct _lfClass         // class or structure
{
    /*000*/ WORD      leaf;         // LF_CLASS, LF_STRUCTURE
    /*002*/ WORD      count;        // number of members
    /*004*/ CV_prop_t property;     // type properties
    /*006*/ DWORD     field;        // LF_FIELD descriptor index
    /*00A*/ DWORD     derived;
    /*00E*/ DWORD     vshape;
    /*012*/ BYTE      data [1];      // size and name
    /*012*/ }
lfClass, *PlfClass, **PPlfClass;

#define lfClass_ sizeof (lfClass)

// -----------------------------------------------------------------

typedef lfClass lfStructure, *PlfStructure, **PPlfStructure;
#define lfStructure_ sizeof (lfStructure)

// -----------------------------------------------------------------

typedef struct _lfUnion         // union
{
    /*000*/ WORD      leaf;         // LF_UNION
    /*002*/ WORD      count;        // number of members
    /*004*/ CV_prop_t property;     // type properties
    /*006*/ DWORD     field;        // LF_FIELD descriptor index
    /*00A*/ BYTE      data [1];      // size and name
    /*00A*/ }
lfUnion, *PlfUnion, **PPlfUnion;

#define lfUnion_ sizeof (lfUnion)

// -----------------------------------------------------------------

typedef struct _lfEnum          // enumeration
{
    /*000*/ WORD      leaf;         // LF_ENUM
    /*002*/ WORD      count;        // number of members
    /*004*/ CV_prop_t property;     // type properties
    /*006*/ DWORD     utype;        // underlying type
    /*00A*/ DWORD     field;        // LF_FIELD descriptor index
    /*00E*/ BYTE      Name [1];      // name
    /*00E*/ }
lfEnum, *PlfEnum, **PPlfEnum;

#define lfEnum_ sizeof (lfEnum)

// -----------------------------------------------------------------

typedef struct _lfPointer       // pointer to type
{
    /*000*/ struct lfPointerBody
        /*000*/     {
            /*000*/     WORD  leaf;         // LF_POINTER
            /*002*/     DWORD utype;        // underlying type
            /*006*/     struct lfPointerAttr
                /*006*/         {
                    /*006.0*/       DWORD ptrtype     :  5; // pointer type
                    /*006.5*/       DWORD ptrmode     :  3; // pointer mode
                    /*007.0*/       DWORD isflat32    :  1; // 0:32 pointer
                    /*007.1*/       DWORD isvolatile  :  1; // volatile pointer
                    /*007.2*/       DWORD isconst     :  1; // constant pointer
                    /*007.3*/       DWORD isunaligned :  1; // unaligned pointer
                    /*007.4*/       DWORD isrestrict  :  1; // restricted pointer
                    /*007.5*/       DWORD unused      : 19; // currently unused
                    /*00A*/         } attr;
            /*00A*/     };
    /*00A*/ union
        /*00A*/     {
            /*00A*/     struct
                /*00A*/         {
                    /*00A*/         DWORD pmclass;
                    /*00E*/         WORD  pmenum;
                    /*010*/         } pm;
            /*00A*/     WORD bseg;
            /*00A*/     BYTE Sym [1];
            /*00A*/     struct
                /*00A*/         {
                    /*00A*/         DWORD index;
                    /*00E*/         BYTE  name [1];
                    /*00E*/         } btype;
            /*010*/     } pbase;
    /*010*/ }
lfPointer, *PlfPointer, **PPlfPointer;

#define lfPointer_ sizeof (lfPointer)

// -----------------------------------------------------------------

typedef struct _lfProc          // procedure
{
    /*000*/ WORD  leaf;             // LF_PROCEDURE
    /*002*/ DWORD rvtype;           // return value type
    /*006*/ BYTE  calltype;         // calling convention (CV_call_e)
    /*007*/ BYTE  reserved;         // currently not used
    /*008*/ WORD  parmcount;        // number of parameters
    /*00A*/ DWORD arglist;          // argument list type
    /*00E*/ }
lfProc, *PlfProc, **PPlfProc;

#define lfProc_ sizeof (lfProc)

// -----------------------------------------------------------------

typedef struct _lfMFunc         // member function
{
    WORD  leaf;             // LF_MFUNCTION
    DWORD rvtype;           // return value type
    DWORD classtype;        // containing class type
    DWORD thistype;         // this-pointer type
    BYTE  calltype;         // calling convention (CV_call_e)
    BYTE  reserved;         // currently not used
    WORD  parmcount;        // number of parameters
    DWORD arglist;          // argument list type
    LONG  thisadjust;       // this-adjuster
}
lfMFunc, *PlfMFunc, **PPlfMFunc;

#define lfMFunc_ sizeof (lfMFunc)

// -----------------------------------------------------------------

typedef struct _lfArgList       // procedure argument list
{
    WORD  leaf;             // LF_ARGLIST
    DWORD count;            // number of arguments
    DWORD arg [1];           // argument types
}
lfArgList, *PlfArgList, **PPlfArgList;

#define lfArgList_ sizeof (lfArgList)

// -----------------------------------------------------------------

typedef struct _lfVTShape       // virtual function table shape
{
    WORD leaf;              // LF_VTSHAPE
    WORD count;             // number of VFT entries
    BYTE desc [1];           // 4-bit descriptor list
}
lfVTShape, *PlfVTShape, **PPlfVTShape;

#define lfVTShape_ sizeof (lfVTShape)

// -----------------------------------------------------------------

#pragma pack(1)
typedef struct _lfEnumerate     // enumeration member
{
    WORD         leaf;      // LF_ENUMERATE
    CV_fldattr_t attr;
    BYTE         value [1];
}
lfEnumerate, *PlfEnumerate, **PPlfEnumerate;
#pragma pack()

#define lfEnumerate_ sizeof (lfEnumerate)

// -----------------------------------------------------------------

typedef struct _lfMember        // non-static data member
{
    WORD         leaf;      // LF_MEMBER
    CV_fldattr_t attr;
    DWORD        index;
    BYTE         offset [1];
}
lfMember, *PlfMember, **PPlfMember;

#define lfMember_ sizeof (lfMember)

// -----------------------------------------------------------------

typedef struct _lfBClass        // base class field
{
    WORD         leaf;      // LF_BCLASS
    CV_fldattr_t attr;
    DWORD        index;
    BYTE         offset [1];
}
lfBClass, *PlfBClass, **PPlfBClass;

#define lfBClass_ sizeof (lfBClass)

// -----------------------------------------------------------------

typedef struct _lfVFuncTab      // virtual function table pointer
{
    WORD  leaf;             // LF_VFUNCTAB
    WORD  pad0;             // padding
    DWORD type;             // VFT pointer type
}
lfVFuncTab, *PlfVFuncTab, **PPlfVFuncTab;

#define lfVFuncTab_ sizeof (lfVFuncTab)

// -----------------------------------------------------------------

typedef struct _lfOneMethod     // non-overloaded method
{
    WORD         leaf;      // LF_ONEMETHOD
    CV_fldattr_t attr;
    DWORD        index;
    DWORD        vbaseoff [1]; // VFT base offset, if present
}
lfOneMethod, *PlfOneMethod, **PPlfOneMethod;

#define lfOneMethod_ sizeof (lfOneMethod)

// -----------------------------------------------------------------

typedef struct _lfMethod        // overloaded method list
{
    WORD  leaf;             // LF_METHOD
    WORD  count;            // number of occurrences
    DWORD mList;            // LF_METHODLIST descriptor index
    BYTE  Name [1];
}
lfMethod, *PlfMethod, **PPlfMethod;

#define lfMethod_ sizeof (lfMethod)

// -----------------------------------------------------------------

typedef struct _lfNestType      // nested type definition
{
    WORD  leaf;             // LF_NESTTYPE
    WORD  pad0;
    DWORD index;
    BYTE  Name [1];
}
lfNestType, *PlfNestType, **PPlfNestType;

#define lfNestType_ sizeof (lfNestType)

// -----------------------------------------------------------------

typedef union _lfSubRecord
{
    /*000*/ WORD        leaf;      // LF_*
    /*000*/ lfEnumerate Enumerate; // LF_ENUMERATE
    /*000*/ lfMember    Member;    // LF_MEMBER
    /*000*/ lfBClass    BClass;    // LF_BCLASS
    /*000*/ lfVFuncTab  VFuncTab;  // LF_VFUNCTAB
    /*000*/ lfOneMethod OneMethod; // LF_ONEMETHOD
    /*000*/ lfMethod    Method;    // LF_METHOD
    /*000*/ lfNestType  NestType;  // LF_NESTTYPE
}
lfSubRecord, *PlfSubRecord, **PPlfSubRecord;

#define lfSubRecord_ sizeof (lfSubRecord)

// -----------------------------------------------------------------

#pragma pack(1)
typedef struct _lfFieldList     // struct/union/enum members
{
    /*000*/ WORD        leaf;       // LF_FIELDLIST
    /*002*/ lfSubRecord SubRecord;
    /*002*/ }
lfFieldList, *PlfFieldList, **PPlfFieldList;
#pragma pack()

#define lfFieldList_ sizeof (lfFieldList)

// -----------------------------------------------------------------

typedef union _lfRecord
{
    /*000*/ WORD        leaf;      // LF_*
    /*000*/ lfArray     Array;     // LF_ARRAY
    /*000*/ lfBitfield  Bitfield;  // LF_BITFIELD
    /*000*/ lfClass     Class;     // LF_CLASS
    /*000*/ lfStructure Structure; // LF_STRUCTURE
    /*000*/ lfUnion     Union;     // LF_UNION
    /*000*/ lfEnum      Enum;      // LF_ENUM
    /*000*/ lfPointer   Pointer;   // LF_POINTER
    /*000*/ lfProc      Proc;      // LF_PROCEDURE
    /*000*/ lfMFunc     MFunc;     // LF_MFUNCTION
    /*000*/ lfArgList   ArgList;   // LF_ARGLIST
    /*000*/ lfVTShape   VTShape;   // LF_VTSHAPE
    /*000*/ lfFieldList FieldList; // LF_FIELDLIST
}
lfRecord, *PlfRecord, **PPlfRecord;

#define lfRecord_ sizeof (lfRecord)

// -----------------------------------------------------------------

#pragma pack ()

// =================================================================
// DBI STRUCTURES (STREAM #3)
//
// Debug info (DBI) stream begins with header and followed by several
// sections whose sizes are defined in header.
// =================================================================

// -----------------------------------------------------------------
// Debug info stream header v.4.0.

#pragma pack (1)
typedef struct tagDBI_STREAM_HEADER_400
{
    WORD wGSStreamNumber;           // Global symbol stream number
    WORD wPSStreamNumber;           // Global symobol publics stream number
    WORD wSymStreamNumber;          // Symbol stream number
    WORD wReserved;                 // Reserved
    LONG cbModuleInfo;              // Size of module info section
    LONG cbSectionContrib;          // Size of section contribution
    LONG cbSectionMap;              // Size of section map
    LONG cbFileInfo;                // Size of file info section
}
DBI_STREAM_HEADER_400, *PDBI_STREAM_HEADER_400, **PPDBI_STREAM_HEADER_400;

// -----------------------------------------------------------------
// Debug info stream header v.7.0.

#define DBI_HDR_SIGNATURE 0xFFFFFFFF  // Header signature
#define DBI_HDR_VERSION   19990903    // Header version

typedef struct tagDBI_STREAM_HEADER_700
{
    DWORD dwSignature;             // Signature (DBI_HDR_SIGNATURE)
    DWORD dwVersion;               // Header version (DBI_HDR_VERSION)
    DWORD dwAge;                   // Age
    WORD  wGSStreamNumber;         // Global symbol stream number
    WORD  wVerPdbDllMajMin;        // Version of PDB DLL ???
    WORD  wPSStreamNumber;         // Global symobol publics stream number
    WORD  wVerPdbDllBuild;         // Version of PDB DLL ???
    union
    {
        WORD  wSymStreamNumber;      // Symbol stream number
        DWORD dwReserved;
    };
    LONG  cbModuleInfo;            // Size of module info section
    LONG  cbSectionContrib;        // Size of section contribution
    LONG  cbSectionMap;            // Size of section map
    LONG  cbFileInfo;              // Size of file info section
    LONG  cbTSMap;                 // Thread storage map ???
    DWORD iMFC;                    // ???
    LONG  cbDbgHdr;                // Size of debug header ???
    LONG  cbECInfo;                // ???
    WORD  wFlags;                  // Flags
    WORD  wMachine;                // Architecture enumeration
    DWORD dwReserved2[1];          // Reserved
}
DBI_STREAM_HEADER_700, *PDBI_STREAM_HEADER_700, **PPDBI_STREAM_HEADER_700;

// -----------------------------------------------------------------
// Edit and continue info

struct EditAndContinueInfo
{
    DWORD niSrcFile; // Source file index
    DWORD niPdbFile; // PDB file index???
};

// -----------------------------------------------------------------
// Section contribution
//
// Section contribution is a block of code belonging to a segment (section) that was contrubuted
// by a compiland module. Section contribution information is used to find which compiland module
// has contributed the code located at some address, and then to determine corresponding
// source file name and line number for the code.

typedef struct tagDBI_SECTION_CCONTRIB_40
{
    WORD  wSectionIndex;          // Section index
    WORD  wReserved1;             // Reserved
    LONG  lOffset;                // Offset
    LONG  cbSize;                 // Size of contribution
    DWORD dwCharacteristics;      // Characterisitcs
    WORD  wModuleIndex;           // Module index
    WORD  wReserved2;             // Reserved
}
DBI_SECTION_CONTRIB_40, *PDBI_SECTION_CONTRIB_40, **PPDBI_SECTION_CONTRIB_40;

typedef struct tagDBI_SECTION_CCONTRIB
{
    DBI_SECTION_CONTRIB_40  SectionContrib40;  // Section contrib v4.0
    DWORD dwDataCrc;                       // Data CRC value
    DWORD dwRelocCrc;                      // CRC of reloc ???
}
DBI_SECTION_CONTRIB, *PDBI_SECTION_CONTRIB, **PPDBI_SECTION_CONTRIB;

// -----------------------------------------------------------------
// Module info
//
// Info related to a compiland module (module name, stream number and so on).

typedef struct tagDBI_MODULE_INFO
{
    DWORD  dwModulePtr;      // Module pointer
    DBI_SECTION_CONTRIB SectionContrib;   // This module's first section contribution
    struct
    {
        WORD fWritten   : 1;   // TRUE if module has been written since DBI opened
        WORD fECEnabled : 1;   // Edit and continue info enabled
        WORD Reserved   : 6;   // Reserved
        WORD iTSM       : 8;   //
    };
    WORD   wStreamNumber;    // Stream number of module debug info (symbols, lines, fpo), or snNil
    LONG   cbSymbols;        // Size of local symbols debug info in stream sn
    LONG   cbLines;          // Size of line number debug info in stream sn
    LONG   cbC13Lines;       //
    WORD   ifileMac;         // Size of frame pointer opt debug info in stream sn
    WORD   wReserved;        // Currently not used
    LONG   mpifileichFile;   // array [0..ifileMac) of offsets into dbi.bufFilenames
    EditAndContinueInfo ecInfo; // Edit and continue info
    BYTE   uchName[1];       // Module name zero-terminated string followed by obj file name zero-terminated string.
}
DBI_MODULE_INFO, *PDBI_MODULE_INFO, **PPDBI_MODULE_INFO;

#pragma pack(1)
struct DBI_ECINFO_HEADER
{
    DWORD dwSignature;  // Signature 0xFEEFFEEF UTF-8???
    DWORD dwUnknown1;   // Count of something
    DWORD cbNames;      // Size of names
};
#pragma pack()

// =================================================================
// FPO STRUCTURES
// =================================================================

#pragma pack (1)

// -----------------------------------------------------------------
// Frame pointer omission (FPO) Record

#ifndef _WIN32

typedef struct __FPO_DATA
{
    DWORD ulOffStart;
    DWORD cbProcSize;
    DWORD cdwLocals;
    WORD  cdwParams;
    struct
    {
        WORD cbProlog : 8;
        WORD cbRegs   : 3;
        WORD fHasSEH  : 1;
        WORD fUseBP   : 1;
        WORD reserved : 1;
        WORD cbFrame  : 2;
    };
}
_FPO_DATA, *P_FPO_DATA, **PP_FPO_DATA;

#endif //!_WIN32

// -----------------------------------------------------------------

#pragma pack ()

// =================================================================
// PSGSI STRUCTURES
// =================================================================

#pragma pack (1)

// -----------------------------------------------------------------
// This subsection contains the globally compacted public symbols from the sstPublics. The format
// of the table is a header specifying the symbol and address hash functions, the length of the
// symbol information, the length of the symbol hash function data, and the length of address hash
// function data. This is followed by symbol information, which is followed by the symbol hash
// tables, and then followed by the address hash tables.

typedef struct _PSGSIHDR
{
    LONG  cbSymHash;       // Count or number of bytes in the symbol hash table.
    LONG  cbAddrMap;       // Count or number of bytes in the address hashing table.
    DWORD dwThunkCount;    // Count of thunks
    LONG  cbSizeOfThunk;   // Size of thunk
    WORD  wThunkTableSectionIndex; // Index of a thunk table section
    WORD  wReserved;         // Reserved
    LONG  lThunkTableOffset; // Offset of thunk table
    DWORD dwSectionCount;  // Section count
}
PSGSIHDR, *PPSGSIHDR, **PPPSGSIHDR;

// -----------------------------------------------------------------
// Global symbols
//
// This subsection contains globally compacted symbols. The format of the table is a header
// specifying the symbol and address hash functions, the length of the symbol information, the
// length of the symbol hash function data, and the length of address hash function data. This is
// followed by the symbol information, which followed by the symbol hash tables, and then
// followed by the address hash tables.

#pragma pack(1)
typedef struct tagGSI_STREAM_HEADER
{
    DWORD dwSignature;        // Signature
    DWORD dwVersion;          // Version of the header
    DWORD dwHashTableSize;    // Size of the hash table
    DWORD dwBucketTableSize;  // Size of the hash buckets table
    // Hash table follows this header.
    // Hash buckets table follows the hash table.
}
GSI_STREAM_HEADER, *PGSI_STREAM_HEADER, **PPGSI_STREAM_HEADER;
#pragma pack()

#pragma pack(1)
struct GSI_HASH_RECORD
{
    DWORD dwSymOffset;        //!< Hash or offset ???
    DWORD dwRefCount;         //!< Ref count
};
#pragma pack()



// -----------------------------------------------------------------

#pragma pack ()

// =================================================================
// SYMBOL RECORDS
//
// Describes the symbols in the object file. The record for each symbol
// contains the symbol name, the symbol address and other information needed to describe the
// symbol.
// =================================================================

#pragma pack (1)

// -----------------------------------------------------------------

typedef enum _CV_PUBSYMFLAGS_e
{
    cvpsfNone     = 0x00000000,
    cvpsfCode     = 0x00000001,
    cvpsfFunction = 0x00000002,
    cvpsfManaged  = 0x00000004,
    cvpsfMSIL     = 0x00000008,
}
CV_PUBSYMFLAGS_e, *PCV_PUBSYMFLAGS_e, **PPCV_PUBSYMFLAGS_e;

// -----------------------------------------------------------------

typedef union _CV_GENERIC_FLAG
{
    /*000.0*/ WORD cstyle  :  1;
    /*000.1*/ WORD rsclean :  1;
    /*000.2*/ WORD unused  : 14;
    /*002*/ }
CV_GENERIC_FLAG, *PCV_GENERIC_FLAG, **PPCV_GENERIC_FLAG;

#define CV_GENERIC_FLAG_ sizeof (CV_GENERIC_FLAG)

// -----------------------------------------------------------------

typedef union _CV_PUBSYMFLAGS
{
    /*000*/ DWORD grfFlags; // CV_PUBSYMFLAGS_e
    /*000*/ struct
        /*000*/     {
            /*000.0*/   DWORD fCode     :  1;
            /*000.1*/   DWORD fFunction :  1;
            /*000.2*/   DWORD fManaged  :  1;
            /*000.3*/   DWORD fMSIL     :  1;
            /*000.4*/   DWORD reserved  : 28;
            /*004*/     };
    /*004*/ }
CV_PUBSYMFLAGS, *PCV_PUBSYMFLAGS, **PPCV_PUBSYMFLAGS;

#define CV_PUBSYMFLAGS_ sizeof (CV_PUBSYMFLAGS)

// -----------------------------------------------------------------
// Flag definition used in the S_LPROC32 symbol record

union CV_PROCFLAGS
{
    BYTE bAll;
    BYTE grfAll;
    struct
    {
        BYTE CV_PFLAG_NOFPO      : 1;  // True if function has frame pointer omitted.
        BYTE CV_PFLAG_INT        : 1;  // True if function is interrupt routine.
        BYTE CV_PFLAG_FAR        : 1;  // True if function performs far return.
        BYTE CV_PFLAG_NEVER      : 1;  // True if function never returns.
        BYTE CV_PFLAG_NOTREACHED : 1;  //
        BYTE CV_PFLAG_CUST_CALL  : 1;  //
        BYTE CV_PFLAG_NOINLINE   : 1;  //
        BYTE unused              : 1;  //
    };
};

// -----------------------------------------------------------------

typedef struct _CV_LVARFLAGS
{
    /*000.0*/ WORD fIsParam   :  1;
    /*000.1*/ WORD fAddrTaken :  1;
    /*000.2*/ WORD fCompGenx  :  1;
    /*000.3*/ WORD unused     : 13;
    /*002*/ }
CV_LVARFLAGS, *PCV_LVARFLAGS, **PPCV_LVARFLAGS;

#define CV_LVARFLAGS_ sizeof (CV_LVARFLAGS)

// -----------------------------------------------------------------

typedef struct _CV_lvar_attr
{
    /*000*/ DWORD        off;
    /*004*/ WORD         seg;
    /*006*/ CV_LVARFLAGS flags;
    /*008*/ }
CV_lvar_attr, *PCV_lvar_attr, **PPCV_lvar_attr;

#define CV_lvar_attr_ sizeof (CV_lvar_attr)

// -----------------------------------------------------------------
// Alignment
//
// This symbol is inserted by the CVPACK utility to pad symbol space so that the next symbol will
// not cross a page boundary.

struct SYM_ALIGN
{
    WORD wSymLength;      // Record length.
    WORD wSymType;        // Record type (S_ALIGN).
    // Unused data follow. Use the length field that precedes every symbol record
    // to skip this record. The pad bytes must be zero.
};

//------------------------------------------------------------------
// Symbol header structure (OMF header).

#pragma pack(1)
typedef struct _OMF_HEADER
{
    WORD wRecordSize; // Length of the record in bytes, not including this member.
    WORD wRecordType; // Symbol type.
}
OMF_HEADER, *POMF_HEADER, **PPOMF_HEADER;
#pragma pack()

// -----------------------------------------------------------------
// Annotation
//
// Specifies an annotated location in program code.

struct SYM_ANNOTATION
{
    WORD  wSymLength;   // Record length.
    WORD  wSymType;     // Record type (S_ANNOTATION).
    DWORD dwOffset;     // Offset part of symbol address.
    WORD  wSegment;     // Segment part of symbol address.
    WORD  cbName;       // Length of symbol name.
    BYTE  uchName[1];   // Name of this symbol.
};

// -----------------------------------------------------------------
// Many register symbol
//
// This record is used to specify that a symbol is stored in a set of registers.

struct SYM_ATTRMANYREG
{
    WORD         wSymLength;      // Record length
    WORD         wSymType;        // Record type (S_MANYREG)
    DWORD        dwTypeIndex;     // Symbol type index
    CV_lvar_attr Attr;            //
    BYTE         uchCount;        // Count of the register enumerations that follow.
    BYTE         uchRegisters[1]; // List of registers in which the symbol is stored. The registers are listed
    // high order register first.
    // Name of the symbol follows.
};

struct SYM_ATTRMANYREG2
{
    WORD         wSymLength;      // Record length
    WORD         wSymType;        // Record type
    DWORD        dwTypeIndex;     // Symbol type index
    CV_lvar_attr Attr;            //
    WORD         wCount;          // Count of the register enumerations that follow.
    WORD         wRegisters[1];   // List of registers in which the symbol is stored. The registers are listed
    // high order register first.
    // Name of the symbol follows.
};

// -----------------------------------------------------------------

typedef struct _ATTRREGREL
{
    /*000*/ WORD         reclen;   // record length
    /*002*/ WORD         rectyp;   // record type
    /*004*/ DWORD        off;
    /*008*/ DWORD        typind;
    /*00C*/ WORD         reg;
    /*00E*/ CV_lvar_attr attr;
    /*016*/ BYTE         name [1];
    /*017*/ }
ATTRREGREL, *PATTRREGREL, **PPATTRREGREL;

#define ATTRREGREL_ sizeof (ATTRREGREL)

// -----------------------------------------------------------------

typedef struct _ATTRREGSYM
{
    /*000*/ WORD         reclen;   // record length
    /*002*/ WORD         rectyp;   // record type
    /*004*/ DWORD        typind;
    /*008*/ CV_lvar_attr attr;
    /*010*/ WORD         reg;
    /*012*/ BYTE         name [1];
    /*013*/ }
ATTRREGSYM, *PATTRREGSYM, **PPATTRREGSYM;

#define ATTRREGSYM_ sizeof (ATTRREGSYM)

// -----------------------------------------------------------------

typedef struct _ATTRSLOTSYM
{
    /*000*/ WORD         reclen;   // record length
    /*002*/ WORD         rectyp;   // record type
    /*004*/ DWORD        iSlot;
    /*008*/ DWORD        typind;
    /*00C*/ CV_lvar_attr attr;
    /*014*/ BYTE         name [1];
    /*015*/ }
ATTRSLOTSYM, *PATTRSLOTSYM, **PPATTRSLOTSYM;

#define ATTRSLOTSYM_ sizeof (ATTRSLOTSYM)

// -----------------------------------------------------------------
// Block start
//
// This symbol specifies the start of an inner block of lexically scoped symbols. The lexical scope
// is terminated by a matching S_END symbol.

struct SYM_BLOCK
{
    WORD  wSymLength;   // Record length
    WORD  wSymType;     // Record type (S_BLOCK)
    DWORD pParent;      // Offset of lexical scope parent symbol.
    DWORD pEnd;         // Offset of lexical scope end.
};

struct SYM_BLOCK16
{
    WORD  wSymLength;   // Record length
    WORD  wSymType;     // Record type (S_BLOCK16)
    DWORD pParent;      // Offset of lexical scope parent symbol.
    DWORD pEnd;         // Offset of lexical scope end.
    WORD  wLength;      // Length in bytes of the scope of this block.
    WORD  wOffset;      // Offset portion of the segmented procedure address.
    WORD  wSegment;     // Segment portion of the segmented procedure address.
    BYTE  uchName[1];   // Name of this symbol.
};

struct SYM_BLOCK32
{
    WORD  wSymLength;   // Record length
    WORD  wSymType;     // Record type (S_BLOCK32)
    DWORD pParent;      // Offset of lexical scope parent symbol.
    DWORD pEnd;         // Offset of lexical scope end.
    DWORD dwLength;     // Length in bytes of the scope of this block.
    DWORD dwOffset;     // Offset portion of the segmented procedure address.
    WORD  wSegment;     // Segment portion of the segmented procedure address.
    BYTE  uchName[1];   // Name of this symbol.
};

// -----------------------------------------------------------------
// BP Relative
//
// This symbol specifies symbols that are allocated on the stack for a procedure. For C and C++,
// these include the actual function parameters and the local nonstatic variables of functions.

struct SYM_BPREL_16
{
    WORD  wSymLength;   // Record length
    WORD  wSynType;     // Record type (S_BPREL16)
    SHORT nOffset;      // Signed offset relative to BP. If offset is 0, the symbol was assigned to
    // a register or never instantiated by the optimizer and cannot be
    // evaluated because its location is unknown.
    WORD  wTypeIndex;   // Type of symbol.
    BYTE  uchName[1];   // Length-prefixed name of symbol.
};

struct SYM_BPREL_32
{
    WORD  wSymLength;   // Record length
    WORD  wSymType;     // Record type (S_BPREL32)
    LONG  lOffset;      // Signed offset relative to BP. If offset is 0, the symbol was assigned to
    // a register or never instantiated by the optimizer and cannot be
    // evaluated because its location is unknown.
    DWORD dwTypeIndex;  // Type of symbol.
    BYTE  uchName[1];   // Length-prefixed name of symbol.
};

struct SYM_BPREL_32_16
{
    WORD wSymLength;    // Record length
    WORD wSymType;      // Record type (S_BPREL32)
    LONG lOffset;       // Signed offset relative to BP. If offset is 0, the symbol was assigned to
    // a register or never instantiated by the optimizer and cannot be
    // evaluated because its location is unknown.
    WORD wTypeIndex;    // Type of symbol.
    BYTE uchName[1];    // Length-prefixed name of symbol.
};

// -----------------------------------------------------------------
// Change Execution Model
//
// This record is used to notify the debugger that, starting at the given code offset and until the
// address specified by the next Change Execution Model record, the execution model is of the
// specified type. The native execution model is assumed in the absence of Change Execution
// Model records.

struct SYM_CEXMODEL16
{
    WORD reclen;   // Record length
    WORD rectyp;   // Record type (S_CEXMODEL16)
    WORD wOffset;  // Offset portion of start of the block.
    WORD wSegment; // Segment portion of the start of block.
    WORD wModel;   // The execution model.
    union
    {
        struct
        {
            WORD wFcnHeader; // Offset of the Pcode procedure's Function Header.
            WORD wSPI;       // Offset of the Pcode segment's Segment Pcode Information.
        } Pcode;
        struct
        {
            WORD  wSubType; // COBOL execution model subtype.
            WORD  wFlag;    // Flag
        } COBOL;
    } Variant;
};

struct SYM_CEXMODEL32
{
    WORD  wSymLength;   // Record length
    WORD  wSymType;     // Record type (S_CEXMODEL32).
    DWORD dwOffset;     // Offset portion of start of the block.
    WORD  wSegment;     // Segment portion of the start of block.
    WORD  wModel;       // The execution model.
    union
    {
        struct
        {
            DWORD dwFcnHeader; // Offset of the Pcode procedure's Function Header.
            DWORD dwSPI;       // Offset of the Pcode segment's Segment Pcode Information.
        } Pcode;
        struct
        {
            WORD  wSubType; // COBOL execution model subtype.
            WORD  wFlag;    // Flag
        } COBOL;
        struct
        {
            DWORD dwCallTableOffset; //
            WORD  wCallTableSegment; //
        } pcode32Mac;
    } Variant;
};

// -----------------------------------------------------------------
// Compile Flag.
//
// This symbol communicates with Microsoft debugger compile-time information, such as the
// language and version number of the language processor, the ambient model for code and data,
// and the target processor, on a per-module basis.

struct SYM_COMPILE_FLAG
{
    WORD wSymLength;             // Record length
    WORD wSymType;               // Record type (S_COMPILE)
    BYTE uchMachine;             // Enumeration specifying target processor.
    struct
    {                            // Flags showing compile-time options:
        BYTE Language        : 8;  // Language
        BYTE PCodePresent    : 1;  // PCode presents
        BYTE FloatPrecission : 2;  // Float precission (set to 1 if the compiler follows the ANSI C
        // floating-point precision rules)
        BYTE FloatPackage    : 2;  // Float package
        BYTE AmbientData     : 3;  // Ambient data
        BYTE AmbientCode     : 3;  // Ambient code
        BYTE Mode32          : 1;  // Compiled for 32-bit addresses
        BYTE Reserved        : 4;  // Reserved
    } Flags;
    BYTE uchVersion [1];         // Length-prefixed string specifying language processor version.
};

// Enumeration specifying target processor. Values not specified in the
// following list are reserved:
enum SCF_MACHINE
{
    SCFM_INTEL8080      = 0x00,   // Intel 8080
    SCFM_INTEL9086      = 0x01,   // Intel 8086
    SCFM_INTEL808286    = 0x02,   // Intel 80286
    SCFM_INTEL808386    = 0x03,   // Intel 80386
    SCFM_INTEL40486     = 0x04,   // Intel 80486
    SCFM_PENTIUM        = 0x05,   // Intel Pentium
    SCFM_MIPS           = 0x10,   // MIPS R4000
    SCFM_MIPS_RESERVED1 = 0x11,   // Reserved for future MIPS processor
    SCFM_MIPS_RESERVED2 = 0x12,   // Reserved for future MIPS processor
    SCFM_MC68000        = 0x20,   // MC68000
    SCFM_MC68010        = 0x21,   // MC68010
    SCFM_MC68020        = 0x22,   // MC68020
    SCFM_MC68030        = 0x23,   // MC68030
    SCFM_MC68040        = 0x24,   // MC68040
    SCFM_DEC            = 0x30,   // DEC Alpha
};

// Language enumerations
enum SCF_LANGUAGE
{
    SCFL_C              = 0,      // C
    SCFL_CPP            = 1,      // C++
    SCFL_FORTRAN        = 2,      // Fortran
    SCFL_MASM           = 3,      // Masm
    SCFL_PASCAL         = 4,      // Pascal
    SCFL_BASIC          = 5,      // Basic
    SCFL_COBOL          = 6,      // COBOL
};

// Ambient code and data memory model enumeration
enum SCF_AMBIENT
{
    SCFA_NEAR           = 0,      // Near
    SCFA_FAR            = 1,      // Far
    SCFA_HUGE           = 2,      // Huge
};

// Floating-package enumeration
enum SCF_FLOAT_PACKAGE
{
    SCFF_HARDWARE       = 0,      // Hardware processor (80x87 for Intel 80x86 processors)
    SCFF_EMULATOR       = 1,      // Emulator
    SCFF_ALTMATH        = 2,      // Altmath
};

//-----------------------------------------------------------------
// Compile
//
// This symbol communicates with Microsoft debugger compile-time information, such as the
// language and version number of the language processor, the ambient model for code and data,
// and the target processor, on a per-module basis.

struct SYM_COMPILE
{
    OMF_HEADER Header;                 // Header
    struct                             // Flags:
    {
        DWORD fLanguage       :  8;      // Language
        DWORD fEC             :  1;      // Edit and continue enabled
        DWORD fNoDbgInfo      :  1;      // No debug info presents
        DWORD fLTCG           :  1;      // Link-time code generation enabled
        DWORD fNoDataAlign    :  1;      // No data alignment presents
        DWORD fManagedPresent :  1;      // Managed code presents
        DWORD Reserved        : 19;      // Reserved space
    } Flags;
    WORD wMachine;                     // Machine type
    WORD wVerFEMajor;                  // FE version ??
    WORD wVerFEMinor;
    WORD wVerFEBuild;
    WORD wVerMajor;                    // Version ??
    WORD wVerMinor;
    WORD wVerBuild;
    BYTE uchVerStr[1];
};

// -----------------------------------------------------------------
//  Constant
//
// This record is used to output constants and C enumerations. If used to output an enumeration,
// then the type index refers to the containing enum.

typedef struct tagSYM_CONSTANT
{
    OMF_HEADER Header;      // Header
    DWORD dwTypeIndex;      // Type of symbol or containing enum.
    WORD  wValue;           // Numeric leaf containing the value of symbol.
    BYTE  uchName[1];       // Length-prefixed name of symbol.
}
SYM_CONSTANT, *PSYM_CONSTANT, **PPSYM_CONSTANT;

// 16-bit variant of constant symbol
typedef struct tagSYM_CONSTANT_16
{
    OMF_HEADER Header;     // Header
    WORD wTypeIndex;      // Type of symbol or containing enum.
    WORD wValue;          // Numeric leaf containing the value of symbol.
    BYTE uchName[1];      // Length-prefixed name of symbol.
}
SYM_CONSTANT_16, *PSYM_CONSTANT_16, **PPSYM_CONSTANT_16;

// -----------------------------------------------------------------
// Local Data and Global Data
//
// Local data used for data that is not exported from a module. In C and C++, symbols that
// are declared static are emitted as Local Data symbols. Symbols that are emitted as Local Data
// cannot be moved by the CVPACK utility into the global symbol table for the executable file.
//
// For C and C++, global data symbols that are not specifically declared static are emitted as
// Global Data Symbols and can be compacted by the CVPACK utility into the global symbol
// table.

struct SYM_DATA_16
{
    OMF_HEADER Header;     // Header
    WORD wOffset;          // Offset portion of symbol address.
    WORD wSegment;         // Segment portion of symbol address.
    WORD wTypeIndex;       // Type index of symbol.
    BYTE uchName[1];       // Length-prefixed name of symbol.
};

struct SYM_DATA32
{
    OMF_HEADER Header;     // Header
    DWORD dwTypeIndex;     // Symbol type index
    DWORD dwOffset;        // Offset portion of symbol address
    WORD  wSegment;        // Segment portion of symbol address.
    BYTE  uchName[1];      // Symbol name
};

// -----------------------------------------------------------------
// this at Method Entry
//
// This record is used to describe the this pointer at entry to a method. It is really a wrapper for
// another symbol that describes the this pointer.

struct SYM_ENTRYTHIS
{
    OMF_HEADER Header;     // Header
    BYTE uchThisSym[1];    // Full symbol, including length and symbol type fields, which describes
    // the this pointer.
};

#define ENTRYTHISSYM_ sizeof (ENTRYTHISSYM)

// -----------------------------------------------------------------
// Frame-based procedure.

struct SYM_FRAMEPROC
{
    OMF_HEADER Header;     // Header
    DWORD cbFrame;         // Frame size.
    DWORD cbPad;           // Count of padding bytes.
    DWORD dwPadOffset;     // Offset of padding bytes.
    DWORD cbSaveRegs;      // Count of bytes in saved registers.
    DWORD dwExHdlrOffs;    // Offset of ???
    WORD  wSectExHdlr;     // ???
    struct
    {
        DWORD fHasAlloca  :  1; // TRUE if the function uses allocated memory function.
        DWORD fHasSetJmp  :  1; // TRUE if the function contains a setjmp call
        DWORD fHasLongJmp :  1; // TRUE if the function contains a longjmp call
        DWORD fHasInlAsm  :  1; // TRUE if the function contains inline assembly
        DWORD fHasEH      :  1; // TRUE if the function contains C++-style exception handling.
        DWORD fInlSpec    :  1; // TRUE if the function has been marked with one of the inline, __inline, __forceinline attributes.
        DWORD fHasSEH     :  1; // TRUE if the function contains Win32-style structured exception handling
        DWORD pad         : 25; // Unused
    } Flags;
};

// -----------------------------------------------------------------

typedef struct _FRAMERELSYM
{
    WORD         reclen;   // record length
    WORD         rectyp;   // record type
    LONG         off;
    DWORD        typind;
    CV_lvar_attr attr;
    BYTE         name [1];
}
FRAMERELSYM, *PFRAMERELSYM, **PPFRAMERELSYM;

#define FRAMERELSYM_ sizeof (FRAMERELSYM)

// -----------------------------------------------------------------
// Code label.
//
// Specifies a location in program code.

struct SYM_LABEL16
{
    OMF_HEADER Header;     // Header
    WORD  wOffset;         // Offset portion of the code label address.
    WORD  wSegment;        // Segment portion of the code label address.
    CV_PROCFLAGS Flags;    // Label flags. This uses the same flag definition as in the S_LPROC16 symbol record.
    BYTE  uchName [1];     // Symbol name.
};

struct SYM_LABEL32
{
    OMF_HEADER Header;     // Header
    DWORD dwOffset;        // Offset portion of the code label address.
    WORD  wSegment;        // Segment portion of the code label address.
    CV_PROCFLAGS Flags;    // Label flags. This uses the same flag definition as in the S_LPROC16 symbol record.
    BYTE  uchName[1];      // Symbol name.
};

// -----------------------------------------------------------------

typedef struct _MANPROCSYM
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    DWORD pNext;
    DWORD len;
    DWORD DbgStart;
    DWORD DbgEnd;
    DWORD token;
    DWORD off;
    WORD  seg;
    CV_PROCFLAGS flags;
    WORD  retReg;
    BYTE  name [1];
}
MANPROCSYM, *PMANPROCSYM, **PPMANPROCSYM;

#define MANPROCSYM_ sizeof (MANPROCSYM)

// -----------------------------------------------------------------

typedef struct _MANPROCSYMMIPS
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    DWORD pNext;
    DWORD len;
    DWORD DbgStart;
    DWORD DbgEnd;
    DWORD regSave;
    DWORD fpSave;
    DWORD intOff;
    DWORD fpOff;
    DWORD token;
    DWORD off;
    WORD  seg;
    BYTE  retReg;
    BYTE  frameReg;
    BYTE  name [1];
}
MANPROCSYMMIPS, *PMANPROCSYMMIPS, **PPMANPROCSYMMIPS;

#define MANPROCSYMMIPS_ sizeof (MANPROCSYMMIPS)

// -----------------------------------------------------------------

typedef struct _MANTYPREF
{
    WORD  reclen; // record length
    WORD  rectyp; // record type
    DWORD typind;
}
MANTYPREF, *PMANTYPREF, **PPMANTYPREF;

#define MANTYPREF_ sizeof (MANTYPREF)

// -----------------------------------------------------------------
// Many Registers
//
// This record is used to specify that a symbol is stored in a set of registers.

typedef struct tagSYM_MANYREG
{
    OMF_HEADER Header;     // Header
    DWORD dwTypeIndex;     // Type index of the symbol.
    BYTE  uchCount;        // Count of the register enumerations that follow.
    BYTE  uchRegList[1];   // List of registers in which the symbol is stored. The registers are listed
    // high order register first.
    // Name of the symbol follows the reg list.
}
SYM_MANYREG, *PSYM_MANYREG, **PPSYM_MANYREG;

typedef struct tagSYM_MANYREG_16
{
    OMF_HEADER Header;     // Header
    WORD  wTypeIndex;      // Type index of the symbol.
    BYTE  uchCount;        // Count of the register enumerations that follow.
    BYTE  uchRegList[1];   // List of registers in which the symbol is stored. The registers are listed
    // high order register first.
    // Name of the symbol follows the reg list.
}
SYM_MANYREG_16, *PSYM_MANYREG_16, **PPSYM_MANYREG_16;

typedef struct tagSYM_MANYREG_32
{
    OMF_HEADER Header;     // Header
    DWORD dwTypeIndex;     // Type index of the symbol.
    WORD  wCount;          // Count of the register enumerations that follow.
    WORD  reg [1];
}
SYM_MANYREG_32, *PSYM_MANYREG_32, **PPSYM_MANYREG_32;

// -----------------------------------------------------------------
// Object File Name (S_OBJNAME)
//
// This symbol specifies the name of the object file for this module

struct SYM_OBJNAME
{
    OMF_HEADER Header;   // Symbol header
    DWORD dwSignature;   // Signature for the Microsoft symbol and type information contained in
    // this module.
    BYTE  uchName[1];    // Length-prefixed name of the object file without any path information
    // prepended to the name.
};

// -----------------------------------------------------------------
//

typedef struct _OEMSYMBOL
{
    WORD  reclen;     // record length
    WORD  rectyp;     // record type
    BYTE  idOem [16];
    DWORD typind;
    DWORD rgl [1];
}
OEMSYMBOL, *POEMSYMBOL, **PPOEMSYMBOL;

#define OEMSYMBOL_ sizeof (OEMSYMBOL)

// -----------------------------------------------------------------
// Local or global procedure

struct SYM_PROC16
{
    OMF_HEADER Header;     // Header
    DWORD pParent;         // Offset of lexical parent symbol.
    DWORD pEnd;            // Offset to end of scope symbol.
    DWORD pNext;           // Offset to next sibling.
    WORD  wLength;         // Length in bytes of this procedure.
    WORD  wDbgStart;       // Offset in bytes from the start of the procedure to the point where the
    // stack frame has been set up.
    WORD  wDbgEnd;         // Offset in bytes from the start of the procedure to the point where the
    // procedure is ready to return and has calculated its return value, if any.
    WORD  wOffset;         // Offset portion of the procedure address.
    WORD  wSegment;        // Segment portion of the procedure address.
    WORD  wTypeIndex;      // Type of the procedure type record.
    CV_PROCFLAGS Flags;    // Procedure flags.
    BYTE  uchName[1];      // Symbol name.
};

struct SYM_PROC32
{
    OMF_HEADER Header;     // Header
    DWORD pParent;      // Offset of lexical parent symbol.
    DWORD pEnd;         // Offset to end of scope symbol.
    DWORD pNext;        // Offset to next sibling.
    DWORD dwLength;     // Length in bytes of this procedure.
    DWORD dwDbgStart;   // Offset in bytes from the start of the procedure to the point where the
    // stack frame has been set up. Parameter and frame variables can be
    // viewed at this point.
    DWORD dwDbgEnd;     // Offset in bytes from the start of the procedure to the point where the
    // procedure is ready to return and has calculated its return value, if any.
    // Frame and register variables can still be viewed.
    DWORD dwTypeIndex;  // Type of the procedure type record.
    DWORD dwOffset;     // Offset portion of the procedure address.
    WORD  wSegment;     // Segment portion of the procedure address.
    CV_PROCFLAGS Flags; // Procedure flags.
    BYTE  uchName[1];   // Symbol name.
};

// -----------------------------------------------------------------

typedef struct _PROCSYM32_16t
{
    WORD         reclen;   // record length
    WORD         rectyp;   // record type
    DWORD        pParent;
    DWORD        pEnd;
    DWORD        pNext;
    DWORD        len;
    DWORD        DbgStart;
    DWORD        DbgEnd;
    DWORD        off;
    WORD         seg;
    WORD         typind;
    CV_PROCFLAGS flags;
    BYTE         name [1];
}
PROCSYM32_16t, *PPROCSYM32_16t, **PPPROCSYM32_16t;

#define PROCSYM32_16t_ sizeof (PROCSYM32_16t)

// -----------------------------------------------------------------

typedef struct _PROCSYMIA64
{
    WORD         reclen;   // record length
    WORD         rectyp;   // record type
    DWORD        pParent;
    DWORD        pEnd;
    DWORD        pNext;
    DWORD        len;
    DWORD        DbgStart;
    DWORD        DbgEnd;
    DWORD        typind;
    DWORD        off;
    WORD         seg;
    WORD         retReg;
    CV_PROCFLAGS flags;
    BYTE         name [1];
}
PROCSYMIA64, *PPROCSYMIA64, **PPPROCSYMIA64;

#define PROCSYMIA64_ sizeof (PROCSYMIA64)

// -----------------------------------------------------------------

typedef struct _PROCSYMMIPS
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    DWORD pNext;
    DWORD len;
    DWORD DbgStart;
    DWORD DbgEnd;
    DWORD regSave;
    DWORD fpSave;
    DWORD intOff;
    DWORD fpOff;
    DWORD typinf;
    DWORD off;
    WORD  seg;
    BYTE  retReg;
    BYTE  frameReg;
    BYTE  name [1];
}
PROCSYMMIPS, *PPROCSYMMIPS, **PPPROCSYMMIPS;

#define PROCSYMMIPS_ sizeof (PROCSYMMIPS)

// -----------------------------------------------------------------

typedef struct _PROCSYMMIPS_16t
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    DWORD pNext;
    DWORD len;
    DWORD DbgStart;
    DWORD DbgEnd;
    DWORD regSave;
    DWORD fpSave;
    DWORD intOff;
    DWORD fpOff;
    DWORD off;
    WORD  seg;
    WORD  typind;
    BYTE  retReg;
    BYTE  frameReg;
    BYTE  name [1];
}
PROCSYMMIPS_16t, *PPROCSYMMIPS_16t, **PPPROCSYMMIPS_16t;

#define PROCSYMMIPS_16t_ sizeof (PROCSYMMIPS_16t)

// -----------------------------------------------------------------
// Procedure Reference
//
// This symbol is inserted into the global and static symbol tables to reference a procedure. It is
// used so that the symbol procedure can be found in the hashed search of the global or static
// symbol table. Otherwise, procedures could be found only by searching the symbol table for
// every module.

typedef struct tagSYM_PROCREF
{
    OMF_HEADER Header;     // Header
    DWORD dwChecSum;        // Checksum of the referenced symbol name. The checksum used is the
    // one specified in the header of the sstGlobalSym or sstStaticSym
    // subsections.
    DWORD dwOffset;         // Offset of the procedure symbol record from the beginning of the
    // $$SYMBOL table for the module.
    WORD  wModuleIndex;     // Index of the module that contains this procedure record.
}
SYM_PROCREF, *PSYM_PROCREF, **PPSYM_PROCREF;

// -----------------------------------------------------------------
// Public Symbol
//
// This symbol has the same format as the Local Data symbol. Its use is reserved for
// symbols in the public table that is emitted by the linker into the Symbol and Type OMF portion
// of the executable file.

struct SYM_PUB32
{
    OMF_HEADER Header;     // Header
    CV_PUBSYMFLAGS Flags;  // Flags
    DWORD dwOffset;        // Offset portion of symbol address.
    WORD  wSegment;        // Segment portion of symbol address.
    BYTE  uchName[1];      // Symbol name.
};

// -----------------------------------------------------------------
//

struct SYM_REF
{
    OMF_HEADER Header;     // Header
    DWORD dwSymName;
    DWORD ibSym;
    WORD  imod;
    WORD  usFill;
};

// -----------------------------------------------------------------

typedef struct _REFSYM2
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD sumName;
    DWORD ibSym;
    WORD  imod;
    BYTE  name [1];
}
REFSYM2, *PREFSYM2, **PPREFSYM2;

#define REFSYM2_ sizeof (REFSYM2)

// -----------------------------------------------------------------

typedef struct _REGREL16
{
    WORD reclen;   // record length
    WORD rectyp;   // record type
    WORD off;
    WORD reg;
    WORD typind;
    BYTE name [1];
}
REGREL16, *PREGREL16, **PPREGREL16;

#define REGREL16_ sizeof (REGREL16)

// -----------------------------------------------------------------

typedef struct _REGREL32_16t
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD off;
    WORD  reg;
    WORD  typind;
    BYTE  name [1];
}
REGREL32_16t, *PREGREL32_16t, **PPREGREL32_16t;

#define REGREL32_16t_ sizeof (REGREL32_16t)

// -----------------------------------------------------------------

typedef struct _REGREL32
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD off;
    DWORD typind;
    WORD  reg;
    BYTE  name [1];
}
REGREL32, *PREGREL32, **PPREGREL32;

#define REGREL32_ sizeof (REGREL32)

// -----------------------------------------------------------------
// Register symbol
//
// This symbol record describes a symbol that has been placed in a register.

typedef struct tagSYM_REGISTER
{
    WORD  wSymLength;       // Record length
    WORD  wSymType;         // Record type
    DWORD dwTypeIndex;      // Type index ???
    WORD  wRegister;        // Enumeration of the registers in which the symbol value is stored.
    // This field is treated as two bytes. The high order byte specifies the
    // register in which the high order part of the value is stored. The low
    // byte specifies the register for the low order part of the value.
    BYTE  uchName[1];       // Length-prefixed name of the symbol stored in the register.
    // Name is followed by register-tracking information. Format unspecified.
}
SYM_REGISTER, *PSYM_REGISTER, **PPSYM_REGISTER;

// 16-bit variant
typedef struct tagSYM_REGISTER_16
{
    WORD reclen; // record length
    WORD rectyp; // record type
    WORD typind;
    WORD reg;
    BYTE name [1];
}
SYM_REGISTER_16, *PSYM_REGISTER_16, **PPSYM_REGISTER_16;

#define REGSYM_16t_ sizeof (REGSYM_16t)

// -----------------------------------------------------------------
// Function Return
//
// This symbol is used to describe how a function is called, how the return value, if any, is
// returned, and how the stack is cleaned up.

typedef struct tagSYM_RETURN
{
    WORD wSymLength;       // Record length
    WORD wSymTyp;          // Record type (S_RETURN)
    CV_GENERIC_FLAG Flags; // Flags for function call
    BYTE uchStyle;         // Function return style
    // * Data required by function return style follow.
}
SYM_RETURN, *PSYM_RETURN, **PPSYM_RETURN;

// -----------------------------------------------------------------
// Start Search
// These records are always the first symbol records in a module's $$SYMBOL section. There is
// one Start Search symbol for each segment (PE section) to which the module contributes code.
// Each Start Search symbol contains the segment (PE section) number and $$SYMBOL offset of
// the record of the outermost lexical scope in this module that physically appears first in the
// specified segment of the load image. This referenced symbol is the symbol used to initiate
// context searches within this module.

typedef struct tagSYM_START_SEARCH
{
    WORD  wSymLength;   // Record length
    WORD  wSymType;     // Record type
    DWORD dwSymOffs;    // $$SYMBOL offset of the procedure or thunk record for this module
    // that has the lowest offset for the specified segment.
    WORD  wSegment;     // Segment (PE section) to which this Start Search refers.
}
SYM_START_SEARCH, *PSYM_START_SEARCH, **PPSYM_START_SEARCH;

// -----------------------------------------------------------------

typedef struct _SLINK32
{
    WORD  reclen;    // record length
    WORD  rectyp;    // record type
    DWORD framesize;
    LONG  off;
    WORD  reg;
}
SLINK32, *PSLINK32, **PPSLINK32;

#define SLINK32_ sizeof (SLINK32)

// -----------------------------------------------------------------

typedef struct _SLOTSYM32
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD iSlot;
    DWORD typind;
    BYTE  name [1];
}
SLOTSYM32, *PSLOTSYM32, **PPSLOTSYM32;

#define SLOTSYM32_ sizeof (SLOTSYM32)

// -----------------------------------------------------------------

typedef struct _SYMTYPE
{
    WORD reclen;   // record length
    WORD rectyp;   // record type
    BYTE data [1];
}
SYMTYPE, *PSYMTYPE, **PPSYMTYPE;

#define SYMTYPE_ sizeof (SYMTYPE)

// -----------------------------------------------------------------

typedef struct _THREADSYM32_16t
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD off;
    WORD  seg;
    WORD  typind;
    BYTE  name [1];
}
THREADSYM32_16t, *PTHREADSYM32_16t, **PPTHREADSYM32_16t;

#define THREADSYM32_16t_ sizeof (THREADSYM32_16t)

// -----------------------------------------------------------------

typedef struct _THUNKSYM
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    DWORD pNext;
}
THUNKSYM, *PTHUNKSYM, **PPTHUNKSYM;

#define THUNKSYM_ sizeof (THUNKSYM)

// -----------------------------------------------------------------

typedef struct _THUNKSYM16
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    DWORD pNext;
    WORD  off;
    WORD  seg;
    WORD  len;
    BYTE  ord;
    BYTE  name    [1];
    BYTE  variant [1];
}
THUNKSYM16, *PTHUNKSYM16, **PPTHUNKSYM16;

#define THUNKSYM16_ sizeof (THUNKSYM16)

// -----------------------------------------------------------------
// Thunk start
//
// This symbol is used to specify any piece of code that exists outside of a procedure. The lexical
// scope started by the Thunk Start symbol is closed by a matching End record.

struct SYM_THUNK32
{
    OMF_HEADER Header;    // Header
    DWORD pParent;        //
    DWORD pEnd;           //
    DWORD pNext;          //
    DWORD dwOffset;       // Offset portion of the thunk address.
    WORD  wSegment;       // Segment portion of the thunk address.
    WORD  wLength;        // Length in bytes of this thunk.
    BYTE  uchOrdinal;     // Ordinal specifying the type of thunk
    BYTE  uchName[1];     // Thunk name
    // variant            // Variant field, depending on the value of ordinal. If ordinal is
    // NOTYPE, there will be no variant field. If ordinal is ADJUSTOR, the
    // variant field will be a two-byte signed value specifying the delta to be
    // added to the this pointer, followed by the name of the target function.
    // If the ordinal is VCALL, then the variant field will be a 2-byte signed
    // displacement into the virtual table. Note that because of the variable
    // length name, the data in the variant field will not be in natural
    // alignment. If ordinal is PCODE, the variant is the segment:offset of the
    // pcode entry point.
};

// -----------------------------------------------------------------
// Trampoline
//
//

struct SYM_TRAMPOLINE
{
    OMF_HEADER Header;
    WORD  wTrampType;
    WORD  wThunkSize;
    DWORD dwThunkOffs;
    DWORD dwTargetOffs;
    WORD  wThunkSection;
    WORD  wTargetSection;
};

// -----------------------------------------------------------------
// User-defined Type (UDT)
// This specifies a C typedef or user-defined type, such as classes,
// structures, unions, or enums.

typedef struct tagSYM_UDT
{
    WORD  wSymLength;     // Record length
    WORD  wSymType;       // Record type (S_UDT)
    DWORD dwTypeIndex;    // Type of symbol.
    BYTE  uchName[1];     // Length-prefixed name of the user defined type.
}
SYM_UDT, *PSYM_UDT, **PPSYM_UDT;

// 16-bit variant of UDT symbol
typedef struct tagSYM_UDT_16
{
    WORD wSymLen;          // Record length
    WORD wSymType;         // Record type
    WORD wTypeIndex;       // Type of symbol.
    BYTE uchName [1];      // Length-prefixed name of the user defined type.
}
SYM_UDT_16, *PSYM_UDT_16, **PPSYM_UDT_16;

// -----------------------------------------------------------------

typedef struct _UNAMESPACE
{
    WORD reclen;   // record length
    WORD rectyp;   // record type
    BYTE name [1];
}
UNAMESPACE, *PUNAMESPACE, **PPUNAMESPACE;

#define UNAMESPACE_ sizeof (UNAMESPACE)

// -----------------------------------------------------------------

typedef struct _VPATHSYM16
{
    WORD reclen; // record length
    WORD rectyp; // record type
    WORD off;
    WORD seg;
    WORD root;
    WORD path;
}
VPATHSYM16, *PVPATHSYM16, **PPVPATHSYM16;

#define VPATHSYM16_ sizeof (VPATHSYM16)

// -----------------------------------------------------------------

typedef struct _VPATHSYM32
{
    WORD  reclen; // record length
    WORD  rectyp; // record type
    DWORD root;
    DWORD path;
    DWORD off;
    WORD  seg;
}
VPATHSYM32, *PVPATHSYM32, **PPVPATHSYM32;

#define VPATHSYM32_ sizeof (VPATHSYM32)

// -----------------------------------------------------------------

typedef struct _VPATHSYM32_16t
{
    WORD  reclen;    // record length
    WORD  rectyp;    // record type
    DWORD off;
    WORD  seg;
    WORD  root;
    WORD  path;
}
VPATHSYM32_16t, *PVPATHSYM32_16t, **PPVPATHSYM32_16t;

#define VPATHSYM32_16t_ sizeof (VPATHSYM32_16t)

// -----------------------------------------------------------------

typedef struct _WITHSYM16
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    WORD  len;
    WORD  off;
    WORD  seg;
    BYTE  expr [1];
}
WITHSYM16, *PWITHSYM16, **PPWITHSYM16;

#define WITHSYM16_ sizeof (WITHSYM16)

// -----------------------------------------------------------------

typedef struct _WITHSYM32
{
    WORD  reclen;   // record length
    WORD  rectyp;   // record type
    DWORD pParent;
    DWORD pEnd;
    DWORD len;
    DWORD off;
    WORD  seg;
    BYTE  expr [1];
}
WITHSYM32, *PWITHSYM32, **PPWITHSYM32;

#define WITHSYM32_ sizeof (WITHSYM32)

// -----------------------------------------------------------------

typedef struct _OMAP_DATA
{
    DWORD rva;   // relative virtual address
    DWORD rvaTo; // converted relative virtual address
}
OMAP_DATA, *POMAP_DATA, **PPOMAP_DATA;

#define OMAP_DATA_ sizeof (OMAP_DATA)

// -----------------------------------------------------------------

#pragma pack ()

// Variant symbol
typedef union tagSYM_VARIANT
{
    SYM_ALIGN Align;
    SYM_ANNOTATION Annotation;
    SYM_ATTRMANYREG AttrManyReg;
    SYM_ATTRMANYREG2 AttrManyReg2;
    ATTRREGREL AttrRegRel;
    ATTRREGSYM AttrReg;
    ATTRSLOTSYM AttrSlot;
    SYM_BLOCK Block;
    SYM_BLOCK16 Block16;
    SYM_BLOCK32 Block32;
    SYM_BPREL_16 BpRelative16;
    SYM_BPREL_32 BpRelative32;
    SYM_BPREL_32_16 BpRelative32_16;
    SYM_CEXMODEL16 CexModel16;
    SYM_CEXMODEL32 CexModel32;
    SYM_COMPILE_FLAG CompileFlag;
    SYM_COMPILE Compile;
    SYM_CONSTANT Constant;
    SYM_CONSTANT_16 Constant_16;
    SYM_DATA_16 Data16;
    SYM_DATA32 Data32;
    SYM_ENTRYTHIS EntryThis;
    SYM_FRAMEPROC FrameProc;
    FRAMERELSYM FrameRel;
    SYM_LABEL16 Label16;
    SYM_LABEL32 Label32;
    MANPROCSYM ManProc;
    MANPROCSYMMIPS ManProcMips;
    MANTYPREF ManTypRef;
    SYM_MANYREG_16 ManyReg_16t;
    SYM_MANYREG ManyReg;
    SYM_MANYREG_32 ManyReg32;
    SYM_OBJNAME ObjFileName;
    OEMSYMBOL Oem;
    SYM_PROC16 Proc16;
    SYM_PROC32 Proc32;
    PROCSYM32_16t Proc32_16t;
    PROCSYMIA64 ProcIA64;
    PROCSYMMIPS ProcMips;
    PROCSYMMIPS_16t ProcMips_16t;
    SYM_PROCREF ProcRef;
    SYM_PUB32 Pub32;
    SYM_REF Ref;
    REFSYM2 Ref2;
    REGREL16 RegRel16;
    REGREL32_16t RegRel32_16t;
    REGREL32 RegRel32;
    SYM_REGISTER Register;
    SYM_REGISTER_16 Register_16;
    SYM_RETURN Return;
    SYM_START_SEARCH StartSearch;
    SLINK32 Slink32;
    SLOTSYM32 Slot32;
    SYMTYPE SymType;
    THREADSYM32_16t Thread_16t;
    THUNKSYM Thunk;
    THUNKSYM16 Thunk16;
    SYM_THUNK32 Thunk32;
    SYM_TRAMPOLINE Trampoline;
    SYM_UDT Udt;
    SYM_UDT_16 Udt_16;
    UNAMESPACE UNameSpace;
    VPATHSYM16 VPath16;
    VPATHSYM32 VPath32;
    VPATHSYM32_16t VPath32_16t;
    WITHSYM16 With16;
    WITHSYM32 With32;
} SYM_VARIANT, *PSYM_VARIANT;

