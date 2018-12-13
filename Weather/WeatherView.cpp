
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
extern int animation_status; //�ִϸ��̼� bmp( ���� ������ )�� 3�� �ְ� �� ����(0,1,2)�� ���� bmp�� �ٸ� ���� �����. 
extern int *index;
extern CRect map_rect[17][17];
extern int map_status;
extern u_int fix;
extern int num_towns;
extern std::vector<town_cord> towns; //�������� ������Ʈ ������� ������ ��������.
extern int wakeup_thread;
extern CWinThread* pThread;
extern POINT moving_images_pos[30];
extern CWinThread* bmp_Thread;
extern int moving_images_num;
extern u_int user_do;
extern u_int user_si;
extern u_int user_dong;
extern u_int update;

int cnt_bmp = 0;

#define RATIO 1.27


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
ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CWeatherView ����/�Ҹ�


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
}


BOOL CWeatherView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	//  Window Ŭ���� �Ǵ� ��Ÿ���� �����մϴ�.
	CString strSection = _T("My Section");
	CString str_DialogFix = _T("m_DialogFix");
	CString str_DialogMode = _T("m_DialogMode");
	CString str_DialogDo = _T("m_DialogDo");
	CString str_DialogSi = _T("m_DialogSi");
	CString str_DialogDong = _T("m_DialogDong");
	CString str_DialogUpdate = _T("m_DialogUpdate");
	CString str_DialogDostr = _T("DialogDoStr");
	CString str_DialogDoSistr = _T("DialogSiStr");
	CString str_DialogDongstr = _T("DialogDongStr");
	CString Dialog_CordDo = _T("Dialog_CordDo");
	CString Dialog_CordSi = _T("Dialog_CordSi");
	CString Dialog_CordDong = _T("Dialog_CordDong");

	CWinApp* pApp = AfxGetApp();


	AfxDialogSetting.m_DialogFix = pApp->GetProfileInt(strSection, str_DialogFix, 9999);
	AfxDialogSetting.m_DialogMode = pApp->GetProfileInt(strSection, str_DialogMode, 9999);
	AfxDialogSetting.m_DialogDo = pApp->GetProfileInt(strSection, str_DialogDo, 9999);
	AfxDialogSetting.m_DialogSi = pApp->GetProfileInt(strSection, str_DialogSi, 9999);
	AfxDialogSetting.m_DialogDong = pApp->GetProfileInt(strSection, str_DialogDong, 9999);
	AfxDialogSetting.m_DialogUpdate = pApp->GetProfileInt(strSection, str_DialogUpdate, 9999);
	AfxDialogSetting.Dialog_strDo = pApp->GetProfileString(strSection, str_DialogDostr, _T("error"));
	AfxDialogSetting.Dialog_strSi = pApp->GetProfileString(strSection, str_DialogDoSistr, _T("error"));
	AfxDialogSetting.Dialog_strDong = pApp->GetProfileString(strSection, str_DialogDongstr, _T("error"));
	AfxDialogSetting.m_DialogUpdate = pApp->GetProfileInt(strSection, str_DialogUpdate, 9999);
	AfxDialogSetting.m_DialogUpdate = pApp->GetProfileInt(strSection, str_DialogUpdate, 9999);
	AfxDialogSetting.m_DialogUpdate = pApp->GetProfileInt(strSection, str_DialogUpdate, 9999);
	AfxDialogSetting.Dialog_CordDo = pApp->GetProfileInt(strSection, Dialog_CordDo, 9999);
	AfxDialogSetting.Dialog_CordSi = pApp->GetProfileInt(strSection, Dialog_CordSi, 9999);
	AfxDialogSetting.Dialog_CordDong = pApp->GetProfileInt(strSection, Dialog_CordDong, 9999);

	TRACE(_T("fix = %d\n"), AfxDialogSetting.m_DialogFix);
	TRACE(_T("Mode = %d\n"), AfxDialogSetting.m_DialogMode);
	TRACE(_T("m_DialogDo = %d\n"), AfxDialogSetting.m_DialogDo);
	TRACE(_T("m_DialogSi = %d\n"), AfxDialogSetting.m_DialogSi);
	TRACE(_T("m_DialogDong = %d\n"), AfxDialogSetting.m_DialogDong);
	TRACE(_T("m_DialogUpdate = %d\n"), AfxDialogSetting.m_DialogUpdate);
	TRACE(_T("Dialog_strDo = %s\n"), AfxDialogSetting.Dialog_strDo);
	TRACE(_T("Dialog_strSi = %s\n"), AfxDialogSetting.Dialog_strSi);
	TRACE(_T("Dialog_strDong = %s\n\n\n"), AfxDialogSetting.Dialog_strDong);
	if (
		AfxDialogSetting.m_DialogFix == 9999 ||
		AfxDialogSetting.m_DialogMode == 9999 ||
		AfxDialogSetting.m_DialogDo == 9999 ||
		AfxDialogSetting.m_DialogSi == 9999 ||
		AfxDialogSetting.m_DialogDong == 9999 ||
		AfxDialogSetting.m_DialogUpdate == 9999 ||
		AfxDialogSetting.Dialog_strDo == "error" ||
		AfxDialogSetting.Dialog_strSi == "error" ||
		AfxDialogSetting.Dialog_strDong == "error" ||
		AfxDialogSetting.Dialog_CordDo == 9999 ||
		AfxDialogSetting.Dialog_CordSi == 9999 ||
		AfxDialogSetting.Dialog_CordDong == 9999)
	{
		TRACE(_T("������Ʈ�� �Է� �ȵ� ���"));
		AfxDialogSetting.m_DialogFix = 0;
		AfxDialogSetting.m_DialogMode = 0;
		AfxDialogSetting.m_DialogDo = 0;
		AfxDialogSetting.m_DialogSi = 0;
		AfxDialogSetting.m_DialogDong = 0;
		AfxDialogSetting.Dialog_CordDo = 1;
		AfxDialogSetting.Dialog_CordSi = 1;
		AfxDialogSetting.Dialog_CordDong = 1;
		AfxDialogSetting.m_DialogUpdate = 0;
		AfxDialogSetting.Dialog_strDo = _T("����Ư����");
		AfxDialogSetting.Dialog_strSi = _T("����Ư����");
		AfxDialogSetting.Dialog_strDong = _T("����Ư����");
		AfxDialogSetting.Dialog_CordDo = 1;
		AfxDialogSetting.Dialog_CordSi = 1;
		AfxDialogSetting.Dialog_CordDo = 1;
	}


	mode = AfxDialogSetting.m_DialogMode + 1;
	user_do = AfxDialogSetting.Dialog_CordDo;
	user_si = AfxDialogSetting.Dialog_CordSi;
	user_dong = AfxDialogSetting.Dialog_CordDong;
	update = AfxDialogSetting.m_DialogUpdate + 1;
	AfxDialogSetting.m_DialogFix = 0;
	fix = AfxDialogSetting.m_DialogFix;
	return TRUE;
}

