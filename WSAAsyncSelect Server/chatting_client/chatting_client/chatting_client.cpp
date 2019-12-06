// chatting_client.cpp : ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include "chatting_client.h"

#define MAX_LOADSTRING 100
#define ID_EDIT 100
#define ID_LISTBOX 101
#define ID_BUTTON 102
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
HWND hEdit;
HWND hEdit2;
HWND hEdit3;
WCHAR str[BUFSIZE];
SOCKET sock;
// �� �ڵ� ��⿡ ��� �ִ� �Լ��� ������ �����Դϴ�.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);
////////////////����  �Լ�////////////////////
void InitClientServer(HWND hWnd);
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
      CW_USEDEFAULT, 0, 500, 500, nullptr, nullptr, hInstance, nullptr);

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
	int retval;
	int size;
    switch (message)
    {
	case WM_CREATE:
		hEdit = CreateWindowW(L"edit", NULL,
			WS_CHILD		| WS_VISIBLE	 | WS_BORDER ,
		//	WS_VSCROLL		|									//���� ��ũ�� ����
		//	ES_AUTOHSCROLL	| ES_AUTOVSCROLL | ES_MULTILINE		//Enter Ű ����
			5, 385, 425, 50, hWnd, (HMENU)ID_EDIT, hInst, NULL);
		hEdit2 = CreateWindowW(L"listbox", NULL,
			WS_CHILD	| WS_VISIBLE | WS_BORDER |
			WS_VSCROLL  |
			ES_READONLY,
			5, 5, 475, 380, hWnd, (HMENU)ID_LISTBOX, hInst, NULL);
		hEdit3 = CreateWindowW(L"button", L"������", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			430, 385, 50, 50, hWnd, (HMENU)ID_BUTTON, hInst, NULL);

		InitClientServer(hWnd);
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
				case ID_BUTTON:
					GetWindowText(hEdit, str, BUFSIZE);
					size = (lstrlen(str)+1)*sizeof(WCHAR);
					DisplayText(L"%d", size);
					retval = send(sock, (char*)&size, sizeof(int), 0);
					if (retval == SOCKET_ERROR)
					{
						err_display(L"send()");
						break;
					}
					retval = send(sock, (char*)str, size , 0);
					if (retval == SOCKET_ERROR)
					{
						err_display(L"send()");
						break;
					}
					break;
				//case ID_EDIT:
				//	switch (HIWORD(wParam)) 
				//	{
				//		case EN_CHANGE:
				//			GetWindowText(hEdit, str, 6);
				//			//GetDlgItemText(hWnd, ID_EDIT, str, 6);
				//			retval = send(sock, (char*)str, 12, 0);
				//			if (retval == SOCKET_ERROR)
				//			{
				//				err_display(L"send()");
				//				break;
				//			}
				//			//SetWindowText(hWnd, str);
				//	}
				//	break;
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
	//������â ũ�� �����ϴ� �ڵ�
	case WM_GETMINMAXINFO:	//������ ũ�⳪ ��ġ �ٲٷ��� �� �� �߻��ϴ� �޽���
		((MINMAXINFO*)lParam)->ptMaxTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMaxTrackSize.y = 500;
		((MINMAXINFO*)lParam)->ptMinTrackSize.x = 500;
		((MINMAXINFO*)lParam)->ptMinTrackSize.y = 500;
		return FALSE;
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

//
//ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//
// hWnd : �޽��� �߻��� �������� �ڵ�
// message : WSAAsyncSelect() �Լ� ȣ�� �� ����ߴ� �޽���
// wParam : ��Ʈ��ũ �̺�Ʈ�� �߻��� ����
// lParam : ���� 16��Ʈ -> �߻��� �̺�Ʈ ���� 16��Ʈ -> �����ڵ�
void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, retval;
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
		sock = wParam;
		break;
	case FD_READ:
		retval = recv(wParam, (char*)str, BUFSIZE, 0);
		if (retval == SOCKET_ERROR)
		{
			err_display(L"recv()");
			return;
		}
		addrlen = sizeof(clientaddr);
		getpeername(wParam, (SOCKADDR*)&clientaddr, &addrlen);
		DisplayText(L"[TCP /%S : %d] %s",
			inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), str);
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
	SOCKET client_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (client_sock == INVALID_SOCKET) err_quit(L"sock()");
	//WSAAsyncSelect()�� connect ���
	if (WSAAsyncSelect(client_sock, hWnd, WM_SOCKET, FD_CONNECT | FD_CLOSE) == SOCKET_ERROR) err_quit(L"WSAAsyncSelect()");

	int retval;
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(client_sock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	DisplayText(L"���� ������...!");
}


void DisplayText(LPWSTR fmt, ...)
{
	va_list arg;
	va_start(arg, fmt);

	WCHAR cbuf[BUFSIZE + 256];
	vswprintf(cbuf, BUFSIZE + 256, fmt, arg);

	/*int nLength = GetWindowTextLength(hEdit);
	SendMessage(hEdit, EM_SETSEL, nLength, nLength);
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)cbuf);*/
	SendMessage(hEdit2, LB_ADDSTRING, 0, (LPARAM)cbuf);
	
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