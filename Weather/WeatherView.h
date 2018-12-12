
// WeatherView.h : CWeatherView 클래스의 인터페이스
//

#pragma once
#include "PictureEx.h"


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

// 작업입니다.
public:
	int Show_Town_Main();
	int Animation_Bmp();
	int Print_map(int);
	int Map_into(int map_status);
	int Show_Temp_Map();
	void Animation(int index, CPoint point);
	void enroll_weather();
	town_cord Syncdata(CRect m_rect);
	int Show_Tooltip(int m_status, CPoint point);
// 재정의입니다.
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
//	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
//	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	int M_button_down;
	int M_button_up;
	CPoint down;
	CPoint Up;
//	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	int key_flag;
	CToolTipCtrl* m_pToolTip;
	afx_msg void OnDestroy();
	CPictureEx m_Picture0;
	CPictureEx m_Picture1;
	CPictureEx m_Picture2;
	CPictureEx m_Picture3;
	CPictureEx m_Picture4;
	CPictureEx m_Picture5;
	CPictureEx m_Picture6;

//	afx_msg void OnSetFocus(CWnd* pOldWnd);
//	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	int tooltip_flag;
};

#ifndef _DEBUG  // WeatherView.cpp의 디버그 버전
inline CWeatherDoc* CWeatherView::GetDocument() const
   { return reinterpret_cast<CWeatherDoc*>(m_pDocument); }
#endif

