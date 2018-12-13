// DialogSetting.cpp : 구현 파일입니다.
//
#include "stdafx.h"
#include "Weather.h"
#include "DialogSetting.h"
#include "afxdialogex.h"
#include "xml_parsing.h"

// CDialogSetting 대화 상자입니다.


IMPLEMENT_DYNAMIC(CDialogSetting, CDialogEx)
extern std::vector<address_info> address;
extern u_int fix; // 화면에 고정할 것인지에 대한 flag 변수.
extern u_int user_do;
extern u_int user_si;
extern u_int user_dong; //사용자가 주목하고있는 하나의 특정 도시코드( 시간대별로 나타낼 도시 코드)
extern u_int mode; // 1 = 지도만, 2 = 특정 동네의 시간대별 날씨만, 3 = 둘다  ps. 후에 추가로 위치 배치에 관한 mode가 추가될 수 있음.
extern u_int update; //업데이트 주기. ms
std::vector<CString> addressDo(100);
std::vector<CString> addressSi(100);
std::vector<CString> addressDong(100);
extern int update_flag;
extern int wakeup_thread;
extern CCriticalSection cs;
extern CWinThread* pThread;
extern int POSIT;
extern int map_status;
extern int mode2_x;
extern int mode2_y;

CDialogSetting::CDialogSetting(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_DIALOG1, pParent)
{
	start = 0;
}

CDialogSetting::~CDialogSetting()
{
}

void CDialogSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK1, m_CheckFix);
	DDX_Control(pDX, IDC_COMBO1, m_ComboMode);
	DDX_Control(pDX, IDC_COMBO2, m_ComboDo);
	DDX_Control(pDX, IDC_COMBO3, m_ComboSi);
	DDX_Control(pDX, IDC_COMBO4, m_ComboDong);
	DDX_Control(pDX, IDC_COMBO5, m_ComboUpdate);
}

BEGIN_MESSAGE_MAP(CDialogSetting, CDialogEx)
	ON_WM_PAINT()
	ON_BN_CLICKED(IDOK, &CDialogSetting::OnBnClickedOk)
	ON_CBN_SELCHANGE(IDC_COMBO2, &CDialogSetting::OnCbnSelchangeCombo2)
	ON_CBN_SELCHANGE(IDC_COMBO3, &CDialogSetting::OnCbnSelchangeCombo3)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CDialogSetting 메시지 처리기입니다.

void CDialogSetting::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: 여기에 메시지 처리기 코드를 추가합니다.
					   // 그리기 메시지에 대해서는 CDialogEx::OnPaint()을(를) 호출하지 마십시오.
	CBitmap bitmap;
	bitmap.LoadBitmapW(IDB_BITMAP3);
	BITMAP bmpinfo;
	bitmap.GetBitmap(&bmpinfo);

	CDC dcmem;
	dcmem.CreateCompatibleDC(&dc);
	dcmem.SelectObject(bitmap);

	dc.BitBlt(0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, &dcmem, 0, 0, SRCCOPY);
}

