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
        // 선언과 동시에 모든 멤버를 0으로 초기화
        TRACKMOUSEEVENT track_mouse = { 0 }; ///< 마우스의 움직임을 추적하기 위해 사용하는 구조체
        track_mouse.cbSize = sizeof(TRACKMOUSEEVENT);
        track_mouse.dwFlags = TME_LEAVE; //창 벗어남을 감시함
        track_mouse.hwndTrack = hWnd;

        if (TrackMouseEvent(&track_mouse)) //TrackMouseEvent 함수는 마우스 이벤트 추적을 시작하는 함수
        {
            is_tracking_mouse = true; //추척을 시작했을 경우 플래그 true
        }
    }
}



void PenScrollBar::setupBitmap(LPARAM lParam)
{
    int client_width = LOWORD(lParam);
    int client_height = HIWORD(lParam);

    // 비트맵이 없다면(첫 생성) 비트맵 초기화
    if (scroll_bar_bitmap == nullptr)
    {
        // bitmap 객체는 new로 선언하고 삭제하는 방식을 추천합니다.
        scroll_bar_bitmap = new Gdiplus::Bitmap(client_width, client_height, PixelFormat32bppARGB); //더블 버퍼링 구현을 위한 Bitmap 객체 생성. 사용 후 삭제해야 함
        scroll_bar_bitmap_graphics = new Gdiplus::Graphics(scroll_bar_bitmap); //비트맵 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
        scroll_bar_bitmap_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);				// 안티엘리어싱 적용. 선에서 계단 현상이 줄어든다.

        return;
    }

    // bitmap 객체는 new로 선언하고 삭제하는 방식을 추천합니다.
    Gdiplus::Bitmap* new_bitmap = new Gdiplus::Bitmap(client_width, client_height, PixelFormat32bppARGB); //더블 버퍼링 구현을 위한 Bitmap 객체 생성. 사용 후 삭제해야 함
    Gdiplus::Graphics* new_bitmap_graphics = new Gdiplus::Graphics(new_bitmap); //비트맵 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
    new_bitmap_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);				// 안티엘리어싱 적용. 선에서 계단 현상이 줄어든다.

    // 기존 비트맵의 내용을 새 비트맵에 저장
    new_bitmap_graphics->DrawImage(scroll_bar_bitmap, 0, 0);

    // 기존 비트맵 삭제
    delete scroll_bar_bitmap_graphics;
    delete scroll_bar_bitmap;
    scroll_bar_bitmap_graphics = nullptr;
    scroll_bar_bitmap = nullptr;

    // 새 비트맵을 기존 비트맵 변수에 저장
    scroll_bar_bitmap = new_bitmap;
    scroll_bar_bitmap_graphics = new_bitmap_graphics;
}

void PenScrollBar::deleteBitmap()
{
    // 비트맵 삭제
    delete scroll_bar_bitmap_graphics;
    delete scroll_bar_bitmap;
    scroll_bar_bitmap_graphics = nullptr;
    scroll_bar_bitmap = nullptr;
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
    case WM_SIZE:
    {
        // 비트맵 생성, 삭제
        setupBitmap(lParam);
    }
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
        // gdi+ 사용 설정
        Gdiplus::Graphics hdc_graphics(hdc);
        hdc_graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

        // 비트맵 초기화
        scroll_bar_bitmap_graphics->Clear(Gdiplus::Color(255, 253, 253, 255));
        
        // 스크롤바 비트맵에 그리기
        drawScrollBar();

        //비트맵을 화면에 출력
        hdc_graphics.DrawImage(scroll_bar_bitmap, 0, 0);

        // 펜 반지름 테스트 코드
        //WCHAR t[50];
        //wsprintf(t, L"현재 펜 반지름 %d", (int)pen_style->getRadius());
        //TextOut(hdc, 0, 0, t, wcslen(t));


        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        // 비트맵 삭제
        deleteBitmap();
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
        int pen_width = 0; ///< 펜의 반지름

        scroll_button_x = LOWORD(lParam) - (pen_scroll_button_width / 2); // 스크롤바 버튼 좌상단 x좌표 구하기
        if (scroll_button_x < 0) // 스크롤바 버튼 최소 0으로 설정
        {
            scroll_button_x = 0;
        }
        else if (scroll_button_x > pen_scroll_bar_width) // 스크롤바 버튼 최대를 지정된 최댓값으로 설정
        {
            scroll_button_x = pen_scroll_bar_width;
        }
        // 펜 두께를 0 ~ (최댓값 - 최솟값) 사이의 숫자로 변환한 후 최솟값을 더하기
        //결과적으로 최솟값 ~
        pen_width = scroll_button_x / pen_scroll_bar_ratio + pen_min_radius; 
        
        // 펜 두께 변경 함수
        pen_style->settingPenStyle(pen_style->getSelectPen(), pen_width, pen_style->getPenColor());
        InvalidateRect(hWnd, NULL, FALSE);

    }
}

void PenScrollBar::drawScrollBar()
{
    //스크롤바 버튼용 펜
    Gdiplus::Pen button_pen(Gdiplus::Color::Black, 1);
    Gdiplus::SolidBrush button_brush(Gdiplus::Color::White);

    // 스크롤바용 그리기용 펜 생성, 양 끝을 둥글게 설정
    Gdiplus::Pen bar_pen(Gdiplus::Color::Gray, 7);
    bar_pen.SetStartCap(Gdiplus::LineCapRound);
    bar_pen.SetEndCap(Gdiplus::LineCapRound);

    // 스크롤바 그리기
    scroll_bar_bitmap_graphics->DrawLine(&bar_pen, pen_scroll_button_width / 2,
        pen_scroll_wnd_height / 2,
        pen_scroll_button_width / 2 + pen_scroll_bar_width,
        pen_scroll_wnd_height / 2
    );

    scroll_bar_bitmap_graphics->FillEllipse(&button_brush,
        scroll_button_x,
        (pen_scroll_wnd_height - pen_scroll_button_width) / 2,
        pen_scroll_button_width,
        pen_scroll_button_width);

    scroll_bar_bitmap_graphics->DrawEllipse(&button_pen,
        scroll_button_x,
        (pen_scroll_wnd_height - pen_scroll_button_width) / 2,
        pen_scroll_button_width,
        pen_scroll_button_width);


}


void PenScrollBar::setPenStyle(PenStyle* pen_style)
{
    this->pen_style = pen_style;
}
