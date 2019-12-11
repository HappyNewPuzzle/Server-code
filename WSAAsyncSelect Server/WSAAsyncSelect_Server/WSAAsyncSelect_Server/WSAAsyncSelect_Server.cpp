// WSAAsyncSelect_Server.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "WSAAsyncSelect_Server.h"
#include "Protocal.h"
#define MAX_LOADSTRING 100
/////////////// server data //////////////////
#define SERVERPORT 9000
#define BUFSIZE	   512
#define WM_SOCKET (WM_USER+1)
/////////////////////////////////////////////


SOCKET g_sockArray[10];
int g_sockArraySize = 0;
// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.
HWND hEdit;
int g_recive = 0;
int g_strSize = 0;
WCHAR g_str[BUFSIZE];
// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);
void				InitServer(HWND hWnd);
//////////////error ��� �Լ�/////////////////
void DisplayText(LPWSTR fmt, ...);
void err_quit(LPWSTR msg);
void err_display(LPWSTR msg);
void err_display(int errcode);
/////////////////////////////////////////////


//
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: ���⿡ �ڵ带 �Է��մϴ�.

    // ���� ���ڿ��� �ʱ�ȭ�մϴ�.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WSAASYNCSELECT_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WSAASYNCSELECT_SERVER));
	
    MSG msg;
	_wsetlocale(LC_ALL, L"korean");
    // �⺻ �޽��� �����Դϴ�.
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  �Լ�: MyRegisterClass()
//
//  ����: â Ŭ������ ����մϴ�.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WSAASYNCSELECT_SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WSAASYNCSELECT_SERVER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   �Լ�: InitInstance(HINSTANCE, int)
//
//   ����: �ν��Ͻ� �ڵ��� �����ϰ� �� â�� ����ϴ�.
//
//   ����:
//
//        �� �Լ��� ���� �ν��Ͻ� �ڵ��� ���� ������ �����ϰ�
//        �� ���α׷� â�� ���� ���� ǥ���մϴ�.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // �ν��Ͻ� �ڵ��� ���� ������ �����մϴ�.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      1000, 200, 500, 500, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  �Լ�: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ����:  �� â�� �޽����� ó���մϴ�.
//
//  WM_COMMAND  - ���� ���α׷� �޴��� ó���մϴ�.
//  WM_PAINT    - �� â�� �׸��ϴ�.
//  WM_DESTROY  - ���� �޽����� �Խ��ϰ� ��ȯ�մϴ�.
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
	case WM_CREATE:
		hEdit = CreateWindowW(L"edit", NULL,
			WS_CHILD		| WS_VISIBLE	 |
			WS_HSCROLL		| WS_VSCROLL	 | 
			ES_AUTOHSCROLL	| ES_AUTOVSCROLL | ES_MULTILINE ,
			0, 0, 0, 0, hWnd, (HMENU)100, hInst, NULL);
		//���� �ʱ�ȭ �� ���� ���� �� listen() ��������
		InitServer(hWnd);
		return 0;
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	//�ϴܽ��
	case WM_SETFOCUS:
		SetFocus(hEdit);
		return 0;
	case WM_SOCKET:
		ProcessSocketMessage(hWnd, message, wParam, lParam);
		return 0;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // �޴� ������ ���� �м��մϴ�.
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: ���⿡ hdc�� ����ϴ� �׸��� �ڵ带 �߰��մϴ�.
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}


