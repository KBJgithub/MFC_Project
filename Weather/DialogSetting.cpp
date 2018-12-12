// DialogSetting.cpp : ���� �����Դϴ�.
//
#include "stdafx.h"
#include "Weather.h"
#include "DialogSetting.h"
#include "afxdialogex.h"
#include "xml_parsing.h"

// CDialogSetting ��ȭ �����Դϴ�.
// 0923

IMPLEMENT_DYNAMIC(CDialogSetting, CDialogEx)
extern std::vector<address_info> address;
extern u_int fix; // ȭ�鿡 ������ �������� ���� flag ����.
extern u_int user_do;
extern u_int user_si;
extern u_int user_dong; //����ڰ� �ָ��ϰ��ִ� �ϳ��� Ư�� �����ڵ�( �ð��뺰�� ��Ÿ�� ���� �ڵ�)
extern u_int mode; // 1 = ������, 2 = Ư�� ������ �ð��뺰 ������, 3 = �Ѵ�  ps. �Ŀ� �߰��� ��ġ ��ġ�� ���� mode�� �߰��� �� ����.
extern u_int update; //������Ʈ �ֱ�. ms
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
	ON_CBN_SELCHANGE(IDC_COMBO4, &CDialogSetting::OnCbnSelchangeCombo4)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(IDC_COMBO5, &CDialogSetting::OnCbnSelchangeCombo5)
END_MESSAGE_MAP()

// CDialogSetting �޽��� ó�����Դϴ�.

void CDialogSetting::OnPaint()
{
	CPaintDC dc(this); // device context for painting
					   // TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
					   // �׸��� �޽����� ���ؼ��� CDialogEx::OnPaint()��(��) ȣ������ ���ʽÿ�.
	CBitmap bitmap;
	bitmap.LoadBitmapW(IDB_BITMAP3);
	BITMAP bmpinfo;
	bitmap.GetBitmap(&bmpinfo);

	CDC dcmem;
	dcmem.CreateCompatibleDC(&dc);
	dcmem.SelectObject(bitmap);

	dc.BitBlt(0, 0, bmpinfo.bmWidth, bmpinfo.bmHeight, &dcmem, 0, 0, SRCCOPY);
}

