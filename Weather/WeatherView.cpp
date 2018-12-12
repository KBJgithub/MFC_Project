
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
#include <time.h>
extern std::vector<weather_info> info;
extern CCriticalSection cs;
extern int update_flag;
extern int POSIT;
extern int mode2_x;
extern int mode2_y; 
extern int mode2_width;
extern int mode2_height;
extern CTime cTime;
extern u_int mode;
extern int animation_status; //애니메이션 bmp( 날씨 아이콘 )이 3개 있고 각 상태(0,1,2)에 따라서 bmp를 다른 것을 출력함. 
extern int *index;
extern CRect map_rect[17][17];
extern int map_status;
extern u_int fix;
extern int num_towns;
extern std::vector<town_cord> towns; //날씨정보 업데이트 스레드와 공유할 전역변수.
extern int wakeup_thread;
extern CWinThread* pThread;
#ifdef _DEBUG
#define new DEBUG_NEW
#define RATIO 1.27
#endif



// CWeatherView

IMPLEMENT_DYNCREATE(CWeatherView, CFormView)

BEGIN_MESSAGE_MAP(CWeatherView, CFormView)
	ON_WM_PAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_DESTROY()
ON_WM_KEYUP()
ON_WM_KEYDOWN()
//ON_WM_SETFOCUS()
//ON_WM_SETFOCUS()
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CWeatherView 생성/소멸


CWeatherView::CWeatherView()
	: CFormView(IDD_WEATHER_FORM)
{
	M_button_down = 0;
	M_button_up = 0;
	key_flag = 0;
	tooltip_flag = 0;
}

CWeatherView::~CWeatherView()
{
}


void CWeatherView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PICTURE0, m_Picture0);
	DDX_Control(pDX, IDC_PICTURE1, m_Picture1);
	DDX_Control(pDX, IDC_PICTURE2, m_Picture2);
	DDX_Control(pDX, IDC_PICTURE3, m_Picture3);
	DDX_Control(pDX, IDC_PICTURE4, m_Picture4);
	DDX_Control(pDX, IDC_PICTURE5, m_Picture5);
	DDX_Control(pDX, IDC_PICTURE6, m_Picture6);
}


BOOL CWeatherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	CString strSection = _T("My Section");
	CString str_DialogFix = _T("m_DialogFix");
	CString str_DialogMode = _T("m_DialogMode");
	CString str_DialogDo = _T("m_DialogDo");
	CString str_DialogSi = _T("m_DialogSi");
	CString str_DialogDong = _T("m_DialogDong");
	CString str_DialogUpdate = _T("m_DialogUpdate");

	CWinApp* pApp = AfxGetApp();

	AfxDialogSetting.m_DialogFix = pApp->GetProfileInt(strSection, str_DialogFix, 9999);
	AfxDialogSetting.m_DialogMode = pApp->GetProfileInt(strSection, str_DialogMode, 9999);
	AfxDialogSetting.m_DialogDo = pApp->GetProfileInt(strSection, str_DialogDo, 9999);
	AfxDialogSetting.m_DialogSi = pApp->GetProfileInt(strSection, str_DialogSi, 9999);
	AfxDialogSetting.m_DialogDong = pApp->GetProfileInt(strSection, str_DialogDong, 9999);
	AfxDialogSetting.m_DialogUpdate = pApp->GetProfileInt(strSection, str_DialogUpdate, 9999);


	return TRUE;
}

void CWeatherView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();


	CPoint point(100, 200);
	Animation(0, point);


	m_pToolTip = new CToolTipCtrl;
	m_pToolTip->Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_pToolTip->SetDelayTime(500);
	m_pToolTip->SetMaxTipWidth(170);
	m_pToolTip->Activate(FALSE);

	CSize scrollSize(0, 0);
	SetScrollSizes(MM_TEXT, scrollSize);

}


// CWeatherView 진단

#ifdef _DEBUG
void CWeatherView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWeatherView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
CWeatherDoc* CWeatherView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWeatherDoc)));
	return (CWeatherDoc*)m_pDocument;
}
#endif //_DEBUG


// CWeatherView 메시지 처리기


