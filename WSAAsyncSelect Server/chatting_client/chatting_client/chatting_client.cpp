// chatting_client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "chatting_client.h"
#include "Protocal.h"
#define MAX_LOADSTRING 100

#define ID_LOGIN_ID_EDIT 200
#define ID_LOGIN_PASSWORD_EDIT 201
#define ID_LOGIN_BUTTON 202
#define ID_CHAT_WRITE_EDIT 203
#define ID_CHAT_READ_LISTBOX 204
#define ID_CHAT_SEND_BUTTON 205
/////////////// server data //////////////////
#define SERVERIP	"127.0.0.1"
#define SERVERPORT	9000
#define BUFSIZE		512
#define WM_SOCKET (WM_USER+1)
/////////////////////////////////////////////


// ���� ����:
HINSTANCE hInst;                                // ���� �ν��Ͻ��Դϴ�.
WCHAR szTitle[MAX_LOADSTRING];                  // ���� ǥ���� �ؽ�Ʈ�Դϴ�.
WCHAR szWindowClass[MAX_LOADSTRING];            // �⺻ â Ŭ���� �̸��Դϴ�.

HWND hLoginIdEdit;
HWND hLoginPasswordEdit;
HWND hLoginButton;
HWND hChatWriteEdit;
HWND hChatReadListbox;
HWND hChatSendButton;

