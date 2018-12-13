
// WeatherView.h : CWeatherView Ŭ������ �������̽�
//

#pragma once

UINT Moving_Bmp(LPVOID param);
class CWeatherView : public CFormView
{
protected: // serialization������ ��������ϴ�.
	CWeatherView();
	DECLARE_DYNCREATE(CWeatherView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_WEATHER_FORM };
#endif

// Ư���Դϴ�.
public:
	CWeatherDoc* GetDocument() const;

//****************************************************************************
public: //�Լ�
	int Show_Town_Main();
	int Print_map(int);
	int Map_into(int map_status);
	int Show_Temp_Map();
	town_cord Syncdata(CRect m_rect);
	int Show_Tooltip(int m_status, CPoint point);
	int Print_Unvisible_Bmp(int x, int y, CPaintDC &dc, int R_id);
	int Print_All_Moving_Bmp(CPaintDC &dc);
	int CWeatherView::Print_Unvisible_Bmp(int x, int y, CDC* &cdc, int R_id);
public: //���� 
	int M_button_down;
	int M_button_up;
	CPoint down;
	CPoint Up;
	int key_flag;
	int tooltip_flag;
//****************************************************************************

public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.
	virtual void OnInitialUpdate(); // ���� �� ó�� ȣ��Ǿ����ϴ�.

// �����Դϴ�.
public:
	virtual ~CWeatherView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);	
	CToolTipCtrl* m_pToolTip;
	afx_msg void OnDestroy();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // WeatherView.cpp�� ����� ����
inline CWeatherDoc* CWeatherView::GetDocument() const
   { return reinterpret_cast<CWeatherDoc*>(m_pDocument); }
#endif

