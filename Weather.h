
// Weather.h : Weather ���� ���α׷��� ���� �� ��� ����
//
#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"       // �� ��ȣ�Դϴ�.


// CWeatherApp:
// �� Ŭ������ ������ ���ؼ��� Weather.cpp�� �����Ͻʽÿ�.
//

class CWeatherApp : public CWinApp
{
public:
	CWeatherApp();


// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CWeatherApp theApp;
