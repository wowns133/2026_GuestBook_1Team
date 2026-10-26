#include "PenScrollBar.h"

/**
* @file
* @brief 펜 두께를 조절하는 스크롤바 창을 구현한 파일.
* @details 펜 두께를 조절하는 스크롤바 창을 구현한 파일.
* @author challenjoy01
*/

//----------------static 멤버 초기화-------------//
PenScrollBar* PenScrollBar::static_pen_scroll = nullptr;
bool PenScrollBar::is_pen_scroll_created = false;


PenScrollBar::PenScrollBar()
{
    // WinUi 클래스 객체는 하나만 존재해야 하므로 경고창 띄우고 return
    if (is_pen_scroll_created)
    {
        //오류 메시지 띄우기
        return;
    }
    is_pen_scroll_created = true; //객체 생성되었다고 표시

    static_pen_scroll = this; // 이 객체의 주소를 저장
    pen_scroll_background = CreateSolidBrush(RGB(253, 253, 255)); // 배경 색 설정
}




ATOM PenScrollBar::PenScrollRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 }; //등록하지 않은 값이 있으므로 미리 전체를 0으로 초기화


    wcex.cbSize = sizeof(WNDCLASSEX); // 구조체의 크기 설정 
    wcex.style = CS_HREDRAW | CS_VREDRAW; // 창의 크기가 변하면 다시 그리기, 입력 메시지 관련 설정
    wcex.lpfnWndProc = StaticPenScrollWndProc; // 이 창에서 실행될 메시지 처리 함수 지정 
    wcex.cbClsExtra = 0; // 클래스 여유 메모리. 보통은 0
    wcex.cbWndExtra = 0; // 윈도우 여유 메모리. 보통은 0 
    wcex.hInstance = hInstance; // 인스턴스 핸들 

    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // 마우스 커서 스타일 
    wcex.hbrBackground = pen_scroll_background; // 창 배경색 
    wcex.lpszClassName = L"PenScrollWindowClassType"; // 이 윈도우 창 이름 

    return RegisterClassExW(&wcex);
}


void PenScrollBar::setTrackMouseEvent(HWND hWnd)
{
    if (!is_tracking_mouse)
    {
        track_mouse = new TRACKMOUSEEVENT; //구조체 초기화
        track_mouse->cbSize = sizeof(TRACKMOUSEEVENT);
        track_mouse->dwFlags = TME_LEAVE; //창 벗어남을 감시함
        track_mouse->hwndTrack = hWnd;

        if (TrackMouseEvent(track_mouse)) //TrackMouseEvent 함수는 마우스 이벤트 추적을 시작하는 함수
        {
            is_tracking_mouse = true; //추척을 시작했을 경우 플래그 true
        }
        delete track_mouse;
    }
}


