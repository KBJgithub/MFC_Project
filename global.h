#pragma once
//*************************************************************************
#include <vector>
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

//*************************************************************************



//********************전역변수 선언 부분***********************************
//u_int fix; // 화면에 고정할 것인지에 대한 flag 변수.
//u_int user_do;
//u_int user_si;
//u_int user_dong; //사용자가 주목하고있는 하나의 특정 도시코드( 시간대별로 나타낼 도시 코드)
//u_int mode; // 1 = 지도만, 2 = 특정 동네의 시간대별 날씨만, 3 = 둘다  ps. 후에 추가로 위치 배치에 관한 mode가 추가될 수 있음.
//u_int update; //업데이트 주기.
//u_int map;// mode 1, 3에서 현재 지도상황(ex. 한반도 or 서특시 or 충남 or 천안시 )
//*************************************************************************
