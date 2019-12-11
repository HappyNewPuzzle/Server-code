#pragma once
/*
Request == 클라 -> 서버에 요청
Reply	== 서버 -> 클라에 응답
Notify	== 서버 -> 클라에 통보 (다른 클라의 행동을 보낼 때)
Command == 서버 -> 클라에 명령 (서버에 의해 클라 수정될 때)

*/
enum Protocal
{
	LOGIN_RQ = 300,
	LOGIN_RP,
	CHAT_STR_DATA
};
//8byte
typedef struct Packet
{
	int header;
	int DataSize;
};

typedef struct Login
{
	Packet loginHeader;
};

typedef struct StrData
{
	WCHAR str[50];
	int strSize;
};

typedef struct ChatStrData
{
	Packet chatHeader;
	StrData chatData;
	ChatStrData()
	{
		chatHeader.header = CHAT_STR_DATA;
		chatHeader.DataSize = sizeof(StrData);
		memset(chatData.str, 0, sizeof(chatData.str));
		chatData.strSize = 0;
	}
};
