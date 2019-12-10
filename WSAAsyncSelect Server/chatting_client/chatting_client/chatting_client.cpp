// chatting_client.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
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


// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

HWND hLoginIdEdit;
HWND hLoginPasswordEdit;
HWND hLoginButton;
HWND hChatWriteEdit;
HWND hChatReadListbox;
HWND hChatSendButton;

WCHAR g_str[BUFSIZE];
SOCKET g_serverSock;
// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);
////////////////소켓  함수////////////////////
void InitClientServer(HWND hWnd);
void SendFixedSize(SOCKET sock);
//////////////error 출력 함수/////////////////
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

    // TODO: 여기에 코드를 입력합니다.

    // 전역 문자열을 초기화합니다.
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CHATTING_CLIENT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CHATTING_CLIENT));

    MSG msg;
	_wsetlocale(LC_ALL, L"korean");
    // 기본 메시지 루프입니다.
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
//  함수: MyRegisterClass()
//
//  목적: 창 클래스를 등록합니다.
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
//   함수: InitInstance(HINSTANCE, int)
//
//   목적: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   설명:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

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
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
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
		hLoginButton = CreateWindowW(L"button", L"로그인" , WS_CHILD | WS_VISIBLE | WS_BORDER,
			290, 340, 50, 55, hWnd, (HMENU)ID_LOGIN_BUTTON, hInst, NULL);

		return 0;
		//일단써놈
	case WM_SETFOCUS:
		SetFocus(hChatWriteEdit);
		return 0;
	case WM_SOCKET:
		ProcessSocketMessage(hWnd, message, wParam, lParam);
		return 0;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다.
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
					//	WS_VSCROLL		|									//수직 스크롤 생성
					//	ES_AUTOHSCROLL	| ES_AUTOVSCROLL | ES_MULTILINE		//Enter 키 사용시
					5, 385, 425, 50, hWnd, (HMENU)ID_CHAT_WRITE_EDIT, hInst, NULL);
				hChatReadListbox = CreateWindowW(L"listbox", NULL,
					WS_CHILD | WS_VISIBLE | WS_BORDER |
					WS_VSCROLL |
					ES_READONLY,
					5, 5, 475, 380, hWnd, (HMENU)ID_CHAT_READ_LISTBOX, hInst, NULL);
				hChatSendButton = CreateWindowW(L"button", L"보내기", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
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
				// TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다.
				memdc = CreateCompatibleDC(hdc);
				SelectObject(memdc, hBitmap);
				BitBlt(hdc, 0, 0, 480, 411, memdc, 0, 0, SRCCOPY);
				DeleteDC(memdc);
				DeleteObject(hBitmap);
				EndPaint(hWnd, &ps);	
        }
        break;
	////윈도우창 크기 고정하는 코드
	//case WM_GETMINMAXINFO:	//윈도우 크기나 위치 바꾸려고 할 때 발생하는 메시지
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

// 정보 대화 상자의 메시지 처리기입니다.
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
// hWnd : 메시지 발생한 윈도우의 핸들
// message : WSAAsyncSelect() 함수 호출 시 등록했던 메시지
// wParam : 네트워크 이벤트가 발생한 소켓
// lParam : 하위 16비트 -> 발생한 이벤트 상위 16비트 -> 오류코드
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
		DisplayText(L"Connect 성공!");
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
		DisplayText(L"FD_WRITE 발생");
		break;
	}
}
void InitClientServer(HWND hWnd)
{
	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) return;
	//socket 생성
	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if (clientSock == INVALID_SOCKET) err_quit(L"sock()");
	//WSAAsyncSelect()로 connect 등록
	if (WSAAsyncSelect(clientSock, hWnd, WM_SOCKET, FD_CONNECT | FD_CLOSE) == SOCKET_ERROR) err_quit(L"WSAAsyncSelect()");

	int retval;
	SOCKADDR_IN serveraddr;
	ZeroMemory(&serveraddr, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = inet_addr(SERVERIP);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = connect(clientSock, (SOCKADDR *)&serveraddr, sizeof(serveraddr));
	DisplayText(L"서버 연결중...!");
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
	DisplayText(L"[오류] %s", (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}