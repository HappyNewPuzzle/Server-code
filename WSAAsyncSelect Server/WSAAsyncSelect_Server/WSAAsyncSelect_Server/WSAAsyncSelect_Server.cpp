// WSAAsyncSelect_Server.cpp : 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include "WSAAsyncSelect_Server.h"

#define MAX_LOADSTRING 100
/////////////// server data //////////////////
#define SERVERPORT 9000
#define BUFSIZE	   512
#define WM_SOCKET (WM_USER+1)
/////////////////////////////////////////////

///////////// sock 관리 구조체 ///////////////
struct SOCKETINFO
{
	SOCKET		sock;
	WCHAR		buf[BUFSIZE + 1];
	int			recvbytes;
	int			sendbytes;
	BOOL		recvdelayed;
	SOCKETINFO *next;

};
/////////////////////////////////////////////
SOCKETINFO *SocketInfoList;
SOCKET SockArr[10];
int SockCount = 0;
// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
HWND hEdit;
int recive = 0;
// 이 코드 모듈에 들어 있는 함수의 정방향 선언입니다.
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void				ProcessSocketMessage(HWND, UINT, WPARAM, LPARAM);
void				InitServer(HWND hWnd);
///////////// sock 관리 함수 //////////////////
BOOL AddSocketInfo(SOCKET sock);
SOCKETINFO *GetSocketInfo(SOCKET sock);
void RemoveSocketInfo(SOCKET sock);
//////////////error 출력 함수/////////////////
void DisplayText(LPWSTR fmt, ...);
void err_quit(LPWSTR msg);
void err_display(LPWSTR msg);
void err_display(int errcode);
/////////////////////////////////////////////
char * ConvertWCtoC(wchar_t* str);
wchar_t* ConverCtoWC(char* str);

//
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
    LoadStringW(hInstance, IDC_WSAASYNCSELECT_SERVER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 응용 프로그램 초기화를 수행합니다.
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WSAASYNCSELECT_SERVER));
	
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WSAASYNCSELECT_SERVER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WSAASYNCSELECT_SERVER);
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
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  목적:  주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 응용 프로그램 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
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
		//윈속 초기화 및 소켓 생성 및 listen() 과정까지
		InitServer(hWnd);
		return 0;
	case WM_SIZE:
		MoveWindow(hEdit, 0, 0, LOWORD(lParam), HIWORD(lParam), TRUE);
		return 0;
	//일단써놈
	case WM_SETFOCUS:
		SetFocus(hEdit);
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
int re_size;
void ProcessSocketMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	SOCKETINFO *ptr;
	SOCKET client_sock;
	SOCKADDR_IN clientaddr;
	int addrlen, retval;


	if (WSAGETSELECTERROR(lParam))
	{
		err_display(WSAGETSELECTERROR(lParam));
		RemoveSocketInfo(wParam);
		return;
	}

	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_ACCEPT:
			addrlen = sizeof(clientaddr);
			client_sock = accept(wParam, (SOCKADDR*)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET)
			{
				err_display(L"accpt()");
				return;
			}
			//mbstowcs(ipaddr, inet_ntoa(clientaddr.sin_addr), 100);
			DisplayText(L"[TCP 서버] : Client 접속 \r\n IP 주소 : %S, 포트 번호 : %d\r\n",
				inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
			AddSocketInfo(client_sock);
			if (WSAAsyncSelect(client_sock, hWnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE) == SOCKET_ERROR)
			{
				err_display(L"WSAAsyncSelect(), client_sock");
				RemoveSocketInfo(client_sock);
			}
			SockArr[SockCount++] = client_sock;
			break;
		case FD_READ:
			DisplayText(L"FD_READ  \r\n");
			ptr = GetSocketInfo(wParam);
			//데이터 받기
			if(recive == 0)
			{
				recive = recv(ptr->sock, (char*)&re_size, sizeof(int), 0);
				if (recive == SOCKET_ERROR)
				{
					err_display(L"recv()");
					return;
				}
				DisplayText(L"1. 데이터 길이 받기 %d\r\n", re_size);
			}
			else if(recive != 0)
			{
				retval = recv(ptr->sock, (char*)ptr->buf, re_size, 0);
				if (retval == SOCKET_ERROR)
				{
					err_display(L"recv()");
					return;
				}
				DisplayText(L"2. 데이터 내용 받기 %d\r\n");
				//받은 데이터 출력
				ptr->buf[retval] = '\0';		
				addrlen = sizeof(clientaddr);
				getpeername(wParam, (SOCKADDR*)&clientaddr, &addrlen);
				DisplayText(L"[TCP /%S : %d] %s\r\n", 
					inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port), ptr->buf);
				recive = 0;
				for (int i = 0; i<SockCount; i++)
				{
					DisplayText(L"데이터보냄\r\n");
					retval = send(SockArr[i], (char*)ptr->buf, retval, 0);
					if (retval == SOCKET_ERROR)
					{
						err_display(L"send()");
						RemoveSocketInfo(wParam);
						return;
					}
				}
			}
			break;
		case FD_WRITE:
			DisplayText(L"FD_WRITE 발생\r\n");
			break;
		case FD_CLOSE:
			int i = 0;
			while (1)
			{
				if (ptr->sock == SockArr[i])
				{
					SockArr[i] = NULL;
					break;
				}
				else
				{
					i++;
				}
			}
			RemoveSocketInfo(wParam);
			break;
	}
}


