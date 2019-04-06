// dui-demo.cpp : main source file
//

#include "stdafx.h"
#include "AppManager.h"
#include "Control/SMatrixWindow.h"
#include "Control/SImageMaskWnd.h"
#include "Control/gif/SGifPlayer.h"
#include "Control/gif/SSkinAPNG.h"
#include "Control/gif/SSkinGif.h"
#include "Control/SVscrollbar.h"
#include "Control/SSkinNewScrollBar.h"
#include "Control/SIECtrl.h"
#include "Control/SImageEx.h"

#ifdef _DEBUG
#define SYS_NAMED_RESOURCE _T("soui-sys-resourced.dll")
#else
#define SYS_NAMED_RESOURCE _T("soui-sys-resource.dll")
#endif

//定义唯一的一个R,UIRES对象,ROBJ_IN_CPP是resource.h中定义的宏。
ROBJ_IN_CPP

int WINAPI _tWinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpstrCmdLine, int /*nCmdShow*/)
{
	HRESULT hRes = OleInitialize(NULL);
	SASSERT(SUCCEEDED(hRes));

	int nRet = 0;

	SComMgr *pComMgr = new SComMgr;
	TCHAR szCurrentDir[MAX_PATH] = { 0 };
	GetModuleFileName(NULL, szCurrentDir, sizeof(szCurrentDir));

	{
		BOOL bLoaded = FALSE;
		CAutoRefPtr<SOUI::IImgDecoderFactory> pImgDecoderFactory;
		CAutoRefPtr<SOUI::IRenderFactory> pRenderFactory;
		bLoaded = pComMgr->CreateRender_GDI((IObjRef**)&pRenderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [render] failed!"));
		bLoaded = pComMgr->CreateImgDecoder((IObjRef**)&pImgDecoderFactory);
		SASSERT_FMT(bLoaded, _T("load interface [%s] failed!"), _T("imgdecoder"));

		pRenderFactory->SetImgDecoderFactory(pImgDecoderFactory);
		SApplication *theApp = new SApplication(pRenderFactory, hInstance);

		//向SApplication系统中注册由外部扩展的控件及SkinObj类
		theApp->RegisterWindowClass<SMatrixWindow>();//
		theApp->RegisterWindowClass<SGifPlayer>();//theApp中增加方法：RegisterWindowClass,替换RegisterWndFactory(TplSWindowFactory<SGifPlayer>())
		theApp->RegisterSkinClass<SSkinGif>();//注册SkinGif
		theApp->RegisterSkinClass<SSkinAPNG>();//注册SSkinAPNG
		theApp->RegisterSkinClass<SSkinVScrollbar>();//注册纵向滚动条皮肤
		theApp->RegisterSkinClass<SSkinNewScrollbar>();//注册纵向滚动条皮肤

		theApp->RegisterWindowClass<SImageMaskWnd>();//注册SImageMaskWnd
		theApp->RegisterWindowClass<SIECtrl>();
		theApp->RegisterWindowClass<SImageEx>();

		//加载系统资源
		HMODULE hSysResource = LoadLibrary(SYS_NAMED_RESOURCE);
		if (hSysResource)
		{
			CAutoRefPtr<IResProvider> sysSesProvider;
			CreateResProvider(RES_PE, (IObjRef**)&sysSesProvider);
			sysSesProvider->Init((WPARAM)hSysResource, 0);
			theApp->LoadSystemNamedResource(sysSesProvider);
		}

		// BLOCK: Run application
		{
			SNotifyCenter *pNotifyCenter = new SNotifyCenter;

			CAppManager* pApp = new CAppManager(hInstance);
			pApp->Run();
		}

		delete theApp;
	}

	delete pComMgr;

	OleUninitialize();
	return nRet;
}
