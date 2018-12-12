
// stdafx.cpp : ǥ�� ���� ���ϸ� ��� �ִ� �ҽ� �����Դϴ�.
// Weather.pch�� �̸� �����ϵ� ����� �˴ϴ�.
// stdafx.obj���� �̸� �����ϵ� ���� ������ ���Ե˴ϴ�.

#include "stdafx.h"
#include <vector>
#include <algorithm>
#include "xml_parsing.h"




//********************�������� ���� �κ�***********************************
CDialogSetting AfxDialogSetting;
u_int fix = 0; // ȭ�鿡 ������ �������� ���� flag ����.
u_int user_do = 1;
u_int user_si = 1;
u_int user_dong = 1 ; //����ڰ� �ָ��ϰ��ִ� �ϳ��� Ư�� �����ڵ�( �ð��뺰�� ��Ÿ�� ���� �ڵ�)
u_int mode = 3; // 1 = ������, 2 = Ư�� ������ �ð��뺰 ������, 3 = �Ѵ�  ps. �Ŀ� �߰��� ��ġ ��ġ�� ���� mode�� �߰��� �� ����.
u_int update = 3600; //������Ʈ �ֱ�. s
std::vector<weather_info> info(NUM_OF_COORDINATES);
std::vector<CRect> regions(NUM_OF_REGIONS);
CWinThread* pThread;
CCriticalSection cs;
int num_towns; //�������� ������Ʈ ������� ������ ��������
std::vector<town_cord> towns(8); //�������� ������Ʈ ������� ������ ��������.
int update_flag = 0; //������ ���� �ѹ��̶� ������Ʈ�� �Ǿ��ٸ� flag = 1, �⺻���� 0
int animation_status; //�ִϸ��̼� bmp( ���� ������ )�� 3�� �ְ� �� ����(0,1,2)�� ���� bmp�� �ٸ� ���� �����. 
std::vector<address_info> address(NUM_OF_ADDRESS);
int POSIT = 0;
int *index;
int wakeup_thread = 0;
//================================================================================================
//		[0][0] , ù��° [0] : ��ü �������� �� ����				
//               �ι�° [0] : ��, Ư, �����ÿ��� ������ �� ���� 
//      [0] : �������� ( �ι�°�� �Ʒ��� �Ҵ�� ��ȣ�� �ش��ϴ� ������ �簢�� )
//      [1] : ����      [2] : ��⵵      [3] : ��õ       [4] : ������    [5] : �泲 
//      [6] : ����      [7] : ���        [8] : ���ϵ�   [9] : �뱸
//     [10] : ����ϵ� [11] : ����       [12] : ��󳲵�  [13] : ��� 
//     [14] : �λ�     [15] : ���󳲵�   [16] : ���ֵ�    [17] : �ڷΰ��� ��ư
CRect map_rect[17][17];
int map_status = 0;//����
//================================================================================================



//OOOOOOOOOOOOOOOOOOOOOOOOOOOO
//Mode2 ���� ���� : ����ڰ� ���ϴ� ������ �� ����

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
		cs.Lock(); // �����尡 �����ڿ� ������ ������ ������. 
		user.do_ = user_do;
		user.si = user_si;
		user.dong = user_dong;

		index = new int[num_towns + 1]; //�Ѿ�� ��( �����͸� ������Ʈ �ؾ��ϴ� ���õ� �ڵ��)�� ��� �ִ��� �� �ε����� ������ ����
		index[0] = kp.Get_Index_From_Cord(user);

		for (int loop = 0; loop < num_towns; loop++) //��� �ִ��� �ε����� �����ϴ� ����
		{
			index[loop+1] = kp.Get_Index_From_Cord(towns[loop]);
		}
		kp.Get_Weather(0, info[index[0]].grid, info[index[0]], 1); //���� ù�� ° ���õ� �ڵ�� ����ڰ� ���ϴ� �ּ�, �� ��� �ð��� ������ �޾ƿ´�.
		for (int loop = 0; loop < num_towns; loop++)
		{ //������ ���õ� �ڵ���� ù�� ° 3�ð��� ������ �޾ƿ´�. 
			kp.Get_Weather(0, info[index[loop+1]].grid, info[index[loop+1]], 0);
		} 
		AfxGetMainWnd()->Invalidate(1);
		cs.Unlock(); //�����尡 �����ڿ� ������ �Ұ� �ٸ� �Լ����� info, num_town, towns ������ ������ ��������.
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
CToolTipCtrl*   m_pToolTip;  // ������ �ְ�

OnInitDialog(){}   ���� ������ �ְ�

m_pToolTip = new CToolTipCtrl;
m_pToolTip->Create(this, TTS_ALWAYSTIP|TTS_NOPREFIX);
m_pToolTip->SetDelayTime(500);
m_pToolTip->Activate(TRUE);


void OnMouseMove(UINT nFlags, CPoint point){ // ���콺�̵��Լ����� ����� �ְ�
CString strTemp;
strTemp.Format("%5d,%5d",point.x,point.y);
//        strTemp.Format("%5ld",lUnitSize);
m_pToolTip->Activate(TRUE);
m_pToolTip->AddTool(this,strTemp, CRect(point.x-1,point.y-1,point.x+1,point.y+1), TRUE);    // skin[i].rc    �� ��ư���� Rect...
m_pToolTip->Update();


CDialog::OnMouseMove(nFlags, point);
}


BOOL CMy1Dlg::PreTranslateMessage(MSG* pMsg) 
{ // ȭ�鿡 ��� �̺�Ʈ �ɾ��ְ�
m_pToolTip->RelayEvent(pMsg);
return CDialog::PreTranslateMessage(pMsg);
}
[Reference] : WhiteAT, ��C/C++/MFC - ������ �̿��Ͽ� ���콺 ��ġ ��Ÿ���⡹ http://whiteat.com/?mid=WhiteAT_c&document_srl=5771.
*/
