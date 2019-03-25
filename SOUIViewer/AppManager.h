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
	//
	BOOL Run();

	// ����Ƥ��
	BOOL		LoadSkin(LPCTSTR lpcSkin);
	// ��������Ƥ��
	BOOL		LoadDefaultSkin();
	// ���¼���Ƥ��
	BOOL		ReloadSkin(LPCTSTR lpcSkin = NULL);
	// ����m_ParentHwnd
	void		SetParentHwnd(HWND hWnd);
private:
	TString			m_strSkin;		// Ƥ������·��
	TString			m_strSkinPath;	// Ƥ��Ŀ¼
	TString			m_AppPath;
	SApplication*	m_App;
	BOOL			m_bNewSkin;		// ��Ƥ��������ɣ�ͨ��������Ϣ֪ͨ
	HWND			m_ParentHwnd;	// �����߾��
	CCmdLine*		m_pCmd;
	HINSTANCE		m_hInstance;	
};