//
//  함수: InitServer(HWND hWnd)
//
//  목적:  서버 소켓 생성에 관련된 과정
//
//  설명: 윈속 생성 및 초기화, 
//        소켓 생성 및 bind,listen 과정, 
//		  WSAAsyncSelect() 사용하여 listen_sock 등록

void InitServer(HWND hWnd)
{
	//윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)	return;
	//socket 생성 AF_INET, SOCK_STREAM
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

	//cout << "Server Init Success!" << endl;
	DisplayText(L"Server Success!\r\n");
}

BOOL AddSocketInfo(SOCKET sock)
{
	SOCKETINFO *ptr = new SOCKETINFO;
	if (ptr == NULL) {
		printf("[오류] 메모리가 부족합니다!\n");
		return FALSE;
	}

	ptr->sock = sock;
	ptr->recvbytes = 0;
	ptr->sendbytes = 0;
	ptr->recvdelayed = FALSE;
	ptr->next = SocketInfoList;
	SocketInfoList = ptr;

	return TRUE;
}

SOCKETINFO * GetSocketInfo(SOCKET sock)
{
	SOCKETINFO *ptr = SocketInfoList;

	while (ptr) {
		if (ptr->sock == sock)
			return ptr;
		ptr = ptr->next;
	}

	return NULL;
}

void RemoveSocketInfo(SOCKET sock)
{
	SOCKADDR_IN clientaddr;
	int addrlen = sizeof(clientaddr);
	getpeername(sock, (SOCKADDR *)&clientaddr, &addrlen);
	printf("[TCP 서버] 클라이언트 종료: IP 주소=%s, 포트 번호=%d\n",
		inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

	SOCKETINFO *curr = SocketInfoList;
	SOCKETINFO *prev = NULL;

	while (curr) {
		if (curr->sock == sock) {
			if (prev)
				prev->next = curr->next;
			else
				SocketInfoList = curr->next;
			closesocket(curr->sock);
			delete curr;
			return;
		}
		prev = curr;
		curr = curr->next;
	}
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
	DisplayText(L"[오류] %s", (char *)lpMsgBuf);
	LocalFree(lpMsgBuf);
}

char * ConvertWCtoC(wchar_t* str)

{

	//반환할 char* 변수 선언

	char* pStr;



	//입력받은 wchar_t 변수의 길이를 구함

	int strSize = WideCharToMultiByte(CP_ACP, 0, str, -1, NULL, 0, NULL, NULL);

	//char* 메모리 할당

	pStr = new char[strSize];



	//형 변환 

	WideCharToMultiByte(CP_ACP, 0, str, -1, pStr, strSize, 0, 0);

	return pStr;


}


wchar_t* ConverCtoWC(char* str)

{

	//wchar_t형 변수 선언

	wchar_t* pStr;

	//멀티 바이트 크기 계산 길이 반환

	int strSize = MultiByteToWideChar(CP_ACP, 0, str, -1, NULL, NULL);

	//wchar_t 메모리 할당

	pStr = new WCHAR[strSize];

	//형 변환

	MultiByteToWideChar(CP_ACP, 0, str, strlen(str) + 1, pStr, strSize);

	return pStr;

}
