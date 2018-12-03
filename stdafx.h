
// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 및 프로젝트 관련 포함 파일이 
// 들어 있는 포함 파일입니다.

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 일부 CString 생성자는 명시적으로 선언됩니다.

// MFC의 공통 부분과 무시 가능한 경고 메시지에 대한 숨기기를 해제합니다.
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 핵심 및 표준 구성 요소입니다.
#include <afxext.h>         // MFC 확장입니다.





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // Internet Explorer 4 공용 컨트롤에 대한 MFC 지원입니다.
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // Windows 공용 컨트롤에 대한 MFC 지원입니다.
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC의 리본 및 컨트롤 막대 지원

#ifndef _GLOBAL_
#define _GLOBAL_

//*************************************************************************
#include <vector>

#define NUM_OF_COORDINATES 11 // 나중에 텍스트 파일이 완성되면 완성된 개수로 바꿔야함.
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
	float tmp; //temp[x]일 때 ,현재시간 + 3*x 시간의 온도. 
			   //ex) temp[0] = 현재시간이 포함된 3시간, temp[1] = 전체 48시간 중 2번째 시간.
	float tmx; //최고온도,  999.0 = data missing
	float tmn; //최저온도,  999.0 = data missing
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
	town_cord cord;  // 주소에서 도, 시, 동 코드(우리가 정하는 것) 
					 //ex) 충청남도 = 1, 천안시 = 1, 병천면 = 1
					 //do_ = 1,     si = 1,    dong = 1
	coordinates grid;   //위도,경도 -> 기상청 좌표 x, y로 변환된 기상청 x,y값.
	u_int day; //Date : 1 = 오늘, 2 = 내일, 3 = 모레	
	std::vector<temp_ts> temp;
	u_int sky;   //하늘상태코드, 1 = 맑음, 2 = 구름조금, 3 = 구름많음, 4 = 흐림
	u_int pty;   //강수상태코드, 0 = 없음, 1 = 비, 2 = 비/눈, 3 = 눈/비, 4 = 눈
	u_int pop;   //강수확률 [%]
	float r6;    //6시간 예상 강수량
	float s6;    //6시간 예상 적설량
	float ws;   //wind speed [m/s], 반올림해서 쓴다.
	u_int wd;   //wind dir, 값 {0~7} = {북,북동,동,남동,남,남서,서,북서}
	u_int reh;  //습도[%]
}weather_info;

#endif






