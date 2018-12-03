
// WeatherView.cpp : CWeatherView Ŭ������ ����
//

#include "stdafx.h"
// SHARED_HANDLERS�� �̸� ����, ����� �׸� �� �˻� ���� ó���⸦ �����ϴ� ATL ������Ʈ���� ������ �� ������
// �ش� ������Ʈ�� ���� �ڵ带 �����ϵ��� �� �ݴϴ�.
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

// CWeatherView ����/�Ҹ�

CWeatherView::CWeatherView()
{
	// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.

}

CWeatherView::~CWeatherView()
{
}

BOOL CWeatherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.

	return CView::PreCreateWindow(cs);
}

// CWeatherView �׸���

void CWeatherView::OnDraw(CDC* /*pDC*/)
{
	CWeatherDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: ���⿡ ���� �����Ϳ� ���� �׸��� �ڵ带 �߰��մϴ�.
}


// CWeatherView ����

#ifdef _DEBUG
void CWeatherView::AssertValid() const
{
	CView::AssertValid();
}

void CWeatherView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CWeatherDoc* CWeatherView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWeatherDoc)));
	return (CWeatherDoc*)m_pDocument;
}
#endif //_DEBUG


// CWeatherView �޽��� ó����


void CWeatherView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	KBJ kbj;
	kbj.Get_Coordinates_From_Dat();
	for (int loop = 0; loop < NUM_OF_COORDINATES; loop++)
	{
		kbj.Get_Weather(1, info[loop].grid, info[loop]);
	}
	CView::OnLButtonDown(nFlags, point);
}