BOOL CDialogSetting::OnInitDialog() //처음 Setting창이 열릴 때 Combo Box에 데이터를 넣는 함수.
{                                   //레지스트리의 값을 읽어와서 ComboBox에 값을 넣고 다시 전역 변수에 값을 return.
	CDialogEx::OnInitDialog();

	// TODO:  여기에 추가 초기화 작업을 추가합니다.

	AfxDialogSetting.m_ComboMode.AddString(_T("Mode1"));
	AfxDialogSetting.m_ComboMode.AddString(_T("Mode2"));
	AfxDialogSetting.m_ComboMode.AddString(_T("Mode3"));

	AfxDialogSetting.m_ComboUpdate.AddString(_T("Update1"));
	AfxDialogSetting.m_ComboUpdate.AddString(_T("Update2"));
	AfxDialogSetting.m_ComboUpdate.AddString(_T("Update3"));

	if (m_DialogDo > 17 || m_DialogDo < 0) m_DialogDo = 0;
	if (m_DialogSi > 100 || m_DialogSi < 0) m_DialogSi = 0;
	//m_DialogSi = 0;
	if (m_DialogDong > 100 || m_DialogDong < 0) m_DialogDong = 0;

	if (start == 1) {
		//KBJ kbj;
		//kbj.Get_Town_Address_Cord();
		if (!addressDo.empty())	addressDo.clear();
		if (!addressSi.empty())	addressSi.clear();
		if (!addressDong.empty())	addressDong.clear();
		//addressSi.clear();
		//addressDong.clear();
	}
	if (start > 1) {
		addressDo.clear();
		addressSi.clear();
		addressDong.clear();

		m_ComboDo.ResetContent();
		m_ComboSi.ResetContent();
		m_ComboDong.ResetContent();
	}

	int numdo = 0;
	CString strDo("");
	addressDo.push_back(address[0].addressDo);
	m_ComboDo.AddString(address[0].addressDo);

	for (int i = 0; i < NUM_OF_ADDRESS; i++) {	//3700개 돌고.
		if (address[i].addressDo != "") {		//빈칸 아니면
			if (addressDo[numdo] != address[i].addressDo) {
				CString old_strDo = address[i].addressDo;	//i번째 string넣음.
				if (strDo != old_strDo) {	//바로 전 주소와 다르면
					AfxDialogSetting.m_ComboDo.AddString(address[i].addressDo);	//콤보박스에 추가
					addressDo.push_back(address[i].addressDo);	//cursel 순서 = 배열 순서. vector에 추가
					numdo++;
				}
				strDo = old_strDo;
			}
		}
	}
	numdo++;
	numDo = numdo;

	addressSi.clear();
	m_ComboSi.ResetContent();
	addressDong.clear();
	m_ComboDong.ResetContent();
	int numsi = 0;
	int DoCurser = m_DialogDo;	//레지스트리에서 읽어 온 Do.
	CString strSi("");
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressSi != "") {
			if (addressDo[DoCurser] == address[i].addressDo) {
				CString old_StrSi = address[i].addressSi;
				if (strSi != old_StrSi) {
					AfxDialogSetting.m_ComboSi.AddString(address[i].addressSi);
					addressSi.push_back(address[i].addressSi);
					numsi++;
				}
				strSi = old_StrSi;
			}
		}
	}
	numSi = numsi;
	int numdong = 0;

	addressDong.clear();
	m_ComboDong.ResetContent();

	CString DostrCurser;
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (addressDo[DoCurser] == address[i].addressDo)
			DostrCurser = addressDo[DoCurser];
	}
	CString SistrCurser;
	int SiCurser = m_DialogSi;
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (addressSi[SiCurser] == address[i].addressSi)
			SistrCurser = addressSi[SiCurser];
	}
	strSi = "";
	CString strDong("");
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressSi != "" && address[i].addressDong != "") {
			if (DostrCurser == address[i].addressDo && SistrCurser == address[i].addressSi) {
				CString old_StrSi = address[i].addressSi;
				CString old_StrDong = address[i].addressDong;
				if (strSi != old_StrSi && strDong != old_StrDong) {
					AfxDialogSetting.m_ComboDong.AddString(address[i].addressDong);
					addressDong.push_back(address[i].addressDong);
					numdong++;
				}
				strDong = old_StrDong;
			}
		}
	}
	numDong = numdong;

	m_CheckFix.SetCheck(AfxDialogSetting.m_DialogFix);
	m_ComboMode.SetCurSel(AfxDialogSetting.m_DialogMode);
	m_ComboDo.SetCurSel(AfxDialogSetting.m_DialogDo);
	m_ComboSi.SetCurSel(AfxDialogSetting.m_DialogSi);
	m_ComboDong.SetCurSel(AfxDialogSetting.m_DialogDong);
	m_ComboUpdate.SetCurSel(AfxDialogSetting.m_DialogUpdate);

	m_ComboDo.GetLBText(m_ComboDo.GetCurSel(), AfxDialogSetting.Dialog_strDo);
	m_ComboSi.GetLBText(m_ComboSi.GetCurSel(), AfxDialogSetting.Dialog_strSi);
	m_ComboDong.GetLBText(m_ComboDong.GetCurSel(), AfxDialogSetting.Dialog_strDong);

	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressDo == Dialog_strDo)
			AfxDialogSetting.Dialog_CordDo = address[i].cord.do_;
		if (address[i].addressSi == Dialog_strSi)
			AfxDialogSetting.Dialog_CordSi = address[i].cord.si;
		if (address[i].addressDong == Dialog_strDong)
			AfxDialogSetting.Dialog_CordDong = address[i].cord.dong;
	}
	mode = AfxDialogSetting.m_DialogMode + 1;
	user_do = AfxDialogSetting.Dialog_CordDo;
	user_si = AfxDialogSetting.Dialog_CordSi;
	user_dong = AfxDialogSetting.Dialog_CordDong;
	update = AfxDialogSetting.m_DialogUpdate + 1;
	fix = AfxDialogSetting.m_DialogFix;
	return TRUE;
	// return TRUE unless you set the focus to a control
	// 예외: OCX 속성 페이지는 FALSE를 반환해야 합니다.
}


