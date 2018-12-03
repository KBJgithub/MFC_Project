#include <afxinet.h>
#include "stdafx.h"
#include "xml_parsing.h"

extern weather_info info[NUM_OF_COORDINATES];
CString KBJ::Get_Weather_From_Url(int sequence, coordinates target_town)
{

	CString str;// Session Buffer
	CString str_xml; //Readed String
	CInternetSession session; //Make session object
	CString g_x, g_y;
	g_x.Format(_T("%d&gridy="), target_town.gridx);
	g_y.Format(_T("%d"), target_town.gridy);
	CString sURL = _T("http://www.kma.go.kr/wid/queryDFS.jsp?gridx=66&gridy=109");//44 : start gridx
	//sURL.Append(g_x);
	//sURL.Append(g_y);

	try { //error check. once error is occured, jump to catch( errer handling routine )
		CInternetFile *p_xml = (CInternetFile *)session.OpenURL(sURL, 1, INTERNET_FLAG_TRANSFER_ASCII | INTERNET_FLAG_DONT_CACHE);
		if (p_xml != NULL) 
		{
			p_xml->SetReadBufferSize(BUFFER_SIZE_XML_HEADER); //Reading buffer size at once
			for (int loop = 0; loop < LOOP_NUM_XML_HEADER; loop++) //Ignor header 
			{
				p_xml->ReadString(str); 			
			}
			p_xml->SetReadBufferSize(BUFFER_SIZE_XML_BODY); 

			for (int loop1 = 0; loop1 < sequence; loop1++) //Ignor previous parts til meet target sequence
			{
				for (int loop = 0; loop < LOOP_NUM_XML_BODY; loop++)
				{
					p_xml->ReadString(str); //Return 0 if there is no more information to be read	
					if(loop1 == sequence - 1)str_xml += str; //Add readed infomation from url to str_xml		
				}
			}			
			//Closing Routine.
			p_xml->Close(); //Close xml file
			delete p_xml; //Destroy object.			
			session.Close(); //Close session 
			//Closing end.
		}
	}	
	catch (CInternetException *) 
	{
		AfxMessageBox(_T("인터넷에 접속할 수 없습니다.", "ERROR"));
	}
	return str_xml;
}
CString KBJ::RemoveChar(CString &input)
{
	CString output;
	char *cmp;
	cmp = (LPSTR)input.GetBuffer(input.GetLength());
	char point = 'a';
	for (int loop = 0; loop < input.GetLength() * 2; loop++)
	{
		if (('0' <= cmp[loop] && cmp[loop] <= '9') || (cmp[loop] == '.') || (cmp[loop] == '-'))
			output += cmp[loop];
		else if (cmp[loop] == '"' || cmp[loop] == '/')
			output += point++;
	}
	return output;
}
weather_info KBJ::Parse_info(CString input, weather_info& target)
{
	temp_ts tem1;
	int left, right;

	left = input.Find('c');
	right = input.Find('d');
	target.day = _ttoi(input.Mid(left + 1, right - left - 1));

	left = right;
	right = input.Find('e');
	tem1.tmp = _ttof(input.Mid(left + 1, right - left - 1));

	left = right;
	right = input.Find('f');
	tem1.tmx = _ttof(input.Mid(left + 1, right - left - 1));

	left = right;
	right = input.Find('g');
	tem1.tmn = _ttof(input.Mid(left + 1, right - left - 1));
	target.temp.push_back(tem1);

	left = right;
	right = input.Find('h');
	target.sky = _ttoi(input.Mid(left + 1, right - left - 1));

	left = right;
	right = input.Find('i');
	target.pty = _ttoi(input.Mid(left + 1, right - left - 1));

	left = input.Find('k');
	right = input.Find('l');
	target.pop = _ttoi(input.Mid(left + 1, right - left - 1));

	left = input.Find('n');
	right = input.Find('o');
	target.ws = _ttof(input.Mid(left + 3, 3));

	left = right;
	right = input.Find('p');
	target.wd = _ttoi(input.Mid(left + 1, right - left - 1));

	left = input.Find('r');
	right = input.Find('s');
	target.reh = _ttoi(input.Mid(left + 1, right - left - 1));

	left = right;
	right = input.Find('t');
	target.r6 = _ttof(input.Mid(right - 3, 3));

	left = right;
	right = input.Find('u');
	target.s6 = _ttof(input.Mid(right - 3, 3));

	return target;
}
weather_info KBJ::Get_Weather(int sequence, coordinates target_town, weather_info& target)
{
	return Parse_info(RemoveChar(Get_Weather_From_Url(sequence, target_town)), target);
}
int KBJ::Get_Coordinates_From_Dat()
{
	CString dat_name = _T("Coordinates.dat"); //실행파일이 있는곳에 좌측의 이름으로 dat파일이 존재해야함. 
	CStdioFile dat;                           //형식은 엑셀에서 좌표랑 도시동 코드만 복붙 했을 때 나오는 문자열 형식
	CString buffer;                           //ex) 60\t120\t1\t1\t1  
	int cut = 0, cnt = 0;
	dat.Open(dat_name, CFile::modeRead);
	for (int loop = 0; loop < NUM_OF_COORDINATES; loop++)  //NUM_OF_COORDINATES는 전역으로 정의되어있음. 지금은 텍스트 파일에 정보가
	{                                                      //11줄만 있어서 11로 되어있음 모든 주소가 완성되면 그 주소의 개수로 변할 것.
		dat.ReadString(buffer);
		for (int loop1 = 0; loop1 < buffer.GetLength(); loop1++)
		{			
			if (buffer[loop1] == '\t' || buffer[loop] == '\n')
			{
				switch (cnt)
				{
					case 0:info[loop].grid.gridx = _ttoi(buffer.Mid(cut, loop1 - cut)); break;
					case 1:info[loop].grid.gridy = _ttoi(buffer.Mid(cut+1, loop1 - cut)); break;
					case 2:info[loop].cord.do_ = _ttoi(buffer.Mid(cut + 1, loop1 - cut)); break;
					case 3:info[loop].cord.si = _ttoi(buffer.Mid(cut + 1, loop1 - cut));
					       info[loop].cord.dong = _ttoi(buffer.Mid(cut + 1, buffer.GetLength())); break;
				}
				cut = loop1;
				cnt++;
			}
		}
		cut = 0; cnt = 0;
	}
	dat.Close();
	return 0;
}