void CWeatherView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	GetParentFrame()->RecalcLayout();
	ResizeParentToFit();


	CPoint point(100, 200);
	//****************************************************
	//���� �ʱ�ȭ
	m_pToolTip = new CToolTipCtrl; 
	m_pToolTip->Create(this, TTS_ALWAYSTIP | TTS_NOPREFIX);
	m_pToolTip->SetDelayTime(500);
	m_pToolTip->SetMaxTipWidth(170);
	m_pToolTip->Activate(FALSE);
	//****************************************************
	//���ϸ��̼ǿ� work thread ����
	bmp_Thread = AfxBeginThread(Moving_Bmp, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	bmp_Thread->m_bAutoDelete = FALSE;
	bmp_Thread->ResumeThread();
	//****************************************************
	//������Ʈ���� �ִ� ���� �Ǵ� �ʱⰪ���� �����ϱ�
	CSize scrollSize(0, 0);
	SetScrollSizes(MM_TEXT, scrollSize);
	KBJ k;

	if (mode == 1)
	{
		AfxGetMainWnd()->MoveWindow(0, 0, 700, 700, 1);
		AfxGetMainWnd()->Invalidate(1);
		POSIT = 470;
		k.Set_Towns(map_status);
	}
	else if (mode == 2)
	{
		AfxGetMainWnd()->MoveWindow(0, 0, 920, 327, 1);
		mode2_x = 0;
		mode2_y = 0;

	}
	else if (mode == 3)
	{
		AfxGetMainWnd()->MoveWindow(0, 0, 1640, 700, 1);
		POSIT = 470;
		k.Set_Towns(map_status);
		mode2_x = 700;
		mode2_y = 220;
	}
	//****************************************************
}


// CWeatherView ����

#ifdef _DEBUG
void CWeatherView::AssertValid() const
{
	CFormView::AssertValid();
}

void CWeatherView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
CWeatherDoc* CWeatherView::GetDocument() const // ����׵��� ���� ������ �ζ������� �����˴ϴ�.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CWeatherDoc)));
	return (CWeatherDoc*)m_pDocument;
}
#endif //_DEBUG


// CWeatherView �޽��� ó����


