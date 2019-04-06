#include "stdafx.h"
#include "AppManager.h"
#include <ShellAPI.h>
#include "Dialog/ViewDlg.h"
#include "Common/Utils.h"


template<> CAppManager* CSingleton<CAppManager>::ms_Singleton = NULL;

#pragma warning(disable:4996)
CAppManager::CAppManager(HINSTANCE hInstance)
	:m_dlgMain(NULL)
{
	m_hInstance = hInstance;
	m_AppUI = SApplication::getSingletonPtr();
	assert(m_AppUI);

	TCHAR szCurrentDir[MAX_PATH + 1] = { 0 };
	GetCurrentDirectory(MAX_PATH, szCurrentDir);
	m_AppPath = szCurrentDir;
}

CAppManager::~CAppManager()
{
	if (m_pCmd) delete m_pCmd;
}

BOOL CAppManager::Run()
{
	m_pCmd = new CCmdLine(::GetCommandLine());

	LoadDefaultSkin();
	LPCTSTR lpSkin = m_pCmd->GetParam(1);
	if (lpSkin) LoadSkin(lpSkin);

	m_AppUI->Run(m_dlgMain->m_hWnd);

	return TRUE;
}

BOOL CAppManager::LoadSkin(LPCTSTR lpcSkin)
{
	if (!lpcSkin || _tcslen(lpcSkin) == 0) return FALSE;
	TCHAR szBuf[MAX_PATH] = { 0 };
	::PathCombine(szBuf, m_AppPath.c_str(), lpcSkin);
	SStringT strSkin = szBuf;
	if (strSkin.IsEmpty()) return FALSE;

	size_t ps = strSkin.ReverseFind(_T('\\'));
	if (ps == TString::npos) return FALSE;
	SStringT strPath = strSkin.Mid(0, ps);

	if (strPath.IsEmpty())
		return FALSE;

	// �ͷ��Ѽ��ش���
	for (std::vector<CViewDlg*>::iterator it = m_Dlgs.begin();it != m_Dlgs.end();it++)
	{
		CViewDlg *pdlg = *it;
		pdlg->DestroyWindow();
		delete pdlg;
	}
	m_Dlgs.clear();
	// �ͷ�ResProvider
	if (m_pResProvider)
	{
		// TODO: �ο�SOUIEditer�Զ������
		CAutoRefPtr<SStylePool>		mStylePool; /**<�ֲ�style pool*/
		GETSTYLEPOOLMGR->PopStylePool(mStylePool);
		if (mStylePool && mStylePool->GetCount()) mStylePool->RemoveAll();

		CAutoRefPtr<SSkinPool>		pSkinPool;  /**<�ֲ�skin pool*/
		GETSKINPOOLMGR->PopSkinPool(pSkinPool);
		if (pSkinPool && pSkinPool->GetCount()) pSkinPool->RemoveAll();

		m_AppUI->RemoveResProvider(m_pResProvider);
	}

	CAutoRefPtr<IResProvider>   pResProvider;
	BOOL	bLoaded = FALSE;
	CreateResProvider(RES_FILE, (IObjRef**)&pResProvider);
	bLoaded = pResProvider->Init((LPARAM)(LPCTSTR)strPath, 0);
	SASSERT(bLoaded);

	m_AppUI->AddResProvider(pResProvider);

	pugi::xml_document xmlDoc;
	SStringT strView = strPath + _T("\\view.xml");
	if (xmlDoc.load_file(strView, pugi::parse_default, pugi::encoding_utf8))
	{
		// ����view.xml����LAYOUT
		pugi::xml_node xmlRoot = xmlDoc.child(L"root");
		if (!xmlRoot) return FALSE;
		pugi::xml_node xmlView = xmlRoot.first_child();
		while (xmlView)
		{
			const pugi::char_t* pszPath = xmlView.attribute(_T("path")).value();
			CViewDlg* pDlg = new CViewDlg(pszPath, FALSE);
			pDlg->Create(GetActiveWindow());
			pDlg->SendMessage(WM_INITDIALOG);
			pDlg->CenterWindow(pDlg->m_hWnd);
			pDlg->ShowWindow(SW_SHOWNORMAL);
			PushDlg(pDlg);

			xmlView = xmlView.next_sibling();
		}
	}
	else if (xmlDoc.load_file(strSkin, pugi::parse_default, pugi::encoding_utf8))
	{
		// ���ص�һ��layout
		pugi::xml_node xmlNode = xmlDoc.child(_T("resource")).child(_T("LAYOUT")).child(_T("file"));
		if (!xmlNode) return FALSE;
		TString szLayer = Utils::Format(_T("LAYOUT:%s"), xmlNode.attribute(_T("name")).as_string());
		CViewDlg* pDlg = new CViewDlg(szLayer.c_str(), FALSE);
		pDlg->Create(GetActiveWindow());
		pDlg->SendMessage(WM_INITDIALOG);
		pDlg->CenterWindow(pDlg->m_hWnd);
		pDlg->ShowWindow(SW_SHOWNORMAL);
		PushDlg(pDlg);
	}
	else
		return FALSE;

	m_strSkin = strSkin;
	m_strSkinPath = strPath;
	m_pResProvider = pResProvider;

	return TRUE;
}

