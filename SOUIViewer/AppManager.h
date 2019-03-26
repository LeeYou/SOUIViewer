#pragma once
#include "stdafx.h"
#include "Common/Singleton.h"
#include "Common/CmdLine.h"
#include "Dialog/ViewDlg.h"


class CAppManager: public CSingleton<CAppManager>
{
	friend CViewDlg;
public:
	explicit CAppManager(HINSTANCE hInstance);
	~CAppManager();
public:
	// �������
	BOOL		Run();
	// ����Ƥ��
	BOOL		LoadSkin(LPCTSTR lpcSkin);
	// ��������Ƥ��
	BOOL		LoadDefaultSkin();
	// ���¼���Ƥ��
	BOOL		RefreshSkin();
	// ��������
	BOOL		PushDlg(CViewDlg* pDlg, BOOL bCheckMain = TRUE);
	// ɾ������
	BOOL		PopDlg(CViewDlg* pDlg, BOOL bCheckMain = TRUE);
	// ��մ��ڲ��ͷ�
	BOOL		ClearDlg(BOOL bDelete = TRUE);
private:
	TString			m_strSkin;		// Ƥ������·��
	TString			m_strSkinPath;	// Ƥ��Ŀ¼
	TString			m_AppPath;
	SApplication*	m_AppUI;
	CCmdLine*		m_pCmd;
	HINSTANCE		m_hInstance;	
	CAutoRefPtr<IResProvider> m_pResProvider;		//���̵�UI�ṩ��
	std::vector<CViewDlg*>	m_Dlgs;	// Dlg�б�
	CViewDlg*		m_dlgMain;		// ������
};