LRESULT CALLBACK PenScrollBar::StaticPenScrollWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return static_pen_scroll->PenScrollWndProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK PenScrollBar::PenScrollWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
    case WM_LBUTTONDOWN:
    {
        is_click = true;
        setTrackMouseEvent(hWnd);
        setScrollButton(hWnd, lParam);
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    }
    case WM_MOUSEMOVE:
    {
        setScrollButton(hWnd, lParam);
        break;
    }
    case WM_LBUTTONUP:
    {
        is_click = false;
        //-------------------마우스 추적 완료 처리------------------//
        is_tracking_mouse = false;
        break;
    }
    case WM_MOUSELEAVE:
    {
        is_click = false;
        //-------------------마우스 추적 완료 처리------------------//
        is_tracking_mouse = false;
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        HPEN my_pen, os_pen;
        HBRUSH my_brush, os_brush;

        my_pen = CreatePen(SOLIDPEN, 0, GetSysColor(COLOR_BTNFACE));
        os_pen = (HPEN)SelectObject(hdc, my_pen);


        my_brush = GetSysColorBrush(COLOR_BTNFACE);
        os_brush = (HBRUSH)SelectObject(hdc, my_brush);

        Rectangle(hdc,
            pen_scroll_button_width / 2,
            (pen_scroll_wnd_height - pen_scroll_bar_height) / 2,
            pen_scroll_button_width / 2 + pen_scroll_bar_width,
            (pen_scroll_wnd_height - pen_scroll_bar_height) / 2 + pen_scroll_bar_height);
        Ellipse(hdc,
            pen_scroll_button_width / 2 - pen_scroll_bar_height / 2,
            (pen_scroll_wnd_height - pen_scroll_bar_height) / 2,
            pen_scroll_button_width / 2 - pen_scroll_bar_height / 2 + pen_scroll_bar_height,
            (pen_scroll_wnd_height - pen_scroll_bar_height) / 2 + pen_scroll_bar_height);
        Ellipse(hdc,
            pen_scroll_button_width / 2 + pen_scroll_bar_width - pen_scroll_bar_height / 2,
            (pen_scroll_wnd_height - pen_scroll_bar_height) / 2,
            pen_scroll_button_width / 2 + pen_scroll_bar_width + pen_scroll_bar_height / 2,
            (pen_scroll_wnd_height - pen_scroll_bar_height) / 2 + pen_scroll_bar_height);

        SelectObject(hdc, os_pen);
        SelectObject(hdc, os_brush);
        drawScrollButton(hdc);
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



void PenScrollBar::createPenScrollWindows(HWND hWnd)
{
    pen_scroll_hwnd = CreateWindowExW(
        0,
        L"PenScrollWindowClassType",                   // RegisterClass에서 등록한 클래스 이름
        nullptr, // 창 상단 타이틀 바에 표시될 텍스트. 
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, // 윈도우 스타일 (자식 창임 | 생성 즉시 화면에 보이도록 | 부모 창이 내 영역 제외하고 그리기)
        pen_scroll_wnd_x, pen_scroll_wnd_y, // 창이 나타날 처음 화면 위치 (X, Y) - OS가 알아서 지정 
        pen_scroll_wnd_width, pen_scroll_wnd_height, // 창의 처음 가로, 세로 크기 (Width, Height) - OS가 알아서 지정 
        hWnd, // 부모 윈도우 핸들 
        (HMENU)PEN_SCROLL_ID, // 메뉴 핸들 
        hInst, // 이 창을 만드는 프로그램의 인스턴스 핸들 
        nullptr // 추가 파라미터 
    );
}

void PenScrollBar::setInst(HINSTANCE hInst)
{
    this->hInst = hInst;
}


void PenScrollBar::setScrollButton(HWND hWnd, LPARAM lParam)
{
    if (is_click == true)
    {
        int pen_width = 0;

        scroll_button_x = LOWORD(lParam) - (pen_scroll_button_width / 2); // 스크롤바 버튼 좌상단 x좌표 구하기
        if (scroll_button_x < 0) // 스크롤바 버튼 최소 0으로 설정
        {
            scroll_button_x = 0;
        }
        else if (scroll_button_x > pen_scroll_bar_width) // 스크롤바 버튼 최대를 지정된 최댓값으로 설정
        {
            scroll_button_x = pen_scroll_bar_width;
        }
        pen_width = scroll_button_x / pen_scroll_bar_ratio; // 펜 두께를 0 ~ 50 사이의 숫자로 변환
        // 펜 두께 변경 함수
        pen_style->settingPenStyle(pen_style->getSelectPen(), pen_width, pen_style->getPenColor());
        InvalidateRect(hWnd, NULL, TRUE);

    }
}

void PenScrollBar::drawScrollButton(HDC hdc)
{
    Ellipse(hdc, 
            scroll_button_x, 
            (pen_scroll_wnd_height - pen_scroll_button_width) / 2,
            scroll_button_x + pen_scroll_button_width, 
            (pen_scroll_wnd_height - pen_scroll_button_width) / 2 + pen_scroll_button_width);
}


void PenScrollBar::setPenStyle(PenStyle* pen_style)
{
    this->pen_style = pen_style;
}