// ���� ��ȭ ������ �޽��� ó�����Դϴ�.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	int addrLen, retval;
	ChatStrData chat;
	Packet header;
	WCHAR Data[100];
	if (WSAGETSELECTERROR(lParam))
	{
		err_display(WSAGETSELECTERROR(lParam));
		return;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_ACCEPT:
	{
			addrLen = sizeof(clientAddr);
			clientSock = accept(wParam, (SOCKADDR*)&clientAddr, &addrLen);
			if (clientSock == INVALID_SOCKET)
			{
				err_display(L"accpt()");
				return;
			}
			DisplayText(L"[TCP ����] : Client ���� \r\n IP �ּ� : %S, ��Ʈ ��ȣ : %d\r\n",
				inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
			if (WSAAsyncSelect(clientSock, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				err_display(L"WSAAsyncSelect(), clientSock");
			}
			g_sockArray[g_sockArraySize++] = clientSock;
		break;
	}
	case FD_READ:
		//������ �ޱ�
		
			DisplayText(L"FD_READ g_strSize \r\n");
			retval = recv(wParam, (char*)&header, sizeof(Packet), 0);
			if (retval == SOCKET_ERROR)
			{
				err_display(L"Packet recv()");
				return;
			}
			switch (header.header)
			{
				case CHAT_STR_DATA:
					StrData strData;
					retval = recv(wParam, (char*)&strData, sizeof(StrData), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display(L"StrData recv()");
						return;
					}
					DisplayText(L"1. %s\r\n", strData.str);
					DisplayText(L"2. %d\r\n", strData.strSize);
					addrLen = sizeof(clientAddr);
					getpeername(wParam, (SOCKADDR*)&clientAddr, &addrLen);
					DisplayText(L"[TCP /%S : %d] %s\r\n",
						inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), strData.str);
					ChatStrData chat;
					chat.chatData = strData;
					for (int i = 0; i < g_sockArraySize; i++)
					{
						DisplayText(L"�����ͺ���\r\n");
						retval = send(g_sockArray[i], (char*)&chat, sizeof(ChatStrData), 0);
						if (retval == SOCKET_ERROR)
						{
							err_display(L"ChatStrData send()");
							return;
						}
					}
					break;
			}
		////������ �ޱ�
		//if (g_recive == 0)
		//{
		//	DisplayText(L"FD_READ g_strSize \r\n");
		//	g_recive = recv(wParam, (char*)&g_strSize, sizeof(int), 0);
		//	if (g_recive == SOCKET_ERROR)
		//	{
		//		err_display(L"recv()");
		//		return;
		//	}
		//	DisplayText(L"1. ������ ���� �ޱ� %d\r\n", g_strSize);
		//}
		//else if (g_recive != 0)
		//{
		//	DisplayText(L"FD_READ g_str \r\n");
		//	retval = recv(wParam, (char*)g_str, g_strSize, 0);
		//	if (retval == SOCKET_ERROR)
		//	{
		//		err_display(L"recv()");
		//		return;
		//	}
		//	DisplayText(L"2. ������ ���� �ޱ�\r\n");
		//	//���� ������ ���
		//	g_str[retval] = '\0';
		//	addrLen = sizeof(clientAddr);
		//	getpeername(wParam, (SOCKADDR*)&clientAddr, &addrLen);
		//	DisplayText(L"[TCP /%S : %d] %s\r\n",
		//		inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), g_str);
		//	g_recive = 0;
		//	for (int i = 0; i < g_sockArraySize; i++)
		//	{
		//		DisplayText(L"�����ͺ���\r\n");
		//		retval = send(g_sockArray[i], (char*)g_str, retval, 0);
		//		if (retval == SOCKET_ERROR)
		//		{
		//			err_display(L"send()");
		//			return;
		//		}
		//	}
		//}
		//break;
	case FD_WRITE:
		DisplayText(L"FD_WRITE �߻�\r\n");
		break;
	case FD_CLOSE:

		break;
	}
}

//
//  �Լ�: InitServer(HWND hWnd)
//
//  ����:  ���� ���� ������ ���õ� ����
//
//  ����: ���� ���� �� �ʱ�ȭ, 
//        ���� ���� �� bind,listen ����, 
//		  WSAAsyncSelect() ����Ͽ� listen_sock ���

void InitServer(HWND hWnd)
{
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)	return;
	//socket ���� AF_INET, SOCK_STREAM
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit(L"sock()");
	//bind()
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	if (bind(listen_sock, (SOCKADDR*)&serveraddr, sizeof(serveraddr)) == SOCKET_ERROR) err_quit(L"bind()");
	//listen()
	if (listen(listen_sock, SOMAXCONN) == SOCKET_ERROR) err_quit(L"listen()");
	//WSAAsyncSelect()
	if (WSAAsyncSelect(listen_sock, hWnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE) == SOCKET_ERROR) err_quit(L"WSAAsyncSelect()");

	DisplayText(L"Server Success!\r\n");
}


void DisplayText(LPWSTR fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	WCHAR cbuf[BUFSIZE + 256];
	vswprintf(cbuf,BUFSIZE+256, fmt, arg);

	int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);

	va_end(arg);
}

void err_quit(LPWSTR msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	MessageBox(NULL, (LPCTSTR)lpMsgBuf, msg, MB_ICONERROR);
	LocalFree(lpMsgBuf);
	exit(1);
}

void err_display(LPWSTR msg)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, WSAGetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText(L"[%s] %s", msg, (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

void err_display(int errcode)
{
	LPVOID lpMsgBuf;
	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
		NULL, errcode,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		(LPTSTR)&lpMsgBuf, 0, NULL);
	DisplayText(L"[����] %s", (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

