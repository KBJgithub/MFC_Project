#include <afxinet.h>
#include "stdafx.h"

#include "xml_parsing.h"



CString KBJ::Get_Weather_From_Url(int sequence)
{

	CString str;// Session Buffer
	CString str_xml; //Readed String

	CInternetSession session; //Make session object
	
	//URL address that we will open
	//**NEEDED TO BE ADJUSTED
	CString sURL = _T("http://www.kma.go.kr/wid/queryDFS.jsp?gridx=66&gridy=109");

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
weather_info KBJ::Parse_info(CString input)
{
	weather_info tem;
	temp_ts tem1;
	int left, right;

	left = input.Find('c');
	right = input.Find('d');
	tem.day = _ttoi(input.Mid(left+1, right - left - 1));

	left = right;
	right = input.Find('e');
	tem1.tmp = _ttof(input.Mid(left+1, right - left-1));

	left = right;
	right = input.Find('f');
	tem1.tmx = _ttof(input.Mid(left+1, right - left-1));

	left = right;
	right = input.Find('g');
	tem1.tmn = _ttof(input.Mid(left+1, right - left-1));
	tem.temp.push_back(tem1);

	left = right;
	right = input.Find('h');
	tem.sky = _ttoi(input.Mid(left + 1, right - left - 1));

	left = right;
	right = input.Find('i');
	tem.pty = _ttoi(input.Mid(left + 1, right - left - 1));

	left = input.Find('k');
	right = input.Find('l');
	tem.pop = _ttoi(input.Mid(left + 1, right - left - 1));

	left = input.Find('n');
	right = input.Find('o');
	tem.ws = _ttof(input.Mid(left+3, 3));

	left = right;
	right = input.Find('p');
	tem.wd = _ttoi(input.Mid(left + 1, right - left - 1));

	left = input.Find('r');
	right = input.Find('s');
	tem.reh = _ttoi(input.Mid(left + 1, right - left - 1));

	left = right;
	right = input.Find('t');
	tem.r6 = _ttof(input.Mid(right - 3, 3));

	left = right;
	right = input.Find('u');
	tem.s6 = _ttof(input.Mid(right - 3, 3));
	
	return tem;
}

weather_info KBJ::Get_Weather(int sequence)
{
	return Parse_info(RemoveChar(Get_Weather_From_Url(sequence)));
}
