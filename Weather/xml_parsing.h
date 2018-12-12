#pragma once
//INTRODUCTION
//Class for reading weather information from KMA(Korea Meteorological Administration) url
//URL type :  http://www.kma.go.kr/wid/queryDFS.jsp?gridx=66&gridy=109
//This cord operates without any other specific paser like Cmarkup.
//Use KBJ's parser!!

//Written by KBJ
//Wrting Log
//2018/12/01 22:30  ~  2018/12/02 04:25 (6 hours)
//2018/12/03 20:00  ~
//#ifndef _XML_PARSING_
//#define _XML_PARSING_
#include <afxinet.h>
#include <locale.h>

//**** Define ******************************
#define BUFFER_SIZE_XML_HEADER 501
#define LOOP_NUM_XML_HEADER 9
#define BUFFER_SIZE_XML_BODY 512
#define LOOP_NUM_XML_BODY 21


//******************************************

class KBJ
{
	//Variables 
public:


private:


protected:


	//Functions
public:
	weather_info Parse_info_seq_all(weather_info& target);
	CString Get_Weather_From_Url(int sequence, coordinates target_town); //get body information at sequence, sequence 0 = first 3 hours, now.  1 = next 3 hours
	CString RemoveChar(CString &input); //remove characters except for '0'~'9' and '-'
	weather_info Parse_info_seq_1(CString input, weather_info& target); //parse CString returned from RemoveChar() and save at weather_info structure
	weather_info Get_Weather(int sequence, coordinates target_cord, weather_info& target, int main); //target_town is coordinates that we want to get info 
	int Get_Coordinates_From_Dat();
	int Get_Index_From_Cord(town_cord search); //도시동 좌표 구조체를 (예: 1,1,1)을 가지고 어떤 info변수(weather_info, 좌표,도시동코드, 날씨정보 구조체)  
											   //에 저장이 되어있는지 찾는 함수. 사용방법 : info[Get_Index_From_Cord(town_cord a)] 
	int Set_Towns(int map_status);
	int Get_Town_Address_Cord();
	void enroll_weather();
};






//#endif // !_XML_PARSING_
