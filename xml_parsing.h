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
	CString Get_Weather_From_Url(int sequence, coordinates target_town); //get body information at sequence, sequence 1 = first 3 hours, now.  2 = next 3 hours
	CString RemoveChar(CString &input); //remove characters except for '0'~'9' and '-'
	weather_info Parse_info(CString input, weather_info& target); //parse CString returned from RemoveChar() and save at weather_info structure
	weather_info Get_Weather(int sequence, coordinates target_cord, weather_info& target); //target_town is coordinates that we want to get info 
	int KBJ::Get_Coordinates_From_Dat();

};






//#endif // !_XML_PARSING_
