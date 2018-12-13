
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

//*****************Include & Define****************************************
#include <vector>
#include "DialogSetting.h"
extern CDialogSetting AfxDialogSetting;

#define NUM_OF_COORDINATES 3773  // 나중에 텍스트 파일이 완성되면 완성된 개수로 바꿔야함.
#define NUM_OF_REGIONS 10
#define NUM_SEQUENCE_TO_SHOW 11 //총 몇개의 SEQUENCE( 3시간 단위 )를 가져와 사용자가 원하는 주소의 날씨를 얼마나 긴 시간만큼 보여줄 것인가
#define NUM_OF_ADDRESS 3768 //주소와 도시동 코드를 매치시킬 때 사용하는 데이터의 수,(dat파일의 라인 수 = 동수준의 주소 수 )

//extern u_int fix; // 화면에 고정할 것인지에 대한 flag 변수.
//extern u_int user_do;
//extern u_int user_si;
//extern u_int user_dong; //사용자가 주목하고있는 하나의 특정 도시코드( 시간대별로 나타낼 도시 코드)
//extern u_int mode; // 1 = 지도만, 2 = 특정 동네의 시간대별 날씨만, 3 = 둘다  ps. 후에 추가로 위치 배치에 관한 mode가 추가될 수 있음.
//extern u_int update; //업데이트 주기.
//extern std::vector<weather_info> info(NUM_OF_COORDINATES);
//extern std::vector<CRect> regions(NUM_OF_REGIONS);
//
//
//extern CCriticalSection cs;
//extern int num_towns; //날씨정보 업데이트 스레드와 공유할 전역변수
//extern town_cord* towns; //날씨정보 업데이트 스레드와 공유할 전역변수.
//extern int update_flag = 0; //날씨가 최초 한번이라도 업데이트가 되었다면 flag = 1, 기본값은 0
//
//
//
//
//extern CRect map_rect[17][17];
//extern int map_status;
//
//
////OOOOOOOOOOOOOOOOOOOOOOOOOOOO
////Mode2 관련 변수 : 사용자가 원하는 지역의 상세 정보
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
typedef struct Address_info
{
	CString addressDo;
	CString addressSi;
	CString addressDong;
	town_cord cord;
}address_info;
typedef struct Weather_Info
{
	town_cord cord;  // 주소에서 도, 시, 동 코드(우리가 정하는 것) 
					 //ex) 충청남도 = 1, 천안시 = 1, 병천면 = 1
					 //do_ = 1,     si = 1,    dong = 1
	coordinates grid;   //위도,경도 -> 기상청 좌표 x, y로 변환된 기상청 x,y값.
	u_int day; //Date : 1 = 오늘, 2 = 내일, 3 = 모레	
	std::vector<temp_ts> temp;
	//u_int sky;   //하늘상태코드, 1 = 맑음, 2 = 구름조금, 3 = 구름많음, 4 = 흐림
	//u_int pty;   //강수상태코드, 0 = 없음, 1 = 비, 2 = 비/눈, 3 = 눈/비, 4 = 눈
	std::vector<u_int> sky;
	std::vector<u_int> pty;
	std::vector<u_int> pop; //강수확률 [%]
							//u_int pop;  
	float r6;    //6시간 예상 강수량
	float s6;    //6시간 예상 적설량
	std::vector<float> ws;   //wind speed [m/s], 반올림해서 쓴다.
	u_int wd;   //wind dir, 값 {0~7} = {북,북동,동,남동,남,남서,서,북서}
	std::vector<u_int> reh;  //습도[%]
}weather_info;
//*************************************************************************

#endif







