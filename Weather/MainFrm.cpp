
// MainFrm.cpp : CMainFrame 클래스의 구현
//

#include "stdafx.h"
#include "Weather.h"

#include "MainFrm.h"


#define WM_TRAY_NOTIFICATION WM_APP + 1	
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_MESSAGE(WM_TRAY_NOTIFICATION, OnTrayNotification) //메시지와 메시지 핸들러를 연결 
	ON_WM_DESTROY()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // 상태 줄 표시기
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame 생성/소멸

CMainFrame::CMainFrame()
{
	// TODO: 여기에 멤버 초기화 코드를 추가합니다.
}

CMainFrame::~CMainFrame()
{
}

LRESULT CMainFrame::OnTrayNotification(WPARAM wParam, LPARAM lParam)			//Tray 아이콘을 띄우기 위한 사용자 메시지 함수 
{																				//Tray 아이콘을 띄우기 위해서는 MFC에서 기본으로 제공하는 메시지가 없으므로
																				//직접 메시지를 만들어야함. 
	if ((WM_RBUTTONDOWN) == lParam) {
		CPoint ptMouse;
		::GetCursorPos(&ptMouse);

		CMenu menu;
		menu.LoadMenu(IDR_MAINFRAME);
		CMenu *pMenu = menu.GetSubMenu(0);							//활성화 할 메뉴 지정, 옆의 '0'숫자는 가장 맨처음 '1'은 두번째..... 
		pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,	    // 여러개를 Popup가능한지 보았으나 가능하지 않았음. 
			ptMouse.x, ptMouse.y, AfxGetMainWnd());
	}
	if (lParam == WM_LBUTTONDBLCLK)
	{
		ShowWindow(SW_SHOW);
	}
	return 1;
}
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;										// uID로 여러 Tray를 설정할 수 있다고 함. 0은 개수가 아니라 ID임
	nid.hWnd = GetSafeHwnd();
	nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
	nid.hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);	// 아이콘 등록 IDR_MAINFRAME 아이콘을 사용, 이건 내가 만들 수도 있고 인터넷에서 베껴와도됨 
														// But 아이콘 크기를 32*32비트로 조정하는 것이 좋음. 
	lstrcpy(nid.szTip, _T("내 트레이"));				// 이건 마우스를 아이콘 위에 올려놓으면 _T("")에 툴팁처럼 말이 뜸 이것 또한 이름을 바꾸면 됨. 
	nid.uCallbackMessage = WM_TRAY_NOTIFICATION;		// TRAY 메시지를 띄우기 위해 uCallbackMessage를 사용. 
	BOOL bRet = ::Shell_NotifyIcon(NIM_ADD, &nid);


	//========================================== 트레이 아이콘 선언부 ==============================================================================

	ModifyStyleEx(WS_CAPTION, NULL); //타이틀바 없애기

	::SetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE, ::GetWindowLong(this->GetSafeHwnd(), GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(RGB(0, 0, 0), 255, LWA_ALPHA);// | LWA_COLORKEY);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	cs.style = WS_POPUP;
	//cs.style = WS_OVERLAPPEDWINDOW;
	cs.hMenu = NULL;



//================== 초기 클라이언트 영역 크기 ======================
//	cs.style &= ~(WS_THICKFRAME | WS_MAXIMIZEBOX);                      //크기조절기능과 최대화기능 제거.
	cs.cx = 1920;				//이거 작동 안함. 리소스 파일에서 Dialog Form 크기를 조절하면 그것이 초기로 설정이됨. 
    cs.cy = 1080;
	//===============================================================
	return TRUE;
}

// CMainFrame 진단

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


// CMainFrame 메시지 처리기



void CMainFrame::OnDestroy()
{
	CFrameWnd::OnDestroy();
	//=========================== 트레이 아이콘 제거 ===============================
	NOTIFYICONDATA nid;
	ZeroMemory(&nid, sizeof(nid));
	nid.cbSize = sizeof(nid);
	nid.uID = 0;
	nid.hWnd = GetSafeHwnd();
	BOOL bRet = ::Shell_NotifyIcon(NIM_DELETE, &nid);
	//==============================================================================
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
}

