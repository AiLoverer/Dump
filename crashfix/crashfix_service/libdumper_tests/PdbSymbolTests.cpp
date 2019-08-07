#include "stdafx.h"
#include "Tests.h"
#include "PdbSymbol.h"
#include "Utils.h"
#include "strconv.h"
#include "Misc.h"

class PdbSymbolTests : public CTestCase
{
    BEGIN_TEST_MAP(PdbSymbolTests, "CPdbSymbol class tests")
		REGISTER_TEST(Test_UndecorateSymbolName3);
		REGISTER_TEST(Test_UndecorateSymbolName2);
        REGISTER_TEST(Test_UndecorateSymbolName);
    END_TEST_MAP()

public:

    void SetUp();
    void TearDown();

    void Test_UndecorateSymbolName();
	void Test_UndecorateSymbolName2();
	void Test_UndecorateSymbolName3();
};


REGISTER_TEST_CASE( PdbSymbolTests );

void PdbSymbolTests::SetUp()
{
}

void PdbSymbolTests::TearDown()
{
}

struct _NamePair
{
    std::string sDec;
    std::string sUnd;
};

void PdbSymbolTests::Test_UndecorateSymbolName()
{
    _NamePair p;
    std::vector<_NamePair> aSymNames;

	p.sDec = "?AddProperty@CCrashHandler@@QEAAHVCString@WTL@@0@Z";
	p.sUnd = "public: int __cdecl CCrashHandler::AddProperty(class WTL::CString,class WTL::CString)";
    aSymNames.push_back(p);

	p.sDec = "??_V@YAXPEAX@Z";
    p.sUnd = "void __cdecl operator delete[](void *)";
    aSymNames.push_back(p);

	p.sDec = "??HWTL@@YA?AVCString@0@AEBV10@PEB_W@Z";
    p.sUnd = "class WTL::CString __cdecl WTL::operator+(class WTL::CString const &,wchar_t const *)";
    aSymNames.push_back(p);

	p.sDec = "?floatfield@?$_Iosb@H@std@@2W4_Fmtflags@12@B";
	p.sUnd = "public: static enum std::_Iosb<int>::_Fmtflags const std::_Iosb<int>::floatfield";
    aSymNames.push_back(p);

	p.sDec = "??0bad_alloc@std@@QEAA@PEBD@Z";
    p.sUnd = "public: __cdecl std::bad_alloc::bad_alloc(char const *)";
    aSymNames.push_back(p);

	p.sDec = "?_Min@?$AtlLimits@_J@ATL@@2_JB";
    p.sUnd = "public: static __int64 const ATL::AtlLimits<__int64>::_Min";
	aSymNames.push_back(p);

	p.sDec = "?InvalidParameterHandler@CCrashHandler@@SAXPEB_W00I_K@Z";
    p.sUnd = "public: static void __cdecl CCrashHandler::InvalidParameterHandler(wchar_t const *,wchar_t const *,wchar_t const *,unsigned int,unsigned __int64)";
    aSymNames.push_back(p);

	p.sDec = "?s_pfnPrevHook@CNoUIAssertHook@ATL@@0P6AHHPADPAH@ZA";
    p.sUnd = "private: static int (__cdecl * ATL::CNoUIAssertHook::s_pfnPrevHook)(int,char *,int *)";
	aSymNames.push_back(p);

	p.sDec = "?CrashThread@@YGKPAX@Z";
    p.sUnd = "unsigned long __stdcall CrashThread(void *)";
	aSymNames.push_back(p);

	p.sDec = "?DialogProc@?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@SGHPAUHWND__@@IIJ@Z";
    p.sUnd = "public: static int __stdcall ATL::CDialogImplBaseT<class ATL::CWindow>::DialogProc(struct HWND__ *,unsigned int,unsigned int,long)";
	aSymNames.push_back(p);

	p.sDec = "?ProcessWindowMessage@CMainDlg@@UAEHPAUHWND__@@IIJAAJK@Z";
    p.sUnd = "public: virtual int __thiscall CMainDlg::ProcessWindowMessage(struct HWND__ *,unsigned int,unsigned int,long,long &,unsigned long)";
	aSymNames.push_back(p);

	p.sDec = "?OnOK@CMainDlg@@QAEJGGPAUHWND__@@AAH@Z";
    p.sUnd = "public: long __thiscall CMainDlg::OnOK(unsigned short,unsigned short,struct HWND__ *,int &)";
    aSymNames.push_back(p);

	p.sDec = "?DoCrash@CMainDlg@@QAEXXZ";
    p.sUnd = "public: void __thiscall CMainDlg::DoCrash(void)";
    aSymNames.push_back(p);

	p.sDec = "_WinMain@16";
    p.sUnd = "__stdcall WinMain()";
    aSymNames.push_back(p);

	p.sDec = "?Init@CComCriticalSection@ATL@@QEAAJXZ";
    p.sUnd = "public: long __cdecl ATL::CComCriticalSection::Init(void)";
    aSymNames.push_back(p);

	p.sDec = "?Open@CAtlAllocator@@QEAA_NPEBD@Z";
    p.sUnd = "public: bool __cdecl CAtlAllocator::Open(char const *)";
    aSymNames.push_back(p);

	p.sDec = "?IncRef@CAtlTraceProcess@@QEAAIXZ";
    p.sUnd = "public: unsigned int __cdecl CAtlTraceProcess::IncRef(void)";
    aSymNames.push_back(p);

	p.sDec = "?GetBuffer@CString@WTL@@QEAAPEA_WH@Z";
    p.sUnd = "public: wchar_t * __cdecl WTL::CString::GetBuffer(int)";
    aSymNames.push_back(p);

	p.sDec = "?_Grow_to@?$vector@PEAXV?$allocator@PEAX@std@@@std@@IEBA_K_K@Z";
    p.sUnd = "protected: unsigned __int64 __cdecl std::vector<void *,class std::allocator<void *>>::_Grow_to(unsigned __int64)const";
    aSymNames.push_back(p);

	p.sDec = "?rgszNeverDelete@CRegParser@ATL@@1QBQEB_WB";
    p.sUnd = "protected: static wchar_t const * const * const ATL::CRegParser::rgszNeverDelete";
    aSymNames.push_back(p);

	p.sDec = "?AllocBeforeWrite@CString@WTL@@IEAAHH@Z";
    p.sUnd = "protected: int __cdecl WTL::CString::AllocBeforeWrite(int)";
    aSymNames.push_back(p);

	p.sDec = "?_Min@?$AtlLimits@K@ATL@@2KB";
	p.sUnd = "public: static unsigned long const ATL::AtlLimits<unsigned long>::_Min";
    aSymNames.push_back(p);

	p.sDec = "?VT@?$CVarTypeInfo@PEAPEAUIUnknown@@@ATL@@2GB";
	p.sUnd = "public: static unsigned short const ATL::CVarTypeInfo<struct IUnknown * *>::VT";
    aSymNames.push_back(p);

    p.sDec = "?_Min@?$AtlLimits@I@ATL@@2IB";
	p.sUnd = "public: static unsigned int const ATL::AtlLimits<unsigned int>::_Min";
    aSymNames.push_back(p);

    p.sDec = "?ATL_CREATE_OBJECT@CComApartment@ATL@@2IA";
    p.sUnd = "public: static unsigned int ATL::CComApartment::ATL_CREATE_OBJECT";
    aSymNames.push_back(p);

    p.sDec = "?w2a@strconv_t@@QEAAPEBDPEB_W@Z";
    p.sUnd = "public: char const * __cdecl strconv_t::w2a(wchar_t const *)";
    aSymNames.push_back(p);

    p.sDec = "__imp_wcsncpy_s";
    p.sUnd = "import: wcsncpy_s";
    aSymNames.push_back(p);

    p.sDec = "__imp_??1_Lockit@std@@QEAA@XZ";
    p.sUnd = "import: public: __cdecl std::_Lockit::~_Lockit(void)";
    aSymNames.push_back(p);

    p.sDec = "?RecurseAlloc@@YAHXZ";
    p.sUnd = "int __cdecl RecurseAlloc(void)";
    aSymNames.push_back(p);

    p.sDec = "??0CCritSec@@QEAA@XZ";
    p.sUnd = "public: __cdecl CCritSec::CCritSec(void)";
    aSymNames.push_back(p);

    p.sDec = "??1_Container_base12@std@@QEAA@XZ";
    p.sUnd = "public: __cdecl std::_Container_base12::~_Container_base12(void)";
    aSymNames.push_back(p);

    p.sDec = "??1CAtlAllocator@@QEAA@XZ";
    p.sUnd = "public: __cdecl CAtlAllocator::~CAtlAllocator(void)";
    aSymNames.push_back(p);

	p.sDec = "??1CCritSec@@QEAA@XZ";
	p.sUnd = "public: __cdecl CCritSec::~CCritSec(void)";
    aSymNames.push_back(p);

	p.sDec = "?GetFileExtension@Utility@@YA?AVCString@WTL@@V23@@Z";
	p.sUnd = "class WTL::CString __cdecl Utility::GetFileExtension(class WTL::CString)";
    aSymNames.push_back(p);

	p.sDec = "?GetName@CSharedMem@@QEAA?AVCString@WTL@@XZ";
    p.sUnd = "public: class WTL::CString __cdecl CSharedMem::GetName(void)";
    aSymNames.push_back(p);

	p.sDec = "AtlTraceVA";
    p.sUnd = "AtlTraceVA";
    aSymNames.push_back(p);

    p.sDec = "?LaunchCrashSender@CCrashHandler@@QEAAHVCString@WTL@@HPEAPEAX@Z";
    p.sUnd = "public: int __cdecl CCrashHandler::LaunchCrashSender(class WTL::CString,int,void * *)";
    aSymNames.push_back(p);

    size_t i;
    for(i=0; i<aSymNames.size(); i++)
    {
        std::string sDecoratedName = aSymNames[i].sDec;
        std::string sCorrectUndecoratedName = aSymNames[i].sUnd;
        std::string sUndecoratedName;
        bool bUndecorate = CPdbSymbol::UndecorateSymbolName(sDecoratedName, sUndecoratedName, false);
		TEST_ASSERT_MSG(bUndecorate, "Error undecorating symbol: %s", sDecoratedName.c_str());
        TEST_ASSERT_MSG(sUndecoratedName == sCorrectUndecoratedName, "Undecorated name is incorrect: %s", sDecoratedName.c_str());
    }

    __TEST_CLEANUP__;
}

void PdbSymbolTests::Test_UndecorateSymbolName2()
{
	struct _SymNames
	{
		const char* szDecorated;
		const char* szUndecorated;
	};

	_SymNames aSymNames[] =
	{
		{"?Create@CCurrentGradingSettingsInterior@@CAPAV1@I@Z", "private: static class CCurrentGradingSettingsInterior * __cdecl CCurrentGradingSettingsInterior::Create(unsigned int)"},
		{"?KeyCommandSet@CMapKey2Cmd@CExtCustomizeSite@@QAEXABVCCmdKeyInfo@2@I@Z", "public: void __thiscall CExtCustomizeSite::CMapKey2Cmd::KeyCommandSet(class CExtCustomizeSite::CCmdKeyInfo const &,unsigned int)"},
		{"??0CExtGridCellVariant@@QAE@NGPAVCExtGridDataProvider@@@Z", "public: __thiscall CExtGridCellVariant::CExtGridCellVariant(double,unsigned short,class CExtGridDataProvider *)"},
		{"?ApplyParams@CMDIClientAreaWnd@@AAEXPAVCMFCTabCtrl@@@Z", "private: void __thiscall CMDIClientAreaWnd::ApplyParams(class CMFCTabCtrl *)"},
		{"?EnableSnapToStep@CEFloatSpinButtonCtrl@@QAE_N_N@Z", "public: bool __thiscall CEFloatSpinButtonCtrl::EnableSnapToStep(bool)"},
		{"?UpdateAllCommandItems@CExtCustomizeSite@@UBEHXZ", "public: virtual int __thiscall CExtCustomizeSite::UpdateAllCommandItems(void)const"},
		{"??0XElementButtonGallery@CMFCRibbonInfo@@QAE@XZ", "public: __thiscall CMFCRibbonInfo::XElementButtonGallery::XElementButtonGallery(void)"},
		{"??0CEPointChain3DPool@EPointChain3D@@QAE@IPAX@Z", "public: __thiscall EPointChain3D::CEPointChain3DPool::CEPointChain3DPool(unsigned int,void *)"},
		{"?OnEraseBkgnd@CListCtrlEx@@IAEHPAVCDC@@@Z", "protected: int __thiscall CListCtrlEx::OnEraseBkgnd(class CDC *)"},
		{"?OnOKFromTab@CTabContainer@@MAEXXZ", "protected: virtual void __thiscall CTabContainer::OnOKFromTab(void)"},
		{"?_OnClick@CExtButton@@UAEX_N0@Z", "public: virtual void __thiscall CExtButton::_OnClick(bool,bool)"},
		{"?OnInitDialog@CAboutInterior@@MAEHXZ", "protected: virtual int __thiscall CAboutInterior::OnInitDialog(void)"},
		{"?write@CLog@@QAAXHPB_WZZ", "public: void __cdecl CLog::write(int,wchar_t const *,...)"},
		{"?find@?$_Tree@V?$_Tmap_traits@VCPlayListItemHandle@@HU?$less@VCPlayListItemHandle@@@std@@V?$allocator@U?$pair@$$CBVCPlayListItemHandle@@H@std@@@3@$0A@@std@@@std@@QAE?AViterator@12@ABVCPlayListItemHandle@@@Z", "public: class std::_Tree<class std::_Tmap_traits<class CPlayListItemHandle,int,struct std::less<class CPlayListItemHandle>,class std::allocator<struct std::pair<class CPlayListItemHandle const,int>>,0>>::iterator __thiscall std::_Tree<class std::_Tmap_traits<class CPlayListItemHandle,int,struct std::less<class CPlayListItemHandle>,class std::allocator<struct std::pair<class CPlayListItemHandle const,int>>,0>>::find(class CPlayListItemHandle const &)"},
		{"?lower_bound@?$_Tree@V?$_Tmap_traits@VCPlayListItemHandle@@HU?$less@VCPlayListItemHandle@@@std@@V?$allocator@U?$pair@$$CBVCPlayListItemHandle@@H@std@@@3@$0A@@std@@@std@@QAE?AViterator@12@ABVCPlayListItemHandle@@@Z", "public: class std::_Tree<class std::_Tmap_traits<class CPlayListItemHandle,int,struct std::less<class CPlayListItemHandle>,class std::allocator<struct std::pair<class CPlayListItemHandle const,int>>,0>>::iterator __thiscall std::_Tree<class std::_Tmap_traits<class CPlayListItemHandle,int,struct std::less<class CPlayListItemHandle>,class std::allocator<struct std::pair<class CPlayListItemHandle const,int>>,0>>::lower_bound(class CPlayListItemHandle const &)"},
		{"?_Lbound@?$_Tree@V?$_Tset_traits@V?$CStringT@_WV?$StrTraitATL@_WV?$ChTraitsCRT@_W@ATL@@@ATL@@@ATL@@U?$less@V?$CStringT@_WV?$StrTraitATL@_WV?$ChTraitsCRT@_W@ATL@@@ATL@@@ATL@@@std@@V?$allocator@V?$CStringT@_WV?$StrTraitATL@_WV?$ChTraitsCRT@_W@ATL@@@ATL@@@ATL@@@4@$0A@@std@@@std@@IBEPAU_Node@?$_Tree_nod@V?$_Tset_traits@V?$CStringT@_WV?$StrTraitATL@_WV?$ChTraitsCRT@_W@ATL@@@ATL@@@ATL@@U?$less@V?$CStringT@_WV?$StrTraitATL@_WV?$ChTraitsCRT@_W@ATL@@@ATL@@@ATL@@@std@@V?$allocator@V?$CStringT@_WV?$StrTraitATL@_WV?$ChTraitsCRT@_W@ATL@@@ATL@@@ATL@@@4@$0A@@std@@@2@ABV?$CStringT@_WV?$StrTraitATL@_WV?$ChTraitsCRT@_W@ATL@@@ATL@@@ATL@@@Z", "protected: struct std::_Tree_nod<class std::_Tset_traits<class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t>>>,struct std::less<class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t>>>>,class std::allocator<class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t>>>>,0>>::_Node * __thiscall std::_Tree<class std::_Tset_traits<class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t>>>,struct std::less<class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t>>>>,class std::allocator<class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t>>>>,0>>::_Lbound(class ATL::CStringT<wchar_t,class ATL::StrTraitATL<wchar_t,class ATL::ChTraitsCRT<wchar_t>>> const &)const"},
		{"?ThreadProc@CGraphMgr@@EAEKXZ", "private: virtual unsigned long __thiscall CGraphMgr::ThreadProc(void)"},
		{"?StartWaitingThread@CVideoDriver@@CGKPAX@Z", "private: static unsigned long __stdcall CVideoDriver::StartWaitingThread(void *)"},
		{"?_Min@?$AtlLimits@_K@ATL@@2_KB", "public: static unsigned __int64 const ATL::AtlLimits<unsigned __int64>::_Min"},
		{"?InvalidateRect@CWindow@ATL@@QAEHPBUtagRECT@@H@Z", "public: int __thiscall ATL::CWindow::InvalidateRect(struct tagRECT const *,int)"},
		{"?GetHInstanceAt@CAtlBaseModule@ATL@@QAEPAUHINSTANCE__@@H@Z", "public: struct HINSTANCE__ * __thiscall ATL::CAtlBaseModule::GetHInstanceAt(int)"},
		{"?VT@?$CVarTypeInfo@TtagCY@@@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<union tagCY>::VT"},
		{"?SkipWhiteSpace@CRegParser@ATL@@IAEXXZ", "protected: void __thiscall ATL::CRegParser::SkipWhiteSpace(void)"},
		{"??_G?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAEPAXI@Z", "public: void * __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::`scalar deleting destructor'(unsigned int)"},
		{"?CenterWindow@CWindow@ATL@@QAEHPAUHWND__@@@Z", "public: int __thiscall ATL::CWindow::CenterWindow(struct HWND__ *)"},
		{"??$AtlAdd@K@ATL@@YAJPAKKK@Z", "long __cdecl ATL::AtlAdd<unsigned long>(unsigned long *,unsigned long,unsigned long)"},
		{"?Term@CComModule@ATL@@QAEXXZ", "public: void __thiscall ATL::CComModule::Term(void)"},
		{"?RegisterBuffer@CRegParser@ATL@@QAEJPADH@Z", "public: long __thiscall ATL::CRegParser::RegisterBuffer(char *,int)"},
		{"?AddModule@CAtlAllocator@@QAEHPAUHINSTANCE__@@@Z", "public: int __thiscall CAtlAllocator::AddModule(struct HINSTANCE__ *)"},
		{"?UpdateRegistryFromResourceS@CComModule@ATL@@UAGJPBDHPAU_ATL_REGMAP_ENTRY@2@@Z", "public: virtual long __stdcall ATL::CComModule::UpdateRegistryFromResourceS(char const *,int,struct ATL::_ATL_REGMAP_ENTRY *)"},
		{"?OnOK@CDocumentDlg@@QAEJGGPAUHWND__@@AAH@Z", "public: long __thiscall CDocumentDlg::OnOK(unsigned short,unsigned short,struct HWND__ *,int &)"},
		{"?Detach@CRegKey@ATL@@QAEPAUHKEY__@@XZ", "public: struct HKEY__ * __thiscall ATL::CRegKey::Detach(void)"},
		{"?Add@?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@QAEHABU_AtlUpdateUIElement@CUpdateUIBase@WTL@@@Z", "public: int __thiscall ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement>>::Add(struct WTL::CUpdateUIBase::_AtlUpdateUIElement const &)"},
		{"?VT@?$CVarTypeInfo@PAJ@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<long *>::VT"},
		{"?Add@?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@QAEHABQAVCIdleHandler@WTL@@@Z", "public: int __thiscall ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::Add(class WTL::CIdleHandler * const &)"},
		{"?_xttoi@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@SAHPBD@Z", "public: static int __cdecl WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::_xttoi(char const *)"},
		{"??$AtlConvAllocMemory@_W@ATL@@YAXPAPA_WHPA_WH@Z", "void __cdecl ATL::AtlConvAllocMemory<wchar_t>(wchar_t * *,int,wchar_t *,int)"},
		{"?GetNextCookie@CAtlTraceProcess@@QAEJXZ", "public: long __thiscall CAtlTraceProcess::GetNextCookie(void)"},
		{"?GetClientRect@CWindow@ATL@@QBEHPAUtagRECT@@@Z", "public: int __thiscall ATL::CWindow::GetClientRect(struct tagRECT *)const"},
		{"?_IsValidString@CString@WTL@@SGHPBDH@Z", "public: static int __stdcall WTL::CString::_IsValidString(char const *,int)"},
		{"?Free@?$CAutoVectorPtr@D@ATL@@QAEXXZ", "public: void __thiscall ATL::CAutoVectorPtr<char>::Free(void)"},
		{"??1?$CSimpleArray@PAUHWND__@@V?$CSimpleArrayEqualHelper@PAUHWND__@@@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleArray<struct HWND__ *,class ATL::CSimpleArrayEqualHelper<struct HWND__ *>>::~CSimpleArray<struct HWND__ *,class ATL::CSimpleArrayEqualHelper<struct HWND__ *>>(void)"},
		{"??0?$CSimpleArray@GV?$CSimpleArrayEqualHelper@G@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleArray<unsigned short,class ATL::CSimpleArrayEqualHelper<unsigned short>>::CSimpleArray<unsigned short,class ATL::CSimpleArrayEqualHelper<unsigned short>>(void)"},
		{"??0?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>(void)"},
		{"??0?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>(void)"},
		{"??0?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *>>::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *>>(void)"},
		{"??0?$CSimpleArray@UCModuleInfo@CTraceSnapshot@@V?$CSimpleArrayEqualHelper@UCModuleInfo@CTraceSnapshot@@@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo>>::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo>>(void)"},
		{"?atlTraceCache@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceCache"},
		{"?atlTraceUser@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceUser"},
		{"?AddRef@CRegObject@ATL@@UAGKXZ", "public: virtual unsigned long __stdcall ATL::CRegObject::AddRef(void)"},
		{"?StartTrackMouseLeave@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEHXZ", "public: int __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::StartTrackMouseLeave(void)"},
		{"?Activate@?$CToolTipCtrlT@VCWindow@ATL@@@WTL@@QAEXH@Z", "public: void __thiscall WTL::CToolTipCtrlT<class ATL::CWindow>::Activate(int)"},
		{"??0?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@QAE@XZ", "public: __thiscall ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>(void)"},
		{"?Create@?$CToolTipCtrlT@VCWindow@ATL@@@WTL@@QAEPAUHWND__@@PAU3@V_U_RECT@ATL@@PBDKKV_U_MENUorID@5@PAX@Z", "public: struct HWND__ * __thiscall WTL::CToolTipCtrlT<class ATL::CWindow>::Create(struct HWND__ *,class ATL::_U_RECT,char const *,unsigned long,unsigned long,class ATL::_U_MENUorID,void *)"},
		{"??1?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@UAE@XZ", "public: virtual __thiscall ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::~CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>(void)"},
		{"?OnSize@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnSize(unsigned int,unsigned int,long,int &)"},
		{"?Create@?$CDialogImpl@VCMainDlg@@VCWindow@ATL@@@ATL@@QAEPAUHWND__@@PAU3@J@Z", "public: struct HWND__ * __thiscall ATL::CDialogImpl<class CMainDlg,class ATL::CWindow>::Create(struct HWND__ *,long)"},
		{"??C?$CComPtrBase@UIConnectionPoint@@@ATL@@QBEPAV?$_NoAddRefReleaseOnCComPtr@UIConnectionPoint@@@1@XZ", "public: class ATL::_NoAddRefReleaseOnCComPtr<struct IConnectionPoint> * __thiscall ATL::CComPtrBase<struct IConnectionPoint>::operator->(void)const"},
		{"??C?$CComPtrBase@UIConnectionPointContainer@@@ATL@@QBEPAV?$_NoAddRefReleaseOnCComPtr@UIConnectionPointContainer@@@1@XZ", "public: class ATL::_NoAddRefReleaseOnCComPtr<struct IConnectionPointContainer> * __thiscall ATL::CComPtrBase<struct IConnectionPointContainer>::operator->(void)const"},
		{"??4?$CStaticT@VCWindow@ATL@@@WTL@@QAEAAV01@PAUHWND__@@@Z", "public: class WTL::CStaticT<class ATL::CWindow> & __thiscall WTL::CStaticT<class ATL::CWindow>::operator=(struct HWND__ *)"},
		{"?RegCreateKeyExA@CAtlTransactionManager@ATL@@QAEJPAUHKEY__@@PBDKPADKKQAU_SECURITY_ATTRIBUTES@@PAPAU3@PAK@Z", "public: long __thiscall ATL::CAtlTransactionManager::RegCreateKeyExA(struct HKEY__ *,char const *,unsigned long,char *,unsigned long,unsigned long,struct _SECURITY_ATTRIBUTES * const,struct HKEY__ * *,unsigned long *)"},

		{"??0CTrace@ATL@@AAE@P6AHHPBDH00ZZ@Z", "private: __thiscall ATL::CTrace::CTrace(int (__cdecl *)(int,char const *,int,char const *,char const *,...))"},
		{"?ResourceRegister@CRegObject@ATL@@QAGJPB_WI0@Z", "public: long __stdcall ATL::CRegObject::ResourceRegister(wchar_t const *,unsigned int,wchar_t const *)"},

		{"?s_pfnPrevHook@CNoUIAssertHook@ATL@@0P6AHHPADPAH@ZA", "private: static int (__cdecl * ATL::CNoUIAssertHook::s_pfnPrevHook)(int,char *,int *)"},
		//{"?_pPerfRegFunc@ATL@@3P6AJPAUHINSTANCE__@@@ZA", "long (__cdecl * ATL::_pPerfRegFunc)(struct HINSTANCE__ *)"},
		{"?atlTraceWindowing@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceWindowing"},
		{"?RelayEvent@?$CToolTipCtrlT@VCWindow@ATL@@@WTL@@QAEXPAUtagMSG@@@Z", "public: void __thiscall WTL::CToolTipCtrlT<class ATL::CWindow>::RelayEvent(struct tagMSG *)"},
		{"?GetData@CAtlSafeAllocBufferNode@?$CAtlSafeAllocBufferManager@VCCRTAllocator@ATL@@@_ATL_SAFE_ALLOCA_IMPL@ATL@@QAEPAXXZ", "public: void * __thiscall ATL::_ATL_SAFE_ALLOCA_IMPL::CAtlSafeAllocBufferManager<class ATL::CCRTAllocator>::CAtlSafeAllocBufferNode::GetData(void)"},
		{"??A?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@QAEAAPAVCMessageFilter@WTL@@H@Z", "public: class WTL::CMessageFilter * & __thiscall ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *>>::operator[](int)"},
		{"?GetResourceInstance@CComModule@ATL@@QAEPAUHINSTANCE__@@XZ", "public: struct HINSTANCE__ * __thiscall ATL::CComModule::GetResourceInstance(void)"},
		{"?DeleteValue@CRegKey@ATL@@QAEJPBD@Z", "public: long __thiscall ATL::CRegKey::DeleteValue(char const *)"},
		{"?AddCommonRGSReplacements@?$CAtlModuleT@VCComModule@ATL@@@ATL@@UAEJPAUIRegistrarBase@@@Z", "public: virtual long __thiscall ATL::CAtlModuleT<class ATL::CComModule>::AddCommonRGSReplacements(struct IRegistrarBase *)"},
		{"?AddMessageLoop@CAppModule@WTL@@QAEHPAVCMessageLoop@2@@Z", "public: int __thiscall WTL::CAppModule::AddMessageLoop(class WTL::CMessageLoop *)"},
		{"?ClearReplacements@CExpansionVector@ATL@@QAEJXZ", "public: long __thiscall ATL::CExpansionVector::ClearReplacements(void)"},
		{"?Decrement@CComMultiThreadModel@ATL@@SGKPAJ@Z", "public: static unsigned long __stdcall ATL::CComMultiThreadModel::Decrement(long *)"},
		{"?GetSize@?$CSimpleArray@PAUHWND__@@V?$CSimpleArrayEqualHelper@PAUHWND__@@@ATL@@@ATL@@QBEHXZ", "public: int __thiscall ATL::CSimpleArray<struct HWND__ *,class ATL::CSimpleArrayEqualHelper<struct HWND__ *>>::GetSize(void)const"},
		{"?Init@CDynamicStdCallThunk@ATL@@QAEHKPAX@Z", "public: int __thiscall ATL::CDynamicStdCallThunk::Init(unsigned long,void *)"},
		{"?RemoveResourceInstance@CAtlBaseModule@ATL@@QAE_NPAUHINSTANCE__@@@Z", "public: bool __thiscall ATL::CAtlBaseModule::RemoveResourceInstance(struct HINSTANCE__ *)"},
		{"?VT@?$CVarTypeInfo@PAPA_W@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<wchar_t * *>::VT"},
		{"?OnSetFont@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnSetFont(unsigned int,unsigned int,long,int &)"},
		{"??1CAboutDlg@@UAE@XZ", "public: virtual __thiscall CAboutDlg::~CAboutDlg(void)"},
		{"?AddValue@CRegParser@ATL@@IAEJAAVCRegKey@2@PBDPAD@Z", "protected: long __thiscall ATL::CRegParser::AddValue(class ATL::CRegKey &,char const *,char *)"},
		{"?RevokeClassObject@_ATL_OBJMAP_ENTRY30@ATL@@QAGJXZ", "public: long __stdcall ATL::_ATL_OBJMAP_ENTRY30::RevokeClassObject(void)"},
		{"?ScreenToClient@CWindow@ATL@@QBEHPAUtagPOINT@@@Z", "public: int __thiscall ATL::CWindow::ScreenToClient(struct tagPOINT *)const"},
		{"?Run@@YAHPADH@Z", "int __cdecl Run(char *,int)"},
		{"?SetCurSel@?$CComboBoxT@VCWindow@ATL@@@WTL@@QAEHH@Z", "public: int __thiscall WTL::CComboBoxT<class ATL::CWindow>::SetCurSel(int)"},
		{"?AddString@CParseBuffer@CRegParser@ATL@@QAEHPB_W@Z", "public: int __thiscall ATL::CRegParser::CParseBuffer::AddString(wchar_t const *)"},
		{"?_Min@?$AtlLimits@_J@ATL@@2_JB", "public: static __int64 const ATL::AtlLimits<__int64>::_Min"},
		{"?MessageBoxA@CWindow@ATL@@QAEHPBD0I@Z", "public: int __thiscall ATL::CWindow::MessageBoxA(char const *,char const *,unsigned int)"},
		{"?Close@CRegKey@ATL@@QAEJXZ", "public: long __thiscall ATL::CRegKey::Close(void)"},
		{"?ReleaseBuffer@CString@WTL@@QAEXH@Z", "public: void __thiscall WTL::CString::ReleaseBuffer(int)"},
		{"??1CDynamicStdCallThunk@ATL@@QAE@XZ", "public: __thiscall ATL::CDynamicStdCallThunk::~CDynamicStdCallThunk(void)"},
		{"?AtlGetPerUserRegistration@ATL@@YGJPA_N@Z", "long __stdcall ATL::AtlGetPerUserRegistration(bool *)"},
		{"??0CDynamicStdCallThunk@ATL@@QAE@XZ", "public: __thiscall ATL::CDynamicStdCallThunk::CDynamicStdCallThunk(void)"},
		{"?OnUpdateUiState@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnUpdateUiState(unsigned int,unsigned int,long,int &)"},
		{"?Add@?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAEHABK@Z", "public: int __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>::Add(unsigned long const &)"},
		{"?OnFocus@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnFocus(unsigned int,unsigned int,long,int &)"},
		{"??RCTraceFileAndLineInfo@ATL@@QBAXPBDZZ", "public: void __cdecl ATL::CTraceFileAndLineInfo::operator()(char const *,...)const"},
		{"??B?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@QBEPADXZ", "public: __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::operator char *(void)const"},
		{"??4type_info@@AAEAAV0@ABV0@@Z", "private: class type_info & __thiscall type_info::operator=(class type_info const &)"},
		{"??_U@YAPAXI@Z", "void * __cdecl operator new[](unsigned int)"},
		{"??_V@YAXPAX@Z", "void __cdecl operator delete[](void *)"},
		{"??$?2K@?$Wrapper@K@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@SAPAXIPAK@Z", "public: static void * __cdecl ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::Wrapper<unsigned long>::operator new<unsigned long>(unsigned int,unsigned long *)"},
		{"??$?3U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@Wrapper@?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@SAXPAXPAU_AtlUpdateUIElement@CUpdateUIBase@WTL@@@Z", "public: static void __cdecl ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement>>::Wrapper::operator delete<struct WTL::CUpdateUIBase::_AtlUpdateUIElement>(void *,struct WTL::CUpdateUIBase::_AtlUpdateUIElement *)"},
		{"??HWTL@@YG?AVCString@0@ABV10@PBD@Z", "class WTL::CString __stdcall WTL::operator+(class WTL::CString const &,char const *)"},
		{"??A?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAEAAPAUHINSTANCE__@@H@Z", "public: struct HINSTANCE__ * & __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *>>::operator[](int)"},
		{"??$?2PA_W@?$Wrapper@PA_W@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@SAPAXIPAPA_W@Z", "public: static void * __cdecl ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Wrapper<wchar_t *>::operator new<wchar_t *>(unsigned int,wchar_t * *)"},
		{"??A?$CSimpleArray@PAUHWND__@@V?$CSimpleArrayEqualHelper@PAUHWND__@@@ATL@@@ATL@@QAEAAPAUHWND__@@H@Z", "public: struct HWND__ * & __thiscall ATL::CSimpleArray<struct HWND__ *,class ATL::CSimpleArrayEqualHelper<struct HWND__ *>>::operator[](int)"},
		{"??I?$CComPtrBase@UIConnectionPointContainer@@@ATL@@QAEPAPAUIConnectionPointContainer@@XZ", "public: struct IConnectionPointContainer * * __thiscall ATL::CComPtrBase<struct IConnectionPointContainer>::operator&(void)"},
		{"??BCString@WTL@@QBEPBDXZ", "public: __thiscall WTL::CString::operator char const *(void)const"},
		{"??$?2U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@Wrapper@?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@SAPAXIPAU_AtlUpdateUIElement@CUpdateUIBase@WTL@@@Z", "public: static void * __cdecl ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement>>::Wrapper::operator new<struct WTL::CUpdateUIBase::_AtlUpdateUIElement>(unsigned int,struct WTL::CUpdateUIBase::_AtlUpdateUIElement *)"},
		{"??$?2PAUHINSTANCE__@@@Wrapper@?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@SAPAXIPAPAUHINSTANCE__@@@Z", "public: static void * __cdecl ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *>>::Wrapper::operator new<struct HINSTANCE__ *>(unsigned int,struct HINSTANCE__ * *)"},
		{"??$?3PA_W@?$Wrapper@PA_W@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@SAXPAXPAPA_W@Z", "public: static void __cdecl ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Wrapper<wchar_t *>::operator delete<wchar_t *>(void *,wchar_t * *)"},
		{"??_7CHyperLink@WTL@@6B@", "const WTL::CHyperLink::`vftable'"},
		{"??1CDocumentDlg@@UAE@XZ", "public: virtual __thiscall CDocumentDlg::~CDocumentDlg(void)"},
		{"?SubclassWindow@?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@QAEHPAUHWND__@@@Z", "public: int __thiscall ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::SubclassWindow(struct HWND__ *)"},
		{"?Unlock@CStaticDataInitCriticalSectionLock@WTL@@QAEXXZ", "public: void __thiscall WTL::CStaticDataInitCriticalSectionLock::Unlock(void)"},
		{"?SetWindowLongPtrA@@YAJPAUHWND__@@HJ@Z", "long __cdecl SetWindowLongPtrA(struct HWND__ *,int,long)"},
		{"?SetFocus@CWindow@ATL@@QAEPAUHWND__@@XZ", "public: struct HWND__ * __thiscall ATL::CWindow::SetFocus(void)"},
		{"?MAX_TYPE@CRegParser@ATL@@1HB", "protected: static int const ATL::CRegParser::MAX_TYPE"},
		{"?OnInitDialog@CDocumentDlg@@QAEJIIJAAH@Z", "public: long __thiscall CDocumentDlg::OnInitDialog(unsigned int,unsigned int,long,int &)"},
		{"?AddIdleHandler@CMessageLoop@WTL@@QAEHPAVCIdleHandler@2@@Z", "public: int __thiscall WTL::CMessageLoop::AddIdleHandler(class WTL::CIdleHandler *)"},
		{"?RemoveAll@?$CSimpleArray@PAUHWND__@@V?$CSimpleArrayEqualHelper@PAUHWND__@@@ATL@@@ATL@@QAEXXZ", "public: void __thiscall ATL::CSimpleArray<struct HWND__ *,class ATL::CSimpleArrayEqualHelper<struct HWND__ *>>::RemoveAll(void)"},
		{"??0?$CComPtr@UIConnectionPoint@@@ATL@@QAE@XZ", "public: __thiscall ATL::CComPtr<struct IConnectionPoint>::CComPtr<struct IConnectionPoint>(void)"},
		{"?InternalSetAtIndex@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAEXHABKABQAVCMessageLoop@WTL@@@Z", "public: void __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::InternalSetAtIndex(int,unsigned long const &,class WTL::CMessageLoop * const &)"},
		{"?GetAppId@?$CAtlModuleT@VCComModule@ATL@@@ATL@@SAPB_WXZ", "public: static wchar_t const * __cdecl ATL::CAtlModuleT<class ATL::CComModule>::GetAppId(void)"},
		{"?SubclassWindow@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEHPAUHWND__@@@Z", "public: int __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::SubclassWindow(struct HWND__ *)"},
		{"??0?$CComboBoxT@VCWindow@ATL@@@WTL@@QAE@PAUHWND__@@@Z", "public: __thiscall WTL::CComboBoxT<class ATL::CWindow>::CComboBoxT<class ATL::CWindow>(struct HWND__ *)"},
		{"?_cstrtoi@CString@WTL@@KAHPBD@Z", "protected: static int __cdecl WTL::CString::_cstrtoi(char const *)"},
		{"?RemoveAll@?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@QAEXXZ", "public: void __thiscall ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *>>::RemoveAll(void)"},
		{"?RemoveAll@?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@QAEXXZ", "public: void __thiscall ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::RemoveAll(void)"},
		{"?RemoveAll@?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAEXXZ", "public: void __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>::RemoveAll(void)"},
		{"?RemoveAll@?$CSimpleArray@GV?$CSimpleArrayEqualHelper@G@ATL@@@ATL@@QAEXXZ", "public: void __thiscall ATL::CSimpleArray<unsigned short,class ATL::CSimpleArrayEqualHelper<unsigned short>>::RemoveAll(void)"},
		{"??1?$CComPtr@UIConnectionPoint@@@ATL@@QAE@XZ", "public: __thiscall ATL::CComPtr<struct IConnectionPoint>::~CComPtr<struct IConnectionPoint>(void)"},
		{"??0Thank_you@Define_the_symbol__ATL_MIXED@@QAE@XZ", "public: __thiscall Define_the_symbol__ATL_MIXED::Thank_you::Thank_you(void)"},
		{"?memcpy_s@Checked@ATL@@YAXPAXIPBXI@Z", "void __cdecl ATL::Checked::memcpy_s(void *,unsigned int,void const *,unsigned int)"},
		{"??1_ATL_WIN_MODULE70@ATL@@QAE@XZ", "public: __thiscall ATL::_ATL_WIN_MODULE70::~_ATL_WIN_MODULE70(void)"},
		{"?Release@CString@WTL@@KGXPAUCStringData@2@@Z", "protected: static void __stdcall WTL::CString::Release(struct WTL::CStringData *)"},
		{"??A?$CSimpleArray@UCModuleInfo@CTraceSnapshot@@V?$CSimpleArrayEqualHelper@UCModuleInfo@CTraceSnapshot@@@ATL@@@ATL@@QAEAAUCModuleInfo@CTraceSnapshot@@H@Z", "public: struct CTraceSnapshot::CModuleInfo & __thiscall ATL::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo>>::operator[](int)"},
		{"??_GCDocumentDlg@@UAEPAXI@Z", "public: virtual void * __thiscall CDocumentDlg::`scalar deleting destructor'(unsigned int)"},
		{"??_7CIdleHandler@WTL@@6B@", "const WTL::CIdleHandler::`vftable'"},
		{"?CloseDialog@CMainDlg@@QAEXH@Z", "public: void __thiscall CMainDlg::CloseDialog(int)"},
		{"?ModifyStyle@CWindow@ATL@@QAEHKKI@Z", "public: int __thiscall ATL::CWindow::ModifyStyle(unsigned long,unsigned long,unsigned int)"},
		{"?Append@CParseBuffer@CRegParser@ATL@@QAEHPBDH@Z", "public: int __thiscall ATL::CRegParser::CParseBuffer::Append(char const *,int)"},
		{"?HasSubKeys@CRegParser@ATL@@IAEHPAUHKEY__@@@Z", "protected: int __thiscall ATL::CRegParser::HasSubKeys(struct HKEY__ *)"},
		{"??0_ATL_WIN_MODULE70@ATL@@QAE@XZ", "public: __thiscall ATL::_ATL_WIN_MODULE70::_ATL_WIN_MODULE70(void)"},
		{"??YCString@WTL@@QAEAAV01@PBD@Z", "public: class WTL::CString & __thiscall WTL::CString::operator+=(char const *)"},
		{"?Lock@CComSafeDeleteCriticalSection@ATL@@QAEJXZ", "public: long __thiscall ATL::CComSafeDeleteCriticalSection::Lock(void)"},
		{"?GetItemData@?$CComboBoxT@VCWindow@ATL@@@WTL@@QBEKH@Z", "public: unsigned long __thiscall WTL::CComboBoxT<class ATL::CWindow>::GetItemData(int)const"},
		{"??BCRegKey@ATL@@QBEPAUHKEY__@@XZ", "public: __thiscall ATL::CRegKey::operator struct HKEY__ *(void)const"},
		{"?IsUsingToolTip@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBE_NXZ", "public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::IsUsingToolTip(void)const"},
		{"?_type_info_dtor_internal_method@type_info@@QAEXXZ", "public: void __thiscall type_info::_type_info_dtor_internal_method(void)"},
		{"??0CAtlTraceModule@@QAE@XZ", "public: __thiscall CAtlTraceModule::CAtlTraceModule(void)"},
		{"?GetMessageLoop@CAppModule@WTL@@QBEPAVCMessageLoop@2@K@Z", "public: class WTL::CMessageLoop * __thiscall WTL::CAppModule::GetMessageLoop(unsigned long)const"},
		{"?GetCodeAddress@CDynamicStdCallThunk@ATL@@QAEPAXXZ", "public: void * __thiscall ATL::CDynamicStdCallThunk::GetCodeAddress(void)"},
		{"?VT@?$CVarTypeInfo@G@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<unsigned short>::VT"},
		{"??0IUnknown@@QAE@XZ", "public: __thiscall IUnknown::IUnknown(void)"},
		{"?_cstrisdigit@CString@WTL@@KAHD@Z", "protected: static int __cdecl WTL::CString::_cstrisdigit(char)"},
		{"?atlTraceStencil@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceStencil"},
		{"?AddCategory@CAtlAllocator@@QAEHHPB_W@Z", "public: int __thiscall CAtlAllocator::AddCategory(int,wchar_t const *)"},
		{"??0CComModule@ATL@@QAE@XZ", "public: __thiscall ATL::CComModule::CComModule(void)"},
		{"??1CTrace@ATL@@AAE@XZ", "private: __thiscall ATL::CTrace::~CTrace(void)"},
		{"?ModuleCount@CAtlTraceProcess@@QBEHXZ", "public: int __thiscall CAtlTraceProcess::ModuleCount(void)const"},
		{"?CopyBeforeWrite@CString@WTL@@IAEXXZ", "protected: void __thiscall WTL::CString::CopyBeforeWrite(void)"},
		{"??$?3PAVCMessageLoop@WTL@@@?$Wrapper@PAVCMessageLoop@WTL@@@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@SAXPAXPAPAVCMessageLoop@WTL@@@Z", "public: static void __cdecl ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::Wrapper<class WTL::CMessageLoop *>::operator delete<class WTL::CMessageLoop *>(void *,class WTL::CMessageLoop * *)"},
		{"?atlTraceUser2@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceUser2"},
		{"?AddChar@CParseBuffer@CRegParser@ATL@@QAEHPBD@Z", "public: int __thiscall ATL::CRegParser::CParseBuffer::AddChar(char const *)"},
		{"?AtlInternalQueryInterface@ATL@@YGJPAXPBU_ATL_INTMAP_ENTRY@1@ABU_GUID@@PAPAX@Z", "long __stdcall ATL::AtlInternalQueryInterface(void *,struct ATL::_ATL_INTMAP_ENTRY const *,struct _GUID const &,void * *)"},
		{"?OnFinalMessage@?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@UAEXPAUHWND__@@@Z", "public: virtual void __thiscall ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnFinalMessage(struct HWND__ *)"},
		{"?DoPaint@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEXV?$CDCT@$0A@@2@@Z", "public: void __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::DoPaint(class WTL::CDCT<0>)"},
		{"?Detach@?$CDCT@$00@WTL@@QAEPAUHDC__@@XZ", "public: struct HDC__ * __thiscall WTL::CDCT<1>::Detach(void)"},
		{"?atlTraceNotImpl@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceNotImpl"},
		{"??0Wrapper@?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@QAE@ABU_AtlUpdateUIElement@CUpdateUIBase@WTL@@@Z", "public: __thiscall ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement>>::Wrapper::Wrapper(struct WTL::CUpdateUIBase::_AtlUpdateUIElement const &)"},
		{"?_RTC_AllocaFailure@@YAXPAXPAU_RTC_ALLOCA_NODE@@H@Z", "void __cdecl _RTC_AllocaFailure(void *,struct _RTC_ALLOCA_NODE *,int)"},
		{"?__FreeStdCallThunk@ATL@@YGXPAX@Z", "void __stdcall ATL::__FreeStdCallThunk(void *)"},
		{"??0CString@WTL@@QAE@XZ", "public: __thiscall WTL::CString::CString(void)"},
		{"?GetCategoryCount@CAtlAllocator@@QBEHH@Z", "public: int __thiscall CAtlAllocator::GetCategoryCount(int)const"},
		{"??1?$CComPtrBase@UIConnectionPointContainer@@@ATL@@QAE@XZ", "public: __thiscall ATL::CComPtrBase<struct IConnectionPointContainer>::~CComPtrBase<struct IConnectionPointContainer>(void)"},
		{"?MAX_VALUE@CRegParser@ATL@@1HB", "protected: static int const ATL::CRegParser::MAX_VALUE"},
		{"??BCToolInfo@WTL@@QAEJXZ", "public: __thiscall WTL::CToolInfo::operator long(void)"},
		{"??0CAtlException@ATL@@QAE@J@Z", "public: __thiscall ATL::CAtlException::CAtlException(long)"},
		{"??1CString@WTL@@QAE@XZ", "public: __thiscall WTL::CString::~CString(void)"},
		{"?Add@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QAEHABQADABQA_W@Z", "public: int __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Add(char * const &,wchar_t * const &)"},
		{"?AtlThrowLastWin32@ATL@@YGXXZ", "void __stdcall ATL::AtlThrowLastWin32(void)"},
		{"?VT@?$CVarTypeInfo@F@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<short>::VT"},
		{"?StrFromMap@CRegObject@ATL@@QAEPB_WPAD@Z", "public: wchar_t const * __thiscall ATL::CRegObject::StrFromMap(char *)"},
		{"?GetBuffer@CString@WTL@@QAEPADH@Z", "public: char * __thiscall WTL::CString::GetBuffer(int)"},
		{"?DecRef@CAtlTraceProcess@@QAEIXZ", "public: unsigned int __thiscall CAtlTraceProcess::DecRef(void)"},
		{"?GetKeyAt@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QBEAAPADH@Z", "public: char * & __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::GetKeyAt(int)const"},
		{"?_pAtlModule@ATL@@3PAVCAtlModule@1@A", "class ATL::CAtlModule * ATL::_pAtlModule"},
		{"??_7CAppModule@WTL@@6B@", "const WTL::CAppModule::`vftable'"},
		{"??$?2PAVCIdleHandler@WTL@@@Wrapper@?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@SAPAXIPAPAVCIdleHandler@WTL@@@Z", "public: static void * __cdecl ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::Wrapper::operator new<class WTL::CIdleHandler *>(unsigned int,class WTL::CIdleHandler * *)"},
		{"?GetAppDir@@YA?AVCString@WTL@@XZ", "class WTL::CString __cdecl GetAppDir(void)"},
		{"?VT@?$CVarTypeInfo@PAUtagVARIANT@@@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<struct tagVARIANT *>::VT"},
		{"??0CString@WTL@@QAE@ABV01@@Z", "public: __thiscall WTL::CString::CString(class WTL::CString const &)"},
		{"?FormatV@CString@WTL@@QAEHPBDPAD@Z", "public: int __thiscall WTL::CString::FormatV(char const *,char *)"},
		{"?OnOK@CMainDlg@@QAEJGGPAUHWND__@@AAH@Z", "public: long __thiscall CMainDlg::OnOK(unsigned short,unsigned short,struct HWND__ *,int &)"},
		{"?Name@CAtlTraceModuleInfo@@QBEPB_WXZ", "public: wchar_t const * __thiscall CAtlTraceModuleInfo::Name(void)const"},
		{"?__AllocStdCallThunk_cmn@@YAPAXXZ", "void * __cdecl __AllocStdCallThunk_cmn(void)"},
		{"??_GCAtlAllocator@@QAEPAXI@Z", "public: void * __thiscall CAtlAllocator::`scalar deleting destructor'(unsigned int)"},
		{"??0?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>(void)"},
		{"?GetValueAt@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QBEAAPAVCMessageLoop@WTL@@H@Z", "public: class WTL::CMessageLoop * & __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::GetValueAt(int)const"},
		{"?memcpy_x@SecureHelper@WTL@@YAXPAXIPBXI@Z", "void __cdecl WTL::SecureHelper::memcpy_x(void *,unsigned int,void const *,unsigned int)"},
		{"??_7CMessageFilter@WTL@@6B@", "const WTL::CMessageFilter::`vftable'"},
		{"?AtlInitCommonControls@WTL@@YAHK@Z", "int __cdecl WTL::AtlInitCommonControls(unsigned long)"},
		{"?OnEraseBackground@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnEraseBackground(unsigned int,unsigned int,long,int &)"},
		{"??_G?$CSimpleArray@PAUHWND__@@V?$CSimpleArrayEqualHelper@PAUHWND__@@@ATL@@@ATL@@QAEPAXI@Z", "public: void * __thiscall ATL::CSimpleArray<struct HWND__ *,class ATL::CSimpleArrayEqualHelper<struct HWND__ *>>::`scalar deleting destructor'(unsigned int)"},
		{"??0CString@WTL@@QAE@PBD@Z", "public: __thiscall WTL::CString::CString(char const *)"},
		{"??1?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QAE@XZ", "public: __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::~CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>(void)"},
		{"?VT@?$CVarTypeInfo@PAN@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<double *>::VT"},
		{"??_GCComModule@ATL@@UAEPAXI@Z", "public: virtual void * __thiscall ATL::CComModule::`scalar deleting destructor'(unsigned int)"},
		{"?StrChrA@CRegParser@ATL@@KAPADPADD@Z", "protected: static char * __cdecl ATL::CRegParser::StrChrA(char *,char)"},
		{"?atlTraceUser3@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceUser3"},
		{"??0CRegParser@ATL@@QAE@PAVCRegObject@1@@Z", "public: __thiscall ATL::CRegParser::CRegParser(class ATL::CRegObject *)"},
		{"?AtlThrowImpl@ATL@@YGXJ@Z", "void __stdcall ATL::AtlThrowImpl(long)"},
		{"?SetMultiStringValue@CRegKey@ATL@@QAEJPBD0@Z", "public: long __thiscall ATL::CRegKey::SetMultiStringValue(char const *,char const *)"},
		{"?Base@CAtlTraceProcess@@QBEPAXXZ", "public: void * __thiscall CAtlTraceProcess::Base(void)const"},
		{"?QueryInterface@CRegObject@ATL@@UAGJABU_GUID@@PAPAX@Z", "public: virtual long __stdcall ATL::CRegObject::QueryInterface(struct _GUID const &,void * *)"},
		{"??0CAtlBaseModule@ATL@@QAE@XZ", "public: __thiscall ATL::CAtlBaseModule::CAtlBaseModule(void)"},
		{"??_ECDocumentDlg@@UAEPAXI@Z", "public: virtual void * __thiscall CDocumentDlg::`vector deleting destructor'(unsigned int)"},
		{"?OnEnable@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnEnable(unsigned int,unsigned int,long,int &)"},
		{"??0?$Wrapper@K@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAE@ABK@Z", "public: __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::Wrapper<unsigned long>::Wrapper<unsigned long>(unsigned long const &)"},
		{"?AtlHresultFromWin32@ATL@@YAJK@Z", "long __cdecl ATL::AtlHresultFromWin32(unsigned long)"},
		{"??1CAtlBaseModule@ATL@@QAE@XZ", "public: __thiscall ATL::CAtlBaseModule::~CAtlBaseModule(void)"},
		{"?OnLButtonDown@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnLButtonDown(unsigned int,unsigned int,long,int &)"},
		{"??0CTraceSnapshot@@QAE@XZ", "public: __thiscall CTraceSnapshot::CTraceSnapshot(void)"},
		{"?SetDWORDValue@CRegKey@ATL@@QAEJPBDK@Z", "public: long __thiscall ATL::CRegKey::SetDWORDValue(char const *,unsigned long)"},
		{"??1CHyperLink@WTL@@UAE@XZ", "public: virtual __thiscall WTL::CHyperLink::~CHyperLink(void)"},
		{"?_AtlRaiseException@ATL@@YAXKK@Z", "void __cdecl ATL::_AtlRaiseException(unsigned long,unsigned long)"},
		{"?VT@?$CVarTypeInfo@E@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<unsigned char>::VT"},
		{"?ATL_CREATE_OBJECT@CComApartment@ATL@@2IA", "public: static unsigned int ATL::CComApartment::ATL_CREATE_OBJECT"},
		{"?Free@CCRTAllocator@ATL@@SAXPAX@Z", "public: static void __cdecl ATL::CCRTAllocator::Free(void *)"},
		{"??1CTraceSnapshot@@QAE@XZ", "public: __thiscall CTraceSnapshot::~CTraceSnapshot(void)"},
		{"?Term@CAtlWinModule@ATL@@QAEXXZ", "public: void __thiscall ATL::CAtlWinModule::Term(void)"},
		{"?ocslen@@YAHPB_W@Z", "int __cdecl ocslen(wchar_t const *)"},
		{"?Detach@CParseBuffer@CRegParser@ATL@@QAEPADXZ", "public: char * __thiscall ATL::CRegParser::CParseBuffer::Detach(void)"},
		{"?Create@CRegKey@ATL@@QAEJPAUHKEY__@@PBDPADKKPAU_SECURITY_ATTRIBUTES@@PAK@Z", "public: long __thiscall ATL::CRegKey::Create(struct HKEY__ *,char const *,char *,unsigned long,unsigned long,struct _SECURITY_ATTRIBUTES *,unsigned long *)"},
		{"?TermSettingChangeNotify@CAppModule@WTL@@QAEXXZ", "public: void __thiscall WTL::CAppModule::TermSettingChangeNotify(void)"},
		{"?_IsTracingEnabled@ATL@@YA_NKKI@Z", "bool __cdecl ATL::_IsTracingEnabled(unsigned long,unsigned long,unsigned int)"},
		{"??I?$CComPtrBase@UIConnectionPoint@@@ATL@@QAEPAPAUIConnectionPoint@@XZ", "public: struct IConnectionPoint * * __thiscall ATL::CComPtrBase<struct IConnectionPoint>::operator&(void)"},
		{"?GetWndClassName@?$CToolTipCtrlT@VCWindow@ATL@@@WTL@@SAPBDXZ", "public: static char const * __cdecl WTL::CToolTipCtrlT<class ATL::CWindow>::GetWndClassName(void)"},
		{"?DrawTextA@?$CDCT@$0A@@WTL@@QAEHPADHPAUtagRECT@@I@Z", "public: int __thiscall WTL::CDCT<0>::DrawTextA(char *,int,struct tagRECT *,unsigned int)"},
		{"?Invalidate@CWindow@ATL@@QAEHH@Z", "public: int __thiscall ATL::CWindow::Invalidate(int)"},
		{"?ChToByte@CRegParser@ATL@@KAED@Z", "protected: static unsigned char __cdecl ATL::CRegParser::ChToByte(char)"},
		{"??1CrThreadAutoInstallHelper@@QAE@XZ", "public: __thiscall CrThreadAutoInstallHelper::~CrThreadAutoInstallHelper(void)"},
		{"??$?3PAVCIdleHandler@WTL@@@Wrapper@?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@SAXPAXPAPAVCIdleHandler@WTL@@@Z", "public: static void __cdecl ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::Wrapper::operator delete<class WTL::CIdleHandler *>(void *,class WTL::CIdleHandler * *)"},
		{"??2@YAPAXIPAX@Z", "void * __cdecl operator new(unsigned int,void *)"},
		{"?s_trace@CTrace@ATL@@2V12@A", "public: static class ATL::CTrace ATL::CTrace::s_trace"},
		{"?_pAtlAutoThreadModule@ATL@@3PAUIAtlAutoThreadModule@1@A", "struct ATL::IAtlAutoThreadModule * ATL::_pAtlAutoThreadModule"},
		{"?AtlWinModuleExtractCreateWndData@ATL@@YGPAXPAU_ATL_WIN_MODULE70@1@@Z", "void * __stdcall ATL::AtlWinModuleExtractCreateWndData(struct ATL::_ATL_WIN_MODULE70 *)"},
		{"?WindowProc@?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@SGJPAUHWND__@@IIJ@Z", "public: static long __stdcall ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::WindowProc(struct HWND__ *,unsigned int,unsigned int,long)"},
		{"?memmove_s@Checked@ATL@@YAXPAXIPBXI@Z", "void __cdecl ATL::Checked::memmove_s(void *,unsigned int,void const *,unsigned int)"},
		{"?VT@?$CVarTypeInfo@PAM@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<float *>::VT"},
		{"?UpdateRegistryFromResourceD@CComModule@ATL@@UAGJPBDHPAU_ATL_REGMAP_ENTRY@2@@Z", "public: virtual long __stdcall ATL::CComModule::UpdateRegistryFromResourceD(char const *,int,struct ATL::_ATL_REGMAP_ENTRY *)"},
		{"?OnGetFont@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z", "public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnGetFont(unsigned int,unsigned int,long,int &)"},
		{"??A?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@QAEAAPAVCIdleHandler@WTL@@H@Z", "public: class WTL::CIdleHandler * & __thiscall ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::operator[](int)"},
		{"?InitLibId@?$CAtlModuleT@VCComModule@ATL@@@ATL@@SAXXZ", "public: static void __cdecl ATL::CAtlModuleT<class ATL::CComModule>::InitLibId(void)"},
		{"?Lock@CStaticDataInitCriticalSectionLock@WTL@@QAEJXZ", "public: long __thiscall WTL::CStaticDataInitCriticalSectionLock::Lock(void)"},
		{"?Lock@CComCriticalSection@ATL@@QAEJXZ", "public: long __thiscall ATL::CComCriticalSection::Lock(void)"},
		{"??0CUpdateUIBase@WTL@@QAE@XZ", "public: __thiscall WTL::CUpdateUIBase::CUpdateUIBase(void)"},
		{"?RegisterClassObject@_ATL_OBJMAP_ENTRY30@ATL@@QAGJKK@Z", "public: long __stdcall ATL::_ATL_OBJMAP_ENTRY30::RegisterClassObject(unsigned long,unsigned long)"},
		{"??0CParseBuffer@CRegParser@ATL@@QAE@H@Z", "public: __thiscall ATL::CRegParser::CParseBuffer::CParseBuffer(int)"},
		{"?__AllocStdCallThunk@@YGPAXXZ", "void * __stdcall __AllocStdCallThunk(void)"},
		{"?AtlCallTermFunc@ATL@@YGXPAU_ATL_MODULE70@1@@Z", "void __stdcall ATL::AtlCallTermFunc(struct ATL::_ATL_MODULE70 *)"},
		{"?Reset@CAtlTraceCategory@@QAEXPB_WJ@Z", "public: void __thiscall CAtlTraceCategory::Reset(wchar_t const *,long)"},
		{"??0?$CAutoVectorPtr@_W@ATL@@QAE@XZ", "public: __thiscall ATL::CAutoVectorPtr<wchar_t>::CAutoVectorPtr<wchar_t>(void)"},
		{"?UpdateRegistryFromResourceS@CComModule@ATL@@UAGJIHPAU_ATL_REGMAP_ENTRY@2@@Z", "public: virtual long __stdcall ATL::CComModule::UpdateRegistryFromResourceS(unsigned int,int,struct ATL::_ATL_REGMAP_ENTRY *)"},
		{"??1CUpdateUIBase@WTL@@QAE@XZ", "public: __thiscall WTL::CUpdateUIBase::~CUpdateUIBase(void)"},
		{"?DialogProc@?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@SGHPAUHWND__@@IIJ@Z", "public: static int __stdcall ATL::CDialogImplBaseT<class ATL::CWindow>::DialogProc(struct HWND__ *,unsigned int,unsigned int,long)"},
		{"??1?$CAutoVectorPtr@_W@ATL@@QAE@XZ", "public: __thiscall ATL::CAutoVectorPtr<wchar_t>::~CAutoVectorPtr<wchar_t>(void)"},
		{"??$?2UCModuleInfo@CTraceSnapshot@@@Wrapper@?$CSimpleArray@UCModuleInfo@CTraceSnapshot@@V?$CSimpleArrayEqualHelper@UCModuleInfo@CTraceSnapshot@@@ATL@@@ATL@@SAPAXIPAUCModuleInfo@CTraceSnapshot@@@Z", "public: static void * __cdecl ATL::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo>>::Wrapper::operator new<struct CTraceSnapshot::CModuleInfo>(unsigned int,struct CTraceSnapshot::CModuleInfo *)"},
		{"?VT@?$CVarTypeInfo@PAPAUIDispatch@@@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<struct IDispatch * *>::VT"},
		{"?InternalSetAtIndex@?$CSimpleArray@UCModuleInfo@CTraceSnapshot@@V?$CSimpleArrayEqualHelper@UCModuleInfo@CTraceSnapshot@@@ATL@@@ATL@@QAEXHABUCModuleInfo@CTraceSnapshot@@@Z", "public: void __thiscall ATL::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo>>::InternalSetAtIndex(int,struct CTraceSnapshot::CModuleInfo const &)"},
		{"?AtlUnmarshalPtr@ATL@@YGJPAUIStream@@ABU_GUID@@PAPAUIUnknown@@@Z", "long __stdcall ATL::AtlUnmarshalPtr(struct IStream *,struct _GUID const &,struct IUnknown * *)"},
		{"?OnOK@CAboutDlg@@QAEJGGPAUHWND__@@AAH@Z", "public: long __thiscall CAboutDlg::OnOK(unsigned short,unsigned short,struct HWND__ *,int &)"},
		{"??0Wrapper@?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@QAE@ABQAVCIdleHandler@WTL@@@Z", "public: __thiscall ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::Wrapper::Wrapper(class WTL::CIdleHandler * const &)"},
		{"??$?2K@Wrapper@?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@SAPAXIPAK@Z", "public: static void * __cdecl ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>::Wrapper::operator new<unsigned long>(unsigned int,unsigned long *)"},
		{"?CanForceRemoveKey@CRegParser@ATL@@IAEHPBD@Z", "protected: int __thiscall ATL::CRegParser::CanForceRemoveKey(char const *)"},
		{"?VT@?$CVarTypeInfo@D@ATL@@2GB", "public: static unsigned short const ATL::CVarTypeInfo<char>::VT"},
		{"?OnCancel@CMainDlg@@QAEJGGPAUHWND__@@AAH@Z", "public: long __thiscall CMainDlg::OnCancel(unsigned short,unsigned short,struct HWND__ *,int &)"},
		{"?AtlWaitWithMessageLoop@ATL@@YGHPAX@Z", "int __stdcall ATL::AtlWaitWithMessageLoop(void *)"},
		{"?AtlHresultFromLastError@ATL@@YAJXZ", "long __cdecl ATL::AtlHresultFromLastError(void)"},
		{"?RegOpenKeyExA@CAtlTransactionManager@ATL@@QAEJPAUHKEY__@@PBDKKPAPAU3@@Z", "public: long __thiscall ATL::CAtlTransactionManager::RegOpenKeyExA(struct HKEY__ *,char const *,unsigned long,unsigned long,struct HKEY__ * *)"},
		{"?RemoveAll@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QAEXXZ", "public: void __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::RemoveAll(void)"},
		{"?ExtractCreateWndData@CAtlWinModule@ATL@@QAEPAXXZ", "public: void * __thiscall ATL::CAtlWinModule::ExtractCreateWndData(void)"},
		{"?IncCategoryCount@CAtlTraceProcess@@QAEXI@Z", "public: void __thiscall CAtlTraceProcess::IncCategoryCount(unsigned int)"},
		{"?AllocBuffer@CString@WTL@@IAEHH@Z", "protected: int __thiscall WTL::CString::AllocBuffer(int)"},
		{"?Unlock@?$CComCritSecLock@VCComCriticalSection@ATL@@@ATL@@QAEXXZ", "public: void __thiscall ATL::CComCritSecLock<class ATL::CComCriticalSection>::Unlock(void)"},
		{"?Id@CAtlTraceProcess@@QBEKXZ", "public: unsigned long __thiscall CAtlTraceProcess::Id(void)const"},
		{"?_Min@?$AtlLimits@K@ATL@@2KB", "public: static unsigned long const ATL::AtlLimits<unsigned long>::_Min"},
		{"?_Min@?$AtlLimits@J@ATL@@2JB", "public: static long const ATL::AtlLimits<long>::_Min"},
		{"?_Min@?$AtlLimits@I@ATL@@2IB", "public: static unsigned int const ATL::AtlLimits<unsigned int>::_Min"},
		{"?_Min@?$AtlLimits@H@ATL@@2HB", "public: static int const ATL::AtlLimits<int>::_Min"},
		{"??0CDocumentDlg@@QAE@XZ", "public: __thiscall CDocumentDlg::CDocumentDlg(void)"},
		{"?GetSize@?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QBEHXZ", "public: int __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *>>::GetSize(void)const"},
		{"?strcpyA_x@SecureHelper@WTL@@YAXPADIPBD@Z", "void __cdecl WTL::SecureHelper::strcpyA_x(char *,unsigned int,char const *)"},
		{"?Allocate@CCRTAllocator@ATL@@SAPAXI@Z", "public: static void * __cdecl ATL::CCRTAllocator::Allocate(unsigned int)"},
		{"?SetCapture@CWindow@ATL@@QAEPAUHWND__@@XZ", "public: struct HWND__ * __thiscall ATL::CWindow::SetCapture(void)"},
		{"__imp__GetMessageA@16", "import: __stdcall GetMessageA()"},
		{"?Init@_stdcallthunk@ATL@@QAEHKPAX@Z", "public: int __thiscall ATL::_stdcallthunk::Init(unsigned long,void *)"},
		{"?GetWindowLongPtrA@@YAJPAUHWND__@@H@Z", "long __cdecl GetWindowLongPtrA(struct HWND__ *,int)"},
		{"??0?$Wrapper@PAVCMessageLoop@WTL@@@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAE@ABQAVCMessageLoop@WTL@@@Z", "public: __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::Wrapper<class WTL::CMessageLoop *>::Wrapper<class WTL::CMessageLoop *>(class WTL::CMessageLoop * const &)"},
		{"?Name@CAtlTraceCategory@@QBEPB_WXZ", "public: wchar_t const * __thiscall CAtlTraceCategory::Name(void)const"},
		{"?ResourceUnregisterSz@CRegObject@ATL@@QAGJPB_W00@Z", "public: long __stdcall ATL::CRegObject::ResourceUnregisterSz(wchar_t const *,wchar_t const *,wchar_t const *)"},
		{"??$?2PAVCMessageLoop@WTL@@@?$Wrapper@PAVCMessageLoop@WTL@@@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@SAPAXIPAPAVCMessageLoop@WTL@@@Z", "public: static void * __cdecl ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::Wrapper<class WTL::CMessageLoop *>::operator new<class WTL::CMessageLoop *>(unsigned int,class WTL::CMessageLoop * *)"},
		{"__imp__RegSetValueExA@24", "import: __stdcall RegSetValueExA()"},
		{"?ProcessWindowMessage@CMainDlg@@UAEHPAUHWND__@@IIJAAJK@Z", "public: virtual int __thiscall CMainDlg::ProcessWindowMessage(struct HWND__ *,unsigned int,unsigned int,long,long &,unsigned long)"},
		{"?RegDeleteKeyA@CAtlTransactionManager@ATL@@QAEJPAUHKEY__@@PBD@Z", "public: long __thiscall ATL::CAtlTransactionManager::RegDeleteKeyA(struct HKEY__ *,char const *)"},
		{"?_AtlVerifyStackAvailable@_ATL_SAFE_ALLOCA_IMPL@ATL@@YA_NK@Z", "bool __cdecl ATL::_ATL_SAFE_ALLOCA_IMPL::_AtlVerifyStackAvailable(unsigned long)"},
		{"??0Wrapper@?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAE@ABK@Z", "public: __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>::Wrapper::Wrapper(unsigned long const &)"},
		{"??_ECComModule@ATL@@UAEPAXI@Z", "public: virtual void * __thiscall ATL::CComModule::`vector deleting destructor'(unsigned int)"},
		{"__imp__DestroyWindow@4", "import: __stdcall DestroyWindow()"},
		{"?GetModule@CAtlAllocator@@QBEPAVCAtlTraceModule@@H@Z", "public: class CAtlTraceModule * __thiscall CAtlAllocator::GetModule(int)const"},
		{"__imp__InterlockedIncrement@4", "import: __stdcall InterlockedIncrement()"},
		{"__imp__SendMessageA@16", "import: __stdcall SendMessageA()"},
		{"_CLSID_PersistPropset", "CLSID_PersistPropset"},
		{"_IID_IRegistrar", "IID_IRegistrar"},
		{"_GUID_CHECKVALUEEXCLUSIVE", "GUID_CHECKVALUEEXCLUSIVE"},
		{"_GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR", "GUID_DEVINTERFACE_SERENUM_BUS_ENUMERATOR"},
		{"?atlTraceISAPI@ATL@@3VCTraceCategory@1@A", "class ATL::CTraceCategory ATL::atlTraceISAPI"}
	};

	unsigned count = sizeof(aSymNames)/sizeof(_SymNames);
	size_t i;
    for(i=0; i<count; i++)
    {
		printf(".");
		fflush(stdout);

        std::string sDecoratedName = aSymNames[i].szDecorated;
        std::string sCorrectUndecoratedName = aSymNames[i].szUndecorated;
        std::string sUndecoratedName;
        bool bUndecorate = CPdbSymbol::UndecorateSymbolName(sDecoratedName, sUndecoratedName, false);
		TEST_EXPECT_MSG(bUndecorate, "Error undecorating symbol: %s", sDecoratedName.c_str());
		TEST_EXPECT_MSG(sUndecoratedName == sCorrectUndecoratedName, "Undecorated name for symbol %s is incorrect: %s , while expected %s",
			sDecoratedName.c_str(), sUndecoratedName.c_str(), sCorrectUndecoratedName.c_str());
    }

	//__TEST_CLEANUP__;
}

void PdbSymbolTests::Test_UndecorateSymbolName3()
{
	FILE* f = NULL;
	int nLine = 0;

	std::wstring sFileName = Utils::GetTestDataFolder();
#ifdef _WIN32
	sFileName += L"\\sym_names.txt";
#else
	sFileName += L"//sym_names.txt";
#endif

	f =  fopen(strconv::w2a(sFileName).c_str(), "rt");
	TEST_ASSERT(f!=NULL);

	while(!feof(f))
	{
		char szBuf[4096]="";
		char* str = fgets(szBuf, 4096, f);
		if(str==NULL)
			break;

		nLine++;

		std::string sDecoratedName = str;
		trim3(sDecoratedName);

		if(sDecoratedName=="EOF")
			break;

		if(sDecoratedName.at(0)=='#')
			continue; // skip comment

		printf(".");
		fflush(stdout);

		int nPos = sDecoratedName.find(' ');
		TEST_ASSERT(nPos>0);

		std::string sCorrectUndecoratedName = sDecoratedName.substr(nPos+1);
		sDecoratedName = sDecoratedName.substr(0, nPos);

		std::string sUndecoratedName;
		bool bUndecorate = CPdbSymbol::UndecorateSymbolName(sDecoratedName, sUndecoratedName, false);
		TEST_EXPECT_MSG(bUndecorate, "Error undecorating symbol (line %d): %s", nLine, sDecoratedName.c_str());
		TEST_EXPECT_MSG(sUndecoratedName == sCorrectUndecoratedName, "Line %d: Undecorated name for symbol '%s' is incorrect: '%s' , while expected '%s'",
			nLine, sDecoratedName.c_str(), sUndecoratedName.c_str(), sCorrectUndecoratedName.c_str());
	}

	__TEST_CLEANUP__;

	if(f)
		fclose(f);
}

/*
PublicSymbol: [00025BF0][0002:0000EBF0] ?GetAllocLength@CString@WTL@@QBEHXZ(public: int __thiscall WTL::CString::GetAllocLength(void)const )
PublicSymbol: [00039EA0][0002:00022EA0] ??1?$CSimpleArray@GV?$CSimpleArrayEqualHelper@G@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<unsigned short,class ATL::CSimpleArrayEqualHelper<unsigned short>>::~CSimpleArray<unsigned short,class ATL::CSimpleArrayEqualHelper<unsigned short>>(void))
PublicSymbol: [0003A1D0][0002:000231D0] ??1?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>::~CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long>>(void))
PublicSymbol: [000211B0][0002:0000A1B0] ??1?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>::~CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *>>(void))
PublicSymbol: [00021160][0002:0000A160] ??1?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *>>::~CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *>>(void))
PublicSymbol: [0003A1B0][0002:000231B0] ??1?$CSimpleArray@UCModuleInfo@CTraceSnapshot@@V?$CSimpleArrayEqualHelper@UCModuleInfo@CTraceSnapshot@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo>>::~CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo>>(void))
PublicSymbol: [0004BF28][0003:00003F28] ?VT@?$CVarTypeInfo@PAUIUnknown@@@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<struct IUnknown *>::VT)
PublicSymbol: [00038B60][0002:00021B60] ?InternalSetAtIndex@?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@QAEXHABU_AtlUpdateUIElement@CUpdateUIBase@WTL@@@Z(public: void __thiscall ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement>>::InternalSetAtIndex(int,struct WTL::CUpdateUIBase::_AtlUpdateUIElement const &))
PublicSymbol: [00041A80][0002:0002AA80] ?__CxxUnhandledExceptionFilter@@YGJPAU_EXCEPTION_POINTERS@@@Z(long __stdcall __CxxUnhandledExceptionFilter(struct _EXCEPTION_POINTERS *))
PublicSymbol: [0002D710][0002:00016710] ?ClearReplacements@CRegObject@ATL@@UAGJXZ(public: virtual long __stdcall ATL::CRegObject::ClearReplacements(void))
PublicSymbol: [0002B2F0][0002:000142F0] ?AtlA2WHelper@@YGPA_WPA_WPBDHI@Z(wchar_t * __stdcall AtlA2WHelper(wchar_t *,char const *,int,unsigned int))
PublicSymbol: [000264C0][0002:0000F4C0] ?Release@CString@WTL@@IAEXXZ(protected: void __thiscall WTL::CString::Release(void))
PublicSymbol: [00028050][0002:00011050] ?FindKey@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QBEHABK@Z(public: int __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::FindKey(unsigned long const &)const )
PublicSymbol: [0004BF94][0003:00003F94] _IID_IAxWinHostWindow(_IID_IAxWinHostWindow)
PublicSymbol: [000524DC][0004:000004DC] ??_R0?AVCAtlModule@ATL@@@8(class ATL::CAtlModule `RTTI Type Descriptor')
PublicSymbol: [00041990][0002:0002A990] __RTC_GetErrDesc(__RTC_GetErrDesc)
PublicSymbol: [00034210][0002:0001D210] ??1?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::~CTempBuffer<char,1024,class ATL::CCRTAllocator>(void))
PublicSymbol: [00040A50][0002:00029A50] _WinMainCRTStartup(_WinMainCRTStartup)
PublicSymbol: [00027830][0002:00010830] ??1?$CTempBuffer@D$0CAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<char,512,class ATL::CCRTAllocator>::~CTempBuffer<char,512,class ATL::CCRTAllocator>(void))
PublicSymbol: [00039250][0002:00022250] _DestroyWindow@4(_DestroyWindow@4)
PublicSymbol: [00034370][0002:0001D370] ??1?$CTempBuffer@D$0BAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<char,256,class ATL::CCRTAllocator>::~CTempBuffer<char,256,class ATL::CCRTAllocator>(void))
PublicSymbol: [0004E840][0003:00006840] ??_C@_0BI@CIGMDCBH@Stack?5memory?5corruption?$AA@(`string')
PublicSymbol: [00022570][0002:0000B570] ?RemoveMessageLoop@CAppModule@WTL@@QAEHXZ(public: int __thiscall WTL::CAppModule::RemoveMessageLoop(void))
PublicSymbol: [00036FB0][0002:0001FFB0] ?SetItemData@?$CComboBoxT@VCWindow@ATL@@@WTL@@QAEHHK@Z(public: int __thiscall WTL::CComboBoxT<class ATL::CWindow>::SetItemData(int,unsigned long))
PublicSymbol: [0004946C][0003:0000146C] ??_C@_0DF@MIDIJHIL@d?3?2projects?2crashrpt?2trunk?2third@(`string')
PublicSymbol: [000208E0][0002:000098E0] ?FillRect@?$CDCT@$0A@@WTL@@QAEHPBUtagRECT@@PAUHBRUSH__@@@Z(public: int __thiscall WTL::CDCT<0>::FillRect(struct tagRECT const *,struct HBRUSH__ *))
PublicSymbol: [0003C0C0][0002:000250C0] ?CrtDbgReport@CAtlTraceModule@@QBEP6AHHPBDH00ZZXZ(public: int (__cdecl*__thiscall CAtlTraceModule::CrtDbgReport(void)const )(int,char const *,int,char const *,char const *,...))
PublicSymbol: [000390D0][0002:000220D0] _crGenerateErrorReport@4(_crGenerateErrorReport@4)
PublicSymbol: [00049B04][0003:00001B04] ??_C@_1BG@OMKLFNDM@?$AAl?$AAR?$AAe?$AAs?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA0?$AAL?$AA?$AA@(`string')
PublicSymbol: [0004C740][0003:00004740] _IID_ICursorScroll(_IID_ICursorScroll)
PublicSymbol: [0004CD58][0003:00004D58] _GUID_DEVCLASS_FSFILTER_CONTINUOUSBACKUP(_GUID_DEVCLASS_FSFILTER_CONTINUOUSBACKUP)
PublicSymbol: [00049450][0003:00001450] ??_C@_1BI@CGCHJFPG@?$AAm?$AA_?$AAh?$AAW?$AAn?$AAd?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0003C200][0002:00025200] ?wcscpy_s@Checked@ATL@@YAXPA_WIPB_W@Z(void __cdecl ATL::Checked::wcscpy_s(wchar_t *,unsigned int,wchar_t const *))
PublicSymbol: [0004C420][0003:00004420] _CLSID_LocalMachineClasses(_CLSID_LocalMachineClasses)
PublicSymbol: [000341C0][0002:0001D1C0] ??0?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::CTempBuffer<char,1024,class ATL::CCRTAllocator>(void))
PublicSymbol: [00034320][0002:0001D320] ??0?$CTempBuffer@D$0BAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<char,256,class ATL::CCRTAllocator>::CTempBuffer<char,256,class ATL::CCRTAllocator>(void))
PublicSymbol: [000277E0][0002:000107E0] ??0?$CTempBuffer@D$0CAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<char,512,class ATL::CCRTAllocator>::CTempBuffer<char,512,class ATL::CCRTAllocator>(void))
PublicSymbol: [00053AAC][0005:00000AAC] __imp___mbsrchr(__imp___mbsrchr)
PublicSymbol: [0003E670][0002:00027670] ?Close@CAtlAllocator@@QAEX_N@Z(public: void __thiscall CAtlAllocator::Close(bool))
PublicSymbol: [0004BF18][0003:00003F18] ?VT@?$CVarTypeInfo@PA_W@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<wchar_t *>::VT)
PublicSymbol: [0004B5A0][0003:000035A0] ??_C@_0BB@POOIJOEN@Creating?5key?5?$CFs?6?$AA@(`string')
PublicSymbol: [00020460][0002:00009460] ??1?$CDCT@$0A@@WTL@@QAE@XZ(public: __thiscall WTL::CDCT<0>::~CDCT<0>(void))
PublicSymbol: [0005265C][0004:0000065C] ??_R0?AUThank_you@Define_the_symbol__ATL_MIXED@@@8(struct Define_the_symbol__ATL_MIXED::Thank_you `RTTI Type Descriptor')
PublicSymbol: [00041D86][0002:0002AD86] __CrtSetCheckCount(__CrtSetCheckCount)
PublicSymbol: [0004C2A4][0003:000042A4] _CLSID_PSBindCtx(_CLSID_PSBindCtx)
PublicSymbol: [0002CC00][0002:00015C00] ?RegisterFromResource@CRegObject@ATL@@IAEJPB_WPBD1H@Z(protected: long __thiscall ATL::CRegObject::RegisterFromResource(wchar_t const *,char const *,char const *,int))
PublicSymbol: [000289B0][0002:000119B0] ??0?$CComCritSecLock@VCComCriticalSection@ATL@@@ATL@@QAE@AAVCComCriticalSection@1@_N@Z(public: __thiscall ATL::CComCritSecLock<class ATL::CComCriticalSection>::CComCritSecLock<class ATL::CComCriticalSection>(class ATL::CComCriticalSection &,bool))
PublicSymbol: [000383C0][0002:000213C0] ?Add@?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@QAEHABQAVCMessageFilter@WTL@@@Z(public: int __thiscall ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *>>::Add(class WTL::CMessageFilter * const &))
PublicSymbol: [0003E8F0][0002:000278F0] ?MaxSize@CAtlTraceProcess@@QBEKXZ(public: unsigned long __thiscall CAtlTraceProcess::MaxSize(void)const )
PublicSymbol: [0004AA68][0003:00002A68] ??_C@_0CH@CCLPBBAO@Subclassing?5through?5a?5hook?5disca@(`string')
PublicSymbol: [000526F0][0004:000006F0] ___native_dllmain_reason(___native_dllmain_reason)
PublicSymbol: [00041A68][0002:0002AA68] __lock(__lock)
PublicSymbol: [0001A4A0][0002:000034A0] ??1?$CDialogImpl@VCMainDlg@@VCWindow@ATL@@@ATL@@UAE@XZ(public: virtual __thiscall ATL::CDialogImpl<class CMainDlg,class ATL::CWindow>::~CDialogImpl<class CMainDlg,class ATL::CWindow>(void))
PublicSymbol: [00039286][0002:00022286] _InvalidateRect@12(_InvalidateRect@12)
PublicSymbol: [000538DC][0005:000008DC] __imp__GetStartupInfoW@4(__imp__GetStartupInfoW@4)
PublicSymbol: [0004DFC4][0003:00005FC4] ??_C@_1BK@GFEAMNLJ@?$AAn?$AAC?$AAo?$AAo?$AAk?$AAi?$AAe?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0002E950][0002:00017950] ?_tcsstr@@YAPADPADPBD@Z(char * __cdecl _tcsstr(char *,char const *))
PublicSymbol: [0004C81C][0003:0000481C] _CLSID_ThumbnailFCNHandler(_CLSID_ThumbnailFCNHandler)
PublicSymbol: [0004AD40][0003:00002D40] ??_C@_1BC@PMIDFEID@?$AAl?$AAp?$AAa?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [000538E0][0005:000008E0] __imp__HeapSetInformation@16(__imp__HeapSetInformation@16)
PublicSymbol: [0005247C][0004:0000047C] ??_R0?AVCComModule@ATL@@@8(class ATL::CComModule `RTTI Type Descriptor')
PublicSymbol: [0004F340][0003:00007340] ??_R1A@?0A@EA@CIdleHandler@WTL@@8(WTL::CIdleHandler::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [00023CE0][0002:0000CCE0] ?Init@CWndProcThunk@ATL@@QAEHP6GJPAUHWND__@@IIJ@ZPAX@Z(public: int __thiscall ATL::CWndProcThunk::Init(long (__stdcall*)(struct HWND__ *,unsigned int,unsigned int,long),void *))
PublicSymbol: [00053A5C][0005:00000A5C] __imp___except_handler4_common(__imp___except_handler4_common)
PublicSymbol: [0004262C][0002:0002B62C] _VirtualFree@12(_VirtualFree@12)
PublicSymbol: [000426B0][0002:0002B6B0] _UnhandledExceptionFilter@4(_UnhandledExceptionFilter@4)
PublicSymbol: [0004C54C][0003:0000454C] _CLSID_StdFont(_CLSID_StdFont)
PublicSymbol: [00042632][0002:0002B632] _VirtualAlloc@16(_VirtualAlloc@16)
PublicSymbol: [00038F60][0002:00021F60] ??1stringdispid@CComTypeInfoHolder@ATL@@QAE@XZ(public: __thiscall ATL::CComTypeInfoHolder::stringdispid::~stringdispid(void))
PublicSymbol: [00039328][0002:00022328] _DeleteObject@4(_DeleteObject@4)
PublicSymbol: [00020780][0002:00009780] ?SetBkMode@?$CDCT@$0A@@WTL@@QAEHH@Z(public: int __thiscall WTL::CDCT<0>::SetBkMode(int))
PublicSymbol: [00048D80][0003:00000D80] ??_C@_13COJANIEC@?$AA0?$AA?$AA@(`string')
PublicSymbol: [00053954][0005:00000954] __imp__SetEvent@4(__imp__SetEvent@4)
PublicSymbol: [0001C730][0002:00005730] ??0?$CDialogImpl@VCMainDlg@@VCWindow@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CDialogImpl<class CMainDlg,class ATL::CWindow>::CDialogImpl<class CMainDlg,class ATL::CWindow>(void))
PublicSymbol: [0004AB8C][0003:00002B8C] ??_C@_1BM@NFPICFEL@?$AA_?$AAp?$AAM?$AAo?$AAd?$AAu?$AAl?$AAe?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004CD44][0003:00004D44] _GUID_DEVCLASS_FSFILTER_REPLICATION(_GUID_DEVCLASS_FSFILTER_REPLICATION)
PublicSymbol: [00053CC0][0005:00000CC0] __imp__IsWindow@4(__imp__IsWindow@4)
PublicSymbol: [0004F684][0003:00007684] ??_R3CComModule@ATL@@8(ATL::CComModule::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004BFA8][0003:00003FA8] _IID_IAxWinAmbientDispatch(_IID_IAxWinAmbientDispatch)
PublicSymbol: [0004F698][0003:00007698] ??_R2CComModule@ATL@@8(ATL::CComModule::`RTTI Base Class Array')
PublicSymbol: [00053A68][0005:00000A68] __imp___unlock(__imp___unlock)
PublicSymbol: [00052428][0004:00000428] ?__LINE__Var@?1???HWTL@@YG?AVCString@1@ABV21@PBD@Z@4JA(long `class WTL::CString __stdcall WTL::operator+(class WTL::CString const &,char const *)'::`2'::__LINE__Var)
PublicSymbol: [00053078][0005:00000078] __IMPORT_DESCRIPTOR_ole32(__IMPORT_DESCRIPTOR_ole32)
PublicSymbol: [0003FE50][0002:00028E50] ??0CAtlTraceCategory@@QAE@XZ(public: __thiscall CAtlTraceCategory::CAtlTraceCategory(void))
PublicSymbol: [00053C04][0005:00000C04] __imp__GetDC@4(__imp__GetDC@4)
PublicSymbol: [0004BECC][0003:00003ECC] ?VT@?$CVarTypeInfo@PAK@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<unsigned long *>::VT)
PublicSymbol: [00028C70][0002:00011C70] ?Unlock@CComCriticalSection@ATL@@QAEJXZ(public: long __thiscall ATL::CComCriticalSection::Unlock(void))
PublicSymbol: [00052430][0004:00000430] ??_R0?AVCAtlException@ATL@@@8(class ATL::CAtlException `RTTI Type Descriptor')
PublicSymbol: [0003C230][0002:00025230] ??0?$CA2WEX@$0IA@@ATL@@QAE@PBD@Z(public: __thiscall ATL::CA2WEX<128>::CA2WEX<128>(char const *))
PublicSymbol: [00026E90][0002:0000FE90] ?Add@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAEHABKABQAVCMessageLoop@WTL@@@Z(public: int __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>>::Add(unsigned long const &,class WTL::CMessageLoop * const &))
PublicSymbol: [0004C40C][0003:0000440C] _CLSID_AllClasses(_CLSID_AllClasses)
PublicSymbol: [0004CDBC][0003:00004DBC] _GUID_DEVCLASS_FSFILTER_HSM(_GUID_DEVCLASS_FSFILTER_HSM)
PublicSymbol: [0003931C][0002:0002231C] _GetWindowRect@8(_GetWindowRect@8)
PublicSymbol: [0003937C][0002:0002237C] _RegCreateKeyExA@36(_RegCreateKeyExA@36)
PublicSymbol: [000298E0][0002:000128E0] ??$AtlMultiplyThrow@I@ATL@@YAIII@Z(unsigned int __cdecl ATL::AtlMultiplyThrow<unsigned int>(unsigned int,unsigned int))
PublicSymbol: [00029A10][0002:00012A10] _HRESULT_FROM_WIN32(_HRESULT_FROM_WIN32)
PublicSymbol: [00048B40][0003:00000B40] ??_C@_09JKPGEJCH@Interface?$AA@(`string')
PublicSymbol: [0002A340][0002:00013340] ??1CComModule@ATL@@UAE@XZ(public: virtual __thiscall ATL::CComModule::~CComModule(void))
PublicSymbol: [0003E910][0002:00027910] ?CategoryCount@CAtlTraceProcess@@QBEHXZ(public: int __thiscall CAtlTraceProcess::CategoryCount(void)const )
PublicSymbol: [000391FC][0002:000221FC] _LoadCursorA@8(_LoadCursorA@8)
PublicSymbol: [00052208][0004:00000208] ??_R0?AVCIdleHandler@WTL@@@8(class WTL::CIdleHandler `RTTI Type Descriptor')
PublicSymbol: [00053BE4][0005:00000BE4] __imp__DispatchMessageW@4(__imp__DispatchMessageW@4)
PublicSymbol: [0004995C][0003:0000195C] ??_C@_1BI@GBAJIFEH@?$AAc?$AAc?$AAh?$AAL?$AAi?$AAn?$AAk?$AA?5?$AA?$DO?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00052E8C][0004:00000E8C] __dowildcard(__dowildcard)
PublicSymbol: [0001D260][0002:00006260] ?OnDestroy@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::OnDestroy(unsigned int,unsigned int,long,int &))
PublicSymbol: [0004AD80][0003:00002D80] ??_C@_1KA@CAKCAEJM@?$AAc?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAg?$AAr?$AAa?$AAm?$AA?5?$AAf?$AAi?$AAl?$AAe?$AAs?$AA?5?$AA?$CI?$AAx?$AA8?$AA6?$AA?$CJ?$AA?2?$AAm?$AAi?$AAc?$AAr?$AAo?$AAs?$AAo?$AAf?$AAt@(`string')
PublicSymbol: [0001B8D0][0002:000048D0] ?_tcsrchr@@YAPADPADI@Z(char * __cdecl _tcsrchr(char *,unsigned int))
PublicSymbol: [0004C970][0003:00004970] _GUID_DEVCLASS_DOT4PRINT(_GUID_DEVCLASS_DOT4PRINT)
PublicSymbol: [00035110][0002:0001E110]
PublicSymbol: [00022AA0][0002:0000BAA0] ??0_U_RECT@ATL@@QAE@PAUtagRECT@@@Z(public: __thiscall ATL::_U_RECT::_U_RECT(struct tagRECT *))
PublicSymbol: [0003E050][0002:00027050] ?__AllocStdCallThunk@ATL@@YGPAXXZ(void * __stdcall ATL::__AllocStdCallThunk(void))
PublicSymbol: [0004C6B4][0003:000046B4] _GUID_FONTITALIC(_GUID_FONTITALIC)
PublicSymbol: [0004C128][0003:00004128] _IID_IDfReserved1(_IID_IDfReserved1)
PublicSymbol: [0004C150][0003:00004150] _IID_IDfReserved3(_IID_IDfReserved3)
PublicSymbol: [0004C13C][0003:0000413C] _IID_IDfReserved2(_IID_IDfReserved2)
PublicSymbol: [000361D0][0002:0001F1D0] ?OnPostCreate@CMainDlg@@QAEJIIJAAH@Z(public: long __thiscall CMainDlg::OnPostCreate(unsigned int,unsigned int,long,int &))
PublicSymbol: [00049D60][0003:00001D60] ??_C@_1BI@KCHMLGAI@?$AAm?$AA_?$AAh?$AAK?$AAe?$AAy?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00052E90][0004:00000E90] __newmode(__newmode)
PublicSymbol: [0004C8E4][0003:000048E4] _GUID_DEVCLASS_BLUETOOTH(_GUID_DEVCLASS_BLUETOOTH)
PublicSymbol: [000390A0][0002:000220A0] _crAddScreenshot2@8(_crAddScreenshot2@8)
PublicSymbol: [0003915A][0002:0002215A] _InterlockedDecrement@4(_InterlockedDecrement@4)
PublicSymbol: [000392B6][0002:000222B6] _SetWindowPos@28(_SetWindowPos@28)
PublicSymbol: [0004EA78][0003:00006A78] ??_C@_1FM@DNCABAP@?$AAS?$AAO?$AAF?$AAT?$AAW?$AAA?$AAR?$AAE?$AA?2?$AAM?$AAi?$AAc?$AAr?$AAo?$AAs?$AAo?$AAf?$AAt?$AA?2?$AAV?$AAi?$AAs?$AAu?$AAa?$AAl?$AAS?$AAt?$AAu?$AAd?$AAi?$AAo?$AA?2@(`string')
PublicSymbol: [0004B258][0003:00003258] ??_C@_0BN@HJIJAHEL@HKeyFromString?5failed?5on?5?$CFs?6?$AA@(`string')
PublicSymbol: [00026700][0002:0000F700] ?_GetEmptyString@CString@WTL@@KGABV12@XZ(protected: static class WTL::CString const & __stdcall WTL::CString::_GetEmptyString(void))
PublicSymbol: [00048BB8][0003:00000BB8] ??_C@_01CPLAODJH@S?$AA@(`string')
PublicSymbol: [0004270A][0002:0002B70A] _CoReleaseMarshalData@4(_CoReleaseMarshalData@4)
PublicSymbol: [000538FC][0005:000008FC] __imp__GetCurrentThread@0(__imp__GetCurrentThread@0)
PublicSymbol: [00052848][0004:00000848] ?_Module@@3VCAppModule@WTL@@A(class WTL::CAppModule _Module)
PublicSymbol: [00049D40][0003:00001D40] ??_C@_1BK@NNCNHJFG@?$AAp?$AAn?$AAC?$AAh?$AAa?$AAr?$AAs?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00053908][0005:00000908] __imp__GetSystemInfo@4(__imp__GetSystemInfo@4)
PublicSymbol: [00042698][0002:0002B698] _Sleep@4(_Sleep@4)
PublicSymbol: [0003936A][0002:0002236A] _RegQueryValueExA@24(_RegQueryValueExA@24)
PublicSymbol: [0002BC60][0002:00014C60] ??_ECRegObject@ATL@@UAEPAXI@Z(public: virtual void * __thiscall ATL::CRegObject::`vector deleting destructor'(unsigned int))
PublicSymbol: [000344D0][0002:0001D4D0] ??1?$CTempBuffer@E$0BAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>::~CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>(void))
PublicSymbol: [00052844][0004:00000844] ?g_hWorkingThread@@3PAXA(void * g_hWorkingThread)
PublicSymbol: [000525C8][0004:000005C8] ?__LINE__Var@?1??RegCreateKeyExA@CAtlTransactionManager@ATL@@QAEJPAUHKEY__@@PBDKPADKKQAU_SECURITY_ATTRIBUTES@@PAPAU4@PAK@Z@4JA(long `public: long __thiscall ATL::CAtlTransactionManager::RegCreateKeyExA(struct HKEY__ *,char const *,unsigned long,char *,unsigned long,unsigned long,struct _SECURITY_ATTRIBUTES * const,struct HKEY__ * *,unsigned long *)'::`2'::__LINE__Var)
PublicSymbol: [00022FE0][0002:0000BFE0] ?GetWindowTextA@CWindow@ATL@@QBEHPADH@Z(public: int __thiscall ATL::CWindow::GetWindowTextA(char *,int)const )
PublicSymbol: [000391A8][0002:000221A8] _LoadLibraryExA@12(_LoadLibraryExA@12)
PublicSymbol: [0003BA60][0002:00024A60] _AtlTraceGetModuleInfo@12(_AtlTraceGetModuleInfo@12)
PublicSymbol: [00048BA8][0003:00000BA8] ??_C@_03LAIAPFCB@Val?$AA@(`string')
PublicSymbol: [0004F338][0003:00007338] ??_R2CIdleHandler@WTL@@8(WTL::CIdleHandler::`RTTI Base Class Array')
PublicSymbol: [0004F324][0003:00007324] ??_R3CIdleHandler@WTL@@8(WTL::CIdleHandler::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [00036370][0002:0001F370] ?DoCrash@CMainDlg@@QAEXXZ(public: void __thiscall CMainDlg::DoCrash(void))
PublicSymbol: [00028AA0][0002:00011AA0] ?Lock@?$CComCritSecLock@VCComCriticalSection@ATL@@@ATL@@QAEJXZ(public: long __thiscall ATL::CComCritSecLock<class ATL::CComCriticalSection>::Lock(void))
PublicSymbol: [0001C6E0][0002:000056E0]
PublicSymbol: [00041C50][0002:0002AC50] ___security_init_cookie(___security_init_cookie)
PublicSymbol: [00041E10][0002:0002AE10] __FindPESection(__FindPESection)
PublicSymbol: [00034480][0002:0001D480] ??0?$CTempBuffer@E$0BAA@VCCRTAllocator@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>(void))
PublicSymbol: [0001B760][0002:00004760] ??1CrAutoInstallHelper@@QAE@XZ(public: __thiscall CrAutoInstallHelper::~CrAutoInstallHelper(void))
PublicSymbol: [00039388][0002:00022388] _RegSetValueExA@24(_RegSetValueExA@24)
PublicSymbol: [0004CC90][0003:00004C90] _GUID_DEVCLASS_UNKNOWN(_GUID_DEVCLASS_UNKNOWN)
PublicSymbol: [00040460][0002:00029460]
PublicSymbol: [000526A0][0004:000006A0] ?g_pszAllocFileMapName@@3PBDB(char const * const g_pszAllocFileMapName)
PublicSymbol: [0004C358][0003:00004358] _CLSID_ManualResetEvent(_CLSID_ManualResetEvent)
PublicSymbol: [0004CEAC][0003:00004EAC] _GUID_DEVINTERFACE_TAPE(_GUID_DEVINTERFACE_TAPE)
PublicSymbol: [00053894][0005:00000894] __imp__InitializeCriticalSectionAndSpinCount@8(__imp__InitializeCriticalSectionAndSpinCount@8)
PublicSymbol: [0003EAC0][0002:00027AC0] ?FindModule@CAtlAllocator@@QBE_NPB_WPAI@Z(public: bool __thiscall CAtlAllocator::FindModule(wchar_t const *,unsigned int *)const )
PublicSymbol: [0003B530][0002:00024530] _AtlTraceOpenProcess@4(_AtlTraceOpenProcess@4)
PublicSymbol: [00052420][0004:00000420] ?__LINE__Var@?1??strcpyA_x@SecureHelper@WTL@@YAXPADIPBD@Z@4JA(long `void __cdecl WTL::SecureHelper::strcpyA_x(char *,unsigned int,char const *)'::`2'::__LINE__Var)
PublicSymbol: [0004EA30][0003:00006A30] ??_C@_0BB@KCIACLNC@PDBOpenValidate5?$AA@(`string')
PublicSymbol: [00053B04][0005:00000B04] __imp___invoke_watson(__imp___invoke_watson)
PublicSymbol: [0002C300][0002:00015300] ?Add@CExpansionVector@ATL@@QAEHPBDPB_W@Z(public: int __thiscall ATL::CExpansionVector::Add(char const *,wchar_t const *))
PublicSymbol: [0004CBA0][0003:00004BA0] _GUID_DEVCLASS_PRINTER(_GUID_DEVCLASS_PRINTER)
PublicSymbol: [00053980][0005:00000980] __imp__EnterCriticalSection@4(__imp__EnterCriticalSection@4)
PublicSymbol: [0004C600][0003:00004600] _GUID_YPOS(_GUID_YPOS)
PublicSymbol: [0004BB9C][0003:00003B9C] ??_C@_0O@NFIGGCAN@Worker?5thread?$AA@(`string')
PublicSymbol: [000390FA][0002:000220FA] _WaitForSingleObject@8(_WaitForSingleObject@8)
PublicSymbol: [0004E6A4][0003:000066A4] ??_C@_0BC@KIBAILCI@?$CFs?$CFs?$CFp?$CFs?$CFld?$CFs?$CFd?$CFs?$AA@(`string')
PublicSymbol: [00020C50][0002:00009C50] ?Open@CRegKey@ATL@@QAEJPAUHKEY__@@PBDK@Z(public: long __thiscall ATL::CRegKey::Open(struct HKEY__ *,char const *,unsigned long))
PublicSymbol: [00048F90][0003:00000F90] ??_C@_1EI@ONMMIIEG@?$AAg?$AA_?$AAC?$AAr?$AAa?$AAs?$AAh?$AAT?$AAh?$AAr?$AAe?$AAa?$AAd?$AAI?$AAn?$AAf?$AAo?$AA?4?$AAm?$AA_?$AAh?$AAW?$AAa?$AAk?$AAe?$AAU?$AAp?$AAE?$AAv?$AAe?$AAn?$AAt@(`string')
PublicSymbol: [000426DA][0002:0002B6DA] _VirtualQuery@12(_VirtualQuery@12)
PublicSymbol: [0004C588][0003:00004588] _GUID_COLOR(_GUID_COLOR)
PublicSymbol: [0004DBD0][0003:00005BD0] ??_C@_0EB@NAGECGJC@ERROR?5?3?5Unable?5to?5lock?5critical?5@(`string')
PublicSymbol: [0003917E][0002:0002217E] _GetLastError@0(_GetLastError@0)
PublicSymbol: [00053A2C][0005:00000A2C] __imp__exit(__imp__exit)
PublicSymbol: [0004D320][0003:00005320] ??_C@_1HA@EFGOBFOA@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [0002A3E0][0002:000133E0] ?UpdateRegistryFromResourceD@CComModule@ATL@@UAGJIHPAU_ATL_REGMAP_ENTRY@2@@Z(public: virtual long __stdcall ATL::CComModule::UpdateRegistryFromResourceD(unsigned int,int,struct ATL::_ATL_REGMAP_ENTRY *))
PublicSymbol: [000486C4][0003:000006C4] ___xi_a(___xi_a)
PublicSymbol: [00053B00][0005:00000B00] __imp__wcscpy(__imp__wcscpy)
PublicSymbol: [0004BFF8][0003:00003FF8] _IID_IAxWinAmbientDispatchEx(_IID_IAxWinAmbientDispatchEx)
PublicSymbol: [00048000][0003:00000000] ___xc_a(___xc_a)
PublicSymbol: [00020510][0002:00009510] ?IsNotifyButton@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0>>::IsNotifyButton(void)const )
PublicSymbol: [000485C0][0003:000005C0] ___xc_z(___xc_z)
PublicSymbol: [00036CA0][0002:0001FCA0] ?OnHelpAbout@CMainDlg@@QAEJGGPAUHWND__@@AAH@Z(public: long __thiscall CMainDlg::OnHelpAbout(unsigned short,unsigned short,struct HWND__ *,int &))
PublicSymbol: [0004C5EC][0003:000045EC] _GUID_XPOS(_GUID_XPOS)
PublicSymbol: [0004C45C][0003:0000445C] _CLSID_GlobalOptions(_CLSID_GlobalOptions)
PublicSymbol: [000489D0][0003:000009D0] ___xi_z(___xi_z)
PublicSymbol: [0004F534][0003:00007534] ??_R2?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@8(WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::`RTTI Base Class Array')
PublicSymbol: [00049618][0003:00001618] ??_C@_1GK@HEFIAKCC@?$AAd?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAj?$AAe?$AAc?$AAt?$AAs?$AA?2?$AAc?$AAr?$AAa?$AAs?$AAh?$AAr?$AAp?$AAt?$AA?2?$AAt?$AAr?$AAu?$AAn?$AAk?$AA?2?$AAt?$AAh?$AAi?$AAr?$AAd@(`string')
PublicSymbol: [0004F520][0003:00007520] ??_R3?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@8(WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [00042012][0002:0002B012] __crt_debugger_hook(__crt_debugger_hook)
PublicSymbol: [0004CB14][0003:00004B14] _GUID_DEVCLASS_NETCLIENT(_GUID_DEVCLASS_NETCLIENT)
PublicSymbol: [0005294C][0004:0000094C] ?atlTraceString@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceString)
PublicSymbol: [0004D82C][0003:0000582C] __GUID_b196b284_bab4_101a_b69c_00aa00341d07(__GUID_b196b284_bab4_101a_b69c_00aa00341d07)
PublicSymbol: [0004CAC4][0003:00004AC4] _GUID_DEVCLASS_MTD(_GUID_DEVCLASS_MTD)
PublicSymbol: [0004CF74][0003:00004F74] ??_C@_0BA@MKMIKKJB@atlTraceGeneral?$AA@(`string')
PublicSymbol: [00027900][0002:00010900] ??0?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement> >::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement> >(void))
PublicSymbol: [000528F0][0004:000008F0] ?_pPerfUnRegFunc@ATL@@3P6AJXZA(long (__cdecl* ATL::_pPerfUnRegFunc)(void))
PublicSymbol: [00036D00][0002:0001FD00] ??4?$CComboBoxT@VCWindow@ATL@@@WTL@@QAEAAV01@PAUHWND__@@@Z(public: class WTL::CComboBoxT<class ATL::CWindow> & __thiscall WTL::CComboBoxT<class ATL::CWindow>::operator=(struct HWND__ *))
PublicSymbol: [00053CD0][0005:00000CD0] USER32_NULL_THUNK_DATA(USER32_NULL_THUNK_DATA)
PublicSymbol: [0001A4F0][0002:000034F0] ??1?$CUpdateUI@VCMainDlg@@@WTL@@QAE@XZ(public: __thiscall WTL::CUpdateUI<class CMainDlg>::~CUpdateUI<class CMainDlg>(void))
PublicSymbol: [0005373C][0005:0000073C] __imp__RegOpenKeyExA@20(__imp__RegOpenKeyExA@20)
PublicSymbol: [000493D0][0003:000013D0] ??_C@_1GK@IFDJOMEK@?$AAd?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAj?$AAe?$AAc?$AAt?$AAs?$AA?2?$AAc?$AAr?$AAa?$AAs?$AAh?$AAr?$AAp?$AAt?$AA?2?$AAt?$AAr?$AAu?$AAn?$AAk?$AA?2?$AAt?$AAh?$AAi?$AAr?$AAd@(`string')
PublicSymbol: [0004BF38][0003:00003F38] ?VT@?$CVarTypeInfo@PAUIDispatch@@@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<struct IDispatch *>::VT)
PublicSymbol: [00053A14][0005:00000A14] __imp___crt_debugger_hook(__imp___crt_debugger_hook)
PublicSymbol: [000538A4][0005:000008A4] __imp__LoadResource@8(__imp__LoadResource@8)
PublicSymbol: [0002DC40][0002:00016C40] ?HKeyFromString@CRegParser@ATL@@KAPAUHKEY__@@PAD@Z(protected: static struct HKEY__ * __cdecl ATL::CRegParser::HKeyFromString(char *))
PublicSymbol: [0004BE64][0003:00003E64] ?VT@?$CVarTypeInfo@PAD@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<char *>::VT)
PublicSymbol: [00052938][0004:00000938] ?atlTraceAllocation@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceAllocation)
PublicSymbol: [00053848][0005:00000848] __imp__GetObjectType@4(__imp__GetObjectType@4)
PublicSymbol: [0004DD4C][0003:00005D4C] ??_C@_1BC@PMINONJG@?$AAb?$AAS?$AAu?$AAc?$AAc?$AAe?$AAs?$AAs?$AA?$AA@(`string')
PublicSymbol: [000406D0][0002:000296D0] __except_handler4(__except_handler4)
PublicSymbol: [0004C7F4][0003:000047F4] _IID_IBoundObjectSite(_IID_IBoundObjectSite)
PublicSymbol: [000215D0][0002:0000A5D0] ?Run@CMessageLoop@WTL@@QAEHXZ(public: int __thiscall WTL::CMessageLoop::Run(void))
PublicSymbol: [00029890][0002:00012890] ??1?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement> >::~CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement> >(void))
PublicSymbol: [000392C2][0002:000222C2] _BeginPaint@8(_BeginPaint@8)
PublicSymbol: [00053AD8][0005:00000AD8] __imp___CxxThrowException@8(__imp___CxxThrowException@8)
PublicSymbol: [0001C910][0002:00005910] ??0?$CUpdateUI@VCMainDlg@@@WTL@@QAE@XZ(public: __thiscall WTL::CUpdateUI<class CMainDlg>::CUpdateUI<class CMainDlg>(void))
PublicSymbol: [00034D60][0002:0001DD60] ?AllocateHeap@?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@AAEXI@Z(private: void __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::AllocateHeap(unsigned int))
PublicSymbol: [00053994][0005:00000994] __imp__FlushInstructionCache@12(__imp__FlushInstructionCache@12)
PublicSymbol: [00053CA4][0005:00000CA4] __imp__GetCursorPos@4(__imp__GetCursorPos@4)
PublicSymbol: [000403A0][0002:000293A0] ??0type_info@@AAE@ABV0@@Z(private: __thiscall type_info::type_info(class type_info const &))
PublicSymbol: [00029A80][0002:00012A80]
PublicSymbol: [0002E170][0002:00017170] ?EndOfVar@CRegParser@ATL@@IAEHXZ(protected: int __thiscall ATL::CRegParser::EndOfVar(void))
PublicSymbol: [000391BA][0002:000221BA] _DefWindowProcA@16(_DefWindowProcA@16)
PublicSymbol: [000342B0][0002:0001D2B0] ?Allocate@?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@QAEPADI@Z(public: char * __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::Allocate(unsigned int))
PublicSymbol: [00033730][0002:0001C730] ??0CAtlModule@ATL@@QAE@XZ(public: __thiscall ATL::CAtlModule::CAtlModule(void))
PublicSymbol: [00048B74][0003:00000B74] ??_C@_05GIBEAPNE@CLSID?$AA@(`string')
PublicSymbol: [0003913C][0002:0002213C] _GetCurrentThreadId@0(_GetCurrentThreadId@0)
PublicSymbol: [00034EE0][0002:0001DEE0] ??0?$Wrapper@PAD@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QAE@ABQAD@Z(public: __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Wrapper<char *>::Wrapper<char *>(char * const &))
PublicSymbol: [0004F214][0003:00007214] ??_R1A@?0A@EA@?$CUpdateUI@VCMainDlg@@@WTL@@8(WTL::CUpdateUI<class CMainDlg>::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0004F60C][0003:0000760C] ??_R3CAppModule@WTL@@8(WTL::CAppModule::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [000525B8][0004:000005B8] ?__LINE__Var@?1??DeleteSubKey@CRegKey@ATL@@QAEJPBD@Z@4JA(long `public: long __thiscall ATL::CRegKey::DeleteSubKey(char const *)'::`2'::__LINE__Var)
PublicSymbol: [0004F620][0003:00007620] ??_R2CAppModule@WTL@@8(WTL::CAppModule::`RTTI Base Class Array')
PublicSymbol: [0004E728][0003:00006728] ??_C@_07DFDJCKFN@?6Size?3?5?$AA@(`string')
PublicSymbol: [0004AF60][0003:00002F60] ??_C@_1KA@BFHPHMDO@?$AAc?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAg?$AAr?$AAa?$AAm?$AA?5?$AAf?$AAi?$AAl?$AAe?$AAs?$AA?5?$AA?$CI?$AAx?$AA8?$AA6?$AA?$CJ?$AA?2?$AAm?$AAi?$AAc?$AAr?$AAo?$AAs?$AAo?$AAf?$AAt@(`string')
PublicSymbol: [00049B20][0003:00001B20] ??_C@_1KA@IOIDCFGI@?$AAc?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAg?$AAr?$AAa?$AAm?$AA?5?$AAf?$AAi?$AAl?$AAe?$AAs?$AA?5?$AA?$CI?$AAx?$AA8?$AA6?$AA?$CJ?$AA?2?$AAm?$AAi?$AAc?$AAr?$AAo?$AAs?$AAo?$AAf?$AAt@(`string')
PublicSymbol: [00025FF0][0002:0000EFF0] ?ConcatInPlace@CString@WTL@@IAEXHPBD@Z(protected: void __thiscall WTL::CString::ConcatInPlace(int,char const *))
PublicSymbol: [000404A6][0002:000294A6] ??2@YAPAXI@Z(void * __cdecl operator new(unsigned int))
PublicSymbol: [0004CB28][0003:00004B28] _GUID_DEVCLASS_NETSERVICE(_GUID_DEVCLASS_NETSERVICE)
PublicSymbol: [000205C0][0002:000095C0] ?AddCreateWndData@CAtlWinModule@ATL@@QAEXPAU_AtlCreateWndData@2@PAX@Z(public: void __thiscall ATL::CAtlWinModule::AddCreateWndData(struct ATL::_AtlCreateWndData *,void *))
PublicSymbol: [00048B7C][0003:00000B7C] ??_C@_05PDIKPLII@AppID?$AA@(`string')
PublicSymbol: [0004BF44][0003:00003F44] ?pmField@?$CVarTypeInfo@PAPAUIDispatch@@@ATL@@2QQtagVARIANT@@PAPAUIDispatch@@Q3@(public: static struct IDispatch * * tagVARIANT::* ATL::CVarTypeInfo<struct IDispatch * *>::pmField)
PublicSymbol: [000538B8][0005:000008B8] __imp__VirtualQuery@12(__imp__VirtualQuery@12)
PublicSymbol: [000352A0][0002:0001E2A0] ?test_generate_report@@YAXXZ(void __cdecl test_generate_report(void))
PublicSymbol: [0004E774][0003:00006774] ??_C@_09FOPFMPEJ@wsprintfA?$AA@(`string')
PublicSymbol: [000275B0][0002:000105B0] ??0?$CDCT@$00@WTL@@QAE@PAUHDC__@@@Z(public: __thiscall WTL::CDCT<1>::CDCT<1>(struct HDC__ *))
PublicSymbol: [00039370][0002:00022370] _RegDeleteKeyA@8(_RegDeleteKeyA@8)
PublicSymbol: [00028610][0002:00011610] ?DeleteObject@?$CFontT@$0A@@WTL@@QAEHXZ(public: int __thiscall WTL::CFontT<0>::DeleteObject(void))
PublicSymbol: [0004C040][0003:00004040] _IID_IPrintDialogCallback(_IID_IPrintDialogCallback)
PublicSymbol: [0004F208][0003:00007208] ??_R2?$CUpdateUI@VCMainDlg@@@WTL@@8(WTL::CUpdateUI<class CMainDlg>::`RTTI Base Class Array')
PublicSymbol: [0004F1F4][0003:000071F4] ??_R3?$CUpdateUI@VCMainDlg@@@WTL@@8(WTL::CUpdateUI<class CMainDlg>::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004BDFC][0003:00003DFC] ?_Max@?$AtlLimits@H@ATL@@2HB(public: static int const ATL::AtlLimits<int>::_Max)
PublicSymbol: [0004BE04][0003:00003E04] ?_Max@?$AtlLimits@I@ATL@@2IB(public: static unsigned int const ATL::AtlLimits<unsigned int>::_Max)
PublicSymbol: [0004BE0C][0003:00003E0C] ?_Max@?$AtlLimits@J@ATL@@2JB(public: static long const ATL::AtlLimits<long>::_Max)
PublicSymbol: [0004BE14][0003:00003E14] ?_Max@?$AtlLimits@K@ATL@@2KB(public: static unsigned long const ATL::AtlLimits<unsigned long>::_Max)
PublicSymbol: [00040ED2][0002:00029ED2] _wcscpy(_wcscpy)
PublicSymbol: [000392EC][0002:000222EC] _LoadImageA@24(_LoadImageA@24)
PublicSymbol: [0004C254][0003:00004254] _CLSID_PSClassObject(_CLSID_PSClassObject)
PublicSymbol: [00027C60][0002:00010C60] ?AtlCrtErrorCheck@ATL@@YAHH@Z(int __cdecl ATL::AtlCrtErrorCheck(int))
PublicSymbol: [000236A0][0002:0000C6A0] ?IsWindowEnabled@CWindow@ATL@@QBEHXZ(public: int __thiscall ATL::CWindow::IsWindowEnabled(void)const )
PublicSymbol: [00033F40][0002:0001CF40] ?Attach@?$CAutoVectorPtr@D@ATL@@QAEXPAD@Z(public: void __thiscall ATL::CAutoVectorPtr<char>::Attach(char *))
PublicSymbol: [0004BD88][0003:00003D88] ??_C@_1CO@EBHKNFJ@?$AA?3?$AA?3?$AAI?$AAs?$AAW?$AAi?$AAn?$AAd?$AAo?$AAw?$AA?$CI?$AAh?$AAW?$AAn?$AAd?$AAC?$AAe?$AAn?$AAt?$AAe?$AAr?$AA?$CJ?$AA?$AA@(`string')
PublicSymbol: [00041D6E][0002:0002AD6E] __XcptFilter(__XcptFilter)
PublicSymbol: [000391D2][0002:000221D2] _CreateDialogParamA@20(_CreateDialogParamA@20)
PublicSymbol: [00039352][0002:00022352] _SetTextColor@8(_SetTextColor@8)
PublicSymbol: [0004F4D8][0003:000074D8] ??_R1A@?0A@EA@CHyperLink@WTL@@8(WTL::CHyperLink::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0004C77C][0003:0000477C] _IID_ICursorFind(_IID_ICursorFind)
PublicSymbol: [00041BC0][0002:0002ABC0] __setargv(__setargv)
PublicSymbol: [00053990][0005:00000990] __imp__GetCurrentProcess@0(__imp__GetCurrentProcess@0)
PublicSymbol: [0001A430][0002:00003430] ??_ECMainDlg@@UAEPAXI@Z(public: virtual void * __thiscall CMainDlg::`vector deleting destructor'(unsigned int))
PublicSymbol: [0004ABB4][0003:00002BB4] ??_7CComModule@ATL@@6B@(const ATL::CComModule::`vftable')
PublicSymbol: [00040760][0002:00029760] ??_M@YGXPAXIHP6EX0@Z@Z(void __stdcall `eh vector destructor iterator'(void *,unsigned int,int,void (__thiscall*)(void *)))
PublicSymbol: [000528B4][0004:000008B4] ?map@?1??VTFromRegType@CRegParser@ATL@@KAHPBDAAG@Z@4QBUtypemap@?1??123@KAH01@Z@B(struct `protected: static int __cdecl ATL::CRegParser::VTFromRegType(char const *,unsigned short &)'::`2'::typemap const * const `protected: static int __cdecl ATL::CRegParser::VTFromRegType(char const *,unsigned short &)'::`2'::map)
PublicSymbol: [00039142][0002:00022142] _DeleteCriticalSection@4(_DeleteCriticalSection@4)
PublicSymbol: [00039900][0002:00022900] ?Add@?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAEHABQAUHINSTANCE__@@@Z(public: int __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >::Add(struct HINSTANCE__ * const &))
PublicSymbol: [000537F0][0005:000007F0] __imp__crAddFile2A@16(__imp__crAddFile2A@16)
PublicSymbol: [0003F660][0002:00028660] ?Add@?$CSimpleArray@UCModuleInfo@CTraceSnapshot@@V?$CSimpleArrayEqualHelper@UCModuleInfo@CTraceSnapshot@@@ATL@@@ATL@@QAEHABUCModuleInfo@CTraceSnapshot@@@Z(public: int __thiscall ATL::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo> >::Add(struct CTraceSnapshot::CModuleInfo const &))
PublicSymbol: [00048B28][0003:00000B28] ??_C@_03DDIMFGBA@SAM?$AA@(`string')
PublicSymbol: [00029990][0002:00012990] ??$AtlMultiply@I@ATL@@YAJPAIII@Z(long __cdecl ATL::AtlMultiply<unsigned int>(unsigned int *,unsigned int,unsigned int))
PublicSymbol: [0004A594][0003:00002594] ??_C@_1BE@ODLIIOBC@?$AAn?$AAL?$AAe?$AAn?$AA?5?$AA?$DO?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [000526B0][0004:000006B0] ___security_cookie_complement(___security_cookie_complement)
PublicSymbol: [00053AB0][0005:00000AB0] __imp_??_V@YAXPAX@Z(__imp_??_V@YAXPAX@Z)
PublicSymbol: [0004CAEC][0003:00004AEC] _GUID_DEVCLASS_MULTIPORTSERIAL(_GUID_DEVCLASS_MULTIPORTSERIAL)
PublicSymbol: [00032380][0002:0001B380] ?SkipAssignment@CRegParser@ATL@@IAEJPAD@Z(protected: long __thiscall ATL::CRegParser::SkipAssignment(char *))
PublicSymbol: [0004A0E8][0003:000020E8] ??_C@_1EM@NHPDCFJE@?$AAs?$AAt?$AAr?$AAi?$AAn?$AAg?$AAS?$AAr?$AAc?$AA?4?$AAG?$AAe?$AAt?$AAD?$AAa?$AAt?$AAa?$AA?$CI?$AA?$CJ?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA_?$AAa?$AAt?$AAl?$AAt?$AAm?$AAp?$AAD?$AAa@(`string')
PublicSymbol: [0004CD94][0003:00004D94] _GUID_DEVCLASS_FSFILTER_SYSTEMRECOVERY(_GUID_DEVCLASS_FSFILTER_SYSTEMRECOVERY)
PublicSymbol: [00053BD0][0005:00000BD0] __imp__MsgWaitForMultipleObjectsEx@20(__imp__MsgWaitForMultipleObjectsEx@20)
PublicSymbol: [000426E0][0002:0002B6E0] _DispatchMessageW@4(_DispatchMessageW@4)
PublicSymbol: [0001A6B0][0002:000036B0] ?ProcessWindowMessage@CAboutDlg@@UAEHPAUHWND__@@IIJAAJK@Z(public: virtual int __thiscall CAboutDlg::ProcessWindowMessage(struct HWND__ *,unsigned int,unsigned int,long,long &,unsigned long))
PublicSymbol: [0004CA74][0003:00004A74] _GUID_DEVCLASS_MEDIUM_CHANGER(_GUID_DEVCLASS_MEDIUM_CHANGER)
PublicSymbol: [0001A590][0002:00003590] ??0CIdleHandler@WTL@@QAE@XZ(public: __thiscall WTL::CIdleHandler::CIdleHandler(void))
PublicSymbol: [00022080][0002:0000B080] ?Term@CComCriticalSection@ATL@@QAEJXZ(public: long __thiscall ATL::CComCriticalSection::Term(void))
PublicSymbol: [0004BC14][0003:00003C14] ??_7CDocumentDlg@@6B@(const CDocumentDlg::`vftable')
PublicSymbol: [0004F814][0003:00007814] ??_R1A@?0A@EA@IRegistrarBase@@8(IRegistrarBase::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0004B50C][0003:0000350C] ??_C@_0CH@OEJDLABI@Deleting?5non?9empty?5subkey?5?$CFs?5by?5@(`string')
PublicSymbol: [00053A64][0005:00000A64] __imp___lock(__imp___lock)
PublicSymbol: [00053AB4][0005:00000AB4] __imp_??_U@YAPAXI@Z(__imp_??_U@YAPAXI@Z)
PublicSymbol: [00039232][0002:00022232] _DispatchMessageA@4(_DispatchMessageA@4)
PublicSymbol: [0004BBAC][0003:00003BAC] ??_C@_0M@MAHHBDIJ@Main?5thread?$AA@(`string')
PublicSymbol: [00023160][0002:0000C160] ?GetFont@CWindow@ATL@@QBEPAUHFONT__@@XZ(public: struct HFONT__ * __thiscall ATL::CWindow::GetFont(void)const )
PublicSymbol: [0004BF2C][0003:00003F2C] ?pmField@?$CVarTypeInfo@PAUIUnknown@@@ATL@@2QQtagVARIANT@@PAUIUnknown@@Q3@(public: static struct IUnknown * tagVARIANT::* ATL::CVarTypeInfo<struct IUnknown *>::pmField)
PublicSymbol: [00053A84][0005:00000A84] __imp__wcscpy_s(__imp__wcscpy_s)
PublicSymbol: [00053BF4][0005:00000BF4] __imp__SetWindowPos@28(__imp__SetWindowPos@28)
PublicSymbol: [00020990][0002:00009990] ?DrawFocusRect@?$CDCT@$0A@@WTL@@QAEXPBUtagRECT@@@Z(public: void __thiscall WTL::CDCT<0>::DrawFocusRect(struct tagRECT const *))
PublicSymbol: [0004DCB0][0003:00005CB0] ??_C@_19JHCPJMAL@?$AAp?$AA?$CB?$AA?$DN?$AA0?$AA?$AA@(`string')
PublicSymbol: [00052014][0004:00000014] ?rgInitData@WTL@@3PAHA(int * WTL::rgInitData)
PublicSymbol: [00034270][0002:0001D270] ??B?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@QBEPADXZ(public: __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::operator char *(void)const )
PublicSymbol: [000343D0][0002:0001D3D0] ??B?$CTempBuffer@D$0BAA@VCCRTAllocator@ATL@@@ATL@@QBEPADXZ(public: __thiscall ATL::CTempBuffer<char,256,class ATL::CCRTAllocator>::operator char *(void)const )
PublicSymbol: [00034530][0002:0001D530] ??B?$CTempBuffer@E$0BAA@VCCRTAllocator@ATL@@@ATL@@QBEPAEXZ(public: __thiscall ATL::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>::operator unsigned char *(void)const )
PublicSymbol: [0004C8D0][0003:000048D0] _GUID_DEVCLASS_BIOMETRIC(_GUID_DEVCLASS_BIOMETRIC)
PublicSymbol: [00053ABC][0005:00000ABC] __imp__free(__imp__free)
PublicSymbol: [0002A120][0002:00013120] ?GetGITPtr@CAtlModule@ATL@@UAEJPAPAUIGlobalInterfaceTable@@@Z(public: virtual long __thiscall ATL::CAtlModule::GetGITPtr(struct IGlobalInterfaceTable * *))
PublicSymbol: [00048FF8][0003:00000FF8] ??_C@_0EP@GFBNPCIB@HKEY_LOCAL_MACHINE?2Software?2Micr@(`string')
PublicSymbol: [0004A09C][0003:0000209C] ??_C@_1DM@MGMNDNC@?$AAh?$AAW?$AAn?$AAd?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA0?$AA?5?$AA?$HM?$AA?$HM?$AA?5?$AA?3?$AA?3?$AAI?$AAs?$AAW?$AAi?$AAn?$AAd?$AAo?$AAw?$AA?$CI?$AAh?$AAW?$AAn?$AAd?$AA?$CJ?$AA?$AA@(`string')
PublicSymbol: [0001A370][0002:00003370] ??1CMainDlg@@UAE@XZ(public: virtual __thiscall CMainDlg::~CMainDlg(void))
PublicSymbol: [00040704][0002:00029704] _memcpy_s(_memcpy_s)
PublicSymbol: [00028260][0002:00011260] ??0?$CWindowImplRoot@VCWindow@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CWindowImplRoot<class ATL::CWindow>::CWindowImplRoot<class ATL::CWindow>(void))
PublicSymbol: [0004AA1C][0003:00002A1C] ??_C@_1BC@EMCLECFJ@?$AAm?$AA_?$AAp?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [000391A2][0002:000221A2] _FindResourceA@12(_FindResourceA@12)
PublicSymbol: [0004F2C0][0003:000072C0] ??_R3CMessageFilter@WTL@@8(WTL::CMessageFilter::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [00039C90][0002:00022C90] ??1?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >::~CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >(void))
PublicSymbol: [0004F2D4][0003:000072D4] ??_R2CMessageFilter@WTL@@8(WTL::CMessageFilter::`RTTI Base Class Array')
PublicSymbol: [00038D90][0002:00021D90] ??$?3PAVCMessageFilter@WTL@@@Wrapper@?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@SAXPAXPAPAVCMessageFilter@WTL@@@Z(public: static void __cdecl ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *> >::Wrapper::operator delete<class WTL::CMessageFilter *>(void *,class WTL::CMessageFilter * *))
PublicSymbol: [0004AA98][0003:00002A98] ??_C@_1BG@PEGMCOIG@?$AAp?$AAT?$AAh?$AAi?$AAs?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004B8C8][0003:000038C8] ??_C@_11LOCGONAA@?$AA?$AA@(`string')
PublicSymbol: [0004BAE8][0003:00003AE8] ??_C@_06PAOFCDHL@SIGINT?$AA@(`string')
PublicSymbol: [0004B440][0003:00003440] ??_C@_1EA@NHKLICFA@?$AAH?$AAK?$AAC?$AAU?$AA?$AN?$AA?6?$AA?$HL?$AA?7?$AAS?$AAo?$AAf?$AAt?$AAw?$AAa?$AAr?$AAe?$AA?$AN?$AA?6?$AA?7?$AA?$HL?$AA?$AN?$AA?6?$AA?7?$AA?7?$AAC?$AAl?$AAa?$AAs?$AAs?$AAe?$AAs?$AA?$AA@(`string')
PublicSymbol: [0004988C][0003:0000188C] ??_C@_0CO@HIOLJOEH@Software?2Microsoft?2Internet?5Expl@(`string')
PublicSymbol: [000526F4][0004:000006F4] ___native_vcclrit_reason(___native_vcclrit_reason)
PublicSymbol: [00049E78][0003:00001E78] ??_C@_0DD@OEFAJBFL@d?3?2projects?2crashrpt?2trunk?2third@(`string')
PublicSymbol: [0005289C][0004:0000089C] ?m_libid@CAtlModule@ATL@@2U_GUID@@A(public: static struct _GUID ATL::CAtlModule::m_libid)
PublicSymbol: [00029EA0][0002:00012EA0] ??_GCAppModule@WTL@@UAEPAXI@Z(public: virtual void * __thiscall WTL::CAppModule::`scalar deleting destructor'(unsigned int))
PublicSymbol: [00053D48][0005:00000D48] __imp__CoReleaseMarshalData@4(__imp__CoReleaseMarshalData@4)
PublicSymbol: [000283E0][0002:000113E0] ??0?$CFontT@$0A@@WTL@@QAE@PAUHFONT__@@@Z(public: __thiscall WTL::CFontT<0>::CFontT<0>(struct HFONT__ *))
PublicSymbol: [00039130][0002:00022130] _GetModuleHandleA@4(_GetModuleHandleA@4)
PublicSymbol: [00033200][0002:0001C200] ?ResourceUnregister@CRegObject@ATL@@QAGJPB_WI0@Z(public: long __stdcall ATL::CRegObject::ResourceUnregister(wchar_t const *,unsigned int,wchar_t const *))
PublicSymbol: [0004F098][0003:00007098] ??_R1A@?0A@EA@?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@8(ATL::CDialogImplBaseT<class ATL::CWindow>::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [00049AA0][0003:00001AA0] ??_C@_1FE@BKHNIILB@?$AAh?$AAF?$AAo?$AAn?$AAt?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA0?$AA?5?$AA?$HM?$AA?$HM?$AA?5?$AA?3?$AA?3?$AAG?$AAe?$AAt?$AAO?$AAb?$AAj?$AAe?$AAc?$AAt?$AAT?$AAy?$AAp?$AAe?$AA?$CI?$AAh?$AAF@(`string')
PublicSymbol: [000390B8][0002:000220B8] _crInstallA@4(_crInstallA@4)
PublicSymbol: [00049EE0][0003:00001EE0] ??_C@_1GG@GNJAOFIC@?$AAm?$AA_?$AAp?$AAM?$AAs?$AAg?$AAL?$AAo?$AAo?$AAp?$AAM?$AAa?$AAp?$AA?9?$AA?$DO?$AAL?$AAo?$AAo?$AAk?$AAu?$AAp?$AA?$CI?$AA?3?$AA?3?$AAG?$AAe?$AAt?$AAC?$AAu?$AAr?$AAr?$AAe@(`string')
PublicSymbol: [00048BB0][0003:00000BB0] ??_C@_01CKDDGHAB@D?$AA@(`string')
PublicSymbol: [00049BE0][0003:00001BE0] ??_C@_1CA@JAHJEEHI@?$AAh?$AAK?$AAe?$AAy?$AAP?$AAa?$AAr?$AAe?$AAn?$AAt?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00052558][0004:00000558] ??_R0?AUIRegistrarBase@@@8(struct IRegistrarBase `RTTI Type Descriptor')
PublicSymbol: [00021970][0002:0000A970] ?OnIdle@CMessageLoop@WTL@@UAEHH@Z(public: virtual int __thiscall WTL::CMessageLoop::OnIdle(int))
PublicSymbol: [00053D44][0005:00000D44] __imp__CoUninitialize@0(__imp__CoUninitialize@0)
PublicSymbol: [000398A0][0002:000228A0] ??0?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >(void))
PublicSymbol: [0003CF80][0002:00025F80] _InlineIsEqualGUID(_InlineIsEqualGUID)
PublicSymbol: [000538C8][0005:000008C8] __imp__LoadLibraryW@4(__imp__LoadLibraryW@4)
PublicSymbol: [0004CFD4][0003:00004FD4] ??_C@_0BC@DGHHNKDC@atlTraceWindowing?$AA@(`string')
PublicSymbol: [0001DBE0][0002:00006BE0] ?OnGetDlgCode@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnGetDlgCode(unsigned int,unsigned int,long,int &))
PublicSymbol: [000288B0][0002:000118B0] ?IsEqual@?$CSimpleArrayEqualHelper@K@ATL@@SA_NABK0@Z(public: static bool __cdecl ATL::CSimpleArrayEqualHelper<unsigned long>::IsEqual(unsigned long const &,unsigned long const &))
PublicSymbol: [00029FC0][0002:00012FC0] ?Increment@CComMultiThreadModel@ATL@@SGKPAJ@Z(public: static unsigned long __stdcall ATL::CComMultiThreadModel::Increment(long *))
PublicSymbol: [0003C670][0002:00025670] ?AtlAdvise@ATL@@YGJPAUIUnknown@@0ABU_GUID@@PAK@Z(long __stdcall ATL::AtlAdvise(struct IUnknown *,struct IUnknown *,struct _GUID const &,unsigned long *))
PublicSymbol: [0004C948][0003:00004948] _GUID_DEVCLASS_DISPLAY(_GUID_DEVCLASS_DISPLAY)
PublicSymbol: [0002E9B0][0002:000179B0] __tcsstr(__tcsstr)
PublicSymbol: [0004A5B0][0003:000025B0] ??_C@_1GE@MFKCFNEL@?$AAG?$AAe?$AAt?$AAD?$AAa?$AAt?$AAa?$AA?$CI?$AA?$CJ?$AA?9?$AA?$DO?$AAn?$AAD?$AAa?$AAt?$AAa?$AAL?$AAe?$AAn?$AAg?$AAt?$AAh?$AA?5?$AA?$DM?$AA?$DN?$AA?5?$AAG?$AAe?$AAt?$AAD?$AAa?$AAt@(`string')
PublicSymbol: [0004B324][0003:00003324] ??_C@_04OFOCGAOH@HKPD?$AA@(`string')
PublicSymbol: [00029DE0][0002:00012DE0] ?RegisterCategory@CTrace@ATL@@QAEKPBD@Z(public: unsigned long __thiscall ATL::CTrace::RegisterCategory(char const *))
PublicSymbol: [000529D0][0004:000009D0] ?_AtlWinModule@ATL@@3VCAtlWinModule@1@A(class ATL::CAtlWinModule ATL::_AtlWinModule)
PublicSymbol: [00053928][0005:00000928] __imp__VirtualAlloc@16(__imp__VirtualAlloc@16)
PublicSymbol: [0003B760][0002:00024760] _AtlTraceSnapshotProcess@4(_AtlTraceSnapshotProcess@4)
PublicSymbol: [0004BE6C][0003:00003E6C] ?VT@?$CVarTypeInfo@PAE@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<unsigned char *>::VT)
PublicSymbol: [0004CE34][0003:00004E34] _GUID_DEVCLASS_FSFILTER_COPYPROTECTION(_GUID_DEVCLASS_FSFILTER_COPYPROTECTION)
PublicSymbol: [00053D30][0005:00000D30] __imp__CoTaskMemRealloc@8(__imp__CoTaskMemRealloc@8)
PublicSymbol: [000530A0][0005:000000A0] __IMPORT_DESCRIPTOR_COMCTL32(__IMPORT_DESCRIPTOR_COMCTL32)
PublicSymbol: [00001000][0001:00000000] __enc$textbss$begin(__enc$textbss$begin)
PublicSymbol: [0004C18C][0003:0000418C] _IID_IProxy(_IID_IProxy)
PublicSymbol: [0004D64C][0003:0000564C] ??_C@_02DKCKIIND@?$CFs?$AA@(`string')
PublicSymbol: [00029F10][0002:00012F10] ??1CAppModule@WTL@@UAE@XZ(public: virtual __thiscall WTL::CAppModule::~CAppModule(void))
PublicSymbol: [0004BC28][0003:00003C28] ??_C@_1IA@GAGAEEDJ@?$AAd?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAj?$AAe?$AAc?$AAt?$AAs?$AA?2?$AAc?$AAr?$AAa?$AAs?$AAh?$AAr?$AAp?$AAt?$AA?2?$AAt?$AAr?$AAu?$AAn?$AAk?$AA?2?$AAr?$AAe?$AAp?$AAo?$AAr@(`string')
PublicSymbol: [0004CC40][0003:00004C40] _GUID_DEVCLASS_SMARTCARDREADER(_GUID_DEVCLASS_SMARTCARDREADER)
PublicSymbol: [00027470][0002:00010470] ??3_stdcallthunk@ATL@@SAXPAX@Z(public: static void __cdecl ATL::_stdcallthunk::operator delete(void *))
PublicSymbol: [0004D74C][0003:0000574C] ??_C@_1BC@JOPFIJLO@?$AAp?$AAP?$AAr?$AAo?$AAc?$AAe?$AAs?$AAs?$AA?$AA@(`string')
PublicSymbol: [0001B990][0002:00004990] ??0?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAE@K@Z(public: __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >(unsigned long))
PublicSymbol: [000230A0][0002:0000C0A0] ?GetWindowTextLengthA@CWindow@ATL@@QBEHXZ(public: int __thiscall ATL::CWindow::GetWindowTextLengthA(void)const )
PublicSymbol: [00049D30][0003:00001D30] ??_C@_0N@DOHINHCI@Advapi32?4dll?$AA@(`string')
PublicSymbol: [00020B00][0002:00009B00] ??0CRegKey@ATL@@QAE@PAVCAtlTransactionManager@1@@Z(public: __thiscall ATL::CRegKey::CRegKey(class ATL::CAtlTransactionManager *))
PublicSymbol: [00023A70][0002:0000CA70] ?GetDlgItem@CWindow@ATL@@QBE?AV12@H@Z(public: class ATL::CWindow __thiscall ATL::CWindow::GetDlgItem(int)const )
PublicSymbol: [000324A0][0002:0001B4A0] ??0CRegObject@ATL@@QAE@XZ(public: __thiscall ATL::CRegObject::CRegObject(void))
PublicSymbol: [0004E00C][0003:0000600C] ??_7type_info@@6B@(const type_info::`vftable')
PublicSymbol: [00030530][0002:00019530] ?Attach@CRegKey@ATL@@QAEXPAUHKEY__@@@Z(public: void __thiscall ATL::CRegKey::Attach(struct HKEY__ *))
PublicSymbol: [0004C3A8][0003:000043A8] _CLSID_StdAsyncActManager(_CLSID_StdAsyncActManager)
PublicSymbol: [00053BDC][0005:00000BDC] __imp__SetCapture@4(__imp__SetCapture@4)
PublicSymbol: [00040230][0002:00029230] @_RTC_CheckStackVars2@12(@_RTC_CheckStackVars2@12)
PublicSymbol: [00029C20][0002:00012C20] ?RemoveAll@?$CSimpleArray@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@V?$CSimpleArrayEqualHelper@U_AtlUpdateUIElement@CUpdateUIBase@WTL@@@ATL@@@ATL@@QAEXXZ(public: void __thiscall ATL::CSimpleArray<struct WTL::CUpdateUIBase::_AtlUpdateUIElement,class ATL::CSimpleArrayEqualHelper<struct WTL::CUpdateUIBase::_AtlUpdateUIElement> >::RemoveAll(void))
PublicSymbol: [000496BC][0003:000016BC] ?theMap@?1??GetUpdateUIMap@CMainDlg@@SAPBU_AtlUpdateUIMap@CUpdateUIBase@WTL@@XZ@4QBU345@B(struct WTL::CUpdateUIBase::_AtlUpdateUIMap const * const `public: static struct WTL::CUpdateUIBase::_AtlUpdateUIMap const * __cdecl CMainDlg::GetUpdateUIMap(void)'::`2'::theMap)
PublicSymbol: [000526D0][0004:000006D0] ?_RTC_ErrorLevels@@3PAHA(int * _RTC_ErrorLevels)
PublicSymbol: [00019DA0][0002:00002DA0] ??1CMessageLoop@WTL@@QAE@XZ(public: __thiscall WTL::CMessageLoop::~CMessageLoop(void))
PublicSymbol: [0003AB80][0002:00023B80] _AtlTraceGetUpdateEventNameA@4(_AtlTraceGetUpdateEventNameA@4)
PublicSymbol: [0002A6A0][0002:000136A0] ?UpdateRegistryFromResourceS@CAtlModule@ATL@@QAGJPBDHPAU_ATL_REGMAP_ENTRY@2@@Z(public: long __stdcall ATL::CAtlModule::UpdateRegistryFromResourceS(char const *,int,struct ATL::_ATL_REGMAP_ENTRY *))
PublicSymbol: [0003A430][0002:00023430] _AtlTraceRegisterCategoryA@8(_AtlTraceRegisterCategoryA@8)
PublicSymbol: [00039160][0002:00022160] _RaiseException@16(_RaiseException@16)
PublicSymbol: [0004BAD0][0003:00003AD0] ??_C@_07MJAMNEFP@SIGTERM?$AA@(`string')
PublicSymbol: [0003D9F0][0002:000269F0] ??0?$CComPtrBase@UIConnectionPoint@@@ATL@@IAE@XZ(protected: __thiscall ATL::CComPtrBase<struct IConnectionPoint>::CComPtrBase<struct IConnectionPoint>(void))
PublicSymbol: [00053C78][0005:00000C78] __imp__TranslateMessage@4(__imp__TranslateMessage@4)
PublicSymbol: [0004CEC0][0003:00004EC0] _GUID_DEVINTERFACE_WRITEONCEDISK(_GUID_DEVINTERFACE_WRITEONCEDISK)
PublicSymbol: [0004C844][0003:00004844] _GUID_DEVCLASS_1394(_GUID_DEVCLASS_1394)
PublicSymbol: [00053740][0005:00000740] __imp__RegQueryValueExA@24(__imp__RegQueryValueExA@24)
PublicSymbol: [000271B0][0002:000101B0] ??1?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@UAE@XZ(public: virtual __thiscall ATL::CDialogImplBaseT<class ATL::CWindow>::~CDialogImplBaseT<class ATL::CWindow>(void))
PublicSymbol: [0004D9B0][0003:000059B0] ??_C@_1CA@GAOMIJFK@?$AAp?$AAC?$AAo?$AAm?$AAM?$AAo?$AAd?$AAu?$AAl?$AAe?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00040710][0002:00029710] __alloca_probe(__alloca_probe)
PublicSymbol: [00033EF0][0002:0001CEF0] ??1?$CAutoVectorPtr@D@ATL@@QAE@XZ(public: __thiscall ATL::CAutoVectorPtr<char>::~CAutoVectorPtr<char>(void))
PublicSymbol: [000356C0][0002:0001E6C0] ??0CrThreadAutoInstallHelper@@QAE@K@Z(public: __thiscall CrThreadAutoInstallHelper::CrThreadAutoInstallHelper(unsigned long))
PublicSymbol: [0004FD68][0003:00007D68] ___rtc_izz(___rtc_izz)
PublicSymbol: [000220E0][0002:0000B0E0] ?AtlWinModuleTerm@ATL@@YGJPAU_ATL_WIN_MODULE70@1@PAUHINSTANCE__@@@Z(long __stdcall ATL::AtlWinModuleTerm(struct ATL::_ATL_WIN_MODULE70 *,struct HINSTANCE__ *))
PublicSymbol: [00049A8C][0003:00001A8C] ??_C@_0BB@HKOHPAJO@tooltips_class32?$AA@(`string')
PublicSymbol: [0003C270][0002:00025270] ??1?$CA2WEX@$0IA@@ATL@@QAE@XZ(public: __thiscall ATL::CA2WEX<128>::~CA2WEX<128>(void))
PublicSymbol: [0004C1A0][0003:000041A0] _IID_IEnumGeneric(_IID_IEnumGeneric)
PublicSymbol: [00040110][0002:00029110] ??3@YAXPAX@Z(void __cdecl operator delete(void *))
PublicSymbol: [0004A718][0003:00002718] ??_C@_1KI@INPNBEBP@?$AAc?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAg?$AAr?$AAa?$AAm?$AA?5?$AAf?$AAi?$AAl?$AAe?$AAs?$AA?5?$AA?$CI?$AAx?$AA8?$AA6?$AA?$CJ?$AA?2?$AAm?$AAi?$AAc?$AAr?$AAo?$AAs?$AAo?$AAf?$AAt@(`string')
PublicSymbol: [00019CE0][0002:00002CE0] ??0CMessageLoop@WTL@@QAE@XZ(public: __thiscall WTL::CMessageLoop::CMessageLoop(void))
PublicSymbol: [00036AD0][0002:0001FAD0] ?OnCancel@CDocumentDlg@@QAEJGGPAUHWND__@@AAH@Z(public: long __thiscall CDocumentDlg::OnCancel(unsigned short,unsigned short,struct HWND__ *,int &))
PublicSymbol: [0004BF4C][0003:00003F4C] ?pmField@?$CVarTypeInfo@TtagCY@@@ATL@@2QQtagVARIANT@@TtagCY@@Q3@(public: static union tagCY tagVARIANT::* ATL::CVarTypeInfo<union tagCY>::pmField)
PublicSymbol: [000245E0][0002:0000D5E0] ?Init@CToolInfo@WTL@@QAEXIPAUHWND__@@IPAUtagRECT@@PADJ@Z(public: void __thiscall WTL::CToolInfo::Init(unsigned int,struct HWND__ *,unsigned int,struct tagRECT *,char *,long))
PublicSymbol: [0004F434][0003:00007434] ??_R1A@?0A@EA@?$CDialogImpl@VCAboutDlg@@VCWindow@ATL@@@ATL@@8(ATL::CDialogImpl<class CAboutDlg,class ATL::CWindow>::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [00049740][0003:00001740] ??_C@_1CG@OFEMDDJL@?$AA?3?$AA?3?$AAI?$AAs?$AAW?$AAi?$AAn?$AAd?$AAo?$AAw?$AA?$CI?$AAm?$AA_?$AAh?$AAW?$AAn?$AAd?$AA?$CJ?$AA?$AA@(`string')
PublicSymbol: [0004CE20][0003:00004E20] _GUID_DEVCLASS_FSFILTER_SECURITYENHANCER(_GUID_DEVCLASS_FSFILTER_SECURITYENHANCER)
PublicSymbol: [00038FB0][0002:00021FB0] ??1CComBSTR@ATL@@QAE@XZ(public: __thiscall ATL::CComBSTR::~CComBSTR(void))
PublicSymbol: [000526A4][0004:000006A4] ?g_pszKernelObjFmt@@3PBDB(char const * const g_pszKernelObjFmt)
PublicSymbol: [00053D4C][0005:00000D4C] __imp__CoMarshalInterface@24(__imp__CoMarshalInterface@24)
PublicSymbol: [00033EA0][0002:0001CEA0] ??0?$CAutoVectorPtr@D@ATL@@QAE@XZ(public: __thiscall ATL::CAutoVectorPtr<char>::CAutoVectorPtr<char>(void))
PublicSymbol: [0001AC80][0002:00003C80] ??_GCAboutDlg@@UAEPAXI@Z(public: virtual void * __thiscall CAboutDlg::`scalar deleting destructor'(unsigned int))
PublicSymbol: [00048B8C][0003:00000B8C] ??_C@_08KAAPDIAN@NoRemove?$AA@(`string')
PublicSymbol: [0004A93C][0003:0000293C] ??_C@_0BF@IAAKIJDJ@AtlThrow?3?5hr?5?$DN?50x?$CFx?6?$AA@(`string')
PublicSymbol: [0004CC18][0003:00004C18] _GUID_DEVCLASS_SENSOR(_GUID_DEVCLASS_SENSOR)
PublicSymbol: [0004F84C][0003:0000784C] ??_R2IRegistrarBase@@8(IRegistrarBase::`RTTI Base Class Array')
PublicSymbol: [0004F838][0003:00007838] ??_R3IRegistrarBase@@8(IRegistrarBase::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [000391EA][0002:000221EA] _ReleaseCapture@0(_ReleaseCapture@0)
PublicSymbol: [000530B4][0005:000000B4] __IMPORT_DESCRIPTOR_MSVCR100D(__IMPORT_DESCRIPTOR_MSVCR100D)
PublicSymbol: [0001DB70][0002:00006B70] ?OnChar@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnChar(unsigned int,unsigned int,long,int &))
PublicSymbol: [00053A0C][0005:00000A0C] __imp___controlfp_s(__imp___controlfp_s)
PublicSymbol: [000538F8][0005:000008F8] __imp__OpenFileMappingA@12(__imp__OpenFileMappingA@12)
PublicSymbol: [0004B5D4][0003:000035D4] ??_C@_1BI@JPOONHGO@?$AAm?$AA_?$AAh?$AAK?$AAe?$AAy?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00042674][0002:0002B674] _GetCurrentThread@0(_GetCurrentThread@0)
PublicSymbol: [00053C38][0005:00000C38] __imp__GetWindow@8(__imp__GetWindow@8)
PublicSymbol: [0004E530][0003:00006530] ??_C@_1EA@NFKNIFJP@?$AAR?$AAu?$AAn?$AA?9?$AAT?$AAi?$AAm?$AAe?$AA?5?$AAC?$AAh?$AAe?$AAc?$AAk?$AA?5?$AAF?$AAa?$AAi?$AAl?$AAu?$AAr?$AAe?$AA?5?$AA?$CD?$AA?$CF?$AAd?$AA?5?$AA?9?$AA?5?$AA?$CF?$AAs?$AA?$AA@(`string')
PublicSymbol: [000537D8][0005:000007D8] __imp__crInstallToCurrentThread2@4(__imp__crInstallToCurrentThread2@4)
PublicSymbol: [0004D134][0003:00005134] ??_C@_0N@BEIMPMHB@atlTraceUser?$AA@(`string')
PublicSymbol: [0004D8E8][0003:000058E8] ??_C@_1BG@IICBGEDI@?$AAp?$AAp?$AAU?$AAn?$AAk?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0002AFB0][0002:00013FB0] ??$AtlMultiply@H@ATL@@YAJPAHHH@Z(long __cdecl ATL::AtlMultiply<int>(int *,int,int))
PublicSymbol: [0004BEF4][0003:00003EF4] ?VT@?$CVarTypeInfo@M@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<float>::VT)
PublicSymbol: [00035B20][0002:0001EB20] ?PreTranslateMessage@CMainDlg@@UAEHPAUtagMSG@@@Z(public: virtual int __thiscall CMainDlg::PreTranslateMessage(struct tagMSG *))
PublicSymbol: [0003929E][0002:0002229E] _SetFocus@4(_SetFocus@4)
PublicSymbol: [0004C5D8][0003:000045D8] _GUID_YSIZEPIXEL(_GUID_YSIZEPIXEL)
PublicSymbol: [000394B0][0002:000224B0] ??0_ATL_BASE_MODULE70@ATL@@QAE@XZ(public: __thiscall ATL::_ATL_BASE_MODULE70::_ATL_BASE_MODULE70(void))
PublicSymbol: [0004C894][0003:00004894] _GUID_DEVCLASS_APMSUPPORT(_GUID_DEVCLASS_APMSUPPORT)
PublicSymbol: [0005391C][0005:0000091C] __imp__GetProcessHeap@0(__imp__GetProcessHeap@0)
PublicSymbol: [00053A58][0005:00000A58] __imp_?terminate@@YAXXZ(__imp_?terminate@@YAXXZ)
PublicSymbol: [00036D50][0002:0001FD50] ?GetCurSel@?$CComboBoxT@VCWindow@ATL@@@WTL@@QBEHXZ(public: int __thiscall WTL::CComboBoxT<class ATL::CWindow>::GetCurSel(void)const )
PublicSymbol: [00053A28][0005:00000A28] __imp___ismbblead(__imp___ismbblead)
PublicSymbol: [0004CCE0][0003:00004CE0] _GUID_DEVCLASS_WCEUSBS(_GUID_DEVCLASS_WCEUSBS)
PublicSymbol: [00053A70][0005:00000A70] __imp___wcsicmp(__imp___wcsicmp)
PublicSymbol: [00049D0C][0003:00001D0C] ??_C@_1BO@KNMGNOCJ@?$AAh?$AAA?$AAd?$AAv?$AAA?$AAP?$AAI?$AA3?$AA2?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00020320][0002:00009320] ?IsUsingTags@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::IsUsingTags(void)const )
PublicSymbol: [000391CC][0002:000221CC] _IsWindow@4(_IsWindow@4)
PublicSymbol: [00039510][0002:00022510] ??1_ATL_BASE_MODULE70@ATL@@QAE@XZ(public: __thiscall ATL::_ATL_BASE_MODULE70::~_ATL_BASE_MODULE70(void))
PublicSymbol: [00040482][0002:00029482] _memset(_memset)
PublicSymbol: [0004C5C4][0003:000045C4] _GUID_XSIZEPIXEL(_GUID_XSIZEPIXEL)
PublicSymbol: [0004C27C][0003:0000427C] _CLSID_PSInPlaceFrame(_CLSID_PSInPlaceFrame)
PublicSymbol: [00053C80][0005:00000C80] __imp__DrawFocusRect@8(__imp__DrawFocusRect@8)
PublicSymbol: [00039CC0][0002:00022CC0] ?RemoveAll@?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAEXXZ(public: void __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >::RemoveAll(void))
PublicSymbol: [00049930][0003:00001930] ??_C@_1BO@IHBDOHNF@?$AAm?$AA_?$AAh?$AAC?$AAu?$AAr?$AAs?$AAo?$AAr?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004F7B4][0003:000077B4] ??_R4CRegObject@ATL@@6B@(const ATL::CRegObject::`RTTI Complete Object Locator')
PublicSymbol: [00024010][0002:0000D010] ??1CPaintDC@WTL@@QAE@XZ(public: __thiscall WTL::CPaintDC::~CPaintDC(void))
PublicSymbol: [0004D164][0003:00005164] ??_C@_0O@JMDMDOKB@atlTraceUser4?$AA@(`string')
PublicSymbol: [0004BB50][0003:00003B50] ??_C@_0BJ@DKMLNHCP@pure?5virtual?5method?5call?$AA@(`string')
PublicSymbol: [00020F70][0002:00009F70] ?QueryStringValue@CRegKey@ATL@@QAEJPBDPADPAK@Z(public: long __thiscall ATL::CRegKey::QueryStringValue(char const *,char *,unsigned long *))
PublicSymbol: [0004B3B0][0003:000033B0] ??_C@_0O@MJDLIIGK@HKEY_DYN_DATA?$AA@(`string')
PublicSymbol: [00041980][0002:0002A980] __RTC_NumErrors(__RTC_NumErrors)
PublicSymbol: [000367E0][0002:0001F7E0] ?ProcessWindowMessage@CDocumentDlg@@UAEHPAUHWND__@@IIJAAJK@Z(public: virtual int __thiscall CDocumentDlg::ProcessWindowMessage(struct HWND__ *,unsigned int,unsigned int,long,long &,unsigned long))
PublicSymbol: [0004EB1C][0003:00006B1C] ??_C@_1BK@GAEMIDIL@?$AAA?$AAD?$AAV?$AAA?$AAP?$AAI?$AA3?$AA2?$AA?4?$AAD?$AAL?$AAL?$AA?$AA@(`string')
PublicSymbol: [00042680][0002:0002B680] _GetModuleFileNameW@12(_GetModuleFileNameW@12)
PublicSymbol: [00034A80][0002:0001DA80] ?FreeHeap@?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@AAEXXZ(private: void __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::FreeHeap(void))
PublicSymbol: [0004A49C][0003:0000249C] ??_C@_1CM@LPPFPJMH@?$AAG?$AAe?$AAt?$AAD?$AAa?$AAt?$AAa?$AA?$CI?$AA?$CJ?$AA?9?$AA?$DO?$AAn?$AAR?$AAe?$AAf?$AAs?$AA?5?$AA?$DM?$AA?$DN?$AA?5?$AA1?$AA?$AA@(`string')
PublicSymbol: [0004E604][0003:00006604] ??_C@_0CG@IAFNJNEE@Stack?5corrupted?5near?5unknown?5var@(`string')
PublicSymbol: [0004B9F0][0003:000039F0] ??_C@_1HM@MPHIPANE@?$AAd?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAj?$AAe?$AAc?$AAt?$AAs?$AA?2?$AAc?$AAr?$AAa?$AAs?$AAh?$AAr?$AAp?$AAt?$AA?2?$AAt?$AAr?$AAu?$AAn?$AAk?$AA?2?$AAr?$AAe?$AAp?$AAo?$AAr@(`string')
PublicSymbol: [0004C3D0][0003:000043D0] _CLSID_RemoteUnknownPSFactory(_CLSID_RemoteUnknownPSFactory)
PublicSymbol: [0004C22C][0003:0000422C] _CLSID_PSGenObject(_CLSID_PSGenObject)
PublicSymbol: [000404E0][0002:000294E0] __onexit(__onexit)
PublicSymbol: [00035470][0002:0001E470] ?CrashThread@@YGKPAX@Z(unsigned long __stdcall CrashThread(void *))
PublicSymbol: [00040710][0002:00029710] __chkstk(__chkstk)
PublicSymbol: [0003C7A0][0002:000257A0] ?AtlUnadvise@ATL@@YGJPAUIUnknown@@ABU_GUID@@K@Z(long __stdcall ATL::AtlUnadvise(struct IUnknown *,struct _GUID const &,unsigned long))
PublicSymbol: [0004BD30][0003:00003D30] ??_C@_0EI@NJGILBL@ERROR?5?3?5Unable?5to?5lock?5critical?5@(`string')
PublicSymbol: [0004261A][0002:0002B61A] _OutputDebugStringA@4(_OutputDebugStringA@4)
PublicSymbol: [000525D8][0004:000005D8] ?__LINE__Var@?1??SetStringValue@CRegKey@ATL@@QAEJPBD0K@Z@4JA(long `public: long __thiscall ATL::CRegKey::SetStringValue(char const *,char const *,unsigned long)'::`2'::__LINE__Var)
PublicSymbol: [000391DE][0002:000221DE] _SetCursor@4(_SetCursor@4)
PublicSymbol: [00027020][0002:00010020] ?Lookup@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QBEPAVCMessageLoop@WTL@@ABK@Z(public: class WTL::CMessageLoop * __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >::Lookup(unsigned long const &)const )
PublicSymbol: [00053B64][0005:00000B64] __imp__SysFreeString@4(__imp__SysFreeString@4)
PublicSymbol: [0001A900][0002:00003900] ?OnInitDialog@CAboutDlg@@QAEJIIJAAH@Z(public: long __thiscall CAboutDlg::OnInitDialog(unsigned int,unsigned int,long,int &))
PublicSymbol: [00027960][0002:00010960] ??0?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CDialogImplBaseT<class ATL::CWindow>::CDialogImplBaseT<class ATL::CWindow>(void))
PublicSymbol: [00022F20][0002:0000BF20] ?SetWindowTextA@CWindow@ATL@@QAEHPBD@Z(public: int __thiscall ATL::CWindow::SetWindowTextA(char const *))
PublicSymbol: [0004C920][0003:00004920] _GUID_DEVCLASS_DECODER(_GUID_DEVCLASS_DECODER)
PublicSymbol: [0004F3AC][0003:000073AC] ??_R4CIdleHandler@WTL@@6B@(const WTL::CIdleHandler::`RTTI Complete Object Locator')
PublicSymbol: [0004DE38][0003:00005E38] ??_C@_1CC@JCNIBNON@?$AAi?$AAM?$AAo?$AAd?$AAu?$AAl?$AAe?$AA?5?$AA?$DM?$AA?5?$AAm?$AAo?$AAd?$AAc?$AAn?$AAt?$AA?$AA@(`string')
PublicSymbol: [000392A4][0002:000222A4] _GetParent@4(_GetParent@4)
PublicSymbol: [00053950][0005:00000950] __imp__WaitForSingleObject@8(__imp__WaitForSingleObject@8)
PublicSymbol: [000425F6][0002:0002B5F6] ??1type_info@@UAE@XZ(public: virtual __thiscall type_info::~type_info(void))
PublicSymbol: [000391C0][0002:000221C0] _MessageBoxA@16(_MessageBoxA@16)
PublicSymbol: [00052184][0004:00000184] ??_R0?AV?$CUpdateUI@VCMainDlg@@@WTL@@@8(class WTL::CUpdateUI<class CMainDlg> `RTTI Type Descriptor')
PublicSymbol: [0003F2D0][0002:000282D0] ?CleanUp@CAtlAllocator@@QAEXXZ(public: void __thiscall CAtlAllocator::CleanUp(void))
PublicSymbol: [0003F980][0002:00028980] ?RemoveAt@?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAEHH@Z(public: int __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long> >::RemoveAt(int))
PublicSymbol: [0005295C][0004:0000095C] ?atlTraceSync@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceSync)
PublicSymbol: [000391F0][0002:000221F0] _GetCapture@0(_GetCapture@0)
PublicSymbol: [0001F2A0][0002:000082A0] ?Navigate@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::Navigate(void))
PublicSymbol: [0004C9D4][0003:000049D4] _GUID_DEVCLASS_HDC(_GUID_DEVCLASS_HDC)
PublicSymbol: [0002A020][0002:00013020] ?Unlock@CAtlModule@ATL@@UAEJXZ(public: virtual long __thiscall ATL::CAtlModule::Unlock(void))
PublicSymbol: [00053AA4][0005:00000AA4] __imp___purecall(__imp___purecall)
PublicSymbol: [0004AB5C][0003:00002B5C] __GUID_00000146_0000_0000_c000_000000000046(__GUID_00000146_0000_0000_c000_000000000046)
PublicSymbol: [0004D570][0003:00005570] ??_C@_1HO@MNFHLCLF@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [0004D918][0003:00005918] ??_C@_0DG@EKJDFEGF@f?3?2dd?2vctools?2vc7libs?2ship?2atlmf@(`string')
PublicSymbol: [00039376][0002:00022376] _RegDeleteValueA@8(_RegDeleteValueA@8)
PublicSymbol: [0004BE7C][0003:00003E7C] ?VT@?$CVarTypeInfo@PAF@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<short *>::VT)
PublicSymbol: [0003AC80][0002:00023C80] _AtlTraceGetUpdateEventNameU_s@8(_AtlTraceGetUpdateEventNameU_s@8)
PublicSymbol: [00053854][0005:00000854] __imp__GetObjectA@12(__imp__GetObjectA@12)
PublicSymbol: [00037990][0002:00020990] ?PostMessageA@CWindow@ATL@@QAEHIIJ@Z(public: int __thiscall ATL::CWindow::PostMessageA(unsigned int,unsigned int,long))
PublicSymbol: [0003F550][0002:00028550] ?ReleaseSnapshot@CAtlAllocator@@QAEXXZ(public: void __thiscall CAtlAllocator::ReleaseSnapshot(void))
PublicSymbol: [00027E90][0002:00010E90] ?RemoveAll@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAEXXZ(public: void __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >::RemoveAll(void))
PublicSymbol: [0004E718][0003:00006718] ??_C@_0N@MHFFIMFG@?6Address?3?50x?$AA@(`string')
PublicSymbol: [000282F0][0002:000112F0] ??0CMessageMap@ATL@@QAE@XZ(public: __thiscall ATL::CMessageMap::CMessageMap(void))
PublicSymbol: [00041A62][0002:0002AA62] ___dllonexit(___dllonexit)
PublicSymbol: [0004C9E8][0003:000049E8] _GUID_DEVCLASS_HIDCLASS(_GUID_DEVCLASS_HIDCLASS)
PublicSymbol: [000528B0][0004:000008B0] ?_pModule@ATL@@3PAVCComModule@1@A(class ATL::CComModule * ATL::_pModule)
PublicSymbol: [00037B30][0002:00020B30] ?SetIcon@CWindow@ATL@@QAEPAUHICON__@@PAU3@H@Z(public: struct HICON__ * __thiscall ATL::CWindow::SetIcon(struct HICON__ *,int))
PublicSymbol: [0004D840][0003:00005840] ??_C@_1GM@JACHBNBJ@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [000426E6][0002:0002B6E6] _GetMessageW@16(_GetMessageW@16)
PublicSymbol: [000538F0][0005:000008F0] __imp__EncodePointer@4(__imp__EncodePointer@4)
PublicSymbol: [00052294][0004:00000294] ??_R0?AVCHyperLink@WTL@@@8(class WTL::CHyperLink `RTTI Type Descriptor')
PublicSymbol: [00049280][0003:00001280] ??_C@_0BB@DNMEKHBL@test?$EAhotmail?4com?$AA@(`string')
PublicSymbol: [0004BF08][0003:00003F08] ?pmField@?$CVarTypeInfo@N@ATL@@2QQtagVARIANT@@NQ3@(public: static double tagVARIANT::* ATL::CVarTypeInfo<double>::pmField)
PublicSymbol: [0004C1B4][0003:000041B4] _IID_IEnumHolder(_IID_IEnumHolder)
PublicSymbol: [00053904][0005:00000904] __imp__MapViewOfFile@20(__imp__MapViewOfFile@20)
PublicSymbol: [00024990][0002:0000D990] ??4CString@WTL@@QAEAAV01@PBD@Z(public: class WTL::CString & __thiscall WTL::CString::operator=(char const *))
PublicSymbol: [00023E90][0002:0000CE90] ??2_stdcallthunk@ATL@@SAPAXI@Z(public: static void * __cdecl ATL::_stdcallthunk::operator new(unsigned int))
PublicSymbol: [00020830][0002:00009830] ?SetTextColor@?$CDCT@$0A@@WTL@@QAEKK@Z(public: unsigned long __thiscall WTL::CDCT<0>::SetTextColor(unsigned long))
PublicSymbol: [00052A4C][0004:00000A4C] ?_AtlComModule@ATL@@3VCAtlComModule@1@A(class ATL::CAtlComModule ATL::_AtlComModule)
PublicSymbol: [000498C4][0003:000018C4] ??_C@_1BO@BCGAPJE@?$AAl?$AAp?$AAs?$AAt?$AAr?$AAT?$AAe?$AAx?$AAt?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004C1DC][0003:000041DC] _IID_IOleManager(_IID_IOleManager)
PublicSymbol: [0004C68C][0003:0000468C] _GUID_FONTSIZE(_GUID_FONTSIZE)
PublicSymbol: [0004010A][0002:0002910A] _InitCommonControlsEx@4(_InitCommonControlsEx@4)
PublicSymbol: [00033A40][0002:0001CA40] ?GetSize@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QBEHXZ(public: int __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::GetSize(void)const )
PublicSymbol: [00052A7C][0004:00000A7C] ?__AtlThunkPool@@3PAT_SLIST_HEADER@@A(union _SLIST_HEADER * __AtlThunkPool)
PublicSymbol: [00048CB0][0003:00000CB0] ??_7CMessageLoop@WTL@@6B@(const WTL::CMessageLoop::`vftable')
PublicSymbol: [00053C90][0005:00000C90] __imp__CharNextA@4(__imp__CharNextA@4)
PublicSymbol: [0004BB24][0003:00003B24] ??_C@_0P@KBMFAKJA@buffer?5overrun?$AA@(`string')
PublicSymbol: [000335B0][0002:0001C5B0] ?Allocate@?$CAtlSafeAllocBufferManager@VCCRTAllocator@ATL@@@_ATL_SAFE_ALLOCA_IMPL@ATL@@QAEPAXK@Z(public: void * __thiscall ATL::_ATL_SAFE_ALLOCA_IMPL::CAtlSafeAllocBufferManager<class ATL::CCRTAllocator>::Allocate(unsigned long))
PublicSymbol: [0004987C][0003:0000187C] ??_C@_0N@EFDLFMHA@Anchor?5Color?$AA@(`string')
PublicSymbol: [00024520][0002:0000D520] ??0CToolInfo@WTL@@QAE@IPAUHWND__@@IPAUtagRECT@@PADJ@Z(public: __thiscall WTL::CToolInfo::CToolInfo(unsigned int,struct HWND__ *,unsigned int,struct tagRECT *,char *,long))
PublicSymbol: [0004BF54][0003:00003F54] ?pmField@?$CVarTypeInfo@PATtagCY@@@ATL@@2QQtagVARIANT@@PATtagCY@@Q3@(public: static union tagCY * tagVARIANT::* ATL::CVarTypeInfo<union tagCY *>::pmField)
PublicSymbol: [0004BEF8][0003:00003EF8] ?pmField@?$CVarTypeInfo@M@ATL@@2QQtagVARIANT@@MQ3@(public: static float tagVARIANT::* ATL::CVarTypeInfo<float>::pmField)
PublicSymbol: [00048B14][0003:00000B14] ??_C@_06FDHFNJBJ@SYSTEM?$AA@(`string')
PublicSymbol: [00049A38][0003:00001A38] ??_C@_19GCAABOFN@?$AAb?$AAR?$AAe?$AAt?$AA?$AA@(`string')
PublicSymbol: [00028900][0002:00011900] ?GetLogFont@?$CFontT@$0A@@WTL@@QBEHPAUtagLOGFONTA@@@Z(public: int __thiscall WTL::CFontT<0>::GetLogFont(struct tagLOGFONTA *)const )
PublicSymbol: [0001C620][0002:00005620] ??0?$CDialogImpl@VCAboutDlg@@VCWindow@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CDialogImpl<class CAboutDlg,class ATL::CWindow>::CDialogImpl<class CAboutDlg,class ATL::CWindow>(void))
PublicSymbol: [00025D20][0002:0000ED20] ?Init@CString@WTL@@IAEXXZ(protected: void __thiscall WTL::CString::Init(void))
PublicSymbol: [0004C330][0003:00004330] _CLSID_DCOMAccessControl(_CLSID_DCOMAccessControl)
PublicSymbol: [00042608][0002:0002B608] _GetCurrentProcessId@0(_GetCurrentProcessId@0)
PublicSymbol: [0004B844][0003:00003844] ??_C@_0DN@JALIHHGH@ERROR?5?3?5Unable?5to?5initialize?5cri@(`string')
PublicSymbol: [00042704][0002:0002B704] _OpenThreadToken@16(_OpenThreadToken@16)
PublicSymbol: [00021470][0002:0000A470] ?strcpy_x@SecureHelper@WTL@@YAXPADIPBD@Z(void __cdecl WTL::SecureHelper::strcpy_x(char *,unsigned int,char const *))
PublicSymbol: [0003D900][0002:00026900] ??1?$CComPtrBase@UIConnectionPoint@@@ATL@@QAE@XZ(public: __thiscall ATL::CComPtrBase<struct IConnectionPoint>::~CComPtrBase<struct IConnectionPoint>(void))
PublicSymbol: [0004F0BC][0003:000070BC] ??_R3?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@8(ATL::CDialogImplBaseT<class ATL::CWindow>::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0001DC20][0002:00006C20] ?OnSetCursor@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnSetCursor(unsigned int,unsigned int,long,int &))
PublicSymbol: [0004F0D0][0003:000070D0] ??_R2?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@8(ATL::CDialogImplBaseT<class ATL::CWindow>::`RTTI Base Class Array')
PublicSymbol: [000340D0][0002:0001D0D0] ?Attach@?$CAutoVectorPtr@_W@ATL@@QAEXPA_W@Z(public: void __thiscall ATL::CAutoVectorPtr<wchar_t>::Attach(wchar_t *))
PublicSymbol: [0001A430][0002:00003430] ??_GCMainDlg@@UAEPAXI@Z(public: virtual void * __thiscall CMainDlg::`scalar deleting destructor'(unsigned int))
PublicSymbol: [00048CF0][0003:00000CF0] ??_C@_1HI@GCFDHDMO@?$AAd?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAj?$AAe?$AAc?$AAt?$AAs?$AA?2?$AAc?$AAr?$AAa?$AAs?$AAh?$AAr?$AAp?$AAt?$AA?2?$AAt?$AAr?$AAu?$AAn?$AAk?$AA?2?$AAr?$AAe?$AAp?$AAo?$AAr@(`string')
PublicSymbol: [00024760][0002:0000D760] ?GetResourceInstance@ModuleHelper@WTL@@YAPAUHINSTANCE__@@XZ(struct HINSTANCE__ * __cdecl WTL::ModuleHelper::GetResourceInstance(void))
PublicSymbol: [00053A20][0005:00000A20] __imp___CrtSetCheckCount(__imp___CrtSetCheckCount)
PublicSymbol: [00048B08][0003:00000B08] ??_C@_08KEKAJNJI@Software?$AA@(`string')
PublicSymbol: [00041B20][0002:0002AB20] __setdefaultprecision(__setdefaultprecision)
PublicSymbol: [0003D9D0][0002:000269D0] ??0?$CComPtrBase@UIConnectionPointContainer@@@ATL@@IAE@XZ(protected: __thiscall ATL::CComPtrBase<struct IConnectionPointContainer>::CComPtrBase<struct IConnectionPointContainer>(void))
PublicSymbol: [00027200][0002:00010200] ?GetDialogProc@?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@UAEP6GHPAUHWND__@@IIJ@ZXZ(public: virtual int (__stdcall*__thiscall ATL::CDialogImplBaseT<class ATL::CWindow>::GetDialogProc(void))(struct HWND__ *,unsigned int,unsigned int,long))
PublicSymbol: [000528DC][0004:000008DC] ??_B?1??VTFromRegType@CRegParser@ATL@@KAHPBDAAG@Z@51(`protected: static int __cdecl ATL::CRegParser::VTFromRegType(char const *,unsigned short &)'::`2'::`local static guard'{2}')
PublicSymbol: [00041500][0002:0002A500] ?_RTC_StackFailure@@YAXPAXPBD@Z(void __cdecl _RTC_StackFailure(void *,char const *))
PublicSymbol: [0002EC70][0002:00017C70] ??$AtlMultiply@K@ATL@@YAJPAKKK@Z(long __cdecl ATL::AtlMultiply<unsigned long>(unsigned long *,unsigned long,unsigned long))
PublicSymbol: [0004BF04][0003:00003F04] ?VT@?$CVarTypeInfo@N@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<double>::VT)
PublicSymbol: [0002ECF0][0002:00017CF0] ??1CParseBuffer@CRegParser@ATL@@QAE@XZ(public: __thiscall ATL::CRegParser::CParseBuffer::~CParseBuffer(void))
PublicSymbol: [0004C560][0003:00004560] _CLSID_StdPicture(_CLSID_StdPicture)
PublicSymbol: [00048C70][0003:00000C70] ??_C@_0BO@PCDOJCOC@Main?5dialog?5creation?5failed?$CB?6?$AA@(`string')
PublicSymbol: [00053BF0][0005:00000BF0] __imp__GetDlgItem@8(__imp__GetDlgItem@8)
PublicSymbol: [000392AA][0002:000222AA] _GetDlgCtrlID@4(_GetDlgCtrlID@4)
PublicSymbol: [0004BEC8][0003:00003EC8] ?pmField@?$CVarTypeInfo@K@ATL@@2QQtagVARIANT@@KQ3@(public: static unsigned long tagVARIANT::* ATL::CVarTypeInfo<unsigned long>::pmField)
PublicSymbol: [0004CC54][0003:00004C54] _GUID_DEVCLASS_SOUND(_GUID_DEVCLASS_SOUND)
PublicSymbol: [0005392C][0005:0000092C] __imp__VirtualFree@12(__imp__VirtualFree@12)
PublicSymbol: [0004F778][0003:00007778] ??_R1A@?0A@EA@_ATL_MODULE70@ATL@@8(ATL::_ATL_MODULE70::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [00039310][0002:00022310] _GetMonitorInfoA@8(_GetMonitorInfoA@8)
PublicSymbol: [0004F278][0003:00007278] ??_R1CM@?0A@EA@CUpdateUIBase@WTL@@8(WTL::CUpdateUIBase::`RTTI Base Class Descriptor at (44,-1,0,64)')
PublicSymbol: [00049A1C][0003:00001A1C] ??_C@_1BG@CIIAFDOP@?$AAm?$AA_?$AAh?$AAD?$AAC?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004D7B0][0003:000057B0] ??_C@_1GI@CLJCGCPP@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [0004E8E0][0003:000068E0] ??_C@_1GE@KOCFNGPB@?$AA_?$AAc?$AAo?$AAn?$AAt?$AAr?$AAo?$AAl?$AAf?$AAp?$AA_?$AAs?$AA?$CI?$AA?$CI?$AA?$CI?$AAv?$AAo?$AAi?$AAd?$AA?5?$AA?$CK?$AA?$CJ?$AA0?$AA?$CJ?$AA?0?$AA?5?$AA0?$AAx?$AA0?$AA0?$AA0?$AA1@(`string')
PublicSymbol: [00052954][0004:00000954] ?atlTraceUtil@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceUtil)
PublicSymbol: [0004B1D8][0003:000031D8] ??_C@_0BN@HGJLNPOD@Failed?5to?5LoadLibrary?5on?5?$CFs?6?$AA@(`string')
PublicSymbol: [0005292C][0004:0000092C] ?atlTraceDBProvider@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceDBProvider)
PublicSymbol: [00049124][0003:00001124] ??_C@_0L@MHOPPIAO@?2dummy?4ini?$AA@(`string')
PublicSymbol: [000392C8][0002:000222C8] _EndPaint@8(_EndPaint@8)
PublicSymbol: [00053C08][0005:00000C08] __imp__ReleaseDC@8(__imp__ReleaseDC@8)
PublicSymbol: [0004AE40][0003:00002E40] ??_C@_1KA@DCHELMLM@?$AA?$CC?$AAs?$AAt?$AAa?$AAt?$AAi?$AAc?$AAa?$AAl?$AAl?$AAy?$AA?5?$AAl?$AAi?$AAn?$AAk?$AAe?$AAd?$AA?5?$AAi?$AAn?$AA?5?$AAC?$AAR?$AAe?$AAg?$AAO?$AAb?$AAj?$AAe?$AAc?$AAt@(`string')
PublicSymbol: [0004B53C][0003:0000353C] ??_C@_0BG@OEFMOOAA@Ending?5Recovery?5Mode?6?$AA@(`string')
PublicSymbol: [00052030][0004:00000030] ?__LINE__Var@?1??Run@@YAHPADH@Z@4JA(long `int __cdecl Run(char *,int)'::`2'::__LINE__Var)
PublicSymbol: [0004BEB8][0003:00003EB8] ?pmField@?$CVarTypeInfo@J@ATL@@2QQtagVARIANT@@JQ3@(public: static long tagVARIANT::* ATL::CVarTypeInfo<long>::pmField)
PublicSymbol: [00048B84][0003:00000B84] ??_C@_06JBKGCNBB@Delete?$AA@(`string')
PublicSymbol: [00053988][0005:00000988] __imp__InterlockedDecrement@4(__imp__InterlockedDecrement@4)
PublicSymbol: [0004CB8C][0003:00004B8C] _GUID_DEVCLASS_PORTS(_GUID_DEVCLASS_PORTS)
PublicSymbol: [0003B6A0][0002:000246A0] _AtlTraceCloseProcess@4(_AtlTraceCloseProcess@4)
PublicSymbol: [00052E88][0004:00000E88] __fmode(__fmode)
PublicSymbol: [0004BEA8][0003:00003EA8] ?pmField@?$CVarTypeInfo@I@ATL@@2QQtagVARIANT@@IQ3@(public: static unsigned int tagVARIANT::* ATL::CVarTypeInfo<unsigned int>::pmField)
PublicSymbol: [00041A6E][0002:0002AA6E] __except_handler4_common(__except_handler4_common)
PublicSymbol: [000235E0][0002:0000C5E0] ?ShowWindow@CWindow@ATL@@QAEHH@Z(public: int __thiscall ATL::CWindow::ShowWindow(int))
PublicSymbol: [00029EA0][0002:00012EA0] ??_ECAppModule@WTL@@UAEPAXI@Z(public: virtual void * __thiscall WTL::CAppModule::`vector deleting destructor'(unsigned int))
PublicSymbol: [00020410][0002:00009410] ??0?$CDCT@$0A@@WTL@@QAE@PAUHDC__@@@Z(public: __thiscall WTL::CDCT<0>::CDCT<0>(struct HDC__ *))
PublicSymbol: [00053A9C][0005:00000A9C] __imp__wcscmp(__imp__wcscmp)
PublicSymbol: [00031BB0][0002:0001ABB0] ?SetStringValue@CRegKey@ATL@@QAEJPBD0K@Z(public: long __thiscall ATL::CRegKey::SetStringValue(char const *,char const *,unsigned long))
PublicSymbol: [00040656][0002:00029656] __alloca_probe_8(__alloca_probe_8)
PublicSymbol: [00053D58][0005:00000D58] __imp__CoRevokeClassObject@4(__imp__CoRevokeClassObject@4)
PublicSymbol: [000522B8][0004:000002B8] ??_R0?AV?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@@8(class WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> > `RTTI Type Descriptor')
PublicSymbol: [00042006][0002:0002B006] __initterm(__initterm)
PublicSymbol: [000233A0][0002:0000C3A0] ?UpdateWindow@CWindow@ATL@@QAEHXZ(public: int __thiscall ATL::CWindow::UpdateWindow(void))
PublicSymbol: [0004C63C][0003:0000463C] _GUID_TRISTATE(_GUID_TRISTATE)
PublicSymbol: [0004BE98][0003:00003E98] ?pmField@?$CVarTypeInfo@H@ATL@@2QQtagVARIANT@@HQ3@(public: static int tagVARIANT::* ATL::CVarTypeInfo<int>::pmField)
PublicSymbol: [000426EC][0002:0002B6EC] _IsWindowUnicode@4(_IsWindowUnicode@4)
PublicSymbol: [00053968][0005:00000968] __imp__lstrcmpiA@8(__imp__lstrcmpiA@8)
PublicSymbol: [0001CBF0][0002:00005BF0] ?GetUpdateUIMap@CMainDlg@@SAPBU_AtlUpdateUIMap@CUpdateUIBase@WTL@@XZ(public: static struct WTL::CUpdateUIBase::_AtlUpdateUIMap const * __cdecl CMainDlg::GetUpdateUIMap(void))
PublicSymbol: [0004B4F8][0003:000034F8] ??_C@_0BB@FDAMNOMP@Deleting?5Key?5?$CFs?6?$AA@(`string')
PublicSymbol: [0005303C][0005:0000003C] __IMPORT_DESCRIPTOR_USER32(__IMPORT_DESCRIPTOR_USER32)
PublicSymbol: [00052EC8][0004:00000EC8] ___onexitbegin(___onexitbegin)
PublicSymbol: [000390EE][0002:000220EE] _GetModuleFileNameA@12(_GetModuleFileNameA@12)
PublicSymbol: [0004B280][0003:00003280] ?map@?1??HKeyFromString@CRegParser@ATL@@KAPAUHKEY__@@PAD@Z@4QBUkeymap@?1??123@KAPAU4@0@Z@B(struct `protected: static struct HKEY__ * __cdecl ATL::CRegParser::HKeyFromString(char *)'::`2'::keymap const * const `protected: static struct HKEY__ * __cdecl ATL::CRegParser::HKeyFromString(char *)'::`2'::map)
PublicSymbol: [00048E20][0003:00000E20] ??_C@_1HK@ONKBLHEH@?$AAd?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAj?$AAe?$AAc?$AAt?$AAs?$AA?2?$AAc?$AAr?$AAa?$AAs?$AAh?$AAr?$AAp?$AAt?$AA?2?$AAt?$AAr?$AAu?$AAn?$AAk?$AA?2?$AAr?$AAe?$AAp?$AAo?$AAr@(`string')
PublicSymbol: [0004B354][0003:00003354] ??_C@_0BC@GMMMKLBI@HKEY_CURRENT_USER?$AA@(`string')
PublicSymbol: [00049EB8][0003:00001EB8] ??_C@_0CC@HMAHOHMO@?3?3GetMessage?5returned?5?91?5?$CIerror?$CJ@(`string')
PublicSymbol: [00052893][0004:00000893] ?m_bInitFailed@CAtlBaseModule@ATL@@2_NA(public: static bool ATL::CAtlBaseModule::m_bInitFailed)
PublicSymbol: [000526B4][0004:000006B4] ??_R0?AVtype_info@@@8(class type_info `RTTI Type Descriptor')
PublicSymbol: [0004CB00][0003:00004B00] _GUID_DEVCLASS_NET(_GUID_DEVCLASS_NET)
PublicSymbol: [0005374C][0005:0000074C] __imp__RegCreateKeyExA@36(__imp__RegCreateKeyExA@36)
PublicSymbol: [0003DB40][0002:00026B40] ??0CAtlComModule@ATL@@QAE@XZ(public: __thiscall ATL::CAtlComModule::CAtlComModule(void))
PublicSymbol: [0003CDA0][0002:00025DA0] ?AtlComModuleRevokeClassObjects@ATL@@YGJPAU_ATL_COM_MODULE70@1@@Z(long __stdcall ATL::AtlComModuleRevokeClassObjects(struct ATL::_ATL_COM_MODULE70 *))
PublicSymbol: [0004BE8C][0003:00003E8C] ?VT@?$CVarTypeInfo@PAG@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<unsigned short *>::VT)
PublicSymbol: [0004BE88][0003:00003E88] ?pmField@?$CVarTypeInfo@G@ATL@@2QQtagVARIANT@@GQ3@(public: static unsigned short tagVARIANT::* ATL::CVarTypeInfo<unsigned short>::pmField)
PublicSymbol: [00029AC0][0002:00012AC0] ??$?3K@?$Wrapper@K@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@SAXPAXPAK@Z(public: static void __cdecl ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >::Wrapper<unsigned long>::operator delete<unsigned long>(void *,unsigned long *))
PublicSymbol: [0004C650][0003:00004650] _GUID_OPTIONVALUEEXCLUSIVE(_GUID_OPTIONVALUEEXCLUSIVE)
PublicSymbol: [0004C268][0003:00004268] _CLSID_PSInPlaceActive(_CLSID_PSInPlaceActive)
PublicSymbol: [0004EF68][0003:00006F68] ??_R4CMessageLoop@WTL@@6B@(const WTL::CMessageLoop::`RTTI Complete Object Locator')
PublicSymbol: [00049A4C][0003:00001A4C] ??_C@_1CM@NKGAILNM@?$AAm?$AA_?$AAl?$AAp?$AAs?$AAt?$AAr?$AAH?$AAy?$AAp?$AAe?$AAr?$AAL?$AAi?$AAn?$AAk?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00020B60][0002:00009B60] ??1CRegKey@ATL@@QAE@XZ(public: __thiscall ATL::CRegKey::~CRegKey(void))
PublicSymbol: [0004D658][0003:00005658] ??_C@_09IIMHCEDG@?$CFs?$CI?$CFd?$CJ?5?3?5?$AA@(`string')
PublicSymbol: [00049F5C][0003:00001F5C] ??_C@_1BM@EAECDKHO@?$AAp?$AAM?$AAs?$AAg?$AAL?$AAo?$AAo?$AAp?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004EFC0][0003:00006FC0] ??_R4CMainDlg@@6B?$CDialogImpl@VCMainDlg@@VCWindow@ATL@@@ATL@@@(const CMainDlg::`RTTI Complete Object Locator'{for `ATL::CDialogImpl<class CMainDlg,class ATL::CWindow>'})
PublicSymbol: [00052528][0004:00000528] ?__LINE__Var@?1??UpdateRegistryFromResourceS@CAtlModule@ATL@@QAGJPBDHPAU_ATL_REGMAP_ENTRY@3@@Z@4JA(long `public: long __stdcall ATL::CAtlModule::UpdateRegistryFromResourceS(char const *,int,struct ATL::_ATL_REGMAP_ENTRY *)'::`2'::__LINE__Var)
PublicSymbol: [0003D250][0002:00026250] ?AtlWinModuleAddCreateWndData@ATL@@YGXPAU_ATL_WIN_MODULE70@1@PAU_AtlCreateWndData@1@PAX@Z(void __stdcall ATL::AtlWinModuleAddCreateWndData(struct ATL::_ATL_WIN_MODULE70 *,struct ATL::_AtlCreateWndData *,void *))
PublicSymbol: [0004A64C][0003:0000264C] ??_C@_1CM@NFBJJEPK@?$AAG?$AAe?$AAt?$AAD?$AAa?$AAt?$AAa?$AA?$CI?$AA?$CJ?$AA?9?$AA?$DO?$AAn?$AAR?$AAe?$AAf?$AAs?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00021BE0][0002:0000ABE0] ?Init@CComModule@ATL@@QAEJPAU_ATL_OBJMAP_ENTRY30@2@PAUHINSTANCE__@@PBU_GUID@@@Z(public: long __thiscall ATL::CComModule::Init(struct ATL::_ATL_OBJMAP_ENTRY30 *,struct HINSTANCE__ *,struct _GUID const *))
PublicSymbol: [00020370][0002:00009370] ?IsUsingTagsBold@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::IsUsingTagsBold(void)const )
PublicSymbol: [000426A4][0002:0002B6A4] _GetStartupInfoW@4(_GetStartupInfoW@4)
PublicSymbol: [0004B8B8][0003:000038B8] ??_C@_1M@PPPPCDEP@?$AAA?$AAP?$AAP?$AAI?$AAD?$AA?$AA@(`string')
PublicSymbol: [00053CA0][0005:00000CA0] __imp__LoadCursorA@8(__imp__LoadCursorA@8)
PublicSymbol: [000528FC][0004:000008FC] ?WM_ATLGETCONTROL@ATL@@3IA(unsigned int ATL::WM_ATLGETCONTROL)
PublicSymbol: [00042620][0002:0002B620] _OutputDebugStringW@4(_OutputDebugStringW@4)
PublicSymbol: [0004263E][0002:0002B63E] _HeapAlloc@12(_HeapAlloc@12)
PublicSymbol: [000538EC][0005:000008EC] __imp__DecodePointer@4(__imp__DecodePointer@4)
PublicSymbol: [00022E60][0002:0000BE60] ?GetStyle@CWindow@ATL@@QBEKXZ(public: unsigned long __thiscall ATL::CWindow::GetStyle(void)const )
PublicSymbol: [00031EC0][0002:0001AEC0] ?VTFromRegType@CRegParser@ATL@@KAHPBDAAG@Z(protected: static int __cdecl ATL::CRegParser::VTFromRegType(char const *,unsigned short &))
PublicSymbol: [00040EF0][0002:00029EF0] _wcsncpy_s(_wcsncpy_s)
PublicSymbol: [0004C2F4][0003:000042F4] _CLSID_Picture_Metafile(_CLSID_Picture_Metafile)
PublicSymbol: [00040746][0002:00029746] __mbsstr(__mbsstr)
PublicSymbol: [00052918][0004:00000918] ?atlTraceRefcount@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceRefcount)
PublicSymbol: [0003DC60][0002:00026C60] ??1CAtlComModule@ATL@@QAE@XZ(public: __thiscall ATL::CAtlComModule::~CAtlComModule(void))
PublicSymbol: [0004BE78][0003:00003E78] ?pmField@?$CVarTypeInfo@F@ATL@@2QQtagVARIANT@@FQ3@(public: static short tagVARIANT::* ATL::CVarTypeInfo<short>::pmField)
PublicSymbol: [00035B80][0002:0001EB80] ?OnIdle@CMainDlg@@UAEHXZ(public: virtual int __thiscall CMainDlg::OnIdle(void))
PublicSymbol: [00022B90][0002:0000BB90] ??0_U_STRINGorID@ATL@@QAE@PBD@Z(public: __thiscall ATL::_U_STRINGorID::_U_STRINGorID(char const *))
PublicSymbol: [0004CD80][0003:00004D80] _GUID_DEVCLASS_FSFILTER_QUOTAMANAGEMENT(_GUID_DEVCLASS_FSFILTER_QUOTAMANAGEMENT)
PublicSymbol: [00023F30][0002:0000CF30] ??0CPaintDC@WTL@@QAE@PAUHWND__@@@Z(public: __thiscall WTL::CPaintDC::CPaintDC(struct HWND__ *))
PublicSymbol: [0003BC60][0002:00024C60] _AtlTraceGetCategoryInfo@16(_AtlTraceGetCategoryInfo@16)
PublicSymbol: [00026E40][0002:0000FE40] ??1?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >::~CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >(void))
PublicSymbol: [000393B8][0002:000223B8] _VarUI4FromStr@16(_VarUI4FromStr@16)
PublicSymbol: [000305F0][0002:000195F0] ?DeleteSubKey@CRegKey@ATL@@QAEJPBD@Z(public: long __thiscall ATL::CRegKey::DeleteSubKey(char const *))
PublicSymbol: [0003FFD0][0002:00028FD0] ??0CAtlTraceProcess@@QAE@K@Z(public: __thiscall CAtlTraceProcess::CAtlTraceProcess(unsigned long))
PublicSymbol: [00039100][0002:00022100] _SetEvent@4(_SetEvent@4)
PublicSymbol: [00039C00][0002:00022C00] ?InternalSetAtIndex@?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAEXHABQAUHINSTANCE__@@@Z(public: void __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >::InternalSetAtIndex(int,struct HINSTANCE__ * const &))
PublicSymbol: [0004BAF8][0003:00003AF8] ??_C@_06ONOHJOEH@SIGFPE?$AA@(`string')
PublicSymbol: [0004BE60][0003:00003E60] ?pmField@?$CVarTypeInfo@E@ATL@@2QQtagVARIANT@@EQ3@(public: static unsigned char tagVARIANT::* ATL::CVarTypeInfo<unsigned char>::pmField)
PublicSymbol: [000349C0][0002:0001D9C0] ?AllocateBytes@?$CTempBuffer@D$0EAA@VCCRTAllocator@ATL@@@ATL@@QAEPADI@Z(public: char * __thiscall ATL::CTempBuffer<char,1024,class ATL::CCRTAllocator>::AllocateBytes(unsigned int))
PublicSymbol: [00034AE0][0002:0001DAE0] ?AllocateBytes@?$CTempBuffer@D$0BAA@VCCRTAllocator@ATL@@@ATL@@QAEPADI@Z(public: char * __thiscall ATL::CTempBuffer<char,256,class ATL::CCRTAllocator>::AllocateBytes(unsigned int))
PublicSymbol: [00034C00][0002:0001DC00] ?AllocateBytes@?$CTempBuffer@E$0BAA@VCCRTAllocator@ATL@@@ATL@@QAEPAEI@Z(public: unsigned char * __thiscall ATL::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>::AllocateBytes(unsigned int))
PublicSymbol: [00028490][0002:00011490] ?AllocateBytes@?$CTempBuffer@D$0CAA@VCCRTAllocator@ATL@@@ATL@@QAEPADI@Z(public: char * __thiscall ATL::CTempBuffer<char,512,class ATL::CCRTAllocator>::AllocateBytes(unsigned int))
PublicSymbol: [00053914][0005:00000914] __imp__IsProcessorFeaturePresent@4(__imp__IsProcessorFeaturePresent@4)
PublicSymbol: [00020620][0002:00009620] ?Detach@?$CDCT@$0A@@WTL@@QAEPAUHDC__@@XZ(public: struct HDC__ * __thiscall WTL::CDCT<0>::Detach(void))
PublicSymbol: [00042602][0002:0002B602] __invoke_watson(__invoke_watson)
PublicSymbol: [0004B08C][0003:0000308C] ??_C@_1BG@MMFPEKL@?$AAs?$AAz?$AAK?$AAe?$AAy?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004D6F4][0003:000056F4] ??_C@_1CC@ONKAHKMP@?$AAp?$AAM?$AAo?$AAd?$AAu?$AAl?$AAe?$AAI?$AAn?$AAf?$AAo?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004D144][0003:00005144] ??_C@_0O@MKGGJJCH@atlTraceUser2?$AA@(`string')
PublicSymbol: [0004B904][0003:00003904] ??_C@_0CJ@MEABKMPP@Error?5generating?5error?5report?$CB?6E@(`string')
PublicSymbol: [00026DE0][0002:0000FDE0] ??0?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >(void))
PublicSymbol: [00049698][0003:00001698] ??_C@_1BM@OKHEFHBF@?$AAm?$AA_?$AAp?$AAU?$AAI?$AAM?$AAa?$AAp?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00053A98][0005:00000A98] __imp____CxxFrameHandler3(__imp____CxxFrameHandler3)
PublicSymbol: [000537EC][0005:000007EC] __imp__crGetLastErrorMsgA@8(__imp__crGetLastErrorMsgA@8)
PublicSymbol: [000406C2][0002:000296C2] __resetstkoflw(__resetstkoflw)
PublicSymbol: [00053A4C][0005:00000A4C] __imp___commode(__imp___commode)
PublicSymbol: [0001AC80][0002:00003C80] ??_ECAboutDlg@@UAEPAXI@Z(public: virtual void * __thiscall CAboutDlg::`vector deleting destructor'(unsigned int))
PublicSymbol: [000525B4][0004:000005B4] ?__LINE__Var@?1??Attach@CRegKey@ATL@@QAEXPAUHKEY__@@@Z@4JA(long `public: void __thiscall ATL::CRegKey::Attach(struct HKEY__ *)'::`2'::__LINE__Var)
PublicSymbol: [0004F9E8][0003:000079E8] ??_R1A@?0A@EA@type_info@@8(type_info::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0001EEF0][0002:00007EF0] ?DrawTextA@?$CDCT@$00@WTL@@QAEHPADHPAUtagRECT@@I@Z(public: int __thiscall WTL::CDCT<1>::DrawTextA(char *,int,struct tagRECT *,unsigned int))
PublicSymbol: [0004BE58][0003:00003E58] ?pmField@?$CVarTypeInfo@D@ATL@@2QQtagVARIANT@@DQ3@(public: static char tagVARIANT::* ATL::CVarTypeInfo<char>::pmField)
PublicSymbol: [00034650][0002:0001D650] ?FindKey@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QBEHABQAD@Z(public: int __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::FindKey(char * const &)const )
PublicSymbol: [000538D4][0005:000008D4] __imp__UnhandledExceptionFilter@4(__imp__UnhandledExceptionFilter@4)
PublicSymbol: [0004CB78][0003:00004B78] _GUID_DEVCLASS_PNPPRINTERS(_GUID_DEVCLASS_PNPPRINTERS)
PublicSymbol: [000162CA][0001:000152CA] __enc$textbss$end(__enc$textbss$end)
PublicSymbol: [00052418][0004:00000418] ?__LINE__Var@?1??QueryStringValue@CRegKey@ATL@@QAEJPBDPADPAK@Z@4JA(long `public: long __thiscall ATL::CRegKey::QueryStringValue(char const *,char *,unsigned long *)'::`2'::__LINE__Var)
PublicSymbol: [0002B4C0][0002:000144C0] ?ocscpy_s@@YA_NPA_WIPB_W@Z(bool __cdecl ocscpy_s(wchar_t *,unsigned int,wchar_t const *))
PublicSymbol: [00034950][0002:0001D950] ?Free@?$CAutoVectorPtr@_W@ATL@@QAEXXZ(public: void __thiscall ATL::CAutoVectorPtr<wchar_t>::Free(void))
PublicSymbol: [0003EDA0][0002:00027DA0] ?TryAllocate@CAtlTraceSettings@@QAE_NXZ(public: bool __thiscall CAtlTraceSettings::TryAllocate(void))
PublicSymbol: [0004BF3C][0003:00003F3C] ?pmField@?$CVarTypeInfo@PAUIDispatch@@@ATL@@2QQtagVARIANT@@PAUIDispatch@@Q3@(public: static struct IDispatch * tagVARIANT::* ATL::CVarTypeInfo<struct IDispatch *>::pmField)
PublicSymbol: [0004DDF8][0003:00005DF8] ??_C@_1DG@LHMNHOH@?$AA?$CB?$AAm?$AA_?$AAh?$AAM?$AAa?$AAp?$AA?5?$AA?$CG?$AA?$CG?$AA?5?$AA?$CB?$AAm?$AA_?$AAp?$AAB?$AAu?$AAf?$AAf?$AAe?$AAr?$AAS?$AAt?$AAa?$AAr?$AAt?$AA?$AA@(`string')
PublicSymbol: [000390C4][0002:000220C4] _crExceptionFilter@8(_crExceptionFilter@8)
PublicSymbol: [00033DD0][0002:0001CDD0] ?GetValueAt@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QBEAAPA_WH@Z(public: wchar_t * & __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::GetValueAt(int)const )
PublicSymbol: [00033FE0][0002:0001CFE0] ?Detach@?$CAutoVectorPtr@D@ATL@@QAEPADXZ(public: char * __thiscall ATL::CAutoVectorPtr<char>::Detach(void))
PublicSymbol: [0004BA84][0003:00003A84] ??_C@_1BG@EHKPBFEK@?$AAp?$AAL?$AAo?$AAo?$AAp?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00053A54][0005:00000A54] __imp___configthreadlocale(__imp___configthreadlocale)
PublicSymbol: [0004F270][0003:00007270] ??_R2CUpdateUIBase@WTL@@8(WTL::CUpdateUIBase::`RTTI Base Class Array')
PublicSymbol: [0004F25C][0003:0000725C] ??_R3CUpdateUIBase@WTL@@8(WTL::CUpdateUIBase::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004EAF8][0003:00006AF8] ??_C@_0BB@GLNAEDBD@RegQueryValueExW?$AA@(`string')
PublicSymbol: [0004E580][0003:00006580] ??_C@_1GM@OLMCBDMB@?$AAR?$AAu?$AAn?$AAt?$AAi?$AAm?$AAe?$AA?5?$AAC?$AAh?$AAe?$AAc?$AAk?$AA?5?$AAE?$AAr?$AAr?$AAo?$AAr?$AA?4?$AA?6?$AA?$AN?$AA?5?$AAU?$AAn?$AAa?$AAb?$AAl?$AAe?$AA?5?$AAt?$AAo@(`string')
PublicSymbol: [0004A500][0003:00002500] ??_C@_1EM@NIHNIFFB@?$AAn?$AAN?$AAe?$AAw?$AAL?$AAe?$AAn?$AAg?$AAt?$AAh?$AA?5?$AA?$DM?$AA?$DN?$AA?5?$AAG?$AAe?$AAt?$AAD?$AAa?$AAt?$AAa?$AA?$CI?$AA?$CJ?$AA?9?$AA?$DO?$AAn?$AAA?$AAl?$AAl?$AAo?$AAc?$AAL@(`string')
PublicSymbol: [00039166][0002:00022166] _EnterCriticalSection@4(_EnterCriticalSection@4)
PublicSymbol: [000238E0][0002:0000C8E0] ?GetParent@CWindow@ATL@@QBE?AV12@XZ(public: class ATL::CWindow __thiscall ATL::CWindow::GetParent(void)const )
PublicSymbol: [000525D0][0004:000005D0] ?__LINE__Var@?1??AddValue@CRegParser@ATL@@IAEJAAVCRegKey@3@PBDPAD@Z@4JA(long `protected: long __thiscall ATL::CRegParser::AddValue(class ATL::CRegKey &,char const *,char *)'::`2'::__LINE__Var)
PublicSymbol: [00053844][0005:00000844] __imp__SelectObject@8(__imp__SelectObject@8)
PublicSymbol: [0004C06C][0003:0000406C] ??_7Thank_you@Define_the_symbol__ATL_MIXED@@6B@(const Define_the_symbol__ATL_MIXED::Thank_you::`vftable')
PublicSymbol: [0004C054][0003:00004054] _IID_IPrintDialogServices(_IID_IPrintDialogServices)
PublicSymbol: [0004B688][0003:00003688] ??_C@_0DP@JOHDIOJH@CRegKey?3?3RecurseDeleteKey?5?3?5Fail@(`string')
PublicSymbol: [0001D200][0002:00006200] ?OnCreate@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnCreate(unsigned int,unsigned int,long,int &))
PublicSymbol: [0001C680][0002:00005680] ??0?$CStaticT@VCWindow@ATL@@@WTL@@QAE@PAUHWND__@@@Z(public: __thiscall WTL::CStaticT<class ATL::CWindow>::CStaticT<class ATL::CWindow>(struct HWND__ *))
PublicSymbol: [0002C0F0][0002:000150F0] ?AtlW2AHelper@@YGPADPADPB_WHI@Z(char * __stdcall AtlW2AHelper(char *,wchar_t const *,int,unsigned int))
PublicSymbol: [0004E990][0003:00006990] ??_C@_1GG@PHGJJHBC@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAc?$AAr?$AAt?$AA_?$AAb?$AAl?$AAd?$AA?2?$AAs?$AAe?$AAl?$AAf?$AA_?$AAx?$AA8?$AA6?$AA?2?$AAc@(`string')
PublicSymbol: [00053978][0005:00000978] __imp__lstrcpyA@8(__imp__lstrcpyA@8)
PublicSymbol: [0001F570][0002:00008570] ?_ParseColorString@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@SAKPAD@Z(public: static unsigned long __cdecl WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::_ParseColorString(char *))
PublicSymbol: [00052E74][0004:00000E74] ?wsprintffp@@3P6AHPADPBDZZA(int (__cdecl* wsprintffp)(char *,char const *,...))
PublicSymbol: [00052958][0004:00000958] ?atlTraceSecurity@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceSecurity)
PublicSymbol: [0001AB70][0002:00003B70] ?CloseDialog@CAboutDlg@@QAEXH@Z(public: void __thiscall CAboutDlg::CloseDialog(int))
PublicSymbol: [00053890][0005:00000890] __imp__GetLastError@0(__imp__GetLastError@0)
PublicSymbol: [00039202][0002:00022202] _GetClassNameA@12(_GetClassNameA@12)
PublicSymbol: [0003FC60][0002:00028C60] ??0CAtlTraceModuleInfo@@QAE@XZ(public: __thiscall CAtlTraceModuleInfo::CAtlTraceModuleInfo(void))
PublicSymbol: [0004D240][0003:00005240] ??_C@_0DL@JGBOLPLE@f?3?2dd?2vctools?2vc7libs?2ship?2atlmf@(`string')
PublicSymbol: [00038C60][0002:00021C60] ??0Wrapper@?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@QAE@ABQAVCMessageFilter@WTL@@@Z(public: __thiscall ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *> >::Wrapper::Wrapper(class WTL::CMessageFilter * const &))
PublicSymbol: [00040840][0002:00029840] ?__ArrayUnwind@@YGXPAXIHP6EX0@Z@Z(void __stdcall __ArrayUnwind(void *,unsigned int,int,void (__thiscall*)(void *)))
PublicSymbol: [0004A45C][0003:0000245C] ??_C@_1BI@OICILGEO@?$AAn?$AAW?$AAi?$AAd?$AAt?$AAh?$AA?5?$AA?$DO?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [00053838][0005:00000838] __imp__DeleteObject@4(__imp__DeleteObject@4)
PublicSymbol: [0004E85C][0003:0000685C] ??_C@_0CK@FEGOIOPB@Cast?5to?5smaller?5type?5causing?5los@(`string')
PublicSymbol: [00053C58][0005:00000C58] __imp__GetWindowTextA@12(__imp__GetWindowTextA@12)
PublicSymbol: [0003FEA0][0002:00028EA0] ??0CAtlTraceSettings@@QAE@XZ(public: __thiscall CAtlTraceSettings::CAtlTraceSettings(void))
PublicSymbol: [0002BA20][0002:00014A20] ?Release@CRegObject@ATL@@UAGKXZ(public: virtual unsigned long __stdcall ATL::CRegObject::Release(void))
PublicSymbol: [0003A360][0002:00023360] _AtlTraceRegister@8(_AtlTraceRegister@8)
PublicSymbol: [00022C30][0002:0000BC30] ?Create@CWindow@ATL@@QAEPAUHWND__@@PBDPAU3@V_U_RECT@2@0KKV_U_MENUorID@2@PAX@Z(public: struct HWND__ * __thiscall ATL::CWindow::Create(char const *,struct HWND__ *,class ATL::_U_RECT,char const *,unsigned long,unsigned long,class ATL::_U_MENUorID,void *))
PublicSymbol: [00040F10][0002:00029F10] ___report_gsfailure(___report_gsfailure)
PublicSymbol: [0004CD6C][0003:00004D6C] _GUID_DEVCLASS_FSFILTER_CONTENTSCREENER(_GUID_DEVCLASS_FSFILTER_CONTENTSCREENER)
PublicSymbol: [0004B3D8][0003:000033D8] ??_C@_0CE@HPLFHOFC@NextToken?5?3?5Unexpected?5End?5of?5Fi@(`string')
PublicSymbol: [0001DA50][0002:00006A50] ?OnLButtonUp@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnLButtonUp(unsigned int,unsigned int,long,int &))
PublicSymbol: [0004CBC8][0003:00004BC8] _GUID_DEVCLASS_PROCESSOR(_GUID_DEVCLASS_PROCESSOR)
PublicSymbol: [00052140][0004:00000140] ??_R0?AVCWindow@ATL@@@8(class ATL::CWindow `RTTI Type Descriptor')
PublicSymbol: [00052904][0004:00000904] ?clash@Inconsistent_definition_of_symbol__ATL_MIXED@@3U_Please_define_it_the_same_throughout_your_project@1@A(struct Inconsistent_definition_of_symbol__ATL_MIXED::_Please_define_it_the_same_throughout_your_project Inconsistent_definition_of_symbol__ATL_MIXED::clash)
PublicSymbol: [0003C1D0][0002:000251D0] ?strcpy_s@Checked@ATL@@YAXPADIPBD@Z(void __cdecl ATL::Checked::strcpy_s(char *,unsigned int,char const *))
PublicSymbol: [00039262][0002:00022262] _SetWindowTextA@8(_SetWindowTextA@8)
PublicSymbol: [0004D480][0003:00005480] ??_C@_1GI@HPMIGMAH@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [00022470][0002:0000B470] ??0CStaticDataInitCriticalSectionLock@WTL@@QAE@XZ(public: __thiscall WTL::CStaticDataInitCriticalSectionLock::CStaticDataInitCriticalSectionLock(void))
PublicSymbol: [0003909A][0002:0002209A] _crAddPropertyA@8(_crAddPropertyA@8)
PublicSymbol: [0004C434][0003:00004434] _CLSID_CurrentUserClasses(_CLSID_CurrentUserClasses)
PublicSymbol: [0005293C][0004:0000093C] ?atlTraceException@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceException)
PublicSymbol: [00040EF6][0002:00029EF6] _strcpy_s(_strcpy_s)
PublicSymbol: [0004C7B8][0003:000047B8] __DBBMKGUID(__DBBMKGUID)
PublicSymbol: [0004C4FC][0003:000044FC] _CLSID_StockColorPage(_CLSID_StockColorPage)
PublicSymbol: [00042020][0002:0002B020] ?_RTC_GetSrcLine@@YAHPAEPA_WKPAH1K@Z(int __cdecl _RTC_GetSrcLine(unsigned char *,wchar_t *,unsigned long,int *,wchar_t *,unsigned long))
PublicSymbol: [0001CEC0][0002:00005EC0] ??BCTraceCategory@ATL@@QBEKXZ(public: __thiscall ATL::CTraceCategory::operator unsigned long(void)const )
PublicSymbol: [00041C00][0002:0002AC00] __RTC_Terminate(__RTC_Terminate)
PublicSymbol: [00036590][0002:0001F590] ?OnFileNewWindow@CMainDlg@@QAEJGGPAUHWND__@@AAH@Z(public: long __thiscall CMainDlg::OnFileNewWindow(unsigned short,unsigned short,struct HWND__ *,int &))
PublicSymbol: [000525C0][0004:000005C0] ?__LINE__Var@?1??DeleteValue@CRegKey@ATL@@QAEJPBD@Z@4JA(long `public: long __thiscall ATL::CRegKey::DeleteValue(char const *)'::`2'::__LINE__Var)
PublicSymbol: [00022B40][0002:0000BB40] ??0_U_MENUorID@ATL@@QAE@I@Z(public: __thiscall ATL::_U_MENUorID::_U_MENUorID(unsigned int))
PublicSymbol: [0003F2F0][0002:000282F0] ?TakeSnapshot@CAtlAllocator@@QAEXXZ(public: void __thiscall CAtlAllocator::TakeSnapshot(void))
PublicSymbol: [0004D068][0003:00005068] ??_C@_0BD@PJPCCAKP@atlTraceAllocation?$AA@(`string')
PublicSymbol: [00053910][0005:00000910] __imp__InterlockedCompareExchange@12(__imp__InterlockedCompareExchange@12)
PublicSymbol: [0004F1D0][0003:000071D0] ??_R1CM@?0A@EA@?$CUpdateUI@VCMainDlg@@@WTL@@8(WTL::CUpdateUI<class CMainDlg>::`RTTI Base Class Descriptor at (44,-1,0,64)')
PublicSymbol: [00025A20][0002:0000EA20] ?data@CStringData@WTL@@QAEPADXZ(public: char * __thiscall WTL::CStringData::data(void))
PublicSymbol: [0002EA80][0002:00017A80] __tcsncpy_s(__tcsncpy_s)
PublicSymbol: [00038D50][0002:00021D50] ??$?2PAVCMessageFilter@WTL@@@Wrapper@?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@SAPAXIPAPAVCMessageFilter@WTL@@@Z(public: static void * __cdecl ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *> >::Wrapper::operator new<class WTL::CMessageFilter *>(unsigned int,class WTL::CMessageFilter * *))
PublicSymbol: [00052EAC][0004:00000EAC] ___native_startup_lock(___native_startup_lock)
PublicSymbol: [000229E0][0002:0000B9E0] ??1CStaticDataInitCriticalSectionLock@WTL@@QAE@XZ(public: __thiscall WTL::CStaticDataInitCriticalSectionLock::~CStaticDataInitCriticalSectionLock(void))
PublicSymbol: [00053A88][0005:00000A88] __imp___snwprintf_s(__imp___snwprintf_s)
PublicSymbol: [0003F1F0][0002:000281F0] ?RemoveModule@CAtlAllocator@@QAE_NH@Z(public: bool __thiscall CAtlAllocator::RemoveModule(int))
PublicSymbol: [0004C808][0003:00004808] _CLSID_GblComponentCategoriesMgr(_CLSID_GblComponentCategoriesMgr)
PublicSymbol: [00053C24][0005:00000C24] __imp__PostMessageA@16(__imp__PostMessageA@16)
PublicSymbol: [0003E4E0][0002:000274E0] ?Open@CAtlAllocator@@QAE_NPBD@Z(public: bool __thiscall CAtlAllocator::Open(char const *))
PublicSymbol: [0004D124][0003:00005124] ??_C@_0O@KHOBHOBJ@atlTraceISAPI?$AA@(`string')
PublicSymbol: [00041A56][0002:0002AA56] __CRT_RTC_INITW(__CRT_RTC_INITW)
PublicSymbol: [0004C768][0003:00004768] _IID_INotifyDBEvents(_IID_INotifyDBEvents)
PublicSymbol: [0004D114][0003:00005114] ??_C@_0N@KLPNCFEE@atlTraceSync?$AA@(`string')
PublicSymbol: [0004EA44][0003:00006A44] ??_C@_1CK@HHHCOJPD@?$AAE?$AAn?$AAv?$AAi?$AAr?$AAo?$AAn?$AAm?$AAe?$AAn?$AAt?$AAD?$AAi?$AAr?$AAe?$AAc?$AAt?$AAo?$AAr?$AAy?$AA?$AA@(`string')
PublicSymbol: [000338B0][0002:0001C8B0] ??0_ATL_MODULE70@ATL@@QAE@XZ(public: __thiscall ATL::_ATL_MODULE70::_ATL_MODULE70(void))
PublicSymbol: [0001CC80][0002:00005C80] ??0?$CWindowImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@ATL@@QAE@XZ(public: __thiscall ATL::CWindowImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::CWindowImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >(void))
PublicSymbol: [000525A8][0004:000005A8] ?__LINE__Var@?1??PreProcessBuffer@CRegParser@ATL@@QAEJPADPAPAD@Z@4JA(long `public: long __thiscall ATL::CRegParser::PreProcessBuffer(char *,char * *)'::`2'::__LINE__Var)
PublicSymbol: [00042650][0002:0002B650] _IsProcessorFeaturePresent@4(_IsProcessorFeaturePresent@4)
PublicSymbol: [00052458][0004:00000458] ??_R0?AVCAppModule@WTL@@@8(class WTL::CAppModule `RTTI Type Descriptor')
PublicSymbol: [00040EEA][0002:00029EEA] _wcscpy_s(_wcscpy_s)
PublicSymbol: [00029000][0002:00012000] ?GetCodeAddress@_stdcallthunk@ATL@@QAEPAXXZ(public: void * __thiscall ATL::_stdcallthunk::GetCodeAddress(void))
PublicSymbol: [00053ADC][0005:00000ADC] __imp__malloc(__imp__malloc)
PublicSymbol: [000426D4][0002:0002B6D4] _GetSystemTimeAsFileTime@4(_GetSystemTimeAsFileTime@4)
PublicSymbol: [0004CC04][0003:00004C04] _GUID_DEVCLASS_SECURITYACCELERATOR(_GUID_DEVCLASS_SECURITYACCELERATOR)
PublicSymbol: [00024170][0002:0000D170] ??0CClientDC@WTL@@QAE@PAUHWND__@@@Z(public: __thiscall WTL::CClientDC::CClientDC(struct HWND__ *))
PublicSymbol: [00052940][0004:00000940] ?atlTraceTime@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceTime)
PublicSymbol: [00041B1A][0002:0002AB1A] __configthreadlocale(__configthreadlocale)
PublicSymbol: [00039D40][0002:00022D40] ??0CAtlWinModule@ATL@@QAE@XZ(public: __thiscall ATL::CAtlWinModule::CAtlWinModule(void))
PublicSymbol: [0001BB10][0002:00004B10] ??1?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@UAE@XZ(public: virtual __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::~CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >(void))
PublicSymbol: [000525F0][0004:000005F0] ??_R0?AVCDocumentDlg@@@8(class CDocumentDlg `RTTI Type Descriptor')
PublicSymbol: [00040EDE][0002:00029EDE] __vsnwprintf_s(__vsnwprintf_s)
PublicSymbol: [0003A510][0002:00023510] _AtlTraceModifyProcess@20(_AtlTraceModifyProcess@20)
PublicSymbol: [0004C290][0003:00004290] _CLSID_PSDragDrop(_CLSID_PSDragDrop)
PublicSymbol: [0004C538][0003:00004538] _CLSID_ConvertVBX(_CLSID_ConvertVBX)
PublicSymbol: [0004B9B0][0003:000039B0] ??_C@_0DE@HNLODGGM@Warning?3?5implicit?5LoadString?$CI?$CFu?$CJ@(`string')
PublicSymbol: [0001E840][0002:00007840] ?CalcLabelRect@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::CalcLabelRect(void))
PublicSymbol: [0002A600][0002:00013600] ??1_ATL_MODULE70@ATL@@QAE@XZ(public: __thiscall ATL::_ATL_MODULE70::~_ATL_MODULE70(void))
PublicSymbol: [00038310][0002:00021310] ?UIAddChildWindowContainer@CUpdateUIBase@WTL@@QAEHPAUHWND__@@@Z(public: int __thiscall WTL::CUpdateUIBase::UIAddChildWindowContainer(struct HWND__ *))
PublicSymbol: [0002EBA0][0002:00017BA0] ?AtlCoTaskMemCAlloc@ATL@@YAPAXKK@Z(void * __cdecl ATL::AtlCoTaskMemCAlloc(unsigned long,unsigned long))
PublicSymbol: [00041C3C][0002:0002AC3C] ___getmainargs(___getmainargs)
PublicSymbol: [0001D430][0002:00006430] ?OnPaint@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnPaint(unsigned int,unsigned int,long,int &))
PublicSymbol: [00035A60][0002:0001EA60] ?_LoadString@CString@WTL@@KGHIPADI@Z(protected: static int __stdcall WTL::CString::_LoadString(unsigned int,char *,unsigned int))
PublicSymbol: [0003923E][0002:0002223E] _GetMessageA@16(_GetMessageA@16)
PublicSymbol: [000491B8][0003:000011B8] ??_C@_0BJ@NBDKCBDH@?2CrashRptTestd?4exe?0?5?9203?$AA@(`string')
PublicSymbol: [00040EFC][0002:00029EFC] _calloc(_calloc)
PublicSymbol: [0004C9AC][0003:000049AC] _GUID_DEVCLASS_FLOPPYDISK(_GUID_DEVCLASS_FLOPPYDISK)
PublicSymbol: [00037080][0002:00020080] ?AddString@?$CComboBoxT@VCWindow@ATL@@@WTL@@QAEHPBD@Z(public: int __thiscall WTL::CComboBoxT<class ATL::CWindow>::AddString(char const *))
PublicSymbol: [0004EA2C][0003:00006A2C] ??_C@_01KDCPPGHE@r?$AA@(`string')
PublicSymbol: [00039E40][0002:00022E40] ??1CAtlWinModule@ATL@@QAE@XZ(public: __thiscall ATL::CAtlWinModule::~CAtlWinModule(void))
PublicSymbol: [0004271C][0002:0002B71C] _CoUnmarshalInterface@12(_CoUnmarshalInterface@12)
PublicSymbol: [00048B2C][0003:00000B2C] ??_C@_04IHBFOLAC@Mime?$AA@(`string')
PublicSymbol: [000525E0][0004:000005E0] ?__LINE__Var@?1??ChToByte@CRegParser@ATL@@KAED@Z@4JA@506b6aa8(?__LINE__Var@?1??ChToByte@CRegParser@ATL@@KAED@Z@4JA@506b6aa8)
PublicSymbol: [00053C64][0005:00000C64] __imp__CreateWindowExA@48(__imp__CreateWindowExA@48)
PublicSymbol: [000292E0][0002:000122E0] ??0_ATL_MSG@ATL@@QAE@PAUHWND__@@IIJH@Z(public: __thiscall ATL::_ATL_MSG::_ATL_MSG(struct HWND__ *,unsigned int,unsigned int,long,int))
PublicSymbol: [0004CDF8][0003:00004DF8] _GUID_DEVCLASS_FSFILTER_PHYSICALQUOTAMANAGEMENT(_GUID_DEVCLASS_FSFILTER_PHYSICALQUOTAMANAGEMENT)
PublicSymbol: [0004E8C8][0003:000068C8] ?ProcessDetach@NativeDll@<CrtImplementationDetails>@@0IB(private: static unsigned int const <CrtImplementationDetails>::NativeDll::ProcessDetach)
PublicSymbol: [0004E310][0003:00006310] ??_C@_0NN@NGPKDKPD@The?5value?5of?5ESP?5was?5not?5properl@(`string')
PublicSymbol: [00034770][0002:0001D770] ?InternalSetAtIndex@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QAEXHABQADABQA_W@Z(public: void __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::InternalSetAtIndex(int,char * const &,wchar_t * const &))
PublicSymbol: [00052E9C][0004:00000E9C] ___native_startup_state(___native_startup_state)
PublicSymbol: [0003D230][0002:00026230] ?AtlGetVersion@ATL@@YGKPAX@Z(unsigned long __stdcall ATL::AtlGetVersion(void *))
PublicSymbol: [0003933A][0002:0002233A] _DeleteDC@4(_DeleteDC@4)
PublicSymbol: [0004F718][0003:00007718] ??_R3CAtlModule@ATL@@8(ATL::CAtlModule::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004D0F0][0003:000050F0] ??_C@_0N@FEAAACHD@atlTraceUtil?$AA@(`string')
PublicSymbol: [0004F72C][0003:0000772C] ??_R2CAtlModule@ATL@@8(ATL::CAtlModule::`RTTI Base Class Array')
PublicSymbol: [00025C90][0002:0000EC90] ?GetData@CString@WTL@@IBEPAUCStringData@2@XZ(protected: struct WTL::CStringData * __thiscall WTL::CString::GetData(void)const )
PublicSymbol: [00053CCC][0005:00000CCC] __imp__DefWindowProcA@16(__imp__DefWindowProcA@16)
PublicSymbol: [0004BCC8][0003:00003CC8] ??_C@_0EA@DACBAMBK@d?3?2projects?2crashrpt?2trunk?2repor@(`string')
PublicSymbol: [00037330][0002:00020330] ?Create@?$CDialogImpl@VCDocumentDlg@@VCWindow@ATL@@@ATL@@QAEPAUHWND__@@PAU3@J@Z(public: struct HWND__ * __thiscall ATL::CDialogImpl<class CDocumentDlg,class ATL::CWindow>::Create(struct HWND__ *,long))
PublicSymbol: [0003FB60][0002:00028B60] ?InternalSetAtIndex@?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAEXHABK@Z(public: void __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long> >::InternalSetAtIndex(int,unsigned long const &))
PublicSymbol: [000521E0][0004:000001E0] ??_R0?AVCMessageFilter@WTL@@@8(class WTL::CMessageFilter `RTTI Type Descriptor')
PublicSymbol: [00042656][0002:0002B656] _InterlockedCompareExchange@12(_InterlockedCompareExchange@12)
PublicSymbol: [0002A5A0][0002:000135A0] ??1CAtlModule@ATL@@UAE@XZ(public: virtual __thiscall ATL::CAtlModule::~CAtlModule(void))
PublicSymbol: [00039190][0002:00022190] _FreeLibrary@4(_FreeLibrary@4)
PublicSymbol: [0004C0EC][0003:000040EC] _IID_IProxyManager(_IID_IProxyManager)
PublicSymbol: [0004AA34][0003:00002A34] ??_C@_1BG@OHCCAGNO@?$AA?$CB?$AAm?$AA_?$AAb?$AAL?$AAo?$AAc?$AAk?$AAe?$AAd?$AA?$AA@(`string')
PublicSymbol: [00039322][0002:00022322] _GetWindow@8(_GetWindow@8)
PublicSymbol: [000350E0][0002:0001E0E0] ??$?3PAD@?$Wrapper@PAD@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@SAXPAXPAPAD@Z(public: static void __cdecl ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Wrapper<char *>::operator delete<char *>(void *,char * *))
PublicSymbol: [0004C72C][0003:0000472C] _IID_ICursorMove(_IID_ICursorMove)
PublicSymbol: [00049910][0003:00001910] ??_C@_1BK@NJACIMEL@?$AAm?$AA_?$AAh?$AAF?$AAo?$AAn?$AAt?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004D0E0][0003:000050E0] ??_C@_0M@IOBBMAHG@atlTraceMap?$AA@(`string')
PublicSymbol: [00039220][0002:00022220] _GetSysColor@4(_GetSysColor@4)
PublicSymbol: [00039154][0002:00022154] _InterlockedIncrement@4(_InterlockedIncrement@4)
PublicSymbol: [00033C80][0002:0001CC80] ?Lookup@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QBEPA_WABQAD@Z(public: wchar_t * __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Lookup(char * const &)const )
PublicSymbol: [0002BB10][0002:00014B10] ?Init@CComSafeDeleteCriticalSection@ATL@@QAEJXZ(public: long __thiscall ATL::CComSafeDeleteCriticalSection::Init(void))
PublicSymbol: [00048CD8][0003:00000CD8] ??_7CMainDlg@@6B?$CDialogImpl@VCMainDlg@@VCWindow@ATL@@@ATL@@@(const CMainDlg::`vftable'{for `ATL::CDialogImpl<class CMainDlg,class ATL::CWindow>'})
PublicSymbol: [00053984][0005:00000984] __imp__RaiseException@16(__imp__RaiseException@16)
PublicSymbol: [00039112][0002:00022112] _lstrlenA@4(_lstrlenA@4)
PublicSymbol: [0003932E][0002:0002232E] _CreateFontIndirectA@4(_CreateFontIndirectA@4)
PublicSymbol: [0004264A][0002:0002B64A] _HeapFree@12(_HeapFree@12)
PublicSymbol: [0004CF4C][0003:00004F4C] _GUID_DEVINTERFACE_COMPORT(_GUID_DEVINTERFACE_COMPORT)
PublicSymbol: [00053A94][0005:00000A94] __imp_??3@YAXPAX@Z(__imp_??3@YAXPAX@Z)
PublicSymbol: [0004BE94][0003:00003E94] ?VT@?$CVarTypeInfo@H@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<int>::VT)
PublicSymbol: [00039178][0002:00022178] _lstrlenW@4(_lstrlenW@4)
PublicSymbol: [00038960][0002:00021960] ?InternalSetAtIndex@?$CSimpleArray@PAVCMessageFilter@WTL@@V?$CSimpleArrayEqualHelper@PAVCMessageFilter@WTL@@@ATL@@@ATL@@QAEXHABQAVCMessageFilter@WTL@@@Z(public: void __thiscall ATL::CSimpleArray<class WTL::CMessageFilter *,class ATL::CSimpleArrayEqualHelper<class WTL::CMessageFilter *> >::InternalSetAtIndex(int,class WTL::CMessageFilter * const &))
PublicSymbol: [0004D3A8][0003:000053A8] ??_C@_1GM@OMLOBEGL@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [0001F710][0002:00008710] ?CalcLabelParts@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBEXAAPADAAH0101@Z(public: void __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::CalcLabelParts(char * &,int &,char * &,int &,char * &,int &)const )
PublicSymbol: [0004CCB8][0003:00004CB8] _GUID_DEVCLASS_VOLUME(_GUID_DEVCLASS_VOLUME)
PublicSymbol: [0004CE98][0003:00004E98] _GUID_DEVINTERFACE_PARTITION(_GUID_DEVINTERFACE_PARTITION)
PublicSymbol: [000525E8][0004:000005E8] ?__LINE__Var@?1??UpdateRegistryFromResourceS@CAtlModule@ATL@@QAGJIHPAU_ATL_REGMAP_ENTRY@3@@Z@4JA(long `public: long __stdcall ATL::CAtlModule::UpdateRegistryFromResourceS(unsigned int,int,struct ATL::_ATL_REGMAP_ENTRY *)'::`2'::__LINE__Var)
PublicSymbol: [00041B70][0002:0002AB70] __invoke_watson_if_error(__invoke_watson_if_error)
PublicSymbol: [0003A600][0002:00023600] _AtlTraceModifyModule@16(_AtlTraceModifyModule@16)
PublicSymbol: [0004F770][0003:00007770] ??_R2_ATL_MODULE70@ATL@@8(ATL::_ATL_MODULE70::`RTTI Base Class Array')
PublicSymbol: [0004F75C][0003:0000775C] ??_R3_ATL_MODULE70@ATL@@8(ATL::_ATL_MODULE70::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0003B110][0002:00024110] _AtlTraceVU(_AtlTraceVU)
PublicSymbol: [0004A680][0003:00002680] ??_C@_1CE@KKGCPNG@?$AAp?$AAD?$AAa?$AAt?$AAa?$AA?9?$AA?$DO?$AAn?$AAR?$AAe?$AAf?$AAs?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [000404B2][0002:000294B2] _memcpy(_memcpy)
PublicSymbol: [0004C4D4][0003:000044D4] _CLSID_CPicturePropPage(_CLSID_CPicturePropPage)
PublicSymbol: [0001CDE0][0002:00005DE0] ??1?$CWindowImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@ATL@@UAE@XZ(public: virtual __thiscall ATL::CWindowImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::~CWindowImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >(void))
PublicSymbol: [0003AD10][0002:00023D10] _AtlTraceVA(_AtlTraceVA)
PublicSymbol: [0004C718][0003:00004718] _IID_ICursor(_IID_ICursor)
PublicSymbol: [000419B0][0002:0002A9B0] __RTC_SetErrorType(__RTC_SetErrorType)
PublicSymbol: [00053C20][0005:00000C20] __imp__PostQuitMessage@4(__imp__PostQuitMessage@4)
PublicSymbol: [00025740][0002:0000E740] ?wvsprintf_x@SecureHelper@WTL@@YAHPADIPBD0@Z(int __cdecl WTL::SecureHelper::wvsprintf_x(char *,unsigned int,char const *,char *))
PublicSymbol: [00039214][0002:00022214] _CharNextA@4(_CharNextA@4)
PublicSymbol: [00053AE4][0005:00000AE4] __imp__memcpy_s(__imp__memcpy_s)
PublicSymbol: [00052EB0][0004:00000EB0] __NoHeapEnableTerminationOnCorruption(__NoHeapEnableTerminationOnCorruption)
PublicSymbol: [0004F6D0][0003:000076D0] ??_R3?$CAtlModuleT@VCComModule@ATL@@@ATL@@8(ATL::CAtlModuleT<class ATL::CComModule>::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004F6E4][0003:000076E4] ??_R2?$CAtlModuleT@VCComModule@ATL@@@ATL@@8(ATL::CAtlModuleT<class ATL::CComModule>::`RTTI Base Class Array')
PublicSymbol: [000393BE][0002:000223BE] _SysFreeString@4(_SysFreeString@4)
PublicSymbol: [0004E6C0][0003:000066C0] ??_C@_0EI@CLEPFNGI@Stack?5area?5around?5_alloca?5memory@(`string')
PublicSymbol: [0001ADF0][0002:00003DF0] ??_ECHyperLink@WTL@@UAEPAXI@Z(public: virtual void * __thiscall WTL::CHyperLink::`vector deleting destructor'(unsigned int))
PublicSymbol: [00027600][0002:00010600] ??1?$CDCT@$00@WTL@@QAE@XZ(public: __thiscall WTL::CDCT<1>::~CDCT<1>(void))
PublicSymbol: [00022AF0][0002:0000BAF0] ??0_U_MENUorID@ATL@@QAE@PAUHMENU__@@@Z(public: __thiscall ATL::_U_MENUorID::_U_MENUorID(struct HMENU__ *))
PublicSymbol: [000374B0][0002:000204B0] ?DestroyWindow@?$CDialogImpl@VCDocumentDlg@@VCWindow@ATL@@@ATL@@QAEHXZ(public: int __thiscall ATL::CDialogImpl<class CDocumentDlg,class ATL::CWindow>::DestroyWindow(void))
PublicSymbol: [0004E090][0003:00006090] ??_C@_1FA@DALEHHGL@?$AA_?$AA_?$AAn?$AAa?$AAt?$AAi?$AAv?$AAe?$AA_?$AAs?$AAt?$AAa?$AAr?$AAt?$AAu?$AAp?$AA_?$AAs?$AAt?$AAa?$AAt?$AAe?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA_?$AA_?$AAi?$AAn?$AAi?$AAt@(`string')
PublicSymbol: [00052928][0004:00000928] ?atlTraceDBClient@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceDBClient)
PublicSymbol: [0004C2E0][0003:000042E0] _CLSID_StaticDib(_CLSID_StaticDib)
PublicSymbol: [0004AA50][0003:00002A50] ??_C@_1BE@NPGJNKLA@?$AAm?$AA_?$AAb?$AAL?$AAo?$AAc?$AAk?$AAe?$AAd?$AA?$AA@(`string')
PublicSymbol: [0003E080][0002:00027080] ?Init@CAtlAllocator@@QAE_NPBDK@Z(public: bool __thiscall CAtlAllocator::Init(char const *,unsigned long))
PublicSymbol: [0004D6C8][0003:000056C8] ??_C@_1CE@NDBHOLMF@?$AAp?$AAP?$AAr?$AAo?$AAc?$AAe?$AAs?$AAs?$AAI?$AAn?$AAf?$AAo?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004F410][0003:00007410] ??_R1A@?0A@EA@CAboutDlg@@8(CAboutDlg::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0003FBC0][0002:00028BC0] ??0Wrapper@?$CSimpleArray@UCModuleInfo@CTraceSnapshot@@V?$CSimpleArrayEqualHelper@UCModuleInfo@CTraceSnapshot@@@ATL@@@ATL@@QAE@ABUCModuleInfo@CTraceSnapshot@@@Z(public: __thiscall ATL::CSimpleArray<struct CTraceSnapshot::CModuleInfo,class ATL::CSimpleArrayEqualHelper<struct CTraceSnapshot::CModuleInfo> >::Wrapper::Wrapper(struct CTraceSnapshot::CModuleInfo const &))
PublicSymbol: [00052E84][0004:00000E84] __commode(__commode)
PublicSymbol: [0004BE28][0003:00003E28] ?_Max@?$AtlLimits@_J@ATL@@2_JB(public: static __int64 const ATL::AtlLimits<__int64>::_Max)
PublicSymbol: [0004D054][0003:00005054] ??_C@_0BA@MCBFIDIC@atlTraceNotImpl?$AA@(`string')
PublicSymbol: [0003C760][0002:00025760] ??1?$CComPtr@UIConnectionPointContainer@@@ATL@@QAE@XZ(public: __thiscall ATL::CComPtr<struct IConnectionPointContainer>::~CComPtr<struct IConnectionPointContainer>(void))
PublicSymbol: [0004E154][0003:00006154] ??_C@_0DG@HKJMLLLP@A?5local?5variable?5was?5used?5before@(`string')
PublicSymbol: [0005257C][0004:0000057C] ??_R0?AUIUnknown@@@8(struct IUnknown `RTTI Type Descriptor')
PublicSymbol: [00048BB4][0003:00000BB4] ??_C@_01PLPBNMEI@M?$AA@(`string')
PublicSymbol: [00052950][0004:00000950] ?atlTraceMap@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceMap)
PublicSymbol: [00024B10][0002:0000DB10] ?Format@CString@WTL@@QAAHPBDZZ(public: int __cdecl WTL::CString::Format(char const *,...))
PublicSymbol: [00019E50][0002:00002E50] ??0CMainDlg@@QAE@XZ(public: __thiscall CMainDlg::CMainDlg(void))
PublicSymbol: [00053CAC][0005:00000CAC] __imp__ReleaseCapture@0(__imp__ReleaseCapture@0)
PublicSymbol: [00027280][0002:00010280] ??1?$CWindowImplRoot@VCWindow@ATL@@@ATL@@UAE@XZ(public: virtual __thiscall ATL::CWindowImplRoot<class ATL::CWindow>::~CWindowImplRoot<class ATL::CWindow>(void))
PublicSymbol: [0001F040][0002:00008040]
PublicSymbol: [0003FCC0][0002:00028CC0] ?Reset@CAtlTraceModuleInfo@@QAEXPAUHINSTANCE__@@@Z(public: void __thiscall CAtlTraceModuleInfo::Reset(struct HINSTANCE__ *))
PublicSymbol: [00052924][0004:00000924] ?atlTraceHosting@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceHosting)
PublicSymbol: [0003D7F0][0002:000267F0] ??0?$CComPtr@UIConnectionPointContainer@@@ATL@@QAE@XZ(public: __thiscall ATL::CComPtr<struct IConnectionPointContainer>::CComPtr<struct IConnectionPointContainer>(void))
PublicSymbol: [00039148][0002:00022148] _FlushInstructionCache@12(_FlushInstructionCache@12)
PublicSymbol: [0001F110][0002:00008110] ?SetLabel@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAE_NPBD@Z(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::SetLabel(char const *))
PublicSymbol: [000239B0][0002:0000C9B0] ?GetDlgCtrlID@CWindow@ATL@@QBEHXZ(public: int __thiscall ATL::CWindow::GetDlgCtrlID(void)const )
PublicSymbol: [000525CC][0004:000005CC] ?__LINE__Var@?1??RecurseDeleteKey@CRegKey@ATL@@QAEJPBD@Z@4JA(long `public: long __thiscall ATL::CRegKey::RecurseDeleteKey(char const *)'::`2'::__LINE__Var)
PublicSymbol: [00053CB4][0005:00000CB4] __imp__SetCursor@4(__imp__SetCursor@4)
PublicSymbol: [0004B394][0003:00003394] ??_C@_0BG@IDBEHNDO@HKEY_PERFORMANCE_DATA?$AA@(`string')
PublicSymbol: [0003E9D0][0002:000279D0] ?GetCategoryCount@CAtlAllocator@@QBEHABVCAtlTraceModule@@@Z(public: int __thiscall CAtlAllocator::GetCategoryCount(class CAtlTraceModule const &)const )
PublicSymbol: [0004C880][0003:00004880] _GUID_DEVCLASS_ADAPTER(_GUID_DEVCLASS_ADAPTER)
PublicSymbol: [0004BF30][0003:00003F30] ?VT@?$CVarTypeInfo@PAPAUIUnknown@@@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<struct IUnknown * *>::VT)
PublicSymbol: [0004F58C][0003:0000758C] ??_R2?$CWindowImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@ATL@@8(ATL::CWindowImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::`RTTI Base Class Array')
PublicSymbol: [0004F578][0003:00007578] ??_R3?$CWindowImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@ATL@@8(ATL::CWindowImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [000404A0][0002:000294A0] _free(_free)
PublicSymbol: [00029E40][0002:00012E40] ??0CAppModule@WTL@@QAE@XZ(public: __thiscall WTL::CAppModule::CAppModule(void))
PublicSymbol: [00048BE4][0003:00000BE4] ?rgszNeverDelete@CRegParser@ATL@@1QBQBDB(protected: static char const * const * const ATL::CRegParser::rgszNeverDelete)
PublicSymbol: [0004CB50][0003:00004B50] _GUID_DEVCLASS_NODRIVER(_GUID_DEVCLASS_NODRIVER)
PublicSymbol: [0004D674][0003:00005674] ??_C@_1BE@PGKGHBDF@?$AA?$CF?$AAS?$AA?$CI?$AA?$CF?$AAd?$AA?$CJ?$AA?5?$AA?3?$AA?5?$AA?$AA@(`string')
PublicSymbol: [00026FA0][0002:0000FFA0] ?Remove@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAEHABK@Z(public: int __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >::Remove(unsigned long const &))
PublicSymbol: [00053CC4][0005:00000CC4] __imp__SetRectEmpty@4(__imp__SetRectEmpty@4)
PublicSymbol: [0002B6B0][0002:000146B0] ??1CRegObject@ATL@@UAE@XZ(public: virtual __thiscall ATL::CRegObject::~CRegObject(void))
PublicSymbol: [0004CA10][0003:00004A10] _GUID_DEVCLASS_INFINIBAND(_GUID_DEVCLASS_INFINIBAND)
PublicSymbol: [0004F3C4][0003:000073C4] ??_R4CAboutDlg@@6B@(const CAboutDlg::`RTTI Complete Object Locator')
PublicSymbol: [00052230][0004:00000230] ??_R0?AVCAboutDlg@@@8(class CAboutDlg `RTTI Type Descriptor')
PublicSymbol: [000390D6][0002:000220D6] _crInstallToCurrentThread2@4(_crInstallToCurrentThread2@4)
PublicSymbol: [000279C0][0002:000109C0] ?RemoveAt@?$CSimpleMap@KPAVCMessageLoop@WTL@@V?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@@ATL@@QAEHH@Z(public: int __thiscall ATL::CSimpleMap<unsigned long,class WTL::CMessageLoop *,class ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *> >::RemoveAt(int))
PublicSymbol: [0004CD1C][0003:00004D1C] _GUID_DEVCLASS_FSFILTER_UNDELETE(_GUID_DEVCLASS_FSFILTER_UNDELETE)
PublicSymbol: [000528F8][0004:000008F8] ?WM_ATLGETHOST@ATL@@3IA(unsigned int ATL::WM_ATLGETHOST)
PublicSymbol: [0004BAB0][0003:00003AB0] ??_C@_0BK@BPHGBOEO@throw?5C?$CL?$CL?5typed?5exception?$AA@(`string')
PublicSymbol: [0004F940][0003:00007940] ??_R2?$CDialogImpl@VCDocumentDlg@@VCWindow@ATL@@@ATL@@8(ATL::CDialogImpl<class CDocumentDlg,class ATL::CWindow>::`RTTI Base Class Array')
PublicSymbol: [0004F92C][0003:0000792C] ??_R3?$CDialogImpl@VCDocumentDlg@@VCWindow@ATL@@@ATL@@8(ATL::CDialogImpl<class CDocumentDlg,class ATL::CWindow>::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [00042644][0002:0002B644] _GetProcessHeap@0(_GetProcessHeap@0)
PublicSymbol: [0004BEE4][0003:00003EE4] ?VT@?$CVarTypeInfo@_K@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<unsigned __int64>::VT)
PublicSymbol: [0004BED4][0003:00003ED4] ?VT@?$CVarTypeInfo@_J@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<__int64>::VT)
PublicSymbol: [000490C8][0003:000010C8] ??_C@_0BE@JKHADFNA@nVidia?5GeForce?59800?$AA@(`string')
PublicSymbol: [0004EF9C][0003:00006F9C] ??_R1A@?0A@EA@CMessageLoop@WTL@@8(WTL::CMessageLoop::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0004CA9C][0003:00004A9C] _GUID_DEVCLASS_MONITOR(_GUID_DEVCLASS_MONITOR)
PublicSymbol: [0004C6DC][0003:000046DC] _GUID_FONTSTRIKETHROUGH(_GUID_FONTSTRIKETHROUGH)
PublicSymbol: [0002DC00][0002:00016C00] ?GenerateError@CRegParser@ATL@@IAEJI@Z(protected: long __thiscall ATL::CRegParser::GenerateError(unsigned int))
PublicSymbol: [0004E018][0003:00006018] ??_C@_1GA@EHAKPACL@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAc?$AAr?$AAt?$AA_?$AAb?$AAl?$AAd?$AA?2?$AAs?$AAe?$AAl?$AAf?$AA_?$AAx?$AA8?$AA6?$AA?2?$AAc@(`string')
PublicSymbol: [00053CA8][0005:00000CA8] __imp__GetCapture@0(__imp__GetCapture@0)
PublicSymbol: [0002B780][0002:00014780] ??1CExpansionVector@ATL@@QAE@XZ(public: __thiscall ATL::CExpansionVector::~CExpansionVector(void))
PublicSymbol: [0004E8CC][0003:000068CC] ?ProcessAttach@NativeDll@<CrtImplementationDetails>@@0IB(private: static unsigned int const <CrtImplementationDetails>::NativeDll::ProcessAttach)
PublicSymbol: [0004B4AC][0003:000034AC] ??_C@_1BK@ODFKMBGC@?$AAl?$AAp?$AAs?$AAz?$AAR?$AAe?$AAg?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0003A4B0][0002:000234B0] _AtlTraceRegisterCategoryU@8(_AtlTraceRegisterCategoryU@8)
PublicSymbol: [00053998][0005:00000998] __imp__DeleteCriticalSection@4(__imp__DeleteCriticalSection@4)
PublicSymbol: [0003910C][0002:0002210C] _CreateEventA@16(_CreateEventA@16)
PublicSymbol: [0003AC00][0002:00023C00] _AtlTraceGetUpdateEventNameU@4(_AtlTraceGetUpdateEventNameU@4)
PublicSymbol: [0004F0E4][0003:000070E4] ??_R1A@?0A@EA@?$CWindowImplRoot@VCWindow@ATL@@@ATL@@8(ATL::CWindowImplRoot<class ATL::CWindow>::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [00052910][0004:00000910] ?atlTraceQI@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceQI)
PublicSymbol: [0004C754][0003:00004754] _IID_ICursorUpdateARow(_IID_ICursorUpdateARow)
PublicSymbol: [00039106][0002:00022106] _CreateThread@24(_CreateThread@24)
PublicSymbol: [000538A8][0005:000008A8] __imp__FindResourceA@12(__imp__FindResourceA@12)
PublicSymbol: [0003C890][0002:00025890] ?AtlFreeMarshalStream@ATL@@YGJPAUIStream@@@Z(long __stdcall ATL::AtlFreeMarshalStream(struct IStream *))
PublicSymbol: [0002B2B0][0002:000142B0] ?_AtlGetConversionACP@ATL@@YGIXZ(unsigned int __stdcall ATL::_AtlGetConversionACP(void))
PublicSymbol: [0002C780][0002:00015780] ?ResourceRegisterSz@CRegObject@ATL@@QAGJPB_W00@Z(public: long __stdcall ATL::CRegObject::ResourceRegisterSz(wchar_t const *,wchar_t const *,wchar_t const *))
PublicSymbol: [000325C0][0002:0001B5C0] ??0CExpansionVector@ATL@@QAE@XZ(public: __thiscall ATL::CExpansionVector::CExpansionVector(void))
PublicSymbol: [00039060][0002:00022060] ?one@Thank_you@Define_the_symbol__ATL_MIXED@@UAEXXZ(public: virtual void __thiscall Define_the_symbol__ATL_MIXED::Thank_you::one(void))
PublicSymbol: [00053940][0005:00000940] __imp__CloseHandle@4(__imp__CloseHandle@4)
PublicSymbol: [00053804][0005:00000804] CrashRpt1301d_NULL_THUNK_DATA(CrashRpt1301d_NULL_THUNK_DATA)
PublicSymbol: [00052028][0004:00000028] ?_atltmpDataNil@WTL@@3PAUCStringData@1@A(struct WTL::CStringData * WTL::_atltmpDataNil)
PublicSymbol: [00039280][0002:00022280] _UpdateWindow@4(_UpdateWindow@4)
PublicSymbol: [0003FF90][0002:00028F90] ?CrtDbgReport@CAtlTraceModule@@QAEXP6AHHPBDH00ZZ@Z(public: void __thiscall CAtlTraceModule::CrtDbgReport(int (__cdecl*)(int,char const *,int,char const *,char const *,...)))
PublicSymbol: [000404AC][0002:000294AC] _wcslen(_wcslen)
PublicSymbol: [0004B1FC][0003:000031FC] ??_C@_0CC@MOJNOPNN@Failed?5to?5register?0?5cleaning?5up?$CB@(`string')
PublicSymbol: [00037710][0002:00020710] ?AddMessageFilter@CMessageLoop@WTL@@QAEHPAVCMessageFilter@2@@Z(public: int __thiscall WTL::CMessageLoop::AddMessageFilter(class WTL::CMessageFilter *))
PublicSymbol: [0001CE30][0002:00005E30]
PublicSymbol: [000403D0][0002:000293D0] ??_Etype_info@@UAEPAXI@Z(public: virtual void * __thiscall type_info::`vector deleting destructor'(unsigned int))
PublicSymbol: [0004D080][0003:00005080] ??_C@_0BC@PGMFNIFA@atlTraceException?$AA@(`string')
PublicSymbol: [0004CB64][0003:00004B64] _GUID_DEVCLASS_PCMCIA(_GUID_DEVCLASS_PCMCIA)
PublicSymbol: [00053C10][0005:00000C10] __imp__CallWindowProcA@20(__imp__CallWindowProcA@20)
PublicSymbol: [0004AC18][0003:00002C18] ??_C@_1O@MNLDABAF@?$AAM?$AAo?$AAd?$AAu?$AAl?$AAe?$AA?$AA@(`string')
PublicSymbol: [0004DC20][0003:00005C20] ??_C@_1GM@HJGNLPJ@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [0004CDE4][0003:00004DE4] _GUID_DEVCLASS_FSFILTER_ENCRYPTION(_GUID_DEVCLASS_FSFILTER_ENCRYPTION)
PublicSymbol: [0004C628][0003:00004628] _GUID_YSIZE(_GUID_YSIZE)
PublicSymbol: [0004BEA4][0003:00003EA4] ?VT@?$CVarTypeInfo@I@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<unsigned int>::VT)
PublicSymbol: [0004D2D8][0003:000052D8] ??_C@_0DL@DGOEKOEF@ERROR?5?3?5Unable?5to?5lock?5critical?5@(`string')
PublicSymbol: [0003921A][0002:0002221A] _GetFocus@0(_GetFocus@0)
PublicSymbol: [0001BC30][0002:00004C30] ?SetHyperLink@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAE_NPBD@Z(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::SetHyperLink(char const *))
PublicSymbol: [0001DF20][0002:00006F20] ?Init@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEXXZ(public: void __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::Init(void))
PublicSymbol: [00049978][0003:00001978] ??_C@_1BO@OFFCNEOJ@?$AAl?$AAp?$AAs?$AAt?$AAr?$AAL?$AAi?$AAn?$AAk?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [000202D0][0002:000092D0] ?IsCommandButton@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::IsCommandButton(void)const )
PublicSymbol: [00053C18][0005:00000C18] __imp__LoadImageA@24(__imp__LoadImageA@24)
PublicSymbol: [000492B4][0003:000012B4] ??_C@_0P@NAEFNJBH@CrashRpt?5Tests?$AA@(`string')
PublicSymbol: [0004F238][0003:00007238] ??_R1A@?0A@EA@CUpdateUIBase@WTL@@8(WTL::CUpdateUIBase::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0004BADC][0003:00003ADC] ??_C@_07MIPHKCEO@SIGSEGV?$AA@(`string')
PublicSymbol: [00053A7C][0005:00000A7C] __imp__strcpy_s(__imp__strcpy_s)
PublicSymbol: [00021A30][0002:0000AA30] ?Init@CAppModule@WTL@@QAEJPAU_ATL_OBJMAP_ENTRY30@ATL@@PAUHINSTANCE__@@PBU_GUID@@@Z(public: long __thiscall WTL::CAppModule::Init(struct ATL::_ATL_OBJMAP_ENTRY30 *,struct HINSTANCE__ *,struct _GUID const *))
PublicSymbol: [0001F930][0002:00008930] ?DoEraseBackground@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEXV?$CDCT@$0A@@2@@Z(public: void __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::DoEraseBackground(class WTL::CDCT<0>))
PublicSymbol: [0004C614][0003:00004614] _GUID_XSIZE(_GUID_XSIZE)
PublicSymbol: [000358A0][0002:0001E8A0] ?LoadStringA@CString@WTL@@QAEHI@Z(public: int __thiscall WTL::CString::LoadStringA(unsigned int))
PublicSymbol: [00042686][0002:0002B686] _EncodePointer@4(_EncodePointer@4)
PublicSymbol: [00039394][0002:00022394] _CoUninitialize@0(_CoUninitialize@0)
PublicSymbol: [0004B8FC][0003:000038FC] ??_C@_05NAOIJFC@Error?$AA@(`string')
PublicSymbol: [00039094][0002:00022094] _crAddRegKeyA@12(_crAddRegKeyA@12)
PublicSymbol: [00037C00][0002:00020C00] ?IsDialogMessageA@CWindow@ATL@@QAEHPAUtagMSG@@@Z(public: int __thiscall ATL::CWindow::IsDialogMessageA(struct tagMSG *))
PublicSymbol: [0004E69C][0003:0000669C] ??_C@_02LLMPMKNF@?$DO?5?$AA@(`string')
PublicSymbol: [000261E0][0002:0000F1E0] ?ConcatCopy@CString@WTL@@IAEHHPBDH0@Z(protected: int __thiscall WTL::CString::ConcatCopy(int,char const *,int,char const *))
PublicSymbol: [00053028][0005:00000028] __IMPORT_DESCRIPTOR_KERNEL32(__IMPORT_DESCRIPTOR_KERNEL32)
PublicSymbol: [0001D8B0][0002:000068B0] ?OnMouseLeave@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnMouseLeave(unsigned int,unsigned int,long,int &))
PublicSymbol: [00034170][0002:0001D170] ?Detach@?$CAutoVectorPtr@_W@ATL@@QAEPA_WXZ(public: wchar_t * __thiscall ATL::CAutoVectorPtr<wchar_t>::Detach(void))
PublicSymbol: [0003911E][0002:0002211E] _lstrcmpiA@8(_lstrcmpiA@8)
PublicSymbol: [0004C03C][0003:0000403C] ?cbNeverDelete@CRegParser@ATL@@1HB(protected: static int const ATL::CRegParser::cbNeverDelete)
PublicSymbol: [0002F570][0002:00018570] ?RegisterSubkeys@CRegParser@ATL@@IAEJPADPAUHKEY__@@HH@Z(protected: long __thiscall ATL::CRegParser::RegisterSubkeys(char *,struct HKEY__ *,int,int))
PublicSymbol: [00039540][0002:00022540] ?AddResourceInstance@CAtlBaseModule@ATL@@QAE_NPAUHINSTANCE__@@@Z(public: bool __thiscall ATL::CAtlBaseModule::AddResourceInstance(struct HINSTANCE__ *))
PublicSymbol: [00039268][0002:00022268] _GetWindowTextA@12(_GetWindowTextA@12)
PublicSymbol: [0004CB3C][0003:00004B3C] _GUID_DEVCLASS_NETTRANS(_GUID_DEVCLASS_NETTRANS)
PublicSymbol: [0001B700][0002:00004700] ??0CrAutoInstallHelper@@QAE@PAUtagCR_INSTALL_INFOA@@@Z(public: __thiscall CrAutoInstallHelper::CrAutoInstallHelper(struct tagCR_INSTALL_INFOA *))
PublicSymbol: [0003CCE0][0002:00025CE0] ?AtlComModuleRegisterClassObjects@ATL@@YGJPAU_ATL_COM_MODULE70@1@KK@Z(long __stdcall ATL::AtlComModuleRegisterClassObjects(struct ATL::_ATL_COM_MODULE70 *,unsigned long,unsigned long))
PublicSymbol: [0004BE48][0003:00003E48] ?_Max@?$AtlLimits@_K@ATL@@2_KB(public: static unsigned __int64 const ATL::AtlLimits<unsigned __int64>::_Max)
PublicSymbol: [000392E6][0002:000222E6] _LoadStringA@16(_LoadStringA@16)
PublicSymbol: [0004BBF0][0003:00003BF0] ??_C@_09GGPCJIKP@Restarted?$AA@(`string')
PublicSymbol: [0004268C][0002:0002B68C] _DecodePointer@4(_DecodePointer@4)
PublicSymbol: [00035BC0][0002:0001EBC0] ?OnInitDialog@CMainDlg@@QAEJIIJAAH@Z(public: long __thiscall CMainDlg::OnInitDialog(unsigned int,unsigned int,long,int &))
PublicSymbol: [00034700][0002:0001D700] ?IsEqualKey@CExpansionVectorEqualHelper@ATL@@SA_NQAD0@Z(public: static bool __cdecl ATL::CExpansionVectorEqualHelper::IsEqualKey(char * const,char * const))
PublicSymbol: [0004CA88][0003:00004A88] _GUID_DEVCLASS_MODEM(_GUID_DEVCLASS_MODEM)
PublicSymbol: [00049A88][0003:00001A88] ??_C@_03FENLJLHA@?$DMA?$DO?$AA@(`string')
PublicSymbol: [000525A4][0004:000005A4] ?__LINE__Var@?1??NextToken@CRegParser@ATL@@IAEJPAD@Z@4JA(long `protected: long __thiscall ATL::CRegParser::NextToken(char *)'::`2'::__LINE__Var)
PublicSymbol: [00053AE8][0005:00000AE8] __imp___mbsstr(__imp___mbsstr)
PublicSymbol: [0004C934][0003:00004934] _GUID_DEVCLASS_DISKDRIVE(_GUID_DEVCLASS_DISKDRIVE)
PublicSymbol: [00053C2C][0005:00000C2C] __imp__MapWindowPoints@16(__imp__MapWindowPoints@16)
PublicSymbol: [0001D600][0002:00006600] ?GetResourceInstance@CAtlBaseModule@ATL@@QAEPAUHINSTANCE__@@XZ(public: struct HINSTANCE__ * __thiscall ATL::CAtlBaseModule::GetResourceInstance(void))
PublicSymbol: [00037150][0002:00020150] ?Create@?$CDialogImpl@VCAboutDlg@@VCWindow@ATL@@@ATL@@QAEPAUHWND__@@PAU3@J@Z(public: struct HWND__ * __thiscall ATL::CDialogImpl<class CAboutDlg,class ATL::CWindow>::Create(struct HWND__ *,long))
PublicSymbol: [00030C30][0002:00019C30] ?RecurseDeleteKey@CRegKey@ATL@@QAEJPBD@Z(public: long __thiscall ATL::CRegKey::RecurseDeleteKey(char const *))
PublicSymbol: [00025F50][0002:0000EF50] ?AssignCopy@CString@WTL@@IAEXHPBD@Z(protected: void __thiscall WTL::CString::AssignCopy(int,char const *))
PublicSymbol: [0004C7CC][0003:000047CC] __DBCIDGUID(__DBCIDGUID)
PublicSymbol: [00028D20][0002:00011D20] ?StartDialogProc@?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@SGHPAUHWND__@@IIJ@Z(public: static int __stdcall ATL::CDialogImplBaseT<class ATL::CWindow>::StartDialogProc(struct HWND__ *,unsigned int,unsigned int,long))
PublicSymbol: [000404CA][0002:000294CA] __CxxThrowException@8(__CxxThrowException@8)
PublicSymbol: [00039298][0002:00022298] _SetCapture@4(_SetCapture@4)
PublicSymbol: [0004AAB4][0003:00002AB4] ??_C@_1DK@CBPLJEPG@?$AAp?$AAT?$AAh?$AAi?$AAs?$AA?9?$AA?$DO?$AAm?$AA_?$AAp?$AAC?$AAu?$AAr?$AAr?$AAe?$AAn?$AAt?$AAM?$AAs?$AAg?$AA?5?$AA?$DN?$AA?$DN?$AA?5?$AA?$CG?$AAm?$AAs?$AAg?$AA?$AA@(`string')
PublicSymbol: [00038A60][0002:00021A60] ?InternalSetAtIndex@?$CSimpleArray@PAVCIdleHandler@WTL@@V?$CSimpleArrayEqualHelper@PAVCIdleHandler@WTL@@@ATL@@@ATL@@QAEXHABQAVCIdleHandler@WTL@@@Z(public: void __thiscall ATL::CSimpleArray<class WTL::CIdleHandler *,class ATL::CSimpleArrayEqualHelper<class WTL::CIdleHandler *> >::InternalSetAtIndex(int,class WTL::CIdleHandler * const &))
PublicSymbol: [0004CE48][0003:00004E48] _GUID_DEVCLASS_FSFILTER_SYSTEM(_GUID_DEVCLASS_FSFILTER_SYSTEM)
PublicSymbol: [000198D0][0002:000028D0] ?GetModulePath@@YA?AVCString@WTL@@PAUHINSTANCE__@@@Z(class WTL::CString __cdecl GetModulePath(struct HINSTANCE__ *))
PublicSymbol: [000263E0][0002:0000F3E0] ?AllocBeforeWrite@CString@WTL@@IAEHH@Z(protected: int __thiscall WTL::CString::AllocBeforeWrite(int))
PublicSymbol: [00049148][0003:00001148] ??_C@_1EM@DCPBLPHJ@?$AAc?$AAr?$AA_?$AAi?$AAn?$AAs?$AAt?$AAa?$AAl?$AAl?$AA_?$AAh?$AAe?$AAl?$AAp?$AAe?$AAr?$AA?4?$AAm?$AA_?$AAn?$AAI?$AAn?$AAs?$AAt?$AAa?$AAl?$AAl?$AAS?$AAt?$AAa?$AAt@(`string')
PublicSymbol: [0004A434][0003:00002434] ??_C@_1CA@MKICHHOF@?$AAn?$AAP?$AAr?$AAe?$AAc?$AAi?$AAs?$AAi?$AAo?$AAn?$AA?5?$AA?$DO?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004F2DC][0003:000072DC] ??_R1A@?0A@EA@CMessageFilter@WTL@@8(WTL::CMessageFilter::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0003ABC0][0002:00023BC0] _AtlTraceGetUpdateEventNameA_s@8(_AtlTraceGetUpdateEventNameA_s@8)
PublicSymbol: [0004E690][0003:00006690] ??_C@_08KJEDNCKC@?$CFs?$CFs?$CFs?$CFs?$AA@(`string')
PublicSymbol: [0003E000][0002:00027000] ___InlineInterlockedCompareExchangePointer(___InlineInterlockedCompareExchangePointer)
PublicSymbol: [0005290C][0004:0000090C] ?atlTraceCOM@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceCOM)
PublicSymbol: [0003C2A0][0002:000252A0] ??B?$CA2WEX@$0IA@@ATL@@QBEPA_WXZ(public: __thiscall ATL::CA2WEX<128>::operator wchar_t *(void)const )
PublicSymbol: [00040476][0002:00029476] __CrtDbgReportW(__CrtDbgReportW)
PublicSymbol: [00021F50][0002:0000AF50] ?GetModuleInstance@CAtlBaseModule@ATL@@QAEPAUHINSTANCE__@@XZ(public: struct HINSTANCE__ * __thiscall ATL::CAtlBaseModule::GetModuleInstance(void))
PublicSymbol: [0004BF1C][0003:00003F1C] ?pmField@?$CVarTypeInfo@PA_W@ATL@@2QQtagVARIANT@@PA_WQ3@(public: static wchar_t * tagVARIANT::* ATL::CVarTypeInfo<wchar_t *>::pmField)
PublicSymbol: [00053A60][0005:00000A60] __imp___onexit(__imp___onexit)
PublicSymbol: [00023B40][0002:0000CB40] ?IsWindow@CWindow@ATL@@QBEHXZ(public: int __thiscall ATL::CWindow::IsWindow(void)const )
PublicSymbol: [0004C3BC][0003:000043BC] _CLSID_InProcFreeMarshaler(_CLSID_InProcFreeMarshaler)
PublicSymbol: [000286D0][0002:000116D0] ?AllocateHeap@?$CTempBuffer@D$0CAA@VCCRTAllocator@ATL@@@ATL@@AAEXI@Z(private: void __thiscall ATL::CTempBuffer<char,512,class ATL::CCRTAllocator>::AllocateHeap(unsigned int))
PublicSymbol: [0004CED4][0003:00004ED4] _GUID_DEVINTERFACE_VOLUME(_GUID_DEVINTERFACE_VOLUME)
PublicSymbol: [00034FF0][0002:0001DFF0] ??$AtlAddThrow@K@ATL@@YAKKK@Z(unsigned long __cdecl ATL::AtlAddThrow<unsigned long>(unsigned long,unsigned long))
PublicSymbol: [000426CE][0002:0002B6CE] _GetTickCount@0(_GetTickCount@0)
PublicSymbol: [0003EE40][0002:00027E40] ?IncModuleCount@CAtlTraceProcess@@QAEXI@Z(public: void __thiscall CAtlTraceProcess::IncModuleCount(unsigned int))
PublicSymbol: [000526E8][0004:000006E8] ___globallocalestatus(___globallocalestatus)
PublicSymbol: [00053C9C][0005:00000C9C] __imp__GetClassNameA@12(__imp__GetClassNameA@12)
PublicSymbol: [0004BF24][0003:00003F24] ?pmField@?$CVarTypeInfo@PAPA_W@ATL@@2QQtagVARIANT@@PAPA_WQ3@(public: static wchar_t * * tagVARIANT::* ATL::CVarTypeInfo<wchar_t * *>::pmField)
PublicSymbol: [00024250][0002:0000D250] ??1CClientDC@WTL@@QAE@XZ(public: __thiscall WTL::CClientDC::~CClientDC(void))
PublicSymbol: [0003C430][0002:00025430] ??$AtlConvFreeMemory@_W@ATL@@YAXPA_W0H@Z(void __cdecl ATL::AtlConvFreeMemory<wchar_t>(wchar_t *,wchar_t *,int))
PublicSymbol: [0004F5F4][0003:000075F4] ??_R4CAppModule@WTL@@6B@(const WTL::CAppModule::`RTTI Complete Object Locator')
PublicSymbol: [00027890][0002:00010890] ?Allocate@?$CTempBuffer@D$0CAA@VCCRTAllocator@ATL@@@ATL@@QAEPADI@Z(public: char * __thiscall ATL::CTempBuffer<char,512,class ATL::CCRTAllocator>::Allocate(unsigned int))
PublicSymbol: [0004B8CC][0003:000038CC] ??_C@_0CI@GDJKLIDM@Error?5raising?5noncontinuable?5exc@(`string')
PublicSymbol: [0002BD20][0002:00014D20] ??1CComSafeDeleteCriticalSection@ATL@@QAE@XZ(public: __thiscall ATL::CComSafeDeleteCriticalSection::~CComSafeDeleteCriticalSection(void))
PublicSymbol: [0001ACF0][0002:00003CF0] ??1?$CDialogImpl@VCAboutDlg@@VCWindow@ATL@@@ATL@@UAE@XZ(public: virtual __thiscall ATL::CDialogImpl<class CAboutDlg,class ATL::CWindow>::~CDialogImpl<class CAboutDlg,class ATL::CWindow>(void))
PublicSymbol: [00053AD0][0005:00000AD0] __imp___recalloc(__imp___recalloc)
PublicSymbol: [0004C394][0003:00004394] _CLSID_ComBinding(_CLSID_ComBinding)
PublicSymbol: [000525BC][0004:000005BC] ?__LINE__Var@?1??RegDeleteKeyA@CAtlTransactionManager@ATL@@QAEJPAUHKEY__@@PBD@Z@4JA(long `public: long __thiscall ATL::CAtlTransactionManager::RegDeleteKeyA(struct HKEY__ *,char const *)'::`2'::__LINE__Var)
PublicSymbol: [00052908][0004:00000908] ?atlTraceGeneral@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceGeneral)
PublicSymbol: [000391E4][0002:000221E4] _PtInRect@12(_PtInRect@12)
PublicSymbol: [0004AB70][0003:00002B70] ??_C@_1BG@LENCCAOJ@?$AAp?$AAp?$AAG?$AAI?$AAT?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0002B5C0][0002:000145C0] ?EscapeSingleQuote@CAtlModule@ATL@@SAXPA_WIPB_W@Z(public: static void __cdecl ATL::CAtlModule::EscapeSingleQuote(wchar_t *,unsigned int,wchar_t const *))
PublicSymbol: [00039A80][0002:00022A80] ?RemoveAt@?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAEHH@Z(public: int __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >::RemoveAt(int))
PublicSymbol: [00053900][0005:00000900] __imp__MultiByteToWideChar@24(__imp__MultiByteToWideChar@24)
PublicSymbol: [000528E0][0004:000008E0] ?atlTraceUI@WTL@@3VCTraceCategory@ATL@@A(class ATL::CTraceCategory WTL::atlTraceUI)
PublicSymbol: [00042710][0002:0002B710] _CoMarshalInterface@24(_CoMarshalInterface@24)
PublicSymbol: [00028A40][0002:00011A40] ??1?$CComCritSecLock@VCComCriticalSection@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CComCritSecLock<class ATL::CComCriticalSection>::~CComCritSecLock<class ATL::CComCriticalSection>(void))
PublicSymbol: [0001CD70][0002:00005D70] ?TraceV@CTrace@ATL@@QBAXPBDHKI0PAD@Z(public: void __cdecl ATL::CTrace::TraceV(char const *,int,unsigned long,unsigned int,char const *,char *)const )
PublicSymbol: [00032680][0002:0001B680] ??0CComSafeDeleteCriticalSection@ATL@@QAE@XZ(public: __thiscall ATL::CComSafeDeleteCriticalSection::CComSafeDeleteCriticalSection(void))
PublicSymbol: [0003CA90][0002:00025A90] ?AtlComModuleGetClassObject@ATL@@YGJPAU_ATL_COM_MODULE70@1@ABU_GUID@@1PAPAX@Z(long __stdcall ATL::AtlComModuleGetClassObject(struct ATL::_ATL_COM_MODULE70 *,struct _GUID const &,struct _GUID const &,void * *))
PublicSymbol: [00052930][0004:00000930] ?atlTraceSnapin@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceSnapin)
PublicSymbol: [00052900][0004:00000900] ?clash@Define_the_symbol__ATL_MIXED@@3UThank_you@1@A(struct Define_the_symbol__ATL_MIXED::Thank_you Define_the_symbol__ATL_MIXED::clash)
PublicSymbol: [00053768][0005:00000768] ADVAPI32_NULL_THUNK_DATA(ADVAPI32_NULL_THUNK_DATA)
PublicSymbol: [000425FC][0002:0002B5FC] __controlfp_s(__controlfp_s)
PublicSymbol: [0001CC30][0002:00005C30] ??0CTraceFileAndLineInfo@ATL@@QAE@PBDH@Z(public: __thiscall ATL::CTraceFileAndLineInfo::CTraceFileAndLineInfo(char const *,int))
PublicSymbol: [00040EC6][0002:00029EC6] __CrtDbgReport(__CrtDbgReport)
PublicSymbol: [000391B4][0002:000221B4] _ExitProcess@4(_ExitProcess@4)
PublicSymbol: [00040104][0002:00029104] __TrackMouseEvent@4(__TrackMouseEvent@4)
PublicSymbol: [000523A8][0004:000003A8] ??_R0?AV?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@@8(class ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0> > `RTTI Type Descriptor')
PublicSymbol: [00053920][0005:00000920] __imp__HeapAlloc@12(__imp__HeapAlloc@12)
PublicSymbol: [00052EB4][0004:00000EB4] ___dyn_tls_init_callback(___dyn_tls_init_callback)
PublicSymbol: [00022E20][0002:0000BE20] ??BCWindow@ATL@@QBEPAUHWND__@@XZ(public: __thiscall ATL::CWindow::operator struct HWND__ *(void)const )
PublicSymbol: [0004F46C][0003:0000746C] ??_R2?$CDialogImpl@VCAboutDlg@@VCWindow@ATL@@@ATL@@8(ATL::CDialogImpl<class CAboutDlg,class ATL::CWindow>::`RTTI Base Class Array')
PublicSymbol: [0004F458][0003:00007458] ??_R3?$CDialogImpl@VCAboutDlg@@VCWindow@ATL@@@ATL@@8(ATL::CDialogImpl<class CAboutDlg,class ATL::CWindow>::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [000426B6][0002:0002B6B6] _SetUnhandledExceptionFilter@4(_SetUnhandledExceptionFilter@4)
PublicSymbol: [0004BEB4][0003:00003EB4] ?VT@?$CVarTypeInfo@J@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<long>::VT)
PublicSymbol: [0003D5F0][0002:000265F0] ?AtlModuleAddTermFunc@ATL@@YGJPAU_ATL_MODULE70@1@P6GXK@ZK@Z(long __stdcall ATL::AtlModuleAddTermFunc(struct ATL::_ATL_MODULE70 *,void (__stdcall*)(unsigned long),unsigned long))
PublicSymbol: [000426AA][0002:0002B6AA] _TerminateProcess@8(_TerminateProcess@8)
PublicSymbol: [00053050][0005:00000050] __IMPORT_DESCRIPTOR_GDI32(__IMPORT_DESCRIPTOR_GDI32)
PublicSymbol: [00053D40][0005:00000D40] __imp__CoInitialize@4(__imp__CoInitialize@4)
PublicSymbol: [0005269C][0004:0000069C] ?g_pszUpdateEventName@@3PBDB(char const * const g_pszUpdateEventName)
PublicSymbol: [0004CC68][0003:00004C68] _GUID_DEVCLASS_SYSTEM(_GUID_DEVCLASS_SYSTEM)
PublicSymbol: [00038EA0][0002:00021EA0] ??_Estringdispid@CComTypeInfoHolder@ATL@@QAEPAXI@Z(public: void * __thiscall ATL::CComTypeInfoHolder::stringdispid::`vector deleting destructor'(unsigned int))
PublicSymbol: [00027530][0002:00010530] ?GetWindowProc@?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@UAEP6GJPAUHWND__@@IIJ@ZXZ(public: virtual long (__stdcall*__thiscall ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::GetWindowProc(void))(struct HWND__ *,unsigned int,unsigned int,long))
PublicSymbol: [000287B0][0002:000117B0] ?IsEqualKey@?$CSimpleMapEqualHelper@KPAVCMessageLoop@WTL@@@ATL@@SA_NABK0@Z(public: static bool __cdecl ATL::CSimpleMapEqualHelper<unsigned long,class WTL::CMessageLoop *>::IsEqualKey(unsigned long const &,unsigned long const &))
PublicSymbol: [00052000][0004:00000000] ?rcDefault@CWindow@ATL@@2UtagRECT@@A(public: static struct tagRECT ATL::CWindow::rcDefault)
PublicSymbol: [0003926E][0002:0002226E] _GetWindowTextLengthA@4(_GetWindowTextLengthA@4)
PublicSymbol: [0005397C][0005:0000097C] __imp__LeaveCriticalSection@4(__imp__LeaveCriticalSection@4)
PublicSymbol: [000521B8][0004:000001B8] ??_R0?AVCUpdateUIBase@WTL@@@8(class WTL::CUpdateUIBase `RTTI Type Descriptor')
PublicSymbol: [00052894][0004:00000894] ?_AtlRegisterPerUser@ATL@@3_NA(bool ATL::_AtlRegisterPerUser)
PublicSymbol: [00053744][0005:00000744] __imp__RegDeleteKeyA@8(__imp__RegDeleteKeyA@8)
PublicSymbol: [0004DF18][0003:00005F18] ??_C@_1IO@GJLKCBCE@?$AAf?$AA?3?$AA?2?$AAd?$AAd?$AA?2?$AAv?$AAc?$AAt?$AAo?$AAo?$AAl?$AAs?$AA?2?$AAv?$AAc?$AA7?$AAl?$AAi?$AAb?$AAs?$AA?2?$AAs?$AAh?$AAi?$AAp?$AA?2?$AAa?$AAt?$AAl?$AAm?$AAf@(`string')
PublicSymbol: [0004E1B8][0003:000061B8] ??_C@_0BBN@GPMLNJCF@A?5cast?5to?5a?5smaller?5data?5type?5ha@(`string')
PublicSymbol: [0003F130][0002:00028130] ?TryAddRef@CAtlTraceSettings@@QAE_NXZ(public: bool __thiscall CAtlTraceSettings::TryAddRef(void))
PublicSymbol: [000276D0][0002:000106D0] ?SelectFont@?$CDCT@$00@WTL@@QAEPAUHFONT__@@PAU3@@Z(public: struct HFONT__ * __thiscall WTL::CDCT<1>::SelectFont(struct HFONT__ *))
PublicSymbol: [00041850][0002:0002A850] __RTC_UninitUse(__RTC_UninitUse)
PublicSymbol: [00033650][0002:0001C650] ??1?$CAtlSafeAllocBufferManager@VCCRTAllocator@ATL@@@_ATL_SAFE_ALLOCA_IMPL@ATL@@QAE@XZ(public: __thiscall ATL::_ATL_SAFE_ALLOCA_IMPL::CAtlSafeAllocBufferManager<class ATL::CCRTAllocator>::~CAtlSafeAllocBufferManager<class ATL::CCRTAllocator>(void))
PublicSymbol: [0004C1F0][0003:000041F0] _IID_IOlePresObj(_IID_IOlePresObj)
PublicSymbol: [0001ADF0][0002:00003DF0] ??_GCHyperLink@WTL@@UAEPAXI@Z(public: virtual void * __thiscall WTL::CHyperLink::`scalar deleting destructor'(unsigned int))
PublicSymbol: [000390AC][0002:000220AC] _crAddFile2A@16(_crAddFile2A@16)
PublicSymbol: [00020230][0002:00009230] ?IsNotUnderlined@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::IsNotUnderlined(void)const )
PublicSymbol: [000539A0][0005:000009A0] __imp__GetCurrentThreadId@0(__imp__GetCurrentThreadId@0)
PublicSymbol: [00040F08][0002:00029F08] __wcsicmp(__wcsicmp)
PublicSymbol: [000326E0][0002:0001B6E0] ??0CComCriticalSection@ATL@@QAE@XZ(public: __thiscall ATL::CComCriticalSection::CComCriticalSection(void))
PublicSymbol: [0002DD70][0002:00016D70] ?IsSpace@CRegParser@ATL@@IAEHD@Z(protected: int __thiscall ATL::CRegParser::IsSpace(char))
PublicSymbol: [000393A0][0002:000223A0] _CoCreateInstance@20(_CoCreateInstance@20)
PublicSymbol: [0004D904][0003:00005904] __GUID_00000000_0000_0000_c000_000000000046(__GUID_00000000_0000_0000_c000_000000000046)
PublicSymbol: [00034BA0][0002:0001DBA0] ?FreeHeap@?$CTempBuffer@D$0BAA@VCCRTAllocator@ATL@@@ATL@@AAEXXZ(private: void __thiscall ATL::CTempBuffer<char,256,class ATL::CCRTAllocator>::FreeHeap(void))
PublicSymbol: [00034CC0][0002:0001DCC0] ?FreeHeap@?$CTempBuffer@E$0BAA@VCCRTAllocator@ATL@@@ATL@@AAEXXZ(private: void __thiscall ATL::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>::FreeHeap(void))
PublicSymbol: [00041C36][0002:0002AC36] __amsg_exit(__amsg_exit)
PublicSymbol: [00053754][0005:00000754] __imp__RegQueryInfoKeyW@48(__imp__RegQueryInfoKeyW@48)
PublicSymbol: [0004B318][0003:00003318] ??_C@_04CBBMIBLK@HKLM?$AA@(`string')
PublicSymbol: [0004ABE4][0003:00002BE4] ??_C@_1BC@KAILKFFG@?$AAR?$AAE?$AAG?$AAI?$AAS?$AAT?$AAR?$AAY?$AA?$AA@(`string')
PublicSymbol: [00049590][0003:00001590] ??_C@_1BO@IAPFIMAK@?$AAm?$AA_?$AAp?$AAU?$AAI?$AAD?$AAa?$AAt?$AAa?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [0004F394][0003:00007394] ??_R4CMessageFilter@WTL@@6B@(const WTL::CMessageFilter::`RTTI Complete Object Locator')
PublicSymbol: [00032560][0002:0001B560] ??0IRegistrarBase@@QAE@XZ(public: __thiscall IRegistrarBase::IRegistrarBase(void))
PublicSymbol: [00052078][0004:00000078] ??_R0?AV?$CDialogImpl@VCMainDlg@@VCWindow@ATL@@@ATL@@@8(class ATL::CDialogImpl<class CMainDlg,class ATL::CWindow> `RTTI Type Descriptor')
PublicSymbol: [0004E8D8][0003:000068D8] ?ProcessVerifier@NativeDll@<CrtImplementationDetails>@@0IB(private: static unsigned int const <CrtImplementationDetails>::NativeDll::ProcessVerifier)
PublicSymbol: [000525EC][0004:000005EC] ?__LINE__Var@?1??OnInitDialog@CMainDlg@@QAEJIIJAAH@Z@4JA(long `public: long __thiscall CMainDlg::OnInitDialog(unsigned int,unsigned int,long,int &)'::`2'::__LINE__Var)
PublicSymbol: [00041EB0][0002:0002AEB0] __IsNonwritableInCurrentImage(__IsNonwritableInCurrentImage)
PublicSymbol: [00033560][0002:0001C560] ??0?$CAtlSafeAllocBufferManager@VCCRTAllocator@ATL@@@_ATL_SAFE_ALLOCA_IMPL@ATL@@QAE@XZ(public: __thiscall ATL::_ATL_SAFE_ALLOCA_IMPL::CAtlSafeAllocBufferManager<class ATL::CCRTAllocator>::CAtlSafeAllocBufferManager<class ATL::CCRTAllocator>(void))
PublicSymbol: [00029D70][0002:00012D70] ?ChangeCategory@CTrace@ATL@@QAE_NKIW4ATLTRACESTATUS@2@@Z(public: bool __thiscall ATL::CTrace::ChangeCategory(unsigned long,unsigned int,enum ATL::ATLTRACESTATUS))
PublicSymbol: [0004E80C][0003:0000680C] ??_C@_0CK@CNLNOEPB@Local?5variable?5used?5before?5initi@(`string')
PublicSymbol: [00020670][0002:00009670] ?SelectFont@?$CDCT@$0A@@WTL@@QAEPAUHFONT__@@PAU3@@Z(public: struct HFONT__ * __thiscall WTL::CDCT<0>::SelectFont(struct HFONT__ *))
PublicSymbol: [00039304][0002:00022304] _IsDialogMessageA@8(_IsDialogMessageA@8)
PublicSymbol: [0003E480][0002:00027480] ?IncRef@CAtlTraceProcess@@QAEIXZ(public: unsigned int __thiscall CAtlTraceProcess::IncRef(void))
PublicSymbol: [0002A660][0002:00013660] ??1CComCriticalSection@ATL@@QAE@XZ(public: __thiscall ATL::CComCriticalSection::~CComCriticalSection(void))
PublicSymbol: [00053A74][0005:00000A74] __imp__memcmp(__imp__memcmp)
PublicSymbol: [00053C1C][0005:00000C1C] __imp__GetSystemMetrics@4(__imp__GetSystemMetrics@4)
PublicSymbol: [0001A540][0002:00003540] ??0CMessageFilter@WTL@@QAE@XZ(public: __thiscall WTL::CMessageFilter::CMessageFilter(void))
PublicSymbol: [00039226][0002:00022226] _FillRect@12(_FillRect@12)
PublicSymbol: [0002DE20][0002:00016E20] ?NextToken@CRegParser@ATL@@IAEJPAD@Z(protected: long __thiscall ATL::CRegParser::NextToken(char *))
PublicSymbol: [0004F11C][0003:0000711C] ??_R2?$CWindowImplRoot@VCWindow@ATL@@@ATL@@8(ATL::CWindowImplRoot<class ATL::CWindow>::`RTTI Base Class Array')
PublicSymbol: [0004F108][0003:00007108] ??_R3?$CWindowImplRoot@VCWindow@ATL@@@ATL@@8(ATL::CWindowImplRoot<class ATL::CWindow>::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004F5A8][0003:000075A8] ??_R1A@?0A@EA@?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@8(ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0004B0A8][0003:000030A8] ??_C@_1DO@EIGOLNAA@?$AAl?$AAp?$AAs?$AAz?$AAK?$AAe?$AAy?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?5?$AA?$CG?$AA?$CG?$AA?5?$AAl?$AAp?$AAs?$AAz?$AAV?$AAa?$AAl?$AAu?$AAe?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AA?$AA@(`string')
PublicSymbol: [000392B0][0002:000222B0] _GetDlgItem@8(_GetDlgItem@8)
PublicSymbol: [0004CD08][0003:00004D08] _GUID_DEVCLASS_FSFILTER_ACTIVITYMONITOR(_GUID_DEVCLASS_FSFILTER_ACTIVITYMONITOR)
PublicSymbol: [0003DAE0][0002:00026AE0] ?InlineIsEqualUnknown@ATL@@YGHABU_GUID@@@Z(int __stdcall ATL::InlineIsEqualUnknown(struct _GUID const &))
PublicSymbol: [0003C2C0][0002:000252C0] ?Init@?$CA2WEX@$0IA@@ATL@@AAEXPBDI@Z(private: void __thiscall ATL::CA2WEX<128>::Init(char const *,unsigned int))
PublicSymbol: [00052838][0004:00000838] ?g_CrashThreadInfo@@3UCrashThreadInfo@@A(struct CrashThreadInfo g_CrashThreadInfo)
PublicSymbol: [00052034][0004:00000034] ??_R0?AVCMessageLoop@WTL@@@8(class WTL::CMessageLoop `RTTI Type Descriptor')
PublicSymbol: [0004A898][0003:00002898] ??_C@_0DE@JJEOCICO@ERROR?5?9?5Object?5deleted?5before?5wi@(`string')
PublicSymbol: [00022BE0][0002:0000BBE0] ??0CWindow@ATL@@QAE@PAUHWND__@@@Z(public: __thiscall ATL::CWindow::CWindow(struct HWND__ *))
PublicSymbol: [0004AD5C][0003:00002D5C] ??_7CRegObject@ATL@@6B@(const ATL::CRegObject::`vftable')
PublicSymbol: [00053A44][0005:00000A44] __imp____set_app_type(__imp____set_app_type)
PublicSymbol: [0003914E][0002:0002214E] _GetCurrentProcess@0(_GetCurrentProcess@0)
PublicSymbol: [0004BB8C][0003:00003B8C] ??_C@_0O@GHILFHIP@SEH?5exception?$AA@(`string')
PublicSymbol: [0002E210][0002:00017210] ?PreProcessBuffer@CRegParser@ATL@@QAEJPADPAPAD@Z(public: long __thiscall ATL::CRegParser::PreProcessBuffer(char *,char * *))
PublicSymbol: [0004F044][0003:00007044] ??_R1A@?0A@EA@?$CDialogImpl@VCMainDlg@@VCWindow@ATL@@@ATL@@8(ATL::CDialogImpl<class CMainDlg,class ATL::CWindow>::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [00053A34][0005:00000A34] __imp___XcptFilter(__imp___XcptFilter)
PublicSymbol: [00053CBC][0005:00000CBC] __imp__CreateDialogParamA@20(__imp__CreateDialogParamA@20)
PublicSymbol: [000336D0][0002:0001C6D0] ??0?$CAtlModuleT@VCComModule@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CAtlModuleT<class ATL::CComModule>::CAtlModuleT<class ATL::CComModule>(void))
PublicSymbol: [00053764][0005:00000764] __imp__RegCloseKey@4(__imp__RegCloseKey@4)
PublicSymbol: [000390B2][0002:000220B2] _crGetLastErrorMsgA@8(_crGetLastErrorMsgA@8)
PublicSymbol: [000372D0][0002:000202D0] ??0?$CDialogImpl@VCDocumentDlg@@VCWindow@ATL@@@ATL@@QAE@XZ(public: __thiscall ATL::CDialogImpl<class CDocumentDlg,class ATL::CWindow>::CDialogImpl<class CDocumentDlg,class ATL::CWindow>(void))
PublicSymbol: [00021D10][0002:0000AD10] ?Term@CAppModule@WTL@@QAEXXZ(public: void __thiscall WTL::CAppModule::Term(void))
PublicSymbol: [0002BCD0][0002:00014CD0] ??1CComAutoDeleteCriticalSection@ATL@@QAE@XZ(public: __thiscall ATL::CComAutoDeleteCriticalSection::~CComAutoDeleteCriticalSection(void))
PublicSymbol: [0004C088][0003:00004088] _CATID_MARSHALER(_CATID_MARSHALER)
PublicSymbol: [0002BAB0][0002:00014AB0] ?FinalConstruct@CRegObject@ATL@@QAEJXZ(public: long __thiscall ATL::CRegObject::FinalConstruct(void))
PublicSymbol: [0004A2E8][0003:000022E8] ??_C@_1LI@NBDOGJHN@?$AA?$CB?$AA?$CC?$AAF?$AAl?$AAo?$AAa?$AAt?$AAi?$AAn?$AAg?$AA?5?$AAp?$AAo?$AAi?$AAn?$AAt?$AA?5?$AA?$CI?$AA?$CF?$AA?$CF?$AAe?$AA?0?$AA?5?$AA?$CF?$AA?$CF?$AAE?$AA?0?$AA?5?$AA?$CF?$AA?$CF?$AAf?$AA?0@(`string')
PublicSymbol: [00039364][0002:00022364] _RegOpenKeyExA@20(_RegOpenKeyExA@20)
PublicSymbol: [0004E7E4][0003:000067E4] ??_C@_0BP@OGBCLIBO@Stack?5around?5_alloca?5corrupted?$AA@(`string')
PublicSymbol: [0001AE60][0002:00003E60] ?CrashCallback@@YGHPAX@Z(int __stdcall CrashCallback(void *))
PublicSymbol: [0004C074][0003:00004074] _GUID_NULL(_GUID_NULL)
PublicSymbol: [0004D194][0003:00005194] _GUID_ATLVer100(_GUID_ATLVer100)
PublicSymbol: [0004C6F0][0003:000046F0] _GUID_HANDLE(_GUID_HANDLE)
PublicSymbol: [0004265C][0002:0002B65C] _UnmapViewOfFile@4(_UnmapViewOfFile@4)
PublicSymbol: [000218A0][0002:0000A8A0] ?PreTranslateMessage@CMessageLoop@WTL@@UAEHPAUtagMSG@@@Z(public: virtual int __thiscall WTL::CMessageLoop::PreTranslateMessage(struct tagMSG *))
PublicSymbol: [0004F150][0003:00007150] ??_R3CWindow@ATL@@8(ATL::CWindow::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004F164][0003:00007164] ??_R2CWindow@ATL@@8(ATL::CWindow::`RTTI Base Class Array')
PublicSymbol: [0004F63C][0003:0000763C] ??_R1A@?0A@EA@CAppModule@WTL@@8(WTL::CAppModule::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [0002A550][0002:00013550] ??1?$CAtlModuleT@VCComModule@ATL@@@ATL@@UAE@XZ(public: virtual __thiscall ATL::CAtlModuleT<class ATL::CComModule>::~CAtlModuleT<class ATL::CComModule>(void))
PublicSymbol: [00034DE0][0002:0001DDE0] ?AllocateHeap@?$CTempBuffer@D$0BAA@VCCRTAllocator@ATL@@@ATL@@AAEXI@Z(private: void __thiscall ATL::CTempBuffer<char,256,class ATL::CCRTAllocator>::AllocateHeap(unsigned int))
PublicSymbol: [0004D184][0003:00005184] _GUID_ATLVer70(_GUID_ATLVer70)
PublicSymbol: [0003E030][0002:00027030] ?__FreeStdCallThunk@@YGXPAX@Z(void __stdcall __FreeStdCallThunk(void *))
PublicSymbol: [00034E60][0002:0001DE60] ?AllocateHeap@?$CTempBuffer@E$0BAA@VCCRTAllocator@ATL@@@ATL@@AAEXI@Z(private: void __thiscall ATL::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>::AllocateHeap(unsigned int))
PublicSymbol: [00048E0C][0003:00000E0C] ??_7CAboutDlg@@6B@(const CAboutDlg::`vftable')
PublicSymbol: [000404C4][0002:000294C4] _memmove_s(_memmove_s)
PublicSymbol: [00034F30][0002:0001DF30] ??0?$Wrapper@PA_W@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@QAE@ABQA_W@Z(public: __thiscall ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Wrapper<wchar_t *>::Wrapper<wchar_t *>(wchar_t * const &))
PublicSymbol: [00052920][0004:00000920] ?atlTraceControls@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceControls)
PublicSymbol: [0004D174][0003:00005174] _GUID_ATLVer30(_GUID_ATLVer30)
PublicSymbol: [00048AFC][0003:00000AFC] ??_C@_07KJLOKDKB@TypeLib?$AA@(`string')
PublicSymbol: [00032620][0002:0001B620] ??0CComAutoDeleteCriticalSection@ATL@@QAE@XZ(public: __thiscall ATL::CComAutoDeleteCriticalSection::CComAutoDeleteCriticalSection(void))
PublicSymbol: [0004CC7C][0003:00004C7C] _GUID_DEVCLASS_TAPEDRIVE(_GUID_DEVCLASS_TAPEDRIVE)
PublicSymbol: [00053924][0005:00000924] __imp__InterlockedPopEntrySList@4(__imp__InterlockedPopEntrySList@4)
PublicSymbol: [0004A148][0003:00002148] ??_C@_1GI@MJLKJLBP@?$AAd?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAj?$AAe?$AAc?$AAt?$AAs?$AA?2?$AAc?$AAr?$AAa?$AAs?$AAh?$AAr?$AAp?$AAt?$AA?2?$AAt?$AAr?$AAu?$AAn?$AAk?$AA?2?$AAt?$AAh?$AAi?$AAr?$AAd@(`string')
PublicSymbol: [00036C50][0002:0001FC50] ??1?$CDialogImpl@VCDocumentDlg@@VCWindow@ATL@@@ATL@@UAE@XZ(public: virtual __thiscall ATL::CDialogImpl<class CDocumentDlg,class ATL::CWindow>::~CDialogImpl<class CDocumentDlg,class ATL::CWindow>(void))
PublicSymbol: [0004C100][0003:00004100] _IID_IPSFactory(_IID_IPSFactory)
PublicSymbol: [00034410][0002:0001D410] ?Allocate@?$CTempBuffer@D$0BAA@VCCRTAllocator@ATL@@@ATL@@QAEPADI@Z(public: char * __thiscall ATL::CTempBuffer<char,256,class ATL::CCRTAllocator>::Allocate(unsigned int))
PublicSymbol: [0004FE6C][0003:00007E6C] ___rtc_taa(___rtc_taa)
PublicSymbol: [00052160][0004:00000160] ??_R0?AVCMessageMap@ATL@@@8(class ATL::CMessageMap `RTTI Type Descriptor')
PublicSymbol: [0002BDA0][0002:00014DA0] ?AddReplacement@CRegObject@ATL@@UAGJPB_W0@Z(public: virtual long __stdcall ATL::CRegObject::AddReplacement(wchar_t const *,wchar_t const *))
PublicSymbol: [0001D6C0][0002:000066C0] ?OnMouseMove@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QAEJIIJAAH@Z(public: long __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::OnMouseMove(unsigned int,unsigned int,long,int &))
PublicSymbol: [0004F858][0003:00007858] ??_R1A@?0A@EA@IUnknown@@8(IUnknown::`RTTI Base Class Descriptor at (0,-1,0,64)')
PublicSymbol: [000491D8][0003:000011D8] ??_C@_08GNBNPKEG@?1restart?$AA@(`string')
PublicSymbol: [00026A90][0002:0000FA90] ??A?$CSimpleArray@GV?$CSimpleArrayEqualHelper@G@ATL@@@ATL@@QAEAAGH@Z(public: unsigned short & __thiscall ATL::CSimpleArray<unsigned short,class ATL::CSimpleArrayEqualHelper<unsigned short> >::operator[](int))
PublicSymbol: [00053014][0005:00000014] __IMPORT_DESCRIPTOR_SHELL32(__IMPORT_DESCRIPTOR_SHELL32)
PublicSymbol: [0003FA70][0002:00028A70] ??A?$CSimpleArray@KV?$CSimpleArrayEqualHelper@K@ATL@@@ATL@@QAEAAKH@Z(public: unsigned long & __thiscall ATL::CSimpleArray<unsigned long,class ATL::CSimpleArrayEqualHelper<unsigned long> >::operator[](int))
PublicSymbol: [000350A0][0002:0001E0A0] ??$?2PAD@?$Wrapper@PAD@?$CSimpleMap@PADPA_WVCExpansionVectorEqualHelper@ATL@@@ATL@@SAPAXIPAPAD@Z(public: static void * __cdecl ATL::CSimpleMap<char *,wchar_t *,class ATL::CExpansionVectorEqualHelper>::Wrapper<char *>::operator new<char *>(unsigned int,char * *))
PublicSymbol: [00053C3C][0005:00000C3C] __imp__IsWindowEnabled@4(__imp__IsWindowEnabled@4)
PublicSymbol: [0003EA50][0002:00027A50] ?GetModuleCount@CAtlAllocator@@QBEHXZ(public: int __thiscall CAtlAllocator::GetModuleCount(void)const )
PublicSymbol: [00042668][0002:0002B668] _MapViewOfFile@20(_MapViewOfFile@20)
PublicSymbol: [00053BE8][0005:00000BE8] __imp__GetParent@4(__imp__GetParent@4)
PublicSymbol: [0005375C][0005:0000075C] __imp__RevertToSelf@0(__imp__RevertToSelf@0)
PublicSymbol: [00048B4C][0003:00000B4C] ??_C@_08PICKODJL@FileType?$AA@(`string')
PublicSymbol: [0004F3DC][0003:000073DC] ??_R3CAboutDlg@@8(CAboutDlg::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004F8B0][0003:000078B0] ??_R3CDocumentDlg@@8(CDocumentDlg::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [000390BE][0002:000220BE] _crUninstall@0(_crUninstall@0)
PublicSymbol: [0004F3F0][0003:000073F0] ??_R2CAboutDlg@@8(CAboutDlg::`RTTI Base Class Array')
PublicSymbol: [0004F8C4][0003:000078C4] ??_R2CDocumentDlg@@8(CDocumentDlg::`RTTI Base Class Array')
PublicSymbol: [00053C6C][0005:00000C6C] __imp__UnregisterClassA@8(__imp__UnregisterClassA@8)
PublicSymbol: [00049850][0003:00001850] ??_C@_1CE@FDGJDOPA@?$AAc?$AAl?$AAr?$AA?5?$AA?$CB?$AA?$DN?$AA?5?$AA0?$AAx?$AAF?$AAF?$AAF?$AAF?$AAF?$AAF?$AAF?$AAF?$AA?$AA@(`string')
PublicSymbol: [00036B30][0002:0001FB30] ?CloseDialog@CDocumentDlg@@QAEXH@Z(public: void __thiscall CDocumentDlg::CloseDialog(int))
PublicSymbol: [00052330][0004:00000330] ??_R0?AV?$CWindowImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@ATL@@@8(class ATL::CWindowImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> > `RTTI Type Descriptor')
PublicSymbol: [0004C86C][0003:0000486C] _GUID_DEVCLASS_61883(_GUID_DEVCLASS_61883)
PublicSymbol: [00052914][0004:00000914] ?atlTraceRegistrar@ATL@@3VCTraceCategory@1@A(class ATL::CTraceCategory ATL::atlTraceRegistrar)
PublicSymbol: [0004DB20][0003:00005B20] ??_C@_0EN@LNBCEPAE@ERROR?5?3?5Unable?5to?5lock?5critical?5@(`string')
PublicSymbol: [0004C6A0][0003:000046A0] _GUID_FONTBOLD(_GUID_FONTBOLD)
PublicSymbol: [00034570][0002:0001D570] ?Allocate@?$CTempBuffer@E$0BAA@VCCRTAllocator@ATL@@@ATL@@QAEPAEI@Z(public: unsigned char * __thiscall ATL::CTempBuffer<unsigned char,256,class ATL::CCRTAllocator>::Allocate(unsigned int))
PublicSymbol: [00026C00][0002:0000FC00]
PublicSymbol: [000537DC][0005:000007DC] __imp__crExceptionFilter@8(__imp__crExceptionFilter@8)
PublicSymbol: [0004D650][0003:00005650] ??_C@_04FFDMEMDO@?$CFS?3?5?$AA@(`string')
PublicSymbol: [0004B70C][0003:0000370C] ??_C@_0BI@NDCHGJG@Setting?5Value?5?$CFd?5at?5?$CFs?6?$AA@(`string')
PublicSymbol: [000537FC][0005:000007FC] __imp__crAddPropertyA@8(__imp__crAddPropertyA@8)
PublicSymbol: [00039346][0002:00022346] _GetObjectType@4(_GetObjectType@4)
PublicSymbol: [0004E7A4][0003:000067A4] ??_C@_0DE@OHJBPMBP@A?5variable?5is?5being?5used?5without@(`string')
PublicSymbol: [0004BFBC][0003:00003FBC] _IID_IInternalConnection(_IID_IInternalConnection)
PublicSymbol: [0004DEF8][0003:00005EF8] ??_C@_1BG@EIHNOMMF@?$AAm?$AA_?$AAp?$AAP?$AAr?$AAo?$AAc?$AAe?$AAs?$AAs?$AA?$AA@(`string')
PublicSymbol: [0004D764][0003:00005764] ??_C@_1DM@CECLCIOA@?$AAp?$AAp?$AAC?$AAa?$AAt?$AAe?$AAg?$AAo?$AAr?$AAy?$AA?5?$AA?$CG?$AA?$CG?$AA?5?$AAp?$AAf?$AAn?$AAC?$AAr?$AAt?$AAD?$AAb?$AAg?$AAR?$AAe?$AAp?$AAo?$AAr?$AAt?$AA?$AA@(`string')
PublicSymbol: [0004C0D8][0003:000040D8] _IID_IRpcProxy(_IID_IRpcProxy)
PublicSymbol: [00041D74][0002:0002AD74] __cexit(__cexit)
PublicSymbol: [00041070][0002:0002A070] ?_RTC_Failure@@YAXPAXH@Z(void __cdecl _RTC_Failure(void *,int))
PublicSymbol: [0004C3E4][0003:000043E4] _IID_IRichEditOle(_IID_IRichEditOle)
PublicSymbol: [000390F4][0002:000220F4] _GetCommandLineW@0(_GetCommandLineW@0)
PublicSymbol: [0003918A][0002:0002218A] _WideCharToMultiByte@32(_WideCharToMultiByte@32)
PublicSymbol: [000401F0][0002:000291F0] @_RTC_AllocaHelper@12(@_RTC_AllocaHelper@12)
PublicSymbol: [0003A910][0002:00023910] _AtlTraceGetModule@16(_AtlTraceGetModule@16)
PublicSymbol: [0004D028][0003:00005028] ??_C@_0BD@KOADKDIA@atlTraceDBProvider?$AA@(`string')
PublicSymbol: [00053BFC][0005:00000BFC] __imp__BeginPaint@8(__imp__BeginPaint@8)
PublicSymbol: [00053938][0005:00000938] __imp__OutputDebugStringA@4(__imp__OutputDebugStringA@4)
PublicSymbol: [0004B58C][0003:0000358C] ??_C@_0P@IONAJCEO@Opened?5Key?5?$CFs?6?$AA@(`string')
PublicSymbol: [0004EAE8][0003:00006AE8] ??_C@_0M@HLOHPNFA@RegCloseKey?$AA@(`string')
PublicSymbol: [000538E8][0005:000008E8] __imp__InterlockedExchange@8(__imp__InterlockedExchange@8)
PublicSymbol: [0004BEC4][0003:00003EC4] ?VT@?$CVarTypeInfo@K@ATL@@2GB(public: static unsigned short const ATL::CVarTypeInfo<unsigned long>::VT)
PublicSymbol: [000426C8][0002:0002B6C8] _QueryPerformanceCounter@4(_QueryPerformanceCounter@4)
PublicSymbol: [0004D4FC][0003:000054FC] ??_C@_06JMICFIKN@?$CFs_?$CF0x?$AA@(`string')
PublicSymbol: [000391D8][0002:000221D8] _SendMessageA@16(_SendMessageA@16)
PublicSymbol: [00053C5C][0005:00000C5C] __imp__SetWindowTextA@8(__imp__SetWindowTextA@8)
PublicSymbol: [00040ECC][0002:00029ECC] _strcpy(_strcpy)
PublicSymbol: [000376D0][0002:000206D0]
PublicSymbol: [00053934][0005:00000934] __imp__OutputDebugStringW@4(__imp__OutputDebugStringW@4)
PublicSymbol: [00053A08][0005:00000A08] __imp____dllonexit(__imp____dllonexit)
PublicSymbol: [0004CD30][0003:00004D30] _GUID_DEVCLASS_FSFILTER_ANTIVIRUS(_GUID_DEVCLASS_FSFILTER_ANTIVIRUS)
PublicSymbol: [00032780][0002:0001B780] ?UpdateRegistryFromResourceS@CAtlModule@ATL@@QAGJIHPAU_ATL_REGMAP_ENTRY@2@@Z(public: long __stdcall ATL::CAtlModule::UpdateRegistryFromResourceS(unsigned int,int,struct ATL::_ATL_REGMAP_ENTRY *))
PublicSymbol: [00053948][0005:00000948] __imp__GetModuleFileNameA@12(__imp__GetModuleFileNameA@12)
PublicSymbol: [0004C0B0][0003:000040B0] _IID_IRpcStub(_IID_IRpcStub)
PublicSymbol: [0004F890][0003:00007890] ??_R2IUnknown@@8(IUnknown::`RTTI Base Class Array')
PublicSymbol: [0004F87C][0003:0000787C] ??_R3IUnknown@@8(IUnknown::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [00040EC0][0002:00029EC0] __errno(__errno)
PublicSymbol: [0004F7CC][0003:000077CC] ??_R3CRegObject@ATL@@8(ATL::CRegObject::`RTTI Class Hierarchy Descriptor')
PublicSymbol: [0004C484][0003:00004484] _CLSID_ContextSwitcher(_CLSID_ContextSwitcher)
PublicSymbol: [0004F7E0][0003:000077E0] ??_R2CRegObject@ATL@@8(ATL::CRegObject::`RTTI Base Class Array')
PublicSymbol: [00040EE4][0002:00029EE4] __snwprintf_s(__snwprintf_s)
PublicSymbol: [00053964][0005:00000964] __imp__SetLastError@4(__imp__SetLastError@4)
PublicSymbol: [0004D958][0003:00005958] ??_C@_0EH@NIJCHDDG@ERROR?5?3?5Unable?5to?5lock?5critical?5@(`string')
PublicSymbol: [00053AC0][0005:00000AC0] __imp_??2@YAPAXI@Z(__imp_??2@YAPAXI@Z)
PublicSymbol: [000538F4][0005:000008F4] __imp__GetModuleFileNameW@12(__imp__GetModuleFileNameW@12)
PublicSymbol: [000494D0][0003:000014D0] ??_C@_1JO@EBCKIGG@?$AAc?$AA?3?$AA?2?$AAp?$AAr?$AAo?$AAg?$AAr?$AAa?$AAm?$AA?5?$AAf?$AAi?$AAl?$AAe?$AAs?$AA?5?$AA?$CI?$AAx?$AA8?$AA6?$AA?$CJ?$AA?2?$AAm?$AAi?$AAc?$AAr?$AAo?$AAs?$AAo?$AAf?$AAt@(`string')
PublicSymbol: [000392D4][0002:000222D4] _ReleaseDC@8(_ReleaseDC@8)
PublicSymbol: [0004B320][0003:00003320] ??_C@_03DLBIJLAB@HKU?$AA@(`string')
PublicSymbol: [0002BBC0][0002:00014BC0] ?Init@CComCriticalSection@ATL@@QAEJXZ(public: long __thiscall ATL::CComCriticalSection::Init(void))
PublicSymbol: [0004B6D4][0003:000036D4] ??_C@_0CN@BAKOHBFP@Binary?5Data?5does?5not?5fall?5on?5BYT@(`string')
PublicSymbol: [0004BD14][0003:00003D14] ??_C@_1BE@NIAIDPLI@?$AA?$CB?$AAm?$AA_?$AAb?$AAM?$AAo?$AAd?$AAa?$AAl?$AA?$AA@(`string')
PublicSymbol: [000537E4][0005:000007E4] __imp__crInstallA@4(__imp__crInstallA@4)
PublicSymbol: [0001AB10][0002:00003B10] ?OnCancel@CAboutDlg@@QAEJGGPAUHWND__@@AAH@Z(public: long __thiscall CAboutDlg::OnCancel(unsigned short,unsigned short,struct HWND__ *,int &))
PublicSymbol: [0004A838][0003:00002838] ??_C@_0EP@EMDEJOEJ@c?3?2program?5files?5?$CIx86?$CJ?2microsoft@(`string')
PublicSymbol: [00027240][0002:00010240] ?OnFinalMessage@?$CDialogImplBaseT@VCWindow@ATL@@@ATL@@UAEXPAUHWND__@@@Z(public: virtual void __thiscall ATL::CDialogImplBaseT<class ATL::CWindow>::OnFinalMessage(struct HWND__ *))
PublicSymbol: [0004B558][0003:00003558] ??_C@_0CM@LEFFDKCG@Ignoring?5Open?5key?5on?5?$CFs?5?3?5In?5Rec@(`string')
PublicSymbol: [00039358][0002:00022358] _GetObjectA@12(_GetObjectA@12)
PublicSymbol: [000390DC][0002:000220DC] _crUninstallFromCurrentThread@0(_crUninstallFromCurrentThread@0)
PublicSymbol: [00028550][0002:00011550] ?FreeHeap@?$CTempBuffer@D$0CAA@VCCRTAllocator@ATL@@@ATL@@AAEXXZ(private: void __thiscall ATL::CTempBuffer<char,512,class ATL::CCRTAllocator>::FreeHeap(void))
PublicSymbol: [00041D90][0002:0002AD90] __ValidateImageBase(__ValidateImageBase)
PublicSymbol: [00029F60][0002:00012F60] ?Lock@CAtlModule@ATL@@UAEJXZ(public: virtual long __thiscall ATL::CAtlModule::Lock(void))
PublicSymbol: [0004CDA8][0003:00004DA8] _GUID_DEVCLASS_FSFILTER_CFSMETADATASERVER(_GUID_DEVCLASS_FSFILTER_CFSMETADATASERVER)
PublicSymbol: [00021800][0002:0000A800] ?IsIdleMessage@CMessageLoop@WTL@@SAHPAUtagMSG@@@Z(public: static int __cdecl WTL::CMessageLoop::IsIdleMessage(struct tagMSG *))
PublicSymbol: [00020280][0002:00009280] ?IsUnderlineHover@?$CHyperLinkImpl@VCHyperLink@WTL@@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@4@@WTL@@QBE_NXZ(public: bool __thiscall WTL::CHyperLinkImpl<class WTL::CHyperLink,class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::IsUnderlineHover(void)const )
PublicSymbol: [0004BB38][0003:00003B38] ??_C@_0BD@ENDAHIGJ@new?5operator?5fault?$AA@(`string')
PublicSymbol: [0002EFA0][0002:00017FA0] ?AtlCoTaskMemRecalloc@ATL@@YAPAXPAXKK@Z(void * __cdecl ATL::AtlCoTaskMemRecalloc(void *,unsigned long,unsigned long))
PublicSymbol: [0001CCE0][0002:00005CE0] ??RCTraceFileAndLineInfo@ATL@@QBAXKIPBDZZ(public: void __cdecl ATL::CTraceFileAndLineInfo::operator()(unsigned long,unsigned int,char const *,...)const )
PublicSymbol: [00039C60][0002:00022C60] ??0Wrapper@?$CSimpleArray@PAUHINSTANCE__@@V?$CSimpleArrayEqualHelper@PAUHINSTANCE__@@@ATL@@@ATL@@QAE@ABQAUHINSTANCE__@@@Z(public: __thiscall ATL::CSimpleArray<struct HINSTANCE__ *,class ATL::CSimpleArrayEqualHelper<struct HINSTANCE__ *> >::Wrapper::Wrapper(struct HINSTANCE__ * const &))
PublicSymbol: [00028430][0002:00011430] ??1?$CFontT@$0A@@WTL@@QAE@XZ(public: __thiscall WTL::CFontT<0>::~CFontT<0>(void))
PublicSymbol: [0001D030][0002:00006030] ?AddTool@?$CToolTipCtrlT@VCWindow@ATL@@@WTL@@QAEHPAUHWND__@@V_U_STRINGorID@ATL@@PBUtagRECT@@I@Z(public: int __thiscall WTL::CToolTipCtrlT<class ATL::CWindow>::AddTool(struct HWND__ *,class ATL::_U_STRINGorID,struct tagRECT const *,unsigned int))
PublicSymbol: [00029690][0002:00012690] ?DefWindowProcA@?$CWindowImplBaseT@VCWindow@ATL@@V?$CWinTraits@$0FGAAAAAA@$0A@@2@@ATL@@QAEJIIJ@Z(public: long __thiscall ATL::CWindowImplBaseT<class ATL::CWindow,class ATL::CWinTraits<1442840576,0> >::DefWindowProcA(unsigned int,unsigned int,long))
PublicSymbol: [0004C790][0003:00004790] _IID_IEntryID(_IID_IEntryID)
PublicSymbol: [00026680][0002:0000F680] ?SafeStrlen@CString@WTL@@KGHPBD@Z(protected: static int __stdcall WTL::CString::SafeStrlen(char const *))
PublicSymbol: [0003C910][0002:00025910] ?AtlMarshalPtrInProc@ATL@@YGJPAUIUnknown@@ABU_GUID@@PAPAUIStream@@@Z(long __stdcall ATL::AtlMarshalPtrInProc(struct IUnknown *,struct _GUID const &,struct IStream * *))
PublicSymbol: [00036240][0002:0001F240]
PublicSymbol: [000530C8][0005:000000C8] __NULL_IMPORT_DESCRIPTOR(__NULL_IMPORT_DESCRIPTOR)
PublicSymbol: [0003EDF0][0002:00027DF0] ?MarkValid@CAtlTraceSettings@@QAEXJ@Z(public: void __thiscall CAtlTraceSettings::MarkValid(long))
PublicSymbol: [00034D20][0002:0001DD20]

*/