void CWeatherView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
	 if( mode != 2)
		Print_map(map_status);	//mode2�� �ƴϸ� ������ ����Ѵ�.
	
		if (update_flag == 1) //�����Ͱ� ������Ʈ�� �Ǿ����� 
		{
			cs.Lock();
			if (mode == 1 || mode == 3) //������ �µ��� ����Ѵ�.
			{	
				Show_Temp_Map();
			}
			if (mode == 2 || mode == 3)//����ڰ� �ָ��ϴ� ���� �µ��� ǥ�� ����Ѵ�.
				Show_Town_Main();
			tooltip_flag = 1;
			cs.Unlock();
		}

	if (mode == 1 || mode == 3)	Print_All_Moving_Bmp(dc); //������ �ִϸ��̼��� ����Ѵ�. 
	
				   // �׸��� �޽����� ���ؼ��� CFormView::OnPaint()��(��) ȣ������ ���ʽÿ�.
}

int CWeatherView::Show_Town_Main() //����ڰ� �ָ��ϴ� �ּҸ� ǥ���ϴ� ǥ
{
	CDC *cdc = GetDC();
	int tem;
	int rain;
	cs.Lock();
	if ((mode == 2 || mode == 3))
	{
		CBitmap mode2;
		mode2.LoadBitmap(IDB_NALSSI);
		BITMAP bmpinfo;
		int x_shift,x_dis,y_shift,y_dis, x_shift_bmp, y_shift_bmp,x_dis_bmp;
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
		x_shift_bmp = 90; 
		y_shift_bmp = -70;
		x_dis_bmp = 23;
		font.CreatePointFont(100, _T("�޸ո���ü"));
		cdc->SelectObject(&font);
		cdc->SetTextColor(RGB(100, 100, 100));

		CString strDate, strTime, hour, strDo, strSi, strDong, strInfo; // ��ȯ�Ǵ� ��¥�� �ð��� ������ CString ���� ����

		strDo = AfxDialogSetting.Dialog_strDo;
		strSi = AfxDialogSetting.Dialog_strSi;
		strDong = AfxDialogSetting.Dialog_strDong;
		strDate.Format(_T("%04d�� %02d�� %02d��"), cTime.GetYear(), // ���� �⵵ ��ȯ
			cTime.GetMonth(), // ���� �� ��ȯ
			cTime.GetDay()); // ���� �� ��ȯ
		strTime.Format(_T("%02d�� %02d��"), cTime.GetHour(), // ���� �ð� ��ȯ
			cTime.GetMinute(), // ���� �� ��ȯ
			cTime.GetSecond()); // ���� �� ��ȯ
		strInfo.Format(_T("%s %s %s %s %s"), strDo, strSi, strDong, strDate, strTime);
		cdc->TextOut(mode2_x + 300, mode2_y + 10, strInfo);
		for (int loop = 0; loop < 11; loop++)
		{			
			tem = info[index[0]].sky[loop];			
			rain = info[index[0]].pty[loop];
			if (rain == 0) 
			{
				if (cnt_bmp == 0) 
				{
					if (tem == 1)
					{
						Print_Unvisible_Bmp(mode2_x+ x_shift_bmp + (x_shift-x_dis_bmp)*loop, mode2_y + y_shift+y_shift_bmp, cdc, IDB_SUNNY);
					}
					else if (tem == 2)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop+ x_shift_bmp, mode2_y + y_shift+ y_shift_bmp, cdc, IDB_SCLOUD_1);
					}
					else if (tem == 3)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_MCLOUD_1);

					}
					else if (tem == 4)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_CLOUD_1);
					}
				}
				if (cnt_bmp == 1) {
					if (tem == 1)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SUNNY2);
					}
					else if (tem == 2)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SCLOUD_2);

					}
					else if (tem == 3)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_MCLOUD_2);
					}
					else if (tem == 4)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_CLOUD_2);
					}
				}
				if (cnt_bmp == 2) {
					if (tem == 1)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SUNNY3);
					}
					else if (tem == 2)
					{

						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SCLOUD_3);
						//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
					}
					else if (tem == 3)
					{

						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_MCLOUD3);
						//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
					}
					else if (tem == 4)
					{
						Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_CLOUD_3);
						//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
					}
				}
			}
			else if (rain == 1) 
			{
				if (cnt_bmp == 0) 
				{
					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_RAIN_1);
				}
				if (cnt_bmp == 1) 
				{
					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_RAIN_2);
				}
				if (cnt_bmp == 2) {

					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_RAIN_3);

				}
			}
			else if (rain == 2) {
				if (cnt_bmp == 0) {

					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SNOWRAIN_1);

				}
				if (cnt_bmp == 1) {


					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SNOWRAIN_2);


				}
				if (cnt_bmp == 2) {

					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SNOWRAIN_3);

				}
			}
			else if (rain == 3) {
				if (cnt_bmp == 0) {

					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SNOW_1);

				}
				if (cnt_bmp == 1) {


					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SNOW_2);


				}
				if (cnt_bmp == 2) {

					Print_Unvisible_Bmp(mode2_x + (x_shift - x_dis_bmp)*loop + x_shift_bmp, mode2_y + y_shift + y_shift_bmp, cdc, IDB_SNOW_3);

				}
			}
		
	



			hour.Format(_T("%d��"), (cTime.GetHour() + 3 * loop) % 24);
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


