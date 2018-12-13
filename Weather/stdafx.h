
// stdafx.h : ���� ��������� ���� ��������� �ʴ�
// ǥ�� �ý��� ���� ���� �� ������Ʈ ���� ���� ������ 
// ��� �ִ� ���� �����Դϴ�.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // �Ϻ� CString �����ڴ� ��������� ����˴ϴ�.

// MFC�� ���� �κа� ���� ������ ��� �޽����� ���� ����⸦ �����մϴ�.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC �ٽ� �� ǥ�� ���� ����Դϴ�.
#include <afxext.h>         // MFC Ȯ���Դϴ�.





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows ���� ��Ʈ�ѿ� ���� MFC �����Դϴ�.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC�� ���� �� ��Ʈ�� ���� ����




#ifndef _GLOBAL_
#define _GLOBAL_

//*****************Include & Define****************************************
#include <vector>
#include "DialogSetting.h"
extern CDialogSetting AfxDialogSetting;

#define NUM_OF_COORDINATES 3773  // ���߿� �ؽ�Ʈ ������ �ϼ��Ǹ� �ϼ��� ������ �ٲ����.
#define NUM_OF_REGIONS 10
#define NUM_SEQUENCE_TO_SHOW 11 //�� ��� SEQUENCE( 3�ð� ���� )�� ������ ����ڰ� ���ϴ� �ּ��� ������ �󸶳� �� �ð���ŭ ������ ���ΰ�
#define NUM_OF_ADDRESS 3768 //�ּҿ� ���õ� �ڵ带 ��ġ��ų �� ����ϴ� �������� ��,(dat������ ���� �� = �������� �ּ� �� )

//extern u_int fix; // ȭ�鿡 ������ �������� ���� flag ����.
//extern u_int user_do;
//extern u_int user_si;
//extern u_int user_dong; //����ڰ� �ָ��ϰ��ִ� �ϳ��� Ư�� �����ڵ�( �ð��뺰�� ��Ÿ�� ���� �ڵ�)
//extern u_int mode; // 1 = ������, 2 = Ư�� ������ �ð��뺰 ������, 3 = �Ѵ�  ps. �Ŀ� �߰��� ��ġ ��ġ�� ���� mode�� �߰��� �� ����.
//extern u_int update; //������Ʈ �ֱ�.
//extern std::vector<weather_info> info(NUM_OF_COORDINATES);
//extern std::vector<CRect> regions(NUM_OF_REGIONS);
//
//
//extern CCriticalSection cs;
//extern int num_towns; //�������� ������Ʈ ������� ������ ��������
//extern town_cord* towns; //�������� ������Ʈ ������� ������ ��������.
//extern int update_flag = 0; //������ ���� �ѹ��̶� ������Ʈ�� �Ǿ��ٸ� flag = 1, �⺻���� 0
//
//
//
//
//extern CRect map_rect[17][17];
//extern int map_status;
//
//
////OOOOOOOOOOOOOOOOOOOOOOOOOOOO
////Mode2 ���� ���� : ����ڰ� ���ϴ� ������ �� ����
//
//extern int mode2_x;
//extern int mode2_y;
//extern int mode2_width;
//extern int mode2_height;
////OOOOOOOOOOOOOOOOOOOOOOOOOOOO




typedef unsigned int u_int;
typedef unsigned char u_char;
typedef unsigned long u_long;
typedef unsigned short u_short;

typedef unsigned int u_32;
typedef unsigned char u_8;
typedef unsigned short u_16;

typedef int s_32;
typedef char s_8;
typedef short s_16;


//***************** Define Global Function****************************************
UINT Update_Info_Work_Thread(LPVOID param);
void Wait_My(int);
//UINT Image_Print_Work_Thread(LPVOID param);
//*************************************************************************


//*****************Define Structure******************************************

typedef struct Temp_TimeSlot
{
	float tmp; //temp[x]�� �� ,����ð� + 3*x �ð��� �µ�. 
			   //ex) temp[0] = ����ð��� ���Ե� 3�ð�, temp[1] = ��ü 48�ð� �� 2��° �ð�.
	float tmx; //�ְ�µ�,  999.0 = data missing
	float tmn; //�����µ�,  999.0 = data missing
}temp_ts;

typedef struct Coordinates
{
	u_int gridx, gridy;
}coordinates;

typedef struct Town_Cord
{
	u_int do_, si, dong;
}town_cord;
typedef struct Address_info
{
	CString addressDo;
	CString addressSi;
	CString addressDong;
	town_cord cord;
}address_info;
typedef struct Weather_Info
{
	town_cord cord;  // �ּҿ��� ��, ��, �� �ڵ�(�츮�� ���ϴ� ��) 
					 //ex) ��û���� = 1, õ�Ƚ� = 1, ��õ�� = 1
					 //do_ = 1,     si = 1,    dong = 1
	coordinates grid;   //����,�浵 -> ���û ��ǥ x, y�� ��ȯ�� ���û x,y��.
	u_int day; //Date : 1 = ����, 2 = ����, 3 = ��	
	std::vector<temp_ts> temp;
	//u_int sky;   //�ϴû����ڵ�, 1 = ����, 2 = ��������, 3 = ��������, 4 = �帲
	//u_int pty;   //���������ڵ�, 0 = ����, 1 = ��, 2 = ��/��, 3 = ��/��, 4 = ��
	std::vector<u_int> sky;
	std::vector<u_int> pty;
	std::vector<u_int> pop; //����Ȯ�� [%]
							//u_int pop;  
	float r6;    //6�ð� ���� ������
	float s6;    //6�ð� ���� ������
	std::vector<float> ws;   //wind speed [m/s], �ݿø��ؼ� ����.
	u_int wd;   //wind dir, �� {0~7} = {��,�ϵ�,��,����,��,����,��,�ϼ�}
	std::vector<u_int> reh;  //����[%]
}weather_info;
//*************************************************************************

#endif