WCHAR g_str[BUFSIZE];
SOCKET g_serverSock;
// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);
////////////////����  �Լ�////////////////////
void InitClientServer(HWND hWnd);
void SendFixedSize(SOCKET sock);
//////////////error ��� �Լ�/////////////////
void DisplayText(LPWSTR fmt, ...);
void err_quit(LPWSTR msg);
void err_display(LPWSTR msg);
void err_display(int errcode);
/////////////////////////////////////////////

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
    LoadStringW(hInstance, IDC_CHATTING_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // ���� ���α׷� �ʱ�ȭ�� �����մϴ�.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATTING_CLIENT));

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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CHATTING_CLIENT));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_CHATTING_CLIENT);
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
      200, 200, 500, 460, nullptr, nullptr, hInstance, nullptr);

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
HWND hWndNew;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC memdc;
	int retval;
	static HBITMAP hBitmap;
    switch (message)
    {
	case WM_CREATE:
		hBitmap = (HBITMAP)LoadBitmap(((LPCREATESTRUCT)lParam)->hInstance, MAKEINTRESOURCE(IDB_BITMAP1));
		hLoginIdEdit = CreateWindowW(L"edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			180, 340, 100, 25, hWnd, (HMENU)ID_LOGIN_ID_EDIT, hInst, NULL);
		hLoginPasswordEdit = CreateWindowW(L"edit", NULL,
			WS_CHILD | WS_VISIBLE | WS_BORDER,
			180, 370, 100, 25, hWnd, (HMENU)ID_LOGIN_ID_EDIT, hInst, NULL);
		hLoginButton = CreateWindowW(L"button", L"�α���" , WS_CHILD | WS_VISIBLE | WS_BORDER,
			290, 340, 50, 55, hWnd, (HMENU)ID_LOGIN_BUTTON, hInst, NULL);

		return 0;
		//�ϴܽ��
	case WM_SETFOCUS:
		SetFocus(hChatWriteEdit);
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
			case ID_LOGIN_BUTTON:
				InvalidateRect(hWnd,NULL,TRUE);
				InitClientServer(hWnd);
				DestroyWindow(hLoginIdEdit);
				DestroyWindow(hLoginPasswordEdit);
				DestroyWindow(hLoginButton);				
				MoveWindow(hWnd, 200, 200, 500, 500, FALSE);
				hChatWriteEdit = CreateWindowW(L"edit", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER,
					//	WS_VSCROLL		|									//���� ��ũ�� ����
					//	ES_AUTOHSCROLL	| ES_AUTOVSCROLL | ES_MULTILINE		//Enter Ű ����
					5, 385, 425, 50, hWnd, (HMENU)ID_CHAT_WRITE_EDIT, hInst, NULL);
				hChatReadListbox = CreateWindowW(L"listbox", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER |
					WS_VSCROLL |
					ES_READONLY,
					5, 5, 475, 380, hWnd, (HMENU)ID_CHAT_READ_LISTBOX, hInst, NULL);
				hChatSendButton = CreateWindowW(L"button", L"������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
					430, 385, 50, 50, hWnd, (HMENU)ID_CHAT_SEND_BUTTON, hInst, NULL);
				break;
				case ID_CHAT_SEND_BUTTON:
					SendFixedSize(g_serverSock);
					break;
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
				memdc = CreateCompatibleDC(hdc);
				SelectObject(memdc, hBitmap);
				BitBlt(hdc, 0, 0, 480, 411, memdc, 0, 0, SRCCOPY);
				DeleteDC(memdc);
				DeleteObject(hBitmap);
				EndPaint(hWnd, &ps);	
        }
        break;
	////������â ũ�� �����ϴ� �ڵ�
	//case WM_GETMINMAXINFO:	//������ ũ�⳪ ��ġ �ٲٷ��� �� �� �߻��ϴ� �޽���
	//	((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 500;
	//	((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 500;
	//	((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
	//	((MINMAXINFO*)lParam)->ptMinTrackSize.y = 500;
	//	return FALSE;
    case WM_DESTROY:
        PostQuitMessage(0);
		DeleteObject(hBitmap);
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

//
//ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//
// hWnd : �޽��� �߻��� �������� �ڵ�
// message : WSAAsyncSelect() �Լ� ȣ�� �� ����ߴ� �޽���
// wParam : ��Ʈ��ũ �̺�Ʈ�� �߻��� ����
// lParam : ���� 16��Ʈ -> �߻��� �̺�Ʈ ���� 16��Ʈ -> �����ڵ�
void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET clientSock;
	SOCKADDR_IN clientAddr;
	int addrLen, retval;
	if (WSAGETSELECTERROR(lParam))
	{
		err_display(WSAGETSELECTERROR(lParam));
		return;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
	case FD_CONNECT:
		DisplayText(L"Connect ����!");
		if (WSAAsyncSelect(wParam, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
		{
			err_display(L"WSAAsyncSelect(), FD_CONNECT");
		}
		g_serverSock = wParam;
		break;
	case FD_READ:
		retval = recv(wParam, (char*)g_str, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(L"recv()");
			return;
		}
		addrLen = sizeof(clientAddr);
		getpeername(wParam, (SOCKADDR*)&clientAddr, &addrLen);
		DisplayText(L"[TCP /%S : %d] %s",
			inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port), g_str);
		break;
	case FD_WRITE:
		DisplayText(L"FD_WRITE �߻�");
		break;
	}
}
void InitClientServer(HWND hWnd)
{
	//���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;
	//socket ����
	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) err_quit(L"sock()");
	//WSAAsyncSelect()�� connect ���
	if (WSAAsyncSelect(clientSock, hWnd, WM_SOCKET, FD_CONNECT | FD_CLOSE) == SOCKET_ERROR) err_quit(L"WSAAsyncSelect()");

	int retval;
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(clientSock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	DisplayText(L"���� ������...!");
}

void SendFixedSize(SOCKET sock)
{
	int retval;
	int strSize;
	
	GetWindowText(hChatWriteEdit, g_str, BUFSIZE);
	strSize = (lstrlen(g_str) + 1) * sizeof(WCHAR);
	DisplayText(L"%d", strSize);
	retval = send(sock, (char*)&strSize, sizeof(int), 0);
	if (retval == SOCKET_ERROR)
	{
		err_display(L"send()");
		return;
	}
	retval = send(sock, (char*)g_str, strSize, 0);
	if (retval == SOCKET_ERROR)
	{
		err_display(L"send()");
		return;
	}

}
void DisplayText(LPWSTR fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	WCHAR cbuf[BUFSIZE + 256];
	vswprintf(cbuf, BUFSIZE + 256, fmt, arg);

	/*int nLength = GetWindowTextLength(hChatWriteEdit);
	SendMessage(hChatWriteEdit, EM_SETSEL, nLength, nLength);
	SendMessage(hChatWriteEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);*/
	SendMessage(hChatReadListbox, LB_ADDSTRING, 0, (LPARAM)cbuf);
	
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