int CWeatherView::Print_map(int map_sta) //�� ��Ȳ�� �´� ���� ���
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
		bitmap.LoadBitmap(IDB_JEONBUK); break;
	case 11:
		bitmap.LoadBitmap(IDB_GWANGJU); break;
	case 12:
		bitmap.LoadBitmap(IDB_GYEONGNAM); break;
	case 13:
		bitmap.LoadBitmap(IDB_ULSAN); break;
	case 14:
		bitmap.LoadBitmap(IDB_BUSAN); break;
	case 15:
		bitmap.LoadBitmap(IDB_JEONNAM); break;
	case 16:
		bitmap.LoadBitmap(IDB_JEJU); break;
	default:
		 break;
	}
	BITMAP bmpinfo;
	bitmap.GetBitmap(&bmpinfo);

	CDC dcmem;
	dcmem.CreateCompatibleDC(&dc);
	dcmem.SelectObject(bitmap);
	dc.StretchBlt(527 - POSIT, 20, (int)(bmpinfo.bmWidth*RATIO), (int)(bmpinfo.bmHeight*RATIO), &dcmem, 0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, SRCCOPY);
	return map_status;
}

int CWeatherView::Map_into(int map_sta) //���콺�� Ŭ������ ���� --> �������� ������ �� ������ �ٲٴ� �Լ�
{
	CRect Back(457 - (mode / 3)*POSIT, 20, 527 - (mode / 3)*POSIT, 90);
	CPoint point;
	::GetCursorPos(&point);                        // ���� ���콺 ������ ��� 
	ScreenToClient(&point);
	CRgn rgn;
	if (map_sta == 0) {
		if (map_rect[0][0].PtInRect(point)) {   // ����
			map_status = 1;
		}
		else if (map_rect[0][8].PtInRect(point) && (!(map_rect[0][1].PtInRect(point) || map_rect[0][9].PtInRect(point)))) {
			map_status = 2;
		}
		else if (map_rect[0][9].PtInRect(point)) {
			map_status = 3;
		}
		else if (map_rect[0][5].PtInRect(point)) {
			map_status = 4;
		}
		else if (map_rect[0][13].PtInRect(point) && (!(map_rect[0][3].PtInRect(point)))) {		//���󳲵� 
			map_status = 15;
		}
		else if (map_rect[0][10].PtInRect(point)) {
			map_status = 5;
		}
		else if (map_rect[0][4].PtInRect(point)) {
			map_status = 6;
		}
		else if (map_rect[0][11].PtInRect(point)) {
			map_status = 7;
		}
		else if (map_rect[0][3].PtInRect(point)) {
			map_status = 11;
		}
		else if (map_rect[0][7].PtInRect(point) || map_rect[0][12].PtInRect(point)) {
			map_status = 8;
		}
		else if (map_rect[0][2].PtInRect(point)) {      //�뱸   
			map_status = 9;
		}
		else if (map_rect[0][16].PtInRect(point)) {      //����ϵ�  
			map_status = 10;
		}
		else if (map_rect[0][14].PtInRect(point)) {
			map_status = 12;
		}
		else if (map_rect[0][15].PtInRect(point)) {
			map_status = 13;
		}
		else if (map_rect[0][1].PtInRect(point)) {
			map_status = 14;
		}
		else if (map_rect[0][6].PtInRect(point)) {
			map_status = 16;
		}
		else return 0;
	}
	KBJ k;
	k.enroll_weather();
	return 1;
}
void CWeatherView::OnLButtonDown(UINT nFlags, CPoint point) //���콺 ���ʹ�ư ���� �� �ڵ鷯
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if (key_flag == 1)
	{
		GetParent()->PostMessage(WM_NCLBUTTONDOWN, HTCAPTION);

	}
	else if (mode == 1 || mode == 3)
	{
		if (Map_into(map_status)) //������ �簢���� �����ϰ� Ŭ��������
		{                         //������Ʈ�� ���Ӱ� �Ѵ�. 
			cs.Lock();
			update_flag = 0;
			tooltip_flag = 0;
			DWORD nExitCode = NULL;
			GetExitCodeThread(pThread->m_hThread, &nExitCode);
			TerminateThread(pThread->m_hThread, nExitCode);
			pThread = AfxBeginThread(Update_Info_Work_Thread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
			pThread->m_bAutoDelete = FALSE;
			pThread->ResumeThread();
			cs.Unlock();
		}
	}	
	CFormView::OnLButtonDown(nFlags, point);
}
void CWeatherView::OnRButtonDown(UINT nFlags, CPoint point) //������ ���콺 ��ư Ŭ�� �ڵ鷯
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.

	if (map_status != 0) //���� ��Ȳ�� ������ �ƴ϶�� ������ �������ɴϴ�. 
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

