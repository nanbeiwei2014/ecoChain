
// httptestclient.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// ChttptestclientApp: 
// �йش����ʵ�֣������ httptestclient.cpp
//

class ChttptestclientApp : public CWinApp
{
public:
	ChttptestclientApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern ChttptestclientApp theApp;