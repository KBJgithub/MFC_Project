#pragma once
#include "afxwin.h"
#include "stdafx.h"
// CDialogSetting 대화 상자입니다.

typedef struct addressDlg {
	int num;
	CString str;
};


class CDialogSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSetting)

public:
	CDialogSetting(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CDialogSetting();

	// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	DECLARE_MESSAGE_MAP()

public:
	u_int start;
	u_int numDo;
	u_int numSi;
	u_int numDong;
	u_int m_DialogFix;
	u_int m_DialogMode;
	u_int m_DialogDo;
	u_int m_DialogSi;
	u_int m_DialogDong;
	u_int m_DialogUpdate;
	CString m_DialogStrDo;
	CString m_DialogStrSi;
	CString m_DialogStrDong;
	u_int Dialog_CordDo;
	u_int Dialog_CordSi;
	u_int Dialog_CordDong;
	CString Dialog_strDo;
	CString Dialog_strSi;
	CString Dialog_strDong;
public:
	CButton m_CheckFix;
	afx_msg void OnPaint();
	CComboBox m_ComboMode;
	CComboBox m_ComboDo;
	CComboBox m_ComboSi;
	CComboBox m_ComboDong;
	CComboBox m_ComboUpdate;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCombo2();
	afx_msg void OnCbnSelchangeCombo3();
	afx_msg void OnCbnSelchangeCombo4();
	afx_msg void OnDestroy();
	afx_msg void OnCbnSelchangeCombo5();
};
