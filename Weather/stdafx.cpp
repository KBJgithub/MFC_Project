
// stdafx.cpp : 표준 포함 파일만 들어 있는 소스 파일입니다.
// Weather.pch는 미리 컴파일된 헤더가 됩니다.
// stdafx.obj에는 미리 컴파일된 형식 정보가 포함됩니다.

#include "stdafx.h"
#include <vector>
#include <algorithm>
#include "xml_parsing.h"




//********************전역변수 선언 부분***********************************
CDialogSetting AfxDialogSetting;
u_int fix = 0; // 화면에 고정할 것인지에 대한 flag 변수.
u_int user_do = 1;
u_int user_si = 1;
u_int user_dong = 1 ; //사용자가 주목하고있는 하나의 특정 도시코드( 시간대별로 나타낼 도시 코드)
u_int mode = 3; // 1 = 지도만, 2 = 특정 동네의 시간대별 날씨만, 3 = 둘다  ps. 후에 추가로 위치 배치에 관한 mode가 추가될 수 있음.
u_int update = 3600; //업데이트 주기. s
std::vector<weather_info> info(NUM_OF_COORDINATES);
std::vector<CRect> regions(NUM_OF_REGIONS);
CWinThread* pThread;
CCriticalSection cs;
int num_towns; //날씨정보 업데이트 스레드와 공유할 전역변수
std::vector<town_cord> towns(8); //날씨정보 업데이트 스레드와 공유할 전역변수.
int update_flag = 0; //날씨가 최초 한번이라도 업데이트가 되었다면 flag = 1, 기본값은 0
int animation_status; //애니메이션 bmp( 날씨 아이콘 )이 3개 있고 각 상태(0,1,2)에 따라서 bmp를 다른 것을 출력함. 
std::vector<address_info> address(NUM_OF_ADDRESS);
int POSIT = 0;
int *index;
int wakeup_thread = 0;
//================================================================================================
//		[0][0] , 첫번째 [0] : 전체 지도에서 각 구역				
//               두번째 [0] : 도, 특, 광역시에서 분할한 각 구역 
//      [0] : 전국지도 ( 두번째는 아래에 할당된 번호에 해당하는 지역의 사각형 )
//      [1] : 서울      [2] : 경기도      [3] : 인천       [4] : 강원도    [5] : 충남 
//      [6] : 대전      [7] : 충북        [8] : 경상북도   [9] : 대구
//     [10] : 전라북도 [11] : 광주       [12] : 경상남도  [13] : 울산 
//     [14] : 부산     [15] : 전라남도   [16] : 제주도    [17] : 뒤로가기 버튼
CRect map_rect[17][17];
int map_status = 0;//전도
//================================================================================================



//OOOOOOOOOOOOOOOOOOOOOOOOOOOO
//Mode2 관련 변수 : 사용자가 원하는 지역의 상세 정보

int mode2_x = 100;
int mode2_y = 0;
CTime cTime;
CTime start, now;
//OOOOOOOOOOOOOOOOOOOOOOOOOOOO

//*************************************************************************

UINT Update_Info_Work_Thread(LPVOID _param)//Work Thread handler to update weather informations
{
	KBJ kp; //Make object to use functions
	town_cord user;
	CString str;
	while (1)
	{
		update_flag = 1;
		cs.Lock(); // 스레드가 공유자원 권한을 가지기 시작함. 
		user.do_ = user_do;
		user.si = user_si;
		user.dong = user_dong;

		index = new int[num_towns + 1]; //넘어온 값( 데이터를 업데이트 해야하는 도시동 코드들)이 어디에 있는지 그 인덱스를 저장할 변수
		index[0] = kp.Get_Index_From_Cord(user);

		for (int loop = 0; loop < num_towns; loop++) //어디에 있는지 인덱스를 저장하는 과정
		{
			index[loop+1] = kp.Get_Index_From_Cord(towns[loop]);
		}
		kp.Get_Weather(0, info[index[0]].grid, info[index[0]], 1); //제일 첫번 째 도시동 코드는 사용자가 원하는 주소, 즉 모든 시간의 날씨를 받아온다.
		for (int loop = 0; loop < num_towns; loop++)
		{ //나머지 도시동 코드들은 첫번 째 3시간의 정보만 받아온다. 
			kp.Get_Weather(0, info[index[loop+1]].grid, info[index[loop+1]], 0);
		} 
		AfxGetMainWnd()->Invalidate(1);
		cs.Unlock(); //스레드가 공유자원 권한을 잃고 다른 함수에서 info, num_town, towns 변수의 수정이 가능해짐.
	//	Wait_My(update);
		Sleep(100000);
	}
	
	return 0;
}

void Wait_My(int delay)
{
	start = CTime::GetCurrentTime();
	while (((now.GetSecond() - start.GetSecond()) != delay ) && wakeup_thread != 1)
	{
		now = CTime::GetCurrentTime();
	}
	wakeup_thread = 0;
}

/*
CToolTipCtrl*   m_pToolTip;  // 선언해 주고

OnInitDialog(){}   에서 생성해 주고

m_pToolTip = new CToolTipCtrl;
m_pToolTip->Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX);
m_pToolTip->SetDelayTime(500);
m_pToolTip->Activate(TRUE);


void OnMouseMove(UINT nFlags, CPoint point){ // 마우스이동함수에서 출력해 주고
CString strTemp;
strTemp.Format("%5d,%5d",point.x,point.y);
//        strTemp.Format("%5ld",lUnitSize);
m_pToolTip->Activate(TRUE);
m_pToolTip->AddTool(this,strTemp, CRect(point.x-1,point.y-1,point.x+1,point.y+1), TRUE);    // skin[i].rc    각 버튼들의 Rect...
m_pToolTip->Update();


CDialog::OnMouseMove(nFlags, point);
}


BOOL CMy1Dlg::PreTranslateMessage(MSG* pMsg) 
{ // 화면에 출력 이벤트 걸어주고
m_pToolTip->RelayEvent(pMsg);
return CDialog::PreTranslateMessage(pMsg);
}
[Reference] : WhiteAT, 「C/C++/MFC - 툴팁을 이용하여 마우스 위치 나타내기」 http://whiteat.com/?mid=WhiteAT_c&document_srl=5771.
*/