void CDialogSetting::OnBnClickedOk()// Setting창에서 OK버튼을 누를 시 전역 변수에 현재 Curser값을 넣고
{								    // Mode에 맞게 윈도우 크기를 조정하거나 투명으로 만드는 함수.
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.
	//ok 버튼 클릭하면 전역변수 및 레지스트리에 저장하는 코드

	AfxDialogSetting.m_DialogFix = m_CheckFix.GetCheck();
	AfxDialogSetting.m_DialogMode = m_ComboMode.GetCurSel();
	AfxDialogSetting.m_DialogDo = m_ComboDo.GetCurSel();
	AfxDialogSetting.m_DialogSi = m_ComboSi.GetCurSel();
	AfxDialogSetting.m_DialogDong = m_ComboDong.GetCurSel();
	AfxDialogSetting.m_DialogUpdate = m_ComboUpdate.GetCurSel();

	m_ComboDo.GetLBText(m_ComboDo.GetCurSel(), AfxDialogSetting.Dialog_strDo);
	m_ComboSi.GetLBText(m_ComboSi.GetCurSel(), AfxDialogSetting.Dialog_strSi);
	m_ComboDong.GetLBText(m_ComboDong.GetCurSel(), AfxDialogSetting.Dialog_strDong);

	for (int i = 0; i < NUM_OF_ADDRESS; i++)
		if (address[i].addressDo == Dialog_strDo)
			AfxDialogSetting.Dialog_CordDo = address[i].cord.do_;
	for (int i = 0; i < NUM_OF_ADDRESS; i++)
		if (address[i].addressSi == Dialog_strSi)
			AfxDialogSetting.Dialog_CordSi = address[i].cord.si;
	for (int i = 0; i < NUM_OF_ADDRESS; i++)
		if (address[i].addressDong == Dialog_strDong)
			AfxDialogSetting.Dialog_CordDong = address[i].cord.dong;
	cs.Lock();
	mode = AfxDialogSetting.m_DialogMode + 1;
	user_do = AfxDialogSetting.Dialog_CordDo;
	user_si = AfxDialogSetting.Dialog_CordSi;
	user_dong = AfxDialogSetting.Dialog_CordDong;
	update = AfxDialogSetting.m_DialogUpdate + 1;
	fix = AfxDialogSetting.m_DialogFix;

	if (fix == 1)
	{
		AfxGetMainWnd()->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
		AfxGetMainWnd()->SetLayeredWindowAttributes(RGB(255, 255, 255), 255, LWA_COLORKEY);
	}
	else if (fix == 0)
	{
		AfxGetMainWnd()->SetLayeredWindowAttributes(0, 255, LWA_ALPHA);
	}
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
	update_flag = 0;
	DWORD nExitCode = NULL;
	GetExitCodeThread(pThread->m_hThread, &nExitCode);
	TerminateThread(pThread->m_hThread, nExitCode);
	pThread = AfxBeginThread(Update_Info_Work_Thread, NULL, THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
	pThread->m_bAutoDelete = FALSE;
	pThread->ResumeThread();
	cs.Unlock();
	CDialogEx::OnOK();
}

void CDialogSetting::OnCbnSelchangeCombo2()	//Combo Box의 도를 선택했을 때 도에 맞는 시를 넣는 함수.
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int numsi = 0;
	addressSi.clear();
	addressDong.clear();
	m_ComboSi.ResetContent();
	m_ComboDong.ResetContent();

	CString strCurser;
	int DoCurser = m_ComboDo.GetCurSel();
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (addressDo[DoCurser] == address[i].addressDo)
			strCurser = addressDo[DoCurser];
	}
	CString strSi("");
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressSi != "") {
			if (strCurser == address[i].addressDo) {
				CString old_StrSi = address[i].addressSi;
				if (strSi != old_StrSi) {
					AfxDialogSetting.m_ComboSi.AddString(address[i].addressSi);
					addressSi.push_back(address[i].addressSi);
					numsi++;
				}
				strSi = old_StrSi;
			}
		}
	}
	numSi = numsi;
}


void CDialogSetting::OnCbnSelchangeCombo3()	//Combo Box의 시를 선택했을 때 시에 맞는 동를 넣는 함수.
{
	// TODO: 여기에 컨트롤 알림 처리기 코드를 추가합니다.

	int numdong = 0, numsi = 0;

	addressDong.clear();
	m_ComboDong.ResetContent();

	CString DostrCurser;
	int DoCurser = m_ComboDo.GetCurSel();
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (addressDo[DoCurser] == address[i].addressDo)
			DostrCurser = addressDo[DoCurser];
	}
	CString SistrCurser;
	int SiCurser = m_ComboSi.GetCurSel();
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (addressSi[SiCurser] == address[i].addressSi)
			SistrCurser = addressSi[SiCurser];
	}
	CString strSi("");
	CString strDong("");
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressSi != "" && address[i].addressDong != "") {
			if (DostrCurser == address[i].addressDo && SistrCurser == address[i].addressSi) {
				CString old_StrSi = address[i].addressSi;
				CString old_StrDong = address[i].addressDong;
				if (strSi != old_StrSi && strDong != old_StrDong) {
					AfxDialogSetting.m_ComboDong.AddString(address[i].addressDong);
					addressDong.push_back(address[i].addressDong);
					numdong++;
				}
				strDong = old_StrDong;
			}
		}
	}
	numDong = numdong;
}

