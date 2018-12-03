
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

//*************************************************************************
#include <vector>

#define NUM_OF_COORDINATES 11 // ���߿� �ؽ�Ʈ ������ �ϼ��Ǹ� �ϼ��� ������ �ٲ����.
//*************************************************************************

//*************************************************************************
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


//*************************************************************************

//*************************************************************************

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

typedef struct Weather_Info
{
	town_cord cord;  // �ּҿ��� ��, ��, �� �ڵ�(�츮�� ���ϴ� ��) 
					 //ex) ��û���� = 1, õ�Ƚ� = 1, ��õ�� = 1
					 //do_ = 1,     si = 1,    dong = 1
	coordinates grid;   //����,�浵 -> ���û ��ǥ x, y�� ��ȯ�� ���û x,y��.
	u_int day; //Date : 1 = ����, 2 = ����, 3 = ��	
	std::vector<temp_ts> temp;
	u_int sky;   //�ϴû����ڵ�, 1 = ����, 2 = ��������, 3 = ��������, 4 = �帲
	u_int pty;   //���������ڵ�, 0 = ����, 1 = ��, 2 = ��/��, 3 = ��/��, 4 = ��
	u_int pop;   //����Ȯ�� [%]
	float r6;    //6�ð� ���� ������
	float s6;    //6�ð� ���� ������
	float ws;   //wind speed [m/s], �ݿø��ؼ� ����.
	u_int wd;   //wind dir, �� {0~7} = {��,�ϵ�,��,����,��,����,��,�ϼ�}
	u_int reh;  //����[%]
}weather_info;

#endif






