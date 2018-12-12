
// MainFrm.cpp : CMainFrame Ŭ������ ����
//

#include "stdafx.h"
#include "Weather.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#define WM_TRAY_NOTIFICATION WM_APP + 1	
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification) //�޽����� �޽��� �ڵ鷯�� ���� 
	//ON_UPDATE_COMMAND_UI(ID_INDICATOR_POS, OnUpdatePos)
	//ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	ON_WM_DESTROY()
//	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // ���� �� ǥ�ñ�
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame ����/�Ҹ�

CMainFrame::CMainFrame()
{
	// TODO: ���⿡ ��� �ʱ�ȭ �ڵ带 �߰��մϴ�.
}

CMainFrame::~CMainFrame()
{
}

LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)			//Tray �������� ���� ���� ����� �޽��� �Լ� 
{																				//Tray �������� ���� ���ؼ��� MFC���� �⺻���� �����ϴ� �޽����� �����Ƿ�
																				//���� �޽����� ��������. 
	if ((WM_RBUTTONDOWN) == lParam) {
		CPoint ptMouse;
		::GetCursorPos(&ptMouse);

		CMenu menu;
		menu.LoadMenu(IDR_MAINFRAME);
		CMenu *pMenu = menu.GetSubMenu(0);							//Ȱ��ȭ �� �޴� ����, ���� '0'���ڴ� ���� ��ó�� '1'�� �ι�°..... 
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	    // �������� Popup�������� �������� �������� �ʾ���. 
			ptMouse.x, ptMouse.y, AfxGetMainWnd());
	}
	if (lParam == WM_LBUTTONDBLCLK)
	{
		ShowWindow(SW_SHOW);
	}
	/*
	switch (lParam)
	{
	case WM_RBUTTONDOWN:
	{
	CPoint ptMouse;
	::GetCursorPos(&ptMouse);

	CMenu menu;
	menu.LoadMenu(IDR_MAINFRAME);
	CMenu *pMenu = menu.GetSubMenu(0); //Ȱ��ȭ �� �޴� ����
	pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
	ptMouse.x, ptMouse.y, AfxGetMainWnd());
	if (lParam == WM_LBUTTONDOWN) {

	}

	}
	break;*/
	/*
	case WM_LBUTTONDBLCLK:
	{
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();

	BOOL bRet = ::Shell_NotifyIcon(NIM_DELETE, &nid); //Ʈ���̾����� ����
	if (!bRet)
	{
	AfxMessageBox(_T("Ʈ���� ������ ���� ����"));
	return -1;
	}
	AfxGetApp()->m_pMainWnd->ShowWindow(SW_SHOW); //������ Ȱ��ȭ
	}
	break;*/

	return 1;
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;


	//========================================== Ʈ���� ������ ����� ==============================================================================
	// 1. #define WM_TRAY_NOTIFICATION WM_APP + 1 ���ֱ� 
	// 2. Oncreate�� �Ʒ��� ���� �ڵ� �ۼ� 
	// 3.ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification) �޽����� �޽��� �ڵ鷯�� ���� OnMessage �ۼ� 
	// 4. LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam) �Լ� ���� MainFirm.cpp�� 
	// 5. MainFirm ����� afx_msg LRESULT OnTrayNotification(WPARAM, LPARAM); ���� 
	// 6. Ondestroy���� Ʈ���� ������ ���� 
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;										// uID�� ���� Tray�� ������ �� �ִٰ� ��. 0�� ������ �ƴ϶� ID��
	nid.hWnd = GetSafeHwnd();

	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	// ������ ��� IDR_MAINFRAME �������� ���, �̰� ���� ���� ���� �ְ� ���ͳݿ��� �����͵��� 
														// But ������ ũ�⸦ 32*32��Ʈ�� �����ϴ� ���� ����. 
	lstrcpy(nid.szTip, _T("�� Ʈ����"));				// �̰� ���콺�� ������ ���� �÷������� _T("")�� ����ó�� ���� �� �̰� ���� �̸��� �ٲٸ� ��. 
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;		// TRAY �޽����� ���� ���� uCallbackMessage�� ���. 

	BOOL bRet = ::Shell_NotifyIcon(NIM_ADD, &nid);


	//========================================== Ʈ���� ������ ����� ==============================================================================

	ModifyStyleEx(WS_CAPTION, NULL); //Ÿ��Ʋ�� ���ֱ�

	::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(0, 0, 0), 255, LWA_ALPHA);// | LWA_COLORKEY);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs�� �����Ͽ� ���⿡��
	cs.style = WS_POPUP;
	//cs.style = WS_OVERLAPPEDWINDOW;
	cs.hMenu = NULL;



//================== �ʱ� Ŭ���̾�Ʈ ���� ũ�� ======================
//	cs.style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);                      //ũ��������ɰ� �ִ�ȭ��� ����.
	cs.cx = 1920;				//�̰� �۵� ����. ���ҽ� ���Ͽ��� Dialog Form ũ�⸦ �����ϸ� �װ��� �ʱ�� �����̵�. 
    cs.cy = 1080;
	//===============================================================
	return TRUE;
}

// CMainFrame ����

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame �޽��� ó����



void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
	//=========================== Ʈ���� ������ ���� ===============================
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();
	BOOL bRet = ::Shell_NotifyIcon(NIM_DELETE, &nid);
	//==============================================================================
	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰��մϴ�.
}


//void CMainFrame::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
//{
//	// TODO: ���⿡ �޽��� ó���� �ڵ带 �߰� ��/�Ǵ� �⺻���� ȣ���մϴ�.
//	int a;
//	CFrameWnd::OnKeyDown(nChar, nRepCnt, nFlags);
//}