void CWeatherView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.

	if (update_flag == 1)
	{
		cs.Lock();
		if (mode == 1 || mode == 3)
		{
			Print_map(map_status);
			dc.SelectStockObject(NULL_BRUSH);
			for (int loop = 0; loop < 16; loop++)
			{
				dc.Rectangle(map_rect[map_status][loop]);
			}
			Show_Temp_Map();
		}
		if (mode == 2 || mode == 3)
			Show_Town_Main();
		//update_flag = 0;
		tooltip_flag = 1;
		cs.Unlock();
	}	
				   // 그리기 메시지에 대해서는 CFormView::OnPaint()을(를) 호출하지 마십시오.
}

int CWeatherView::Show_Town_Main()
{
	CDC *cdc = GetDC();
	cs.Lock();
	if ((mode == 2 || mode == 3))
	{
		CBitmap mode2;
		mode2.LoadBitmap(IDB_NALSSI);
		BITMAP bmpinfo;
		int x_shift,x_dis,y_shift,y_dis;
		mode2.GetBitmap(&bmpinfo);
		CDC dcmem;
		dcmem.CreateCompatibleDC(cdc);
		dcmem.SelectObject(mode2);
		cdc->StretchBlt(mode2_x, mode2_y, bmpinfo.bmWidth, bmpinfo.bmHeight, &dcmem, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);

		cTime = CTime::GetCurrentTime();
		CFont font;
		cdc->SetBkMode(TRANSPARENT);
		x_shift = 100;
		x_dis = 77;
		y_shift = 160;
		y_dis = 30;
		font.CreatePointFont(100, _T("휴먼매직체"));
		cdc->SelectObject(&font);
		cdc->SetTextColor(RGB(100, 100, 100));

		CString strDate, strTime, hour; // 반환되는 날짜와 시간을 저장할 CString 변수 선언
		strDate.Format(_T("%04d년 %02d월 %02d일"), cTime.GetYear(), // 현재 년도 반환
			cTime.GetMonth(), // 현재 월 반환
			cTime.GetDay()); // 현재 일 반환
		strTime.Format(_T("%02d시 %02d분 %02d초"), cTime.GetHour(), // 현재 시간 반환
			cTime.GetMinute(), // 현재 분 반환
			cTime.GetSecond()); // 현재 초 반환
		cdc->TextOut(mode2_x + 470, mode2_y + 10, strDate);
		cdc->TextOut(mode2_x + 590, mode2_y + 10, strTime);
		cdc->SetBkMode(TRANSPARENT);
		for (int loop = 0; loop < 11; loop++)
		{
			hour.Format(_T("%d시"), (cTime.GetHour() + 3 * loop) % 24);
			cdc->TextOut(mode2_x + x_shift + x_dis * loop , mode2_y + 65 , hour);
			hour.Format(_T("%.1f"), info[index[0]].temp[loop].tmp);
			cdc->TextOut(mode2_x + x_shift + x_dis * loop , mode2_y + y_shift + y_dis * 0, hour);
		
	
			cdc->SetTextColor(RGB(255, 0, 0));
			hour.Format(_T("%.1f"), info[index[0]].temp[loop].tmx);
			cdc->TextOut(mode2_x + x_shift + x_dis * loop , mode2_y + y_shift + y_dis * 1, hour);
			cdc->SetTextColor(RGB(100, 100, 100));
			cdc->SetTextColor(RGB(0, 0, 255));
			hour.Format(_T("%.1f"), info[index[0]].temp[loop].tmn);
			cdc->TextOut(mode2_x + x_shift + x_dis * loop, mode2_y + y_shift + y_dis * 2, hour);
			cdc->SetTextColor(RGB(100, 100, 100));
			hour.Format(_T("%d "), info[index[0]].pop[loop]);
			cdc->TextOut(mode2_x + x_shift + x_dis * loop, mode2_y + y_shift + y_dis * 3, hour);

			cdc->SetTextColor(RGB(100, 100, 100));
			hour.Format(_T("%d "), info[index[0]].reh[loop]);
			cdc->TextOut(mode2_x + x_shift + x_dis * loop, mode2_y + y_shift + y_dis * 4, hour);

			cdc->SetTextColor(RGB(100, 100, 100));
			hour.Format(_T("%.1f"), info[index[0]].ws[loop]);
			cdc->TextOut(mode2_x + x_shift + x_dis * loop, mode2_y + y_shift + y_dis * 5, hour);
		}
	}
	cs.Unlock();
	ReleaseDC(cdc);
	return 0;
}
int CWeatherView::Animation_Bmp()
{
	//날씨 아이콘을 현재 animation_status에따라서
	//3가지 비트맵을 정해진 위치에 계속 지우고 다시 그려주는 과정이 필요함.
	//지우는 과정은 work thread에서 해주니까
	//bmp다시 그리는 과정만 들어가면 됨.

	return 0;
}
int CWeatherView::Print_map(int map_sta)
{
	CClientDC dc(this);
	CRect crt;
	GetClientRect(crt);
	CString str;

	CBitmap bitmap;
	switch (map_sta) {

	case 0:
		bitmap.LoadBitmap(IDB_TOTAL); break;
	case 1:
		bitmap.LoadBitmap(IDB_SEOUL); break;
	case 2:
		bitmap.LoadBitmap(IDB_GYEONGGY); break;
	case 3:
		bitmap.LoadBitmap(IDB_INCHEON); break;
	case 4:
		bitmap.LoadBitmap(IDB_GANGWON); break;
	case 5:
		bitmap.LoadBitmap(IDB_CHUNGNAM); break;
	case 6:
		bitmap.LoadBitmap(IDB_DEAJEON); break;
	case 7:
		bitmap.LoadBitmap(IDB_CHUNGBUK); break;
	case 8:
		bitmap.LoadBitmap(IDB_GYEONGBUK); break;
	case 9:
		bitmap.LoadBitmap(IDB_DEAGU); break;
	case 10:
		bitmap.LoadBitmap(IDB_JEONNAM); break;
	case 11:
		bitmap.LoadBitmap(IDB_GWANGJU); break;
	case 12:
		bitmap.LoadBitmap(IDB_GYEONGNAM); break;
	case 13:
		bitmap.LoadBitmap(IDB_ULSAN); break;
	case 14:
		bitmap.LoadBitmap(IDB_BUSAN); break;
	case 15:
		bitmap.LoadBitmap(IDB_JEONBUK); break;
	case 16:
		bitmap.LoadBitmap(IDB_JEJU); break;
	default:
		AssertValid(); break;
	}
	BITMAP bmpinfo;
	bitmap.GetBitmap(&bmpinfo);

	CDC dcmem;
	dcmem.CreateCompatibleDC(&dc);
	dcmem.SelectObject(bitmap);
	dc.StretchBlt(527-POSIT, 20, bmpinfo.bmWidth*RATIO, bmpinfo.bmHeight*RATIO, &dcmem, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);
	return map_status;
}
int CWeatherView::Map_into(int map_sta)
{
	CRect Back(457 - (mode / 3)*POSIT, 20, 527 - (mode / 3)*POSIT, 90);
	CPoint point;
	::GetCursorPos(&point);                        // 현재 마우스 포인터 얻기 
	ScreenToClient(&point);
	CRgn rgn;
	if (map_sta == 0) {
		if (map_rect[0][0].PtInRect(point)) {   // 서울
			map_status = 1;
			//Invalidate();
		}
		else if (map_rect[0][8].PtInRect(point) && (!(map_rect[0][1].PtInRect(point) || map_rect[0][9].PtInRect(point)))) {
			map_status = 2;
			//Invalidate();
		}
		else if (map_rect[0][9].PtInRect(point)) {
			map_status = 3;
			//Invalidate();
		}
		else if (map_rect[0][5].PtInRect(point)) {
			map_status = 4;
			//Invalidate();
		}
		else if (map_rect[0][16].PtInRect(point) && (!(map_rect[0][3].PtInRect(point)))) {
			map_status = 15;
			//Invalidate();
		}
		else if (map_rect[0][10].PtInRect(point)) {
			map_status = 5;
			//Invalidate();
		}
		else if (map_rect[0][4].PtInRect(point)) {
			map_status = 6;
			//Invalidate();
		}
		else if (map_rect[0][11].PtInRect(point)) {
			map_status = 7;
			//Invalidate();
		}
		else if (map_rect[0][3].PtInRect(point)) {
			map_status = 11;
			//Invalidate();
		}
		else if (map_rect[0][7].PtInRect(point) || map_rect[0][12].PtInRect(point)) {
			map_status = 8;
			//Invalidate();
		}
		else if (map_rect[0][2].PtInRect(point)) {      //대구   
			map_status = 9;
			//Invalidate();
		}
		else if (map_rect[0][13].PtInRect(point)) {      //전라북도   
			map_status = 10;
			//Invalidate();
		}
		else if (map_rect[0][14].PtInRect(point)) {
			map_status = 12;
			//Invalidate();
		}
		else if (map_rect[0][15].PtInRect(point)) {
			map_status = 13;
			//Invalidate();
		}
		else if (map_rect[0][1].PtInRect(point)) {
			map_status = 14;
			//Invalidate();
		}
		else if (map_rect[0][6].PtInRect(point)) {
			map_status = 16;
			//Invalidate();
		}
		else return 0;
	}
	KBJ k;
	k.enroll_weather();
	return 1;
}
void CWeatherView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (key_flag == 1)
	{
		GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION);

	}
	else if (mode == 1 || mode == 3)
	{
		if (Map_into(map_status))
		{
			cs.Lock();
			update_flag = 0;
			tooltip_flag = 0;
			DWORD nExitCode = NULL;
			GetExitCodeThread(pThread->m_hThread, &nExitCode);
			TerminateThread(pThread->m_hThread, nExitCode);
			pThread = AfxBeginThread(Update_Info_Work_Thread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			pThread->m_bAutoDelete = FALSE;
			pThread->ResumeThread();
		//	wakeup_thread = 1;
			cs.Unlock();
		}
	}	
	CFormView::OnLButtonDown(nFlags, point);
}
void CWeatherView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	if (map_status != 0)
	{
		if (mode == 1 || mode == 3)
		{
			cs.Lock();
			map_status = 0;
			KBJ k;
			k.enroll_weather();
			update_flag = 0;
			tooltip_flag = 0;
			DWORD nExitCode = NULL;
			GetExitCodeThread(pThread->m_hThread, &nExitCode);
			TerminateThread(pThread->m_hThread, nExitCode);
			pThread = AfxBeginThread(Update_Info_Work_Thread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			pThread->m_bAutoDelete = FALSE;
			pThread->ResumeThread();
			//wakeup_thread = 1;
			cs.Unlock();
		}
	}

	CFormView::OnRButtonDown(nFlags, point);
}

