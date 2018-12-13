
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
u_int mode = 1; // 1 = ������, 2 = Ư�� ������ �ð��뺰 ������, 3 = �Ѵ�  ps. �Ŀ� �߰��� ��ġ ��ġ�� ���� mode�� �߰��� �� ����.
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

CRect map_rect[17][17];
int map_status = 0;//����

//OOOOOOOOOOOOOOOOOOOOOOOOOOOO
//Mode2 ���� ���� : ����ڰ� ���ϴ� ������ �� ����

int mode2_x = 100;
int mode2_y = 0;
CTime cTime;
CTime start, now;
//OOOOOOOOOOOOOOOOOOOOOOOOOOOO
POINT moving_images_pos[30];
int moving_images_num = 0;
CWinThread* bmp_Thread;
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

		Sleep(3600*1000*update);
	}	
	return 0;
}