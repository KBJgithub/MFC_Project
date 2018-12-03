
// WeatherView.cpp : CWeatherView 클래스의 구현
//

#include "stdafx.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "Weather.h"
#endif

#include "WeatherDoc.h"
#include "WeatherView.h"
#include "xml_parsing.h"

extern weather_info info[NUM_OF_COORDINATES];

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CWeatherView

IMPLEMENT_DYNCREATE(CWeatherView, CView)

BEGIN_MESSAGE_MAP(CWeatherView, CView)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

// CWeatherView 생성/소멸

CWeatherView::CWeatherView()
{
	// TODO: 여기에 생성 코드를 추가합니다.

}

CWeatherView::~CWeatherView()
{
}

BOOL CWeatherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.

	return CView::PreCreateWindow(cs);
}

// CWeatherView 그리기

void CWeatherView::OnDraw(CDC* /*pDC*/)
{
	CWeatherDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 여기에 원시 데이터에 대한 그리기 코드를 추가합니다.
}


// CWeatherView 진단

#ifdef _DEBUG
void CWeatherView::AssertValid() const
{
	CView::AssertValid();
}

void CWeatherView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWeatherDoc* CWeatherView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWeatherDoc)));
	return (CWeatherDoc*)m_pDocument;
}
#endif //_DEBUG


// CWeatherView 메시지 처리기


void CWeatherView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	KBJ kbj;
	kbj.Get_Coordinates_From_Dat();
	for (int loop = 0; loop < NUM_OF_COORDINATES; loop++)
	{
		kbj.Get_Weather(1, info[loop].grid, info[loop]);
	}
	CView::OnLButtonDown(nFlags, point);
}