void CWeatherView::OnMouseMove(UINT nFlags, CPoint point) //���콺 �̵��� ���� ������ ǥ���ϱ� ���� �ڵ鷯
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	if (tooltip_flag == 1)
	{
		Show_Tooltip(map_status, point);	
	}
//============================================================================
	if (map_status == 0) {
		for (int i = 0; i <= 16; i++) {
			if (map_rect[0][i].PtInRect(point)) {
				SetCursor(LoadCursor(NULL, IDC_HAND));
				break;
			}
			else {
				SetCursor(LoadCursor(NULL, IDC_ARROW));
			}
		}
	}
//============================================================================ ���콺 ��
	CFormView::OnMouseMove(nFlags, point);
}

void CWeatherView::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);
}
BOOL CWeatherView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
	CBrush backBrush(RGB(255, 255, 255)); // <- ���Į����. 
	CBrush* pOldBrush = pDC->SelectObject(&backBrush);
	CRect rect; pDC->GetClipBox(&rect);
	pDC->PatBlt(rect.left, rect.top, rect.Width(), rect.Height(), PATCOPY);
	pDC->SelectObject(pOldBrush);
	return TRUE;
}
void CWeatherView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.

}
int CWeatherView::Show_Temp_Map() //���� ���� ������ ǥ���ϴ� �Լ�
{
	int x, y, distance = 40;
	CString output;
	CFont font,m_Font;
	KBJ kb;
	CClientDC dc(this);
	dc.SetBkMode(TRANSPARENT);
	font.CreatePointFont(120, _T("Verdana"));
	m_Font.CreateFont(14,                     // ���ڳ���
		12,                     // ���ڳʺ�
		0,                      // ��°���
		0,                      // ���� �������ǰ���
		FW_BOLD,              // ���ڱ���
		FALSE,                  // Italic ���뿩��
		FALSE,                  // �������뿩��
		FALSE,                  // ��Ҽ����뿩��
		DEFAULT_CHARSET,       // ���ڼ�����
		OUT_DEFAULT_PRECIS,    // ������е�
		CLIP_DEFAULT_PRECIS,   // Ŭ�������е�
		DEFAULT_QUALITY,       // ��¹���ǰ��
		DEFAULT_PITCH,         // �۲�Pitch
		_T("Verdana")           // �۲�
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
	CString str_DialogDostr = _T("DialogDoStr");
	CString str_DialogDoSistr = _T("DialogSiStr");
	CString str_DialogDongstr = _T("DialogDongStr");
	CString Dialog_CordDo = _T("Dialog_CordDo");
	CString Dialog_CordSi = _T("Dialog_CordSi");
	CString Dialog_CordDong = _T("Dialog_CordDong");

	CWinApp* pApp = AfxGetApp();

	pApp->WriteProfileInt(strSection, str_DialogFix, AfxDialogSetting.m_DialogFix);
	pApp->WriteProfileInt(strSection, str_DialogMode, AfxDialogSetting.m_DialogMode);
	pApp->WriteProfileInt(strSection, str_DialogDo, AfxDialogSetting.m_DialogDo);
	pApp->WriteProfileInt(strSection, str_DialogSi, AfxDialogSetting.m_DialogSi);
	pApp->WriteProfileInt(strSection, str_DialogDong, AfxDialogSetting.m_DialogDong);
	pApp->WriteProfileInt(strSection, str_DialogUpdate, AfxDialogSetting.m_DialogUpdate);
	pApp->WriteProfileString(strSection, str_DialogDostr, AfxDialogSetting.Dialog_strDo);
	pApp->WriteProfileString(strSection, str_DialogDoSistr, AfxDialogSetting.Dialog_strSi);
	pApp->WriteProfileString(strSection, str_DialogDongstr, AfxDialogSetting.Dialog_strDong);
	pApp->WriteProfileInt(strSection, Dialog_CordDo, AfxDialogSetting.Dialog_CordDo);
	pApp->WriteProfileInt(strSection, Dialog_CordSi, AfxDialogSetting.Dialog_CordSi);
	pApp->WriteProfileInt(strSection, Dialog_CordDong, AfxDialogSetting.Dialog_CordDong);

	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}

BOOL CWeatherView::PreTranslateMessage(MSG* pMsg) //ctrl + Lbutton���� �����츦 �̵���Ű�� ���� Ű���� �Է� �Լ�
{
	// TODO: ���⿡ Ư��ȭ�� �ڵ带 �߰� ��/�Ǵ� �⺻ Ŭ������ ȣ���մϴ�.
	switch (pMsg->message)
	{
	case WM_KEYDOWN:
		if (pMsg->wParam == VK_CONTROL)
		{
			//AfxMessageBox(TEXT("��ư ���� �ٿ�")); 
			key_flag = 1;
		}
		break;
	case WM_KEYUP:
		if (pMsg->wParam == VK_CONTROL)
		{
			//AfxMessageBox(TEXT("��ư ���� ��")); 
			key_flag = 0;
		}
		break;
	}

	m_pToolTip->RelayEvent(pMsg);

	return CFormView::PreTranslateMessage(pMsg);
}

int CWeatherView::Show_Tooltip(int m_status, CPoint point)//syncdata�Լ����� ���� ������ ���� �����͸� �������� ǥ���ϴ� �Լ�
{
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
						tooltip.Format(_T("�µ� : %.1f �ϴ� %d ���� : %d \r\n����Ȯ�� : %d ���󰭼��� : %.1f ���������� : %.1f \r\nǳ�� : %.1f ǳ�� : %d \r\n ���� : %d"), info[index].temp[0].tmp, info[index].sky[0], info[index].pty[0], info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
						m_pToolTip->AddTool(this, tooltip, NULL, 0);
						m_pToolTip->Activate(TRUE);
						return 1;
					}					 
				}
				m_pToolTip->Activate(FALSE);
				return 0;
			}
			else  /*(map_status == 1)*/ {
				for (loop = 0; loop < num_towns; loop++) {
					if (map_rect[map_status][loop].PtInRect(point)) {
						index = k.Get_Index_From_Cord(Syncdata(map_rect[map_status][loop]));
						tooltip.Format(_T("�µ� : %.1f �ϴ� %d ���� : %d \r\n����Ȯ�� : %d ���󰭼��� : %.1f ���������� : %.1f \r\nǳ�� : %.1f ǳ�� : %d \r\n ���� : %d"), info[index].temp[0].tmp, info[index].sky[0], info[index].pty[0], info[index].pop[0], info[index].r6, info[index].s6, info[index].ws[0], info[index].wd, info[index].reh[0]);
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
town_cord CWeatherView::Syncdata(CRect m_rect)  //����ǥ�ø� ���� ���콺 �����Ͱ� ��ġ�� ���� ���õ� ������ ������ �Լ�
{

	int loop = 0;
	Town_Cord cord = { 0 };
	if (map_status == 0) {
		for (loop = 0; loop < 17; loop++) {
			if (m_rect == map_rect[0][loop]) {
				switch (loop) {
				case 0: cord.do_ = 1; cord.si = 1; cord.dong = 1; break;//����
				case 1: cord.do_ = 2; cord.si = 1; cord.dong = 0; break;//�λ�
				case 2:cord.do_ = 3; cord.si = 1; cord.dong = 0; break;//�뱸
				case 3: cord.do_ = 5; cord.si = 1; cord.dong = 0; break;//����
				case 4: cord.do_ = 6; cord.si = 1; cord.dong = 0; break;//����
				case 5: cord.do_ = 10; cord.si = 1; cord.dong = 0; break;//������
				case 6: cord.do_ = 17; cord.si = 1; cord.dong = 0; break;//����
				case 7: cord.do_ = 15; cord.si = 24; cord.dong = 1; break;//�︪��
				case 8: cord.do_ = 9; cord.si = 1; cord.dong = 0; break;//��⵵
				case 9: cord.do_ = 4; cord.si = 1; cord.dong = 0; break;//��õ
				case 10: cord.do_ = 12; cord.si = 1; cord.dong = 0; break;//��û����
				case 11: cord.do_ = 11; cord.si = 1; cord.dong = 0; break;//���
				case 12: cord.do_ = 15; cord.si = 1; cord.dong = 0; break;//���
				case 13: cord.do_ = 13; cord.si = 1; cord.dong = 0; break;//����
				case 14: cord.do_ = 16; cord.si = 1; cord.dong = 0; break;//�泶
				case 15: cord.do_ = 7; cord.si = 1;  cord.dong = 0; break;//���
				case 16: cord.do_ = 14; cord.si = 1; cord.dong = 0; break;//����
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

UINT Moving_Bmp(LPVOID param) //�ֱ������� ����Ǹ� �ִϸ��̼� ������ ���� 3���� bmp�� � ���� ����ϴ��� �����̼� ���ִ� �Լ� 
{
	while (1)
	{
		if(++cnt_bmp == 3) cnt_bmp = 0;
		AfxGetMainWnd()->Invalidate(1);		
		Sleep(1000);
	}
	return 0;
}
int CWeatherView::Print_All_Moving_Bmp(CPaintDC &dc) //�� ��Ȳ�� �°� ���� �������� ������ ���� �Լ�
{
	int loop;
	KBJ kbj;
	int tem,rain;
	int index = 0;
	int x_shift, y_shift;
	x_shift = 10;
	y_shift = -10;
	if ((mode == 1) || (mode == 3)){
		if (map_status != 0)
		{
			for (loop = 0; loop < num_towns; loop++){

				index = kbj.Get_Index_From_Cord(Syncdata(map_rect[map_status][loop]));
				tem = info[index].sky[0];
				index = kbj.Get_Index_From_Cord(Syncdata(map_rect[map_status][loop]));
				rain = info[index].pty[0];
				if (rain == 0){
					if (cnt_bmp == 0){
						if (tem == 1)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SUNNY);
						}
						else if (tem == 2)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SCLOUD_1);
						}
						else if (tem == 3)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_MCLOUD_1);
						}
						else if (tem == 4)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_CLOUD_1);
						}
					}
					if (cnt_bmp == 1){
						if (tem == 1)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SUNNY);
						}
						else if (tem == 2)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SCLOUD_2);
						}
						else if (tem == 3)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_MCLOUD_2);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
						else if (tem == 4)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_CLOUD_2);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
					}
					if (cnt_bmp == 2){
						if (tem == 1)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SUNNY);
						}
						else if (tem == 2)
						{

							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SCLOUD_3);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
						else if (tem == 3)
						{

							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_MCLOUD3);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
						else if (tem == 4)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
								map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_CLOUD_3);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
					}
				}
				else if (rain == 1){
					if (cnt_bmp == 0){
						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_RAIN_1);
					}
					if (cnt_bmp == 1){


						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_RAIN_2);


					}
					if (cnt_bmp == 2){

						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_RAIN_3);

					}
				}
				else if (rain == 2){
					if (cnt_bmp == 0){

						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SNOWRAIN_1);

					}
					if (cnt_bmp == 1){


						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SNOWRAIN_2);


					}
					if (cnt_bmp == 2){

						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SNOWRAIN_3);

					}
				}
				else if (rain == 3){
					if (cnt_bmp == 0){

						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[0][loop].top + y_shift, dc, IDB_SNOW_1);

					}
					if (cnt_bmp == 1){


						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SNOW_2);


					}
					if (cnt_bmp == 2){

						Print_Unvisible_Bmp(map_rect[map_status][loop].Width() / 2 + map_rect[map_status][loop].left + x_shift,
							map_rect[map_status][loop].Height() / 2 + map_rect[map_status][loop].top + y_shift, dc, IDB_SNOW_3);

					}
				}
			}
		}

		else if (map_status == 0)
		{
			for (loop = 0; loop < num_towns; loop++){
				index = kbj.Get_Index_From_Cord(Syncdata(map_rect[map_status][loop]));
				tem = info[index].sky[0];
				index = kbj.Get_Index_From_Cord(Syncdata(map_rect[map_status][loop]));
				rain = info[index].pty[0];
				if (rain == 0){
					if (cnt_bmp == 0){
						if (tem == 1)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUNNY);
						}
						else if (tem == 2)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SCLOUD_1);
						}
						else if (tem == 3)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_MCLOUD_1);

						}
						else if (tem == 4)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_CLOUD_1);
						}
					}
					if (cnt_bmp == 1){
						if (tem == 1)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUNNY2);
						}
						else if (tem == 2)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SCLOUD_2);

						}
						else if (tem == 3)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_MCLOUD_2);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
						else if (tem == 4)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_CLOUD_2);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
					}
					if (cnt_bmp == 2){
						if (tem == 1)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUNNY3);
						}
						else if (tem == 2)
						{

							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SCLOUD_3);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
						else if (tem == 3)
						{

							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_MCLOUD3);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
						else if (tem == 4)
						{
							Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_CLOUD_3);
							//Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SUN1);
						}
					}
				}
				else if (rain == 1){
					if (cnt_bmp == 0){
						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_RAIN_1);
					}
					if (cnt_bmp == 1){


						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_RAIN_2);


					}
					if (cnt_bmp == 2){

						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_RAIN_3);

					}
				}
				else if (rain == 2){
					if (cnt_bmp == 0){

						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SNOWRAIN_1);

					}
					if (cnt_bmp == 1){


						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SNOWRAIN_2);


					}
					if (cnt_bmp == 2){

						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SNOWRAIN_3);

					}
				}
				else if (rain == 3){
					if (cnt_bmp == 0){
						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SNOW_1);
					}
					if (cnt_bmp == 1){
						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SNOW_2);
					}
					if (cnt_bmp == 2){
						Print_Unvisible_Bmp(map_rect[map_status][loop].left + x_shift, map_rect[map_status][loop].top + y_shift, dc, IDB_SNOW_3);
					}
				}

			}
		}
	}
	return 0;
}


