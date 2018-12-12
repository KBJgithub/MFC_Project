#pragma once
#include "afxwin.h"
#include "stdafx.h"
// CDialogSetting ��ȭ �����Դϴ�.

typedef struct addressDlg {
	int num;
	CString str;
};


class CDialogSetting : public CDialogEx
{
	DECLARE_DYNAMIC(CDialogSetting)

public:
	CDialogSetting(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CDialogSetting();

	// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_DIALOG1 };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
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
