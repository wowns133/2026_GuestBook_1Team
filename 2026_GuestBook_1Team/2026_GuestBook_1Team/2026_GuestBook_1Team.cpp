// 2026_GuestBook_1Team.cpp : 애플리케이션에 대한 진입점을 정의합니다.
//

#include "framework.h"
#include "2026_GuestBook_1Team.h"

///통합 헤더를 만들기 전 임시로 넣은 헤더
#include "Draw.h"
#include "WinUi.h"
#include "FileInOut.h"
#include "Delete.h"
#include "Replay.h"

#define MAX_LOADSTRING 100

// 전역 변수:
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

/*=============================== test ===============================*/
Draw draw;
FileInOut file;
Replay replay;
AllDelete ac;
WinUi win_ui(&draw);

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
    LoadStringW(hInstance, IDC_MY2026GUESTBOOK1TEAM, szWindowClass, MAX_LOADSTRING);

    win_ui.setInst(hInstance);

    MyRegisterClass(hInstance);
    win_ui.setRegisterClass(hInstance); // 윈도우 창 등록


    draw.gdiPlusStart();
    // 애플리케이션 초기화를 수행합니다:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MY2026GUESTBOOK1TEAM));

    MSG msg;
    
    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    draw.gdiPlusEnd();
    return (int)msg.wParam;
}



//
//  함수: MyRegisterClass()
//
//  용도: 창 클래스를 등록합니다.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MY2026GUESTBOOK1TEAM));
    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_MY2026GUESTBOOK1TEAM);
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   함수: InitInstance(HINSTANCE, int)
//
//   용도: 인스턴스 핸들을 저장하고 주 창을 만듭니다.
//
//   주석:
//
//        이 함수를 통해 인스턴스 핸들을 전역 변수에 저장하고
//        주 프로그램 창을 만든 다음 표시합니다.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

    // 최상위 창 생성 및 핸들에 저장
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
        CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    ShowWindow(hWnd, SW_SHOWMAXIMIZED);
    UpdateWindow(hWnd);

    return TRUE;
}

//
//  함수: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  용도: 주 창의 메시지를 처리합니다.
//
//  WM_COMMAND  - 애플리케이션 메뉴를 처리합니다.
//  WM_PAINT    - 주 창을 그립니다.
//  WM_DESTROY  - 종료 메시지를 게시하고 반환합니다.
//
//
//Draw draw;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    switch (message)
    {
    case WM_CREATE:
    {
        // 최상위 창 핸들을 win_ui 객체의 맴버 변수로 저장
        win_ui.setRootWindowHandle(hWnd);

        // 자식 창들을 생성
        win_ui.createWinUIWindows(hWnd);
        //makeTempButton(hWnd, hInst);
        /// 스프레이 랜덤한 위치에 점을 생성하기 위한 난수 초기화
        srand((unsigned)time(NULL));
        return 0;
    }
    break;
    case WM_SIZE:
    {
        win_ui.resizeWinUIWindows(lParam);
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:

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
    // 개발자가 키보드 포커스를 통제하지 않은 상황에서 키보드 포커스는 아래와 같이 동작합니다.(키보트 포커스는 키보드 메시지를 받는 창을 의미하며, 
    // 마우스 포커스와는 별개로 동작합니다.)
    // 1. Static 창이나 사용자 커스텀 창 등은 창을 특정 행동으로 인해(클릭 등) 포커스가 자동으로 변하지 않습니다.
    // 2. Win32 내장 입력 컨트롤 클래스(Edit (텍스트 입력칸), Button (버튼), ListBox, ComboBox 등) 등 특정 창들은 특정 행동(좌클릭 등)을 하면 
    // SetFocus로 자신에게 포커스를 가져옵니다. 이런 식으로 옮겨진 포커스를 돌려 받으려면 개발자의 통제가 필요합니다.
    //
    // 창 종류는 클래스 이름 부분에 따라 달라집니다. Win32에서 정해진 내용을 기입하면 Button, Static 등 정해진 스타일의 창이 되고, 
    // 개발자가 정한 이름을 기입하면 커스텀 창이 됩니다.
    // 
    // 이 프로그램의 버튼을 포함한 모든 창들은 커스텀 창으로 만들어졌으므로 포커스 조작을 하지 않는다면 최상위 부모 창의 WndProc에서 키보드 이벤트를
    // 받습니다.
    case WM_KEYDOWN:
    {
        win_ui.resetScreenSaverTimer();
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
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