BOOL CDialogSetting::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  ���⿡ �߰� �ʱ�ȭ �۾��� �߰��մϴ�.

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

	for (int i = 0; i < NUM_OF_ADDRESS; i++) {	//3700�� ����.
		if (address[i].addressDo != "") {		//��ĭ �ƴϸ�
			if (addressDo[numdo] != address[i].addressDo) {
				CString old_strDo = address[i].addressDo;	//i��° string����.
				if (strDo != old_strDo) {	//�ٷ� �� �ּҿ� �ٸ���
					AfxDialogSetting.m_ComboDo.AddString(address[i].addressDo);	//�޺��ڽ��� �߰�
					addressDo.push_back(address[i].addressDo);	//cursel ���� = �迭 ����. vector�� �߰�
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
	int DoCurser = m_DialogDo;	//������Ʈ������ �о� �� Do.
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

	addressDong.clear();
	m_ComboDong.ResetContent();
	int numdong = 0;
	int SiCurser = m_DialogSi;

	CString strDong("");
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressDong != "") {						//��ĭ ����ó��
			if (addressSi[SiCurser] == address[i].addressSi) {	//Ŀ���� str == ���� data
				CString old_StrDong = address[i].addressDong;
				if (strDong != old_StrDong) {					//�ߺ� ����
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
	return TRUE;  // return TRUE unless you set the focus to a control
				  // ����: OCX �Ӽ� �������� FALSE�� ��ȯ�ؾ� �մϴ�.
}


void CDialogSetting::OnBnClickedOk()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
	//ok ��ư Ŭ���ϸ� �������� �� ������Ʈ���� �����ϴ� �ڵ�
	cs.Lock();
	AfxDialogSetting.m_DialogFix = m_CheckFix.GetCheck();			//fix 0 or 1
	AfxDialogSetting.m_DialogMode = m_ComboMode.GetCurSel();	//Mode 1, 2, 3
	AfxDialogSetting.m_DialogDo = m_ComboDo.GetCurSel();
	AfxDialogSetting.m_DialogSi = m_ComboSi.GetCurSel();
	AfxDialogSetting.m_DialogDong = m_ComboDong.GetCurSel();
	AfxDialogSetting.m_DialogUpdate = m_ComboUpdate.GetCurSel();	//1000ms �� ����.
	mode = AfxDialogSetting.m_DialogMode+1;
	user_do = AfxDialogSetting.m_DialogDo + 1;
	user_si = AfxDialogSetting.m_DialogSi ;
	user_dong = AfxDialogSetting.m_DialogDong ;
	update = AfxDialogSetting.m_DialogUpdate + 1;
	fix = AfxDialogSetting.m_DialogFix;

	if (fix == 1)
	{
		//::SetWindowLong((HWND)AfxGetMainWnd(), GWL_EXSTYLE, WS_POPUP);
		AfxGetMainWnd()->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);
		AfxGetMainWnd()->SetLayeredWindowAttributes(RGB(255, 255, 255), 255, LWA_COLORKEY);
	}
	else if (fix == 0)
	{
		//AfxGetMainWnd()->ModifyStyleEx(0, WS_OVERLAPPEDWINDOW, SWP_FRAMECHANGED);
		AfxGetMainWnd()->SetLayeredWindowAttributes(0, 255, LWA_ALPHA);
	}
	KBJ k;
	if (mode == 1)
	{
		AfxGetMainWnd()->MoveWindow(0, 0, 700, 700, 1);
		POSIT = 470;
		k.Set_Towns(map_status);
	}
	else if (mode == 2)
	{
		AfxGetMainWnd()->MoveWindow(0, 0, 920, 327, 1);
		mode2_x = 0;
		mode2_y = 0;
	//	POSIT = 470;
	}
	else if (mode == 3)
	{
		AfxGetMainWnd()->MoveWindow(0, 0, 1600, 700, 1);
		POSIT = 470;
		k.Set_Towns(map_status);
		mode2_x = 600;
		mode2_y = 200;
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

void CDialogSetting::OnCbnSelchangeCombo2()	//Do
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

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


void CDialogSetting::OnCbnSelchangeCombo3()	//Si
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

	int numdong = 0;

	addressDong.clear();
	m_ComboDong.ResetContent();

	int SiCurser = m_ComboSi.GetCurSel();
	CString strDong("");
	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressDong != "") {
			if (addressSi[SiCurser] == address[i].addressSi) {
				CString old_StrDong = address[i].addressDong;
				if (strDong != old_StrDong) {
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

void CDialogSetting::OnCbnSelchangeCombo4()	//Dong
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.

}

void CDialogSetting::OnDestroy()
{
	CDialogEx::OnDestroy();

	AfxDialogSetting.m_DialogFix = m_CheckFix.GetCheck();			//fix 0 or 1
	AfxDialogSetting.m_DialogMode = m_ComboMode.GetCurSel();	//Mode 1, 2, 3
	AfxDialogSetting.m_DialogDo = m_ComboDo.GetCurSel();
	AfxDialogSetting.m_DialogSi = m_ComboSi.GetCurSel();
	AfxDialogSetting.m_DialogDong = m_ComboDong.GetCurSel();
	AfxDialogSetting.m_DialogUpdate = m_ComboUpdate.GetCurSel();	//1000ms �� ����.

	m_ComboDo.GetLBText(m_ComboDo.GetCurSel(), AfxDialogSetting.Dialog_strDo);
	m_ComboSi.GetLBText(m_ComboSi.GetCurSel(), AfxDialogSetting.Dialog_strSi);
	m_ComboDong.GetLBText(m_ComboDong.GetCurSel(), AfxDialogSetting.Dialog_strDong);

	for (int i = 0; i < NUM_OF_ADDRESS; i++) {
		if (address[i].addressDo == Dialog_strDo) {
			if (address[i].addressSi == Dialog_strSi) {
				if (address[i].addressDong == Dialog_strDong) {
					AfxDialogSetting.Dialog_CordDo = address[i].cord.do_;
					AfxDialogSetting.Dialog_CordSi = address[i].cord.si;
					AfxDialogSetting.Dialog_CordDong = address[i].cord.dong;
				}
			}
		}
	}
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


void CDialogSetting::OnCbnSelchangeCombo5()
{
	// TODO: ���⿡ ��Ʈ�� �˸� ó���� �ڵ带 �߰��մϴ�.
}