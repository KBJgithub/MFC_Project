
// WeatherView.h : CWeatherView 클래스의 인터페이스
//

#pragma once

UINT Moving_Bmp(LPVOID param);
class CWeatherView : public CFormView
{
protected: // serialization에서만 만들어집니다.
	CWeatherView();
	DECLARE_DYNCREATE(CWeatherView)

public:
#ifdef AFX_DESIGN_TIME
	enum{ IDD = IDD_WEATHER_FORM };
#endif

// 특성입니다.
public:
	CWeatherDoc* GetDocument() const;

//****************************************************************************
public: //함수
	int Show_Town_Main();
	int Print_map(int);
	int Map_into(int map_status);
	int Show_Temp_Map();
	town_cord Syncdata(CRect m_rect);
	int Show_Tooltip(int m_status, CPoint point);
	int Print_Unvisible_Bmp(int x, int y, CPaintDC &dc, int R_id);
	int Print_All_Moving_Bmp(CPaintDC &dc);
	int CWeatherView::Print_Unvisible_Bmp(int x, int y, CDC* &cdc, int R_id);
public: //변수 
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.
	virtual void OnInitialUpdate(); // 생성 후 처음 호출되었습니다.

// 구현입니다.
public:
	virtual ~CWeatherView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
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

#ifndef _DEBUG  // WeatherView.cpp의 디버그 버전
inline CWeatherDoc* CWeatherView::GetDocument() const
   { return reinterpret_cast<CWeatherDoc*>(m_pDocument); }
#endif