int CWeatherView::Print_Unvisible_Bmp(int x, int y, CPaintDC &dc, int R_id) // ��� ������ ��Ʈ���� ���� �Լ�, ���� dc
{
	HBITMAP old;
	HBITMAP bmp;
	switch (R_id)
	{
		case IDB_SUNNY:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY)); break;
		case IDB_SUNNY2:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY2)); break;
		case IDB_SUNNY3:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY3)); break;
		case IDB_SCLOUD_1:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SCLOUD_1)); break;
		case IDB_SCLOUD_2:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SCLOUD_2)); break;
		case IDB_SCLOUD_3:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SCLOUD_3)); break;
		case IDB_MCLOUD_1:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MCLOUD_1)); break;
		case IDB_MCLOUD_2:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MCLOUD_2)); break;
		case IDB_MCLOUD3:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MCLOUD3)); break;
		case IDB_CLOUD_1:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CLOUD_1)); break;
		case IDB_CLOUD_2:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CLOUD_2)); break;
		case IDB_CLOUD_3:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CLOUD_3)); break;
		case IDB_RAIN_1:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RAIN_1)); break;
		case IDB_RAIN_2:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RAIN_2)); break;
		case IDB_RAIN_3:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RAIN_3)); break;
		case IDB_SNOWRAIN_1:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOWRAIN_1)); break;
		case IDB_SNOWRAIN_2:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOWRAIN_2)); break;
		case IDB_SNOWRAIN_3:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOWRAIN_3)); break;
		case IDB_SNOW_1:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOW_1)); break;
		case IDB_SNOW_2:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOW_2)); break;
		case IDB_SNOW_3:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOW_3)); break;
		default:
			bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY)); break;
	}
	if (bmp != NULL)
	{
		BITMAP bmInfo;
		CDC memDC;
		memDC.CreateCompatibleDC(&dc);
		old = (HBITMAP)::SelectObject(memDC.m_hDC, bmp);
		::GetObject(bmp, sizeof(BITMAP), &bmInfo);
		::TransparentBlt(dc.m_hDC, x, y, bmInfo.bmWidth, bmInfo.bmHeight, memDC.m_hDC,
			 0, 0, bmInfo.bmWidth, bmInfo.bmHeight, RGB(44, 42, 54));
		::SelectObject(memDC.m_hDC, old);
		memDC.DeleteDC();
	}
	return 0;
}