void CWeatherView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (tooltip_flag == 1)
	{
		Show_Tooltip(map_status, point);	
	}

	CFormView::OnMouseMove(nFlags, point);
}

void CWeatherView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
}
BOOL CWeatherView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	CBrush backBrush(RGB(255, 255, 255)); // <- 흰색칼러로. 
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect; pDC->GetClipBox(&rect);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);
	return TRUE;
}
void CWeatherView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.

}
int CWeatherView::Show_Temp_Map()
{
	int x, y, distance = 40;
	CString output;
	CFont font,m_Font;
	KBJ kb;
	CClientDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	font.CreatePointFont(120, _T("Verdana"));
	m_Font.CreateFont(14,                     // 글자높이
		12,                     // 글자너비
		0,                      // 출력각도
		0,                      // 기준 선에서의각도
		FW_BOLD,              // 글자굵기
		FALSE,                  // Italic 적용여부
		FALSE,                  // 밑줄적용여부
		FALSE,                  // 취소선적용여부
		DEFAULT_CHARSET,       // 문자셋종류
		OUT_DEFAULT_PRECIS,    // 출력정밀도
		CLIP_DEFAULT_PRECIS,   // 클리핑정밀도
		DEFAULT_QUALITY,       // 출력문자품질
		DEFAULT_PITCH,         // 글꼴Pitch
		_T("Verdana")           // 글꼴
	);
	dc.SelectObject(&m_Font);
	if ((mode == 1) || (mode == 3)) {
		for (int loop = 0; loop < num_towns; loop++)
		{
			if (map_status == 0 && loop == num_towns - 2)
			{

				x = map_rect[map_status][loop].left;
				y = map_rect[map_status][loop].top;
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmp);
				dc.SetTextColor(RGB(0, 0, 0));
				dc.TextOut(x +70, y + distance-20, output);
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmx);
				dc.SetTextColor(RGB(255, 0, 0));
				dc.TextOut(x + 118 , y + -20+ distance, output);
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmn);
				dc.SetTextColor(RGB(0, 0, 255));
				dc.TextOut(x + 173 , y + distance - 20, output);
			}
			else if(map_status != 0)
			{
				x = map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left;
				y = map_rect[map_status][loop].Height()  / 2 + map_rect[map_status][loop].top;
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmp);
				dc.SetTextColor(RGB(0, 0, 0));
				dc.TextOut(x , y + distance, output);
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmx);
				dc.SetTextColor(RGB(255, 0, 0));
				dc.TextOut(x + 35, y + ((distance / 2) + distance), output);
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmn);
				dc.SetTextColor(RGB(0, 0, 255));
				dc.TextOut(x - 35 , y + ((distance / 2) + distance), output);
			}
			else
			{
				x = map_rect[map_status][loop].left;
				y = map_rect[map_status][loop].top;
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmp);
				dc.SetTextColor(RGB(0, 0, 0));
				dc.TextOut(x , y + distance, output);
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmx);
				dc.SetTextColor(RGB(255, 0, 0));
				dc.TextOut(x + 35 , y + ((distance / 2) + distance), output);
				output.Format(_T("%.1f"), info[kb.Get_Index_From_Cord(towns[loop])].temp[0].tmn);
				dc.SetTextColor(RGB(0, 0, 255));
				dc.TextOut(x - 35, y + ((distance / 2) + distance), output);
			}
		}
	}
	return 0;
}
void CWeatherView::OnDestroy()
{
	CFormView::OnDestroy();

	CString strSection = _T("My Section");
	CString str_DialogFix = _T("m_DialogFix");
	CString str_DialogMode = _T("m_DialogMode");
	CString str_DialogDo = _T("m_DialogDo");
	CString str_DialogSi = _T("m_DialogSi");
	CString str_DialogDong = _T("m_DialogDong");
	CString str_DialogUpdate = _T("m_DialogUpdate");

	CWinApp* pApp = AfxGetApp();

	pApp->WriteProfileInt(strSection, str_DialogFix, AfxDialogSetting.m_DialogFix);
	pApp->WriteProfileInt(strSection, str_DialogMode, AfxDialogSetting.m_DialogMode);
	pApp->WriteProfileInt(strSection, str_DialogDo, AfxDialogSetting.m_DialogDo);
	pApp->WriteProfileInt(strSection, str_DialogSi, AfxDialogSetting.m_DialogSi);
	pApp->WriteProfileInt(strSection, str_DialogDong, AfxDialogSetting.m_DialogDong);
	pApp->WriteProfileInt(strSection, str_DialogUpdate, AfxDialogSetting.m_DialogUpdate);

	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

void CWeatherView::Animation(int index, CPoint point) {
	//	CPaintDC dc(this);
	switch (index) {

	case 0: m_Picture0.MoveWindow(point.x, point.y, 100, 90);
		m_Picture0.Load(_T("맑음.gif"));
		m_Picture0.Draw();
	case 1: m_Picture1.MoveWindow(point.x, point.y, 100, 90);
		m_Picture1.Load(_T("구름조금.gif"));
		m_Picture1.Draw();
	case 2: m_Picture2.MoveWindow(point.x, point.y, 100, 90);
		m_Picture2.Load(_T("구름많음.gif"));
		m_Picture2.Draw();
	case 3: m_Picture3.MoveWindow(point.x, point.y, 100, 90);
		m_Picture3.Load(_T("흐림.gif"));
		m_Picture3.Draw();
	case 4: m_Picture4.MoveWindow(point.x, point.y, 100, 90);
		m_Picture4.Load(_T("비.gif"));
		m_Picture4.Draw();
	case 5: m_Picture5.MoveWindow(point.x, point.y, 100, 90);
		m_Picture5.Load(_T("눈비.gif"));
		m_Picture5.Draw();
	case 6: m_Picture6.MoveWindow(point.x, point.y, 100, 90);
		m_Picture6.Load(_T("눈.gif"));
		m_Picture6.Draw();
	}

	/*
	m_Picture0.MoveWindow(100, 100, 400, 500);

	if (m_Picture0.Load(_T("res\\눈비.gif")))
	{
	m_Picture0.Draw();
	}

	m_Picture1.MoveWindow(200, 200, 400, 500);

	if (m_Picture1.Load(_T("res\\흐림.gif")))
	{
	m_Picture1.Draw();
	}
	*/
}

void CWeatherView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nChar == VK_CONTROL && key_flag != 1)
		key_flag = 1;
	CFormView::OnKeyDown(nChar, nRepCnt, nFlags);
}


