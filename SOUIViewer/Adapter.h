#pragma once
#include <helper/SAdapterBase.h>

class CMcAdapter : public SMcAdapterBase
{
public:

public:
	/* LvAdatperImpl */
	virtual int getCount();
	virtual void getView(int position, SWindow * pItem, pugi::xml_node xmlTemplate);
	/* IMcAdapter */
	//��ȡһ������ģ���ж�Ӧ�Ĵ�������
	//int iCol: �����
	virtual SStringW GetColumnName(int iCol) const;
public:
};

