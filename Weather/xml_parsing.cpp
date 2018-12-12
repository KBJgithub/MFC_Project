#include <afxinet.h>
#include <locale.h>
#include "stdafx.h"
#include "xml_parsing.h"

extern std::vector<weather_info> info;
extern CTime cTime; 
extern int map_status;
extern int num_towns; //날씨정보 업데이트 스레드와 공유할 전역변수
extern std::vector<town_cord> towns; //날씨정보 업데이트 스레드와 공유할 전역변수.
extern CRect map_rect[17][17];
extern u_int mode;
extern std::vector<address_info> address;
extern int POSIT;
CString KBJ::Get_Weather_From_Url(int sequence, coordinates target_town)
{

	CString str;// Session Buffer
	CString str_xml; //Readed String
	CInternetSession session; //Make session object
	CString g_x, g_y;
	g_x.Format(_T("%d&gridy="), target_town.gridx);
	g_y.Format(_T("%d"), target_town.gridy);
	CString sURL = _T("http://www.kma.go.kr/wid/queryDFS.jsp?gridx");//=66&gridy=109");//44 : start gridx
	sURL.Append(g_x);
	sURL.Append(g_y);

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

			for (int loop1 = 0; loop1 < sequence+1; loop1++) //Ignor previous parts til meet target sequence
			{
				for (int loop = 0; loop < LOOP_NUM_XML_BODY; loop++)
				{
					p_xml->ReadString(str); //Return 0 if there is no more information to be read	
					if(loop1 == sequence)str_xml += str; //Add readed infomation from url to str_xml		
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
weather_info KBJ::Parse_info_seq_1(CString input, weather_info& target)
{
	temp_ts tem1;
	int left, right, cnt = 0;
	CString tem;
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
	target.pop.push_back( _ttoi(input.Mid(left + 1, right - left - 1)));

	left = input.Find('n');
	right = input.Find('o');
	target.ws.push_back(_ttof(input.Mid(left + 3, 3)));

	left = right;
	right = input.Find('p');
	target.wd = _ttoi(input.Mid(left + 1, right - left - 1));

	left = input.Find('r');
	right = input.Find('s');
	target.reh.push_back(_ttoi(input.Mid(left + 1, right - left - 1)));

	left = right;
	right = input.Find('t');
	target.r6 = _ttof(input.Mid(right - 3, 3));

	left = right;
	right = input.Find('u');
	target.s6 = _ttof(input.Mid(right - 3, 3));
	while ((target.temp[0].tmx == -999) && cnt != 15 )
	{
		cnt++;
		tem = RemoveChar(Get_Weather_From_Url(cnt, target.grid));
		left = tem.Find('e');
		right = tem.Find('f');
		target.temp[0].tmx = _ttof(tem.Mid(left + 1, right - left - 1));
	}
	cnt = 0;
	while ((target.temp[0].tmn == -999) && cnt != 15)
	{
		cnt++;
		tem = RemoveChar(Get_Weather_From_Url(cnt, target.grid));
		left = tem.Find('f'); 
		right = tem.Find('g');
		target.temp[0].tmn = _ttof(tem.Mid(left+1 , right - left - 1));
		//target.temp.push_back(tem1);
	}

		

	return target;
}
weather_info KBJ::Parse_info_seq_all(weather_info& target)
{
	temp_ts tem1;
	int left, right;
	float tem_min, tem_max;
	CString input;
	Parse_info_seq_1(RemoveChar(Get_Weather_From_Url(0, target.grid)), target);
	for(int loop = 1; loop < NUM_SEQUENCE_TO_SHOW; loop++)
	{
		input = RemoveChar(Get_Weather_From_Url(loop, target.grid));
		left = input.Find('d');
		right = input.Find('e');
		tem1.tmp = _ttof(input.Mid(left + 1, right - left - 1));

		left = right;
		right = input.Find('f');
		tem1.tmx = _ttof(input.Mid(left + 1, right - left - 1));
		
		left = right;
		right = input.Find('g');
		tem1.tmn = _ttof(input.Mid(left + 1, right - left - 1));
		target.temp.push_back(tem1);

		left = input.Find('k');
		right = input.Find('l');
		target.pop.push_back(_ttoi(input.Mid(left + 1, right - left - 1)));

		left = input.Find('n');
		right = input.Find('o');
		target.ws.push_back(_ttof(input.Mid(left + 3, 3)));

		left = input.Find('r');
		right = input.Find('s');
		target.reh.push_back(_ttoi(input.Mid(left + 1, right - left - 1)));
	}
	for (int loop = 0; loop < 11; loop++)
	{
		if (target.temp[loop].tmn != -999)
			tem_min = target.temp[loop].tmn;
		if (target.temp[loop].tmx != -999)
			tem_max = target.temp[loop].tmx;
	}
	for (int loop = 0; loop < 11; loop++)
	{
		if (target.temp[loop].tmn == -999)
			target.temp[loop].tmn = tem_min;
		if (target.temp[loop].tmx == -999)
			target.temp[loop].tmx = tem_max;
	}
	return target;
}
weather_info KBJ::Get_Weather(int sequence, coordinates target_town, weather_info& target, int main) //main == 1일 때 target은 24시간의 날씨정보를 받아옴.
{                                                                                                    //main = 0일 때, target에 첫번 째 3시간의 정보만 저장됨)
	if (main == 1) return Parse_info_seq_all(target);
	return Parse_info_seq_1(RemoveChar(Get_Weather_From_Url(sequence, target_town)), target);
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
			if (buffer[loop1] == '\t' || buffer[loop1] == '\n')
			{
				switch (cnt)
				{
					case 0:info[loop].grid.gridx = _ttoi(buffer.Mid(cut, loop1 - cut)); break;
					case 1:info[loop].grid.gridy = _ttoi(buffer.Mid(cut+1, loop1 - cut)); break;
					case 2:info[loop].cord.do_ = _ttoi(buffer.Mid(cut + 1, loop1 - cut)); break;
					case 3:info[loop].cord.si = _ttoi(buffer.Mid(cut + 1, loop1 - cut));
					       info[loop].cord.dong = _ttoi(buffer.Mid(cut + 3, buffer.GetLength())); break;
					default: break;
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
int KBJ::Get_Index_From_Cord(town_cord search)
{
	int cnt = 0;
	for (int loop = 0; loop < NUM_OF_COORDINATES; loop++)
	{			
		if ((info[loop].cord.dong == search.dong)&&
			(info[loop].cord.do_ == search.do_)&&
			(info[loop].cord.si  == search.si)) break;
		else cnt++;
	}
	return cnt;
}
//		[0][0] , 첫번째 [0] : 전체 지도에서 각 구역				
//               두번째 [0] : 도, 특, 광역시에서 분할한 각 구역 
//      [0] : 전국지도 ( 두번째는 아래에 할당된 번호에 해당하는 지역의 사각형 )
//      [1] : 서울      [2] : 경기도      [3] : 인천       [4] : 강원도    [5] : 충남 
//      [6] : 대전      [7] : 충북        [8] : 경상북도   [9] : 대구
//     [10] : 전라북도 [11] : 광주       [12] : 경상남도  [13] : 울산 
//     [14] : 부산     [15] : 전라남도   [16] : 제주도    [17] : 뒤로가기 버튼
int KBJ::Set_Towns(int map_status)
{
	switch (map_status)
	{
	case 0://전도
		map_rect[0][0].SetRect(602 - ((mode == 3) || (mode == 1))*POSIT, 153, 640 - ((mode == 3) || (mode == 1))*POSIT, 184);      //서울
		map_rect[0][1].SetRect(832 - ((mode == 3) || (mode == 1))*POSIT, 470, 900 - ((mode == 3) || (mode == 1))*POSIT, 506);   // 부산
		map_rect[0][2].SetRect(775 - ((mode == 3) || (mode == 1))*POSIT, 384, 817 - ((mode == 3) || (mode == 1))*POSIT, 426);      // 대구
		map_rect[0][3].SetRect(587 - ((mode == 3) || (mode == 1))*POSIT, 481, 632 - ((mode == 3) || (mode == 1))*POSIT, 507);   // 광주
		map_rect[0][4].SetRect(652 - ((mode == 3) || (mode == 1))*POSIT, 313, 681 - ((mode == 3) || (mode == 1))*POSIT, 352);      // 대전
		map_rect[0][5].SetRect(709 - ((mode == 3) || (mode == 1))*POSIT, 85, 863 - ((mode == 3) || (mode == 1))*POSIT, 207);      // 강원도
		map_rect[0][6].SetRect(532 - ((mode == 3) || (mode == 1))*POSIT, 645, 628 - ((mode == 3) || (mode == 1))*POSIT, 691);   // 제주
		map_rect[0][7].SetRect(933 - ((mode == 3) || (mode == 1))*POSIT, 226, 977 - ((mode == 3) || (mode == 1))*POSIT, 246);   //울릉도
		map_rect[0][8].SetRect(541 - ((mode == 3) || (mode == 1))*POSIT, 61, 677 - ((mode == 3) || (mode == 1))*POSIT, 228);      //경기
		map_rect[0][9].SetRect(568 - ((mode == 3) || (mode == 1))*POSIT, 156, 601 - ((mode == 3) || (mode == 1))*POSIT, 194);      // 인천
		map_rect[0][10].SetRect(539 - ((mode == 3) || (mode == 1))*POSIT, 254, 649 - ((mode == 3) || (mode == 1))*POSIT, 363);      // 충남
		map_rect[0][11].SetRect(682 - ((mode == 3) || (mode == 1))*POSIT, 236, 729 - ((mode == 3) || (mode == 1))*POSIT, 366);      // 충북
		map_rect[0][12].SetRect(755 - ((mode == 3) || (mode == 1))*POSIT, 240, 910 - ((mode == 3) || (mode == 1))*POSIT, 382);      // 경북
		map_rect[0][13].SetRect(531 - ((mode == 3) || (mode == 1))*POSIT, 474, 689 - ((mode == 3) || (mode == 1))*POSIT, 585);   // 전북
		map_rect[0][14].SetRect(702 - ((mode == 3) || (mode == 1))*POSIT, 429, 827 - ((mode == 3) || (mode == 1))*POSIT, 549);   // 경남
		map_rect[0][15].SetRect(852 - ((mode == 3) || (mode == 1))*POSIT, 417, 901 - ((mode == 3) || (mode == 1))*POSIT, 460);   // 울산
		map_rect[0][16].SetRect(557 - ((mode == 3) || (mode == 1))*POSIT, 382, 686 - ((mode == 3) || (mode == 1))*POSIT, 462);   // 전남
		break;
	case 1:
		//num_towns = 3;                           //서울
		map_rect[1][0].SetRect(733 - ((mode == 3) || (mode == 1))*POSIT, 96, 1065 - ((mode == 3) || (mode == 1))*POSIT, 322);
		map_rect[1][1].SetRect(841 - ((mode == 3) || (mode == 1))*POSIT, 343, 1221 - ((mode == 3) || (mode == 1))*POSIT, 615);
		map_rect[1][2].SetRect(552 - ((mode == 3) || (mode == 1))*POSIT, 339, 803 - ((mode == 3) || (mode == 1))*POSIT, 602);
		break;
	case 2:
		//num_towns = 3;                           //경기
		map_rect[2][0].SetRect(564 - ((mode == 3) || (mode == 1))*POSIT, 124, 922 - ((mode == 3) || (mode == 1))*POSIT, 336);
		map_rect[2][1].SetRect(783 - ((mode == 3) || (mode == 1))*POSIT, 355, 994 - ((mode == 3) || (mode == 1))*POSIT, 593);
		map_rect[2][2].SetRect(612 - ((mode == 3) || (mode == 1))*POSIT, 358, 773 - ((mode == 3) || (mode == 1))*POSIT, 597);

		break;
	case 3:
		//num_towns = 3;                           //인천
		map_rect[3][0].SetRect(564 - ((mode == 3) || (mode == 1))*POSIT, 147, 826 - ((mode == 3) || (mode == 1))*POSIT, 347);
		map_rect[3][1].SetRect(877 - ((mode == 3) || (mode == 1))*POSIT, 323, 1012 - ((mode == 3) || (mode == 1))*POSIT, 519);
		map_rect[3][2].SetRect(573 - ((mode == 3) || (mode == 1))*POSIT, 382, 840 - ((mode == 3) || (mode == 1))*POSIT, 550);
		break;
	case 4:
		//	num_towns = 4;                           //강원도
		map_rect[4][0].SetRect(537 - ((mode == 3) || (mode == 1))*POSIT, 161, 812 - ((mode == 3) || (mode == 1))*POSIT, 420);
		map_rect[4][1].SetRect(831 - ((mode == 3) || (mode == 1))*POSIT, 122, 1015 - ((mode == 3) || (mode == 1))*POSIT, 364);
		map_rect[4][2].SetRect(967 - ((mode == 3) || (mode == 1))*POSIT, 403, 1178 - ((mode == 3) || (mode == 1))*POSIT, 622);
		map_rect[4][3].SetRect(740 - ((mode == 3) || (mode == 1))*POSIT, 438, 945 - ((mode == 3) || (mode == 1))*POSIT, 627);
		break;
	case 5:
		//	num_towns = 4;                           //충남
		map_rect[5][0].SetRect(560 - ((mode == 3) || (mode == 1))*POSIT, 199, 774 - ((mode == 3) || (mode == 1))*POSIT, 473);
		map_rect[5][1].SetRect(781 - ((mode == 3) || (mode == 1))*POSIT, 110, 1058 - ((mode == 3) || (mode == 1))*POSIT, 288);
		map_rect[5][2].SetRect(786 - ((mode == 3) || (mode == 1))*POSIT, 308, 967 - ((mode == 3) || (mode == 1))*POSIT, 554);
		map_rect[5][3].SetRect(981 - ((mode == 3) || (mode == 1))*POSIT, 311, 1186 - ((mode == 3) || (mode == 1))*POSIT, 597);

		break;
	case 6:
		//	num_towns = 2;                           //대전
		map_rect[6][0].SetRect(592 - ((mode == 3) || (mode == 1))*POSIT, 151, 951 - ((mode == 3) || (mode == 1))*POSIT, 327);
		map_rect[6][1].SetRect(594 - ((mode == 3) || (mode == 1))*POSIT, 341, 911 - ((mode == 3) || (mode == 1))*POSIT, 535);
		break;
	case 7:
		//	num_towns = 4;                           //충북
		map_rect[7][0].SetRect(577 - ((mode == 3) || (mode == 1))*POSIT, 148, 772 - ((mode == 3) || (mode == 1))*POSIT, 292);
		map_rect[7][1].SetRect(783 - ((mode == 3) || (mode == 1))*POSIT, 132, 993 - ((mode == 3) || (mode == 1))*POSIT, 291);
		map_rect[7][2].SetRect(570 - ((mode == 3) || (mode == 1))*POSIT, 303, 802 - ((mode == 3) || (mode == 1))*POSIT, 440);
		map_rect[7][3].SetRect(620 - ((mode == 3) || (mode == 1))*POSIT, 457, 826 - ((mode == 3) || (mode == 1))*POSIT, 605);
		break;
	case 8:
		//	num_towns = 4;                           //경상북도
		map_rect[8][0].SetRect(553 - ((mode == 3) || (mode == 1))*POSIT, 241, 701 - ((mode == 3) || (mode == 1))*POSIT, 548);
		map_rect[8][1].SetRect(717 - ((mode == 3) || (mode == 1))*POSIT, 139, 976 - ((mode == 3) || (mode == 1))*POSIT, 330);
		map_rect[8][2].SetRect(743 - ((mode == 3) || (mode == 1))*POSIT, 345, 1028 - ((mode == 3) || (mode == 1))*POSIT, 596);
		map_rect[8][3].SetRect(1021 - ((mode == 3) || (mode == 1))*POSIT, 102, 1175 - ((mode == 3) || (mode == 1))*POSIT, 213);
		break;
	case 9:
		//	num_towns = 2;                           //대구
		map_rect[9][0].SetRect(584 - ((mode == 3) || (mode == 1))*POSIT, 120, 945 - ((mode == 3) || (mode == 1))*POSIT, 314);
		map_rect[9][1].SetRect(588 - ((mode == 3) || (mode == 1))*POSIT, 325, 922 - ((mode == 3) || (mode == 1))*POSIT, 576);
		break;
	case 10:
		//	num_towns = 4;                           //전라남도
		map_rect[10][0].SetRect(587 - ((mode == 3) || (mode == 1))*POSIT, 140, 1121 - ((mode == 3) || (mode == 1))*POSIT, 272);
		map_rect[10][1].SetRect(567 - ((mode == 3) || (mode == 1))*POSIT, 305, 737 - ((mode == 3) || (mode == 1))*POSIT, 535);
		map_rect[10][2].SetRect(756 - ((mode == 3) || (mode == 1))*POSIT, 296, 912 - ((mode == 3) || (mode == 1))*POSIT, 564);
		map_rect[10][3].SetRect(920 - ((mode == 3) || (mode == 1))*POSIT, 286, 1103 - ((mode == 3) || (mode == 1))*POSIT, 570);
		break;
	case 11:
		//	num_towns = 2;                           //광주
		map_rect[11][0].SetRect(540 - ((mode == 3) || (mode == 1))*POSIT, 156, 844 - ((mode == 3) || (mode == 1))*POSIT, 534);
		map_rect[11][1].SetRect(853 - ((mode == 3) || (mode == 1))*POSIT, 155, 1106 - ((mode == 3) || (mode == 1))*POSIT, 528);
		break;
	case 12:
		//	num_towns = 4;                           //경상남도
		map_rect[12][0].SetRect(544 - ((mode == 3) || (mode == 1))*POSIT, 186, 678 - ((mode == 3) || (mode == 1))*POSIT, 613);
		map_rect[12][1].SetRect(690 - ((mode == 3) || (mode == 1))*POSIT, 157, 935 - ((mode == 3) || (mode == 1))*POSIT, 362);
		map_rect[12][2].SetRect(690 - ((mode == 3) || (mode == 1))*POSIT, 384, 937 - ((mode == 3) || (mode == 1))*POSIT, 611);
		map_rect[12][3].SetRect(949 - ((mode == 3) || (mode == 1))*POSIT, 216, 1114 - ((mode == 3) || (mode == 1))*POSIT, 433);
		break;
	case 13:
		//	num_towns = 2;                           //울산
		map_rect[13][0].SetRect(525 - ((mode == 3) || (mode == 1))*POSIT, 163, 849 - ((mode == 3) || (mode == 1))*POSIT, 596);
		map_rect[13][1].SetRect(859 - ((mode == 3) || (mode == 1))*POSIT, 161, 1060 - ((mode == 3) || (mode == 1))*POSIT, 597);
		break;
	case 14:
		//	num_towns = 2;                           //부산
		map_rect[14][0].SetRect(541 - ((mode == 3) || (mode == 1))*POSIT, 308, 815 - ((mode == 3) || (mode == 1))*POSIT, 610);//사상구
		map_rect[14][1].SetRect(839 - ((mode == 3) || (mode == 1))*POSIT, 129, 1035 - ((mode == 3) || (mode == 1))*POSIT, 425);//기장군
		break;
	case 15:
		//	num_towns = 4;                           //전라북도
		map_rect[15][0].SetRect(564 - ((mode == 3) || (mode == 1))*POSIT, 253, 757 - ((mode == 3) || (mode == 1))*POSIT, 570);//목포
		map_rect[15][1].SetRect(773 - ((mode == 3) || (mode == 1))*POSIT, 146, 999 - ((mode == 3) || (mode == 1))*POSIT, 323);//담양
		map_rect[15][2].SetRect(773 - ((mode == 3) || (mode == 1))*POSIT, 363, 995 - ((mode == 3) || (mode == 1))*POSIT, 582);//장흥
		map_rect[15][3].SetRect(1012 - ((mode == 3) || (mode == 1))*POSIT, 189, 1145 - ((mode == 3) || (mode == 1))*POSIT, 240);//광양
		break;
	case 16:
		//	num_towns = 4;                           //제주도
		map_rect[16][0].SetRect(526 - ((mode == 3) || (mode == 1))*POSIT, 252, 699 - ((mode == 3) || (mode == 1))*POSIT, 564); //한경명
																															   //	map_rect[16][1].SetRect(712 -((mode == 3)||(mode == 1))*POSIT, 251, 839 -((mode == 3)||(mode == 1))*POSIT, 562); //안덕면
		map_rect[16][1].SetRect(848 - ((mode == 3) || (mode == 1))*POSIT, 245, 963 - ((mode == 3) || (mode == 1))*POSIT, 554); //성산읍
																															   //	map_rect[16][3].SetRect(971 -((mode == 3)||(mode == 1))*POSIT, 237, 1128 -((mode == 3)||(mode == 1))*POSIT, 554);//우도
		break;
		//   case 17:
		//      num_towns = 4;
		//      break;
	default:AfxMessageBox(_T("Map Status Error")); break;
	}
	return 0;
}
int KBJ::Get_Town_Address_Cord()
{

	CString dat_name = _T("Address.dat"); //실행파일이 있는곳에 좌측의 이름으로 dat파일이 존재해야함. 
	CStdioFile dat;                           //형식은 엑셀에서 좌표랑 도시동 코드만 복붙 했을 때 나오는 문자열 형식
	CString buffer;                           //ex) 60\t120\t1\t1\t1  
	
	int cut = 0, cnt = 0;
	char *pOldLocale = setlocale(LC_ALL, "Korean");
	dat.Open(dat_name, CFile::modeRead);
	for (int loop = 0; loop < NUM_OF_ADDRESS; loop++)  //NUM_OF_COORDINATES는 전역으로 정의되어있음. 지금은 텍스트 파일에 정보가
	{                                                      //11줄만 있어서 11로 되어있음 모든 주소가 완성되면 그 주소의 개수로 변할 것.
		dat.ReadString(buffer);
		for (int loop1 = 0; loop1 < buffer.GetLength(); loop1++)
		{
			if (buffer[loop1] == '\t' || buffer[loop1] == '\n')
			{
				switch (cnt)
				{
					//Ascii_To_Unicode(buffer.Mid(cut, loop1 - cut), address[loop].address); break;//
				case 0:address[loop].addressDo.Append(buffer.Mid(cut, loop1 - cut));  break;// = _ttoi(buffer.Mid(cut, loop1 - cut)); break;
				case 1:address[loop].addressSi.Append(buffer.Mid(cut + 1, loop1 - cut-1));  break;
				case 2:address[loop].addressDong.Append(buffer.Mid(cut + 1, loop1 - cut-1));  break;
				case 3:address[loop].cord.do_ = _ttoi(buffer.Mid(cut + 1, loop1 - cut)); break;
				case 4:address[loop].cord.si = _ttoi(buffer.Mid(cut + 1, loop1 - cut));
					address[loop].cord.dong = _ttoi(buffer.Mid(cut + 3, buffer.GetLength())); break;
				default: break;
				}
				cut = loop1;
				cnt++;
			}
		}
		cut = 0; cnt = 0;
	}
	dat.Close();
	setlocale(LC_ALL, pOldLocale);
	return 0;
}
void KBJ::enroll_weather()                                    // 도시동 코드 로딩. 
{
	if (map_status == 0)
	{//전도
		num_towns = 8; //날씨를 표현할 개수
		towns[0].do_ = 1; towns[0].si = 1; towns[0].dong = 1;  // 서울
		towns[1].do_ = 2; towns[1].si = 1; towns[1].dong = 0;  // 부산
		towns[2].do_ = 3; towns[2].si = 1; towns[2].dong = 0;  // 대구
		towns[3].do_ = 5; towns[3].si = 1; towns[3].dong = 0;  // 광주
		towns[4].do_ = 6; towns[4].si = 1; towns[4].dong = 0;  // 대전
		towns[5].do_ = 10; towns[5].si = 1; towns[5].dong = 0; // 강원도
		towns[6].do_ = 17; towns[6].si = 1; towns[6].dong = 0; // 제주
		towns[7].do_ = 15; towns[7].si = 24; towns[7].dong = 1;  // 울릉도
	}
	else if (map_status == 1)
	{//서울시
		num_towns = 3; //날씨를 표현할 개수
		towns[0].do_ = 1; towns[0].si = 23; towns[0].dong = 3;  //강남
		towns[1].do_ = 1; towns[1].si = 9; towns[1].dong = 2; //강북
		towns[2].do_ = 1; towns[2].si = 19; towns[2].dong = 1; //영등포
	}
	else if (map_status == 2) {//경기도 
		num_towns = 3; //날씨를 표현할 개수
		towns[0].do_ = 9; towns[0].si = 14; towns[0].dong = 1;  //동두천
		towns[1].do_ = 9; towns[1].si = 32; towns[1].dong = 2; //이천시
		towns[2].do_ = 9; towns[2].si = 35; towns[2].dong = 3; //화성시
	}
	else if (map_status == 3) {//인천광역시 
		num_towns = 3; //날씨를 표현할 개수
		towns[0].do_ = 4; towns[0].si = 10; towns[0].dong = 7;  //옹진군
		towns[1].do_ = 4; towns[1].si = 6; towns[1].dong = 1; //부평시
		towns[2].do_ = 4; towns[2].si = 9; towns[2].dong = 1; //화성시

	}
	else if (map_status == 4) {//강원도 
		num_towns = 4; //날씨를 표현할 개수
		towns[0].do_ = 10; towns[0].si = 1; towns[0].dong = 1;  //춘천시
		towns[1].do_ = 10; towns[1].si = 2; towns[1].dong = 1; //원주시
		towns[2].do_ = 10; towns[2].si = 6; towns[2].dong = 1; //속초시
		towns[3].do_ = 10; towns[3].si = 7; towns[3].dong = 1; //삼척시
	}
	else if (map_status == 5) {//충남 
		num_towns = 4; //날씨를 표현할 개수
		towns[0].do_ = 12; towns[0].si = 1; towns[0].dong = 1;  //천안시
		towns[1].do_ = 12; towns[1].si = 6; towns[1].dong = 1; //서산
		towns[2].do_ = 12; towns[2].si = 10; towns[2].dong = 1; //금산군
		towns[3].do_ = 12; towns[3].si = 13; towns[3].dong = 1; //청양군
	}
	else if (map_status == 6) {//대전 
		num_towns = 2; //날씨를 표현할 개수
		towns[0].do_ = 6; towns[0].si = 4; towns[0].dong = 1;  //유성구
		towns[1].do_ = 6; towns[1].si = 2; towns[1].dong = 1; //중구
	}
	else if (map_status == 7) {//충북
		num_towns = 4; //날씨를 표현할 개수
		towns[0].do_ = 11; towns[0].si = 2; towns[0].dong = 1;  //청주시
		towns[1].do_ = 11; towns[1].si = 6; towns[1].dong = 1; //제천시
		towns[2].do_ = 11; towns[2].si = 9; towns[2].dong = 1;  //영동군
		towns[3].do_ = 11; towns[3].si = 13; towns[3].dong = 1; //음성군
	}
	else if (map_status == 8) {//경상북도
		num_towns = 4; //날씨를 표현할 개수
		towns[0].do_ = 15; towns[0].si = 3; towns[0].dong = 1;  //경주시
		towns[1].do_ = 15; towns[1].si = 9; towns[1].dong = 1; //상주시
		towns[2].do_ = 15; towns[2].si = 22; towns[2].dong = 1;  //봉화군
		towns[3].do_ = 15; towns[3].si = 24; towns[3].dong = 1; //울릉군
	}
	else if (map_status == 9) {//대구
		num_towns = 2; //날씨를 표현할 개수
		towns[0].do_ = 3; towns[0].si = 5; towns[0].dong = 1;  //북구
		towns[1].do_ = 3; towns[1].si = 7; towns[1].dong = 1; //달성군

	}
	else if (map_status == 10) {//전라북도
		num_towns = 4; //날씨를 표현할 개수
		towns[0].do_ = 13; towns[0].si = 3; towns[0].dong = 1;  //군산시
		towns[1].do_ = 13; towns[1].si = 6; towns[1].dong = 1; //남원시
		towns[2].do_ = 13; towns[2].si = 13; towns[2].dong = 1;  //순창군
		towns[3].do_ = 13; towns[3].si = 14; towns[3].dong = 5; //고창군
	}

	else if (map_status == 11) {//광주
		num_towns = 2; //날씨를 표현할 개수
		towns[0].do_ = 5; towns[0].si = 4; towns[0].dong = 1;  //북구
		towns[1].do_ = 5; towns[1].si = 5; towns[1].dong = 1; //광산구
	}
	else if (map_status == 12) {//경상남도
		num_towns = 4; //날씨를 표현할 개수
		towns[0].do_ = 16; towns[0].si = 5; towns[0].dong = 1;  //통영시
		towns[1].do_ = 16; towns[1].si = 7; towns[1].dong = 1; //김해시
		towns[2].do_ = 16; towns[2].si = 13; towns[2].dong = 1;  //창년군
		towns[3].do_ = 16; towns[3].si = 16; towns[3].dong = 1; //하동군
	}

	else if (map_status == 13) {//울산
		num_towns = 2; //날씨를 표현할 개수
		towns[0].do_ = 7; towns[0].si = 3; towns[0].dong = 1;  //동구
		towns[1].do_ = 7; towns[1].si = 5; towns[1].dong = 1; //을주군
	}

	else if (map_status == 14) {//부산
		num_towns = 2; //날씨를 표현할 개수
		towns[0].do_ = 2; towns[0].si = 9; towns[0].dong = 4;  //해운대
		towns[1].do_ = 2; towns[1].si = 12; towns[1].dong = 1; //강서구
	}
	else if (map_status == 15) {//전라남도
		num_towns = 4; //날씨를 표현할 개수
		towns[0].do_ = 14; towns[0].si = 5; towns[0].dong = 1;  //광양시
		towns[1].do_ = 14; towns[1].si = 6; towns[1].dong = 1; //담양군
		towns[2].do_ = 14; towns[2].si = 12; towns[2].dong = 1;  //장흥군
		towns[3].do_ = 14; towns[3].si = 22; towns[3].dong = 1; //신안군
	}

	else if (map_status == 16) {//제주도
		num_towns = 2; //날씨를 표현할 개수
		towns[0].do_ = 17; towns[0].si = 1; towns[0].dong = 3;  //한경면
		towns[1].do_ = 17; towns[1].si = 2; towns[1].dong = 11; //성산읍
	}
}