void CWeatherView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (nChar == VK_CONTROL)
		key_flag = 0;
	CFormView::OnKeyUp(nChar, nRepCnt, nFlags);
}


BOOL CWeatherView::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 여기에 특수화된 코드를 추가 및/또는 기본 클래스를 호출합니다.
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_CONTROL)
		{
			//AfxMessageBox(TEXT("버튼 왼쪽 다운")); 
			key_flag = 1;
		}
		break;
	case WM_KEYUP:
		if (pMsg->wParam == VK_CONTROL)
		{
			//AfxMessageBox(TEXT("버튼 왼쪽 업")); 
			key_flag = 0;
		}
		break;
	}

	m_pToolTip->RelayEvent(pMsg);

	return CFormView::PreTranslateMessage(pMsg);
}

int CWeatherView::Show_Tooltip(int m_status, CPoint point) {

		CString tooltip;
		KBJ k;

		int loop = 0;
		int index = 0;
		if ((mode == 1) || (mode == 3)) {
			if (map_status == 0) {
				for (loop = 0; loop < 17; loop++) {
					if (map_rect[0][loop].PtInRect(point)) {		
						index = k.Get_Index_From_Cord(Syncdata(map_rect[0][loop]));
						k.Parse_info_seq_1(k.RemoveChar(k.Get_Weather_From_Url(0, info[index].grid)),info[index]);
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}					 
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 1) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[1][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[1][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 2) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[2][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[2][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 3) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[3][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[3][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 4) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[4][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[4][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 5) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[5][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[5][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0], info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 6) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[6][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[6][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 7) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[7][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[7][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 8) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[8][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[8][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 9) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[9][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[9][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}				
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 10) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[10][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[10][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 11) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[11][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[11][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 12) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[12][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[12][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 13) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[13][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[13][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 14) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[14][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[14][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 15) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[15][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[15][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else if (map_status == 16) {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[16][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[16][loop]));
						tooltip.Format(_T("온도 : %.1f 하늘 %d 강수 : %d \r\n강수확률 : %d 예상강수량 : %.1f 예상적설량 : %.1f \r\n풍속 : %.1f 풍향 : %d \r\n 습도 : %d"), info[index].temp[0].tmp, info[index].sky, info[index].pty, info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
		}
	return 0;
}
town_cord CWeatherView::Syncdata(CRect m_rect) {

	int loop = 0;
	Town_Cord cord = { 0 };
	if (map_status == 0) {
		for (loop = 0; loop < 17; loop++) {
			if (m_rect == map_rect[0][loop]) {
				switch (loop) {
				case 0: cord.do_ = 1; cord.si = 1; cord.dong = 1; break;//서울
				case 1: cord.do_ = 2; cord.si = 1; cord.dong = 0; break;//부산
				case 2:cord.do_ = 3; cord.si = 1; cord.dong = 0; break;//대구
				case 3: cord.do_ = 5; cord.si = 1; cord.dong = 0; break;//광주
				case 4: cord.do_ = 6; cord.si = 1; cord.dong = 0; break;//대전
				case 5: cord.do_ = 10; cord.si = 1; cord.dong = 0; break;//강원도
				case 6: cord.do_ = 17; cord.si = 1; cord.dong = 0; break;//제주
				case 7: cord.do_ = 15; cord.si = 24; cord.dong = 1; break;//울릉도
				case 8: cord.do_ = 9; cord.si = 1; cord.dong = 0; break;//경기도
				case 9: cord.do_ = 4; cord.si = 1; cord.dong = 0; break;//인천
				case 10: cord.do_ = 12; cord.si = 1; cord.dong = 0; break;//충청남도
				case 11: cord.do_ = 11; cord.si = 1; cord.dong = 0; break;//충북
				case 12: cord.do_ = 15; cord.si = 1; cord.dong = 0; break;//경북
				case 13: cord.do_ = 13; cord.si = 1; cord.dong = 0; break;//전북
				case 14: cord.do_ = 16; cord.si = 1; cord.dong = 0; break;//경낭
				case 15: cord.do_ = 7; cord.si = 1;  cord.dong = 0; break;//울산
				case 16: cord.do_ = 14; cord.si = 1; cord.dong = 0; break;//전남
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 1) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[1][loop]) {
				switch (loop) {
				case 0: cord.do_ = 1; cord.si = 23; cord.dong = 3; break;
				case 1: cord.do_ = 1; cord.si = 9; cord.dong = 2; break;
				case 2: cord.do_ = 1; cord.si = 19; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 2) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[2][loop]) {
				switch (loop) {
				case 0: cord.do_ = 9; cord.si = 14; cord.dong = 1; break;
				case 1: cord.do_ = 9; cord.si = 35; cord.dong = 3; break;
				case 2: cord.do_ = 9; cord.si = 32; cord.dong = 2; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 3) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[3][loop]) {
				switch (loop) {
				case 0: cord.do_ = 4; cord.si = 10; cord.dong = 7; break;
				case 1: cord.do_ = 4; cord.si = 6; cord.dong = 1; break;
				case 2: cord.do_ = 4; cord.si = 9; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 4) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[4][loop]) {
				switch (loop) {
				case 0: cord.do_ = 10; cord.si = 1; cord.dong = 1; break;
				case 1: cord.do_ = 10; cord.si = 2; cord.dong = 1; break;
				case 2: cord.do_ = 10; cord.si = 6; cord.dong = 1; break;
				case 3: cord.do_ = 10; cord.si = 7; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 5) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[5][loop]) {
				switch (loop) {
				case 0: cord.do_ = 12; cord.si = 1; cord.dong = 1; break;
				case 1: cord.do_ = 12; cord.si = 6; cord.dong = 1; break;
				case 2: cord.do_ = 12; cord.si = 10; cord.dong = 1; break;
				case 3: cord.do_ = 12; cord.si = 13; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 6) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[6][loop]) {
				switch (loop) {
				case 0: cord.do_ = 6; cord.si = 4; cord.dong = 1; break;
				case 1: cord.do_ = 6; cord.si = 2; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 7) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[7][loop]) {
				switch (loop) {
				case 0: cord.do_ = 11; cord.si = 2; cord.dong = 1; break;
				case 1: cord.do_ = 11; cord.si = 6; cord.dong = 1; break;
				case 2: cord.do_ = 11; cord.si = 9; cord.dong = 1; break;
				case 3: cord.do_ = 11; cord.si = 13; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 8) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[8][loop]) {
				switch (loop) {
				case 0: cord.do_ = 15; cord.si = 3; cord.dong = 1; break;
				case 1: cord.do_ = 15; cord.si = 9; cord.dong = 1; break;
				case 2: cord.do_ = 15; cord.si = 22; cord.dong = 1; break;
				case 3: cord.do_ = 15; cord.si = 24; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 9) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[9][loop]) {
				switch (loop) {
				case 0: cord.do_ = 3; cord.si = 5; cord.dong = 1; break;
				case 1: cord.do_ = 3; cord.si = 7; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 10) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[10][loop]) {
				switch (loop) {				
				case 0: cord.do_ = 13; cord.si = 3; cord.dong = 1; break;
				case 1: cord.do_ = 13; cord.si = 6; cord.dong = 1; break;
				case 2: cord.do_ = 13; cord.si = 13; cord.dong = 1; break;
				case 3: cord.do_ = 13; cord.si = 14; cord.dong = 5; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 11) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[11][loop]) {
				switch (loop) {
				case 0: cord.do_ = 5; cord.si = 4; cord.dong = 1; break;
				case 1: cord.do_ = 5; cord.si = 5; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 12) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[12][loop]) {
				switch (loop) {
				case 0: cord.do_ = 16; cord.si = 5; cord.dong = 1; break;
				case 1: cord.do_ = 16; cord.si = 7; cord.dong = 1; break;
				case 2: cord.do_ = 16; cord.si = 13; cord.dong = 1; break;
				case 3: cord.do_ = 16; cord.si = 16; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 13) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[13][loop]) {
				switch (loop) {
				case 0: cord.do_ = 7; cord.si = 5; cord.dong = 1; break;
				case 1: cord.do_ = 7; cord.si = 3; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 14) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[14][loop]) {
				switch (loop) {
				case 0: cord.do_ = 2; cord.si = 9; cord.dong = 4; break;
				case 1: cord.do_ = 2; cord.si = 12; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 15) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[15][loop]) {
				switch (loop) {
				case 0: cord.do_ = 14; cord.si = 5; cord.dong = 1; break;
				case 1: cord.do_ = 14; cord.si = 6; cord.dong = 1; break;
				case 2: cord.do_ = 14; cord.si = 12; cord.dong = 1; break;
				case 3: cord.do_ = 14; cord.si = 22; cord.dong = 1; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}
	else if (map_status == 16) {
		for (loop = 0; loop < num_towns; loop++) {
			if (m_rect == map_rect[16][loop]) {
				switch (loop) {
				case 0: cord.do_ = 17; cord.si = 1; cord.dong = 3; break;
				case 1: cord.do_ = 17; cord.si = 2; cord.dong = 11; break;
				default: cord = { 0 }; break;
				}
			}
		}
	}

	return cord;
}

