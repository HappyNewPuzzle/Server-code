#pragma once
/*
Request == Ŭ�� -> ������ ��û
Reply	== ���� -> Ŭ�� ����
Notify	== ���� -> Ŭ�� �뺸 (�ٸ� Ŭ���� �ൿ�� ���� ��)
Command == ���� -> Ŭ�� ��� (������ ���� Ŭ�� ������ ��)

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