int CWeatherView::Print_Unvisible_Bmp(int x, int y, CDC* &cdc, int R_id)  //���� ��� ��Ʈ���� ����ϴ� �Լ�, ���� cdc
{
	HBITMAP old;
	HBITMAP bmp;
	switch (R_id)
	{
	case IDB_SUNNY:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY)); break;
	case IDB_SUNNY2:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY2)); break;
	case IDB_SUNNY3:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY3)); break;
	case IDB_SCLOUD_1:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SCLOUD_1)); break;
	case IDB_SCLOUD_2:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SCLOUD_2)); break;
	case IDB_SCLOUD_3:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SCLOUD_3)); break;
	case IDB_MCLOUD_1:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MCLOUD_1)); break;
	case IDB_MCLOUD_2:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MCLOUD_2)); break;
	case IDB_MCLOUD3:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_MCLOUD3)); break;
	case IDB_CLOUD_1:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CLOUD_1)); break;
	case IDB_CLOUD_2:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CLOUD_2)); break;
	case IDB_CLOUD_3:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_CLOUD_3)); break;
	case IDB_RAIN_1:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RAIN_1)); break;
	case IDB_RAIN_2:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RAIN_2)); break;
	case IDB_RAIN_3:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_RAIN_3)); break;
	case IDB_SNOWRAIN_1:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOWRAIN_1)); break;
	case IDB_SNOWRAIN_2:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOWRAIN_2)); break;
	case IDB_SNOWRAIN_3:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOWRAIN_3)); break;
	case IDB_SNOW_1:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOW_1)); break;
	case IDB_SNOW_2:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOW_2)); break;
	case IDB_SNOW_3:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SNOW_3)); break;
	default:
		bmp = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_SUNNY)); break;
	}
	if (bmp != NULL)
	{
		BITMAP bmInfo;
		CDC memDC;
		memDC.CreateCompatibleDC(cdc);
		old = (HBITMAP)::SelectObject(memDC.m_hDC, bmp);
		::GetObject(bmp, sizeof(BITMAP), &bmInfo);
		::TransparentBlt(cdc->m_hDC, x, y, bmInfo.bmWidth, bmInfo.bmHeight, memDC.m_hDC,
			0, 0, bmInfo.bmWidth, bmInfo.bmHeight, RGB(44, 42, 54));
		::SelectObject(memDC.m_hDC, old);
		memDC.DeleteDC();
	}
	return 0;
}
