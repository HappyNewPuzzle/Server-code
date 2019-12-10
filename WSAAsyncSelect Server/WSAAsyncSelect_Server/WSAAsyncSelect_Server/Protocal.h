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