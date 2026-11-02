#include "ScreenSaver.h"


//----------------static 멤버 초기화-------------//
ScreenSaver* ScreenSaver::static_screen_saver = nullptr;
bool ScreenSaver::is_screen_saver_created = false;


//-------------------세팅 함수들-----------------//
ScreenSaver::ScreenSaver()
{
    // WinUi 클래스 객체는 하나만 존재해야 하므로 경고창 띄우고 return
    if (is_screen_saver_created)
    {
        //오류 메시지 띄우기
        return;
    }
    is_screen_saver_created = true; //객체 생성되었다고 표시

    static_screen_saver = this; // 이 객체의 주소를 저장
    screen_saver_background = CreateSolidBrush(RGB(253, 253, 255)); // 배경 색 설정
}

void ScreenSaver::setOtherWindowsHandle(HWND root_window_hwnd, HWND main_ui_hwnd, HWND canvas_hwnd)
{
    this->root_window_hwnd = root_window_hwnd;
    this->main_ui_hwnd = main_ui_hwnd;
    this->canvas_hwnd = canvas_hwnd;
}

void ScreenSaver::setInst(HINSTANCE hInst)
{
    this->hInst = hInst;
}

//--------------------창 관련 함수들--------------------//

ATOM ScreenSaver::ScreenSaverRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 }; //등록하지 않은 값이 있으므로 미리 전체를 0으로 초기화


    wcex.cbSize = sizeof(WNDCLASSEX); // 구조체의 크기 설정 
    wcex.style = CS_HREDRAW | CS_VREDRAW; // 창의 크기가 변하면 다시 그리기, 입력 메시지 관련 설정
    wcex.lpfnWndProc = StaticScreenSaverWndProc; // 이 창에서 실행될 메시지 처리 함수 지정 
    wcex.cbClsExtra = 0; // 클래스 여유 메모리. 보통은 0
    wcex.cbWndExtra = 0; // 윈도우 여유 메모리. 보통은 0 
    wcex.hInstance = hInstance; // 인스턴스 핸들 

    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // 마우스 커서 스타일 
    wcex.hbrBackground = screen_saver_background; // 창 배경색 
    wcex.lpszClassName = L"ScreenSaverWindowClassType"; // 이 윈도우 창 이름 

    return RegisterClassExW(&wcex);
}


HWND ScreenSaver::createScreenSaverWindow(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    screen_saver_hwnd = CreateWindowExW(
        0,        // 스타일
        L"ScreenSaverWindowClassType",                   // RegisterClass에서 등록한 클래스 이름
        nullptr,                                    // 창 상단 타이틀 바에 표시될 텍스트. 자식은 보통 없음
        WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,    // 윈도우 스타일
        rect.left, rect.top,                                       // 창이 나타날 처음 화면 위치 (x, y) - OS가 알아서 지정 
        rect.right - rect.left, rect.bottom - rect.top,                            // 창의 처음 가로, 세로 크기 (Width, Height) 
        hWnd,                                       // 부모 윈도우 핸들 
        (HMENU)SCREEN_SAVER_WINDOW_ID,                          // 자식 윈도우 ID 
        hInst,                                      // 프로그램의 인스턴스 핸들 
        nullptr                                     // 추가 파라미터 
    );

    return screen_saver_hwnd;
}


LRESULT CALLBACK ScreenSaver::StaticScreenSaverWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return static_screen_saver->ScreenSaverWndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK ScreenSaver::ScreenSaverWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {

    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:

        switch (wmId)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    // 창의 가시성이 변경었거나 변경되려고 할 때 발생하는 메시지
    case WM_SHOWWINDOW:
    {
        // lParam은 이 메시지가 발생한 이유에 대한 메시지. 0이면 ShowWindow(이 창 핸들, XXX)로 발생했다는 뜻
        if (lParam == 0)
        {
            // wParam은 이 창이 보이게 될 때인지(TRUE) 안 보이게 될 때인지(FALSE)를 나타냄 
            if (wParam == TRUE)
            {
                // 화면에 서명 재생
            }
            else if (wParam == FALSE)
            {

            }
        }
    }
    break;
    // 움직임이 발생하면 화면 보호기 창을 숨긴다
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_KEYDOWN:
    {
        hideScreenSaver();
    }
    break;
    // WM_MOUSEMOVE는 창이 보이게 되었을 때 그 위치에 마우스가 있다면 직접 움직이지 않아도 반드시 1회 발생함
    case WM_MOUSEMOVE:
    {
        // WM_MOUSEMOVE 최초 발생 시 무시하기
        if (is_first_mouse_move == 1)
        {
            is_first_mouse_move -= 1;
            break;
        }
        // WM_MOUSEMOVE 최초 발생이 아닐 경우 화면 보호기 창을 숨긴다.
        hideScreenSaver();
    }
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


//-----------------------------화면 보호기 동작 관련 함수들----------------------------//

void ScreenSaver::screenSaverTimer()
{
    // 남은 시간 -1
    screen_saver_time -= 1;

    // 남은 시간이 없다면
    if (screen_saver_time <= 0)
    {
        //MessageBox(hWnd, L"메시지", L"메시지", MB_OK);

        // 창 띄우기
        ShowWindow(screen_saver_hwnd, SW_SHOW);
        
        //창 켜짐 표시
        is_screen_saver_on = 1;

        // 타이머 삭제
        KillTimer(main_ui_hwnd, SCREEN_SAVER_TIMER_ID);

        // 키보드 포커스를 화면 보호기로 넘겨서 화면 보호기가 키보드 이벤트를 받도록 만들기
        SetFocus(screen_saver_hwnd);

        // 타임 변수 초기화
        screen_saver_time = SCREEN_SAVE_TIME_LIMIT;
    }
}


void ScreenSaver::resetScreenSaverTimer()
{
    // 화면보호기 실행까지의 시간 초기화
    screen_saver_time = SCREEN_SAVE_TIME_LIMIT;
}


void ScreenSaver::hideScreenSaver()
{
    if (is_screen_saver_on == 1)
    {
        // 화면 보호기 창 숨기기
        ShowWindow(screen_saver_hwnd, SW_HIDE);

        // 창 꺼짐 표시
        is_screen_saver_on = 0;

        // 타이머 재설정
        SetTimer(main_ui_hwnd, SCREEN_SAVER_TIMER_ID, 1000, NULL);

        // 다음 화면보호기 실행 시 첫 WM_MOUSEMOVE를 스킵 하기 위해 is_first_mouse_move를 1로 복구
        is_first_mouse_move = 1;

        // 키보드 포커스를 최상위 부모 창에 돌려 주는 것으로 키보트 메시지를 최상위 부모 창이 받도록 설정
        SetFocus(root_window_hwnd);
    }
}