BOOL CAppManager::LoadDefaultSkin()
{
	CAutoRefPtr<IResProvider>   pResProvider;
	BOOL	bLoaded = FALSE;

	{
		CreateResProvider(RES_PE, (IObjRef**)&pResProvider);
		bLoaded = pResProvider->Init((WPARAM)m_hInstance, 0);
		SASSERT(bLoaded);
	}

	m_AppUI->InitXmlNamedID(namedXmlID, ARRAYSIZE(namedXmlID), TRUE);
	m_AppUI->AddResProvider(pResProvider);

	m_dlgMain = new CViewDlg(_T("LAYOUT:XML_MAINWND"), TRUE);
	m_dlgMain->Create(GetActiveWindow());
	m_dlgMain->SendMessage(WM_INITDIALOG);
	m_dlgMain->CenterWindow(m_dlgMain->m_hWnd);
	m_dlgMain->ShowWindow(SW_SHOWNORMAL);

	return TRUE;
}

BOOL CAppManager::RefreshSkin()
{
	if (m_strSkin.empty())
		return FALSE;
	LoadSkin(m_strSkin.c_str());
	return TRUE;
}

BOOL CAppManager::PushDlg(CViewDlg* pDlg, BOOL bCheckMain)
{
	if (!pDlg) return FALSE;
	m_Dlgs.push_back(pDlg);
	if (bCheckMain && m_dlgMain && m_dlgMain->IsWindow())
		m_dlgMain->ShowWindow(SW_HIDE);
	return TRUE;
}

BOOL CAppManager::PopDlg(CViewDlg* pDlg, BOOL bCheckMain)
{
	BOOL bRet = FALSE;
	for (std::vector<CViewDlg*>::iterator it = m_Dlgs.begin(); it != m_Dlgs.end();)
	{
		if (pDlg == *it)
		{
			m_Dlgs.erase(it);
			bRet = TRUE;
			break;
		}
		++it;
	}
	if (m_Dlgs.size() == 0)
	{
		if (bCheckMain && m_dlgMain && m_dlgMain->IsWindow())
			m_dlgMain->ShowWindow(SW_SHOW);
	}
	return bRet;
}

BOOL CAppManager::ClearDlg(BOOL bDelete /*= TRUE*/)
{
	if (bDelete)
	{
		for (std::vector<CViewDlg*>::iterator it = m_Dlgs.begin(); it != m_Dlgs.end();)
		{
			CViewDlg *pDlg = *it;
			pDlg->OnClose();
			delete pDlg;
		}
	}
	m_Dlgs.clear();
	return FALSE;
}

BOOL CAppManager::ShowDlgMain(int nShow)
{
	if (m_dlgMain)
		return m_dlgMain->ShowWindow(nShow);
	return FALSE;
}

