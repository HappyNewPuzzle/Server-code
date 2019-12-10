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
	CHAT_RQ,
	CHAT_RP
};

typedef struct Packet
{
	int header;
	int size;
};

typedef struct LOGIN
{
	Packet loginHeader;
};

typedef struct CHAT
{
	Packet chatHeader;
	WCHAR str[10];

};