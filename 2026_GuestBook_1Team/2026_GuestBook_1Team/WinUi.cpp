#pragma once
#include "WinUi.h"


void makeTempButton(HWND hWnd, HINSTANCE hInst)
{
    // --- PEN STYLE (1 ~ 5) ---
    CreateWindowW(L"BUTTON", L"solid", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE1, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"spray", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        50, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE2, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"brush", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        90, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE3, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"highlighter", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        130, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE4, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"S5", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        170, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE5, hInst, nullptr);

    // --- PEN COLOR (1 ~ 5) ---
    CreateWindowW(L"BUTTON", L"black", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        210, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR1, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"red", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR2, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"green", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        290, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR3, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"blue", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        330, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR4, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"yellow", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        370, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR5, hInst, nullptr);

    // ---기능 버튼들 ---
    CreateWindowW(L"BUTTON", L"ER", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        410, 10, 30, 30, hWnd, (HMENU)ID_ERASER, hInst, nullptr); // 지우개
    CreateWindowW(L"BUTTON", L"PLAY", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        450, 10, 30, 30, hWnd, (HMENU)ID_REPLAY, hInst, nullptr); // 재생
    CreateWindowW(L"BUTTON", L"SAVE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        490, 10, 30, 30, hWnd, (HMENU)ID_SAVE_FILE, hInst, nullptr); // 저장
    CreateWindowW(L"BUTTON", L"LOAD", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        530, 10, 30, 30, hWnd, (HMENU)ID_LOAD_FILE, hInst, nullptr); // 열기
}



//----------------static 멤버 초기화-------------//
WinUi* WinUi::static_winui = nullptr;
bool WinUi::is_winui_created = false;

WinUi::WinUi(Draw* draw)
{
    // WinUi 클래스 객체는 하나만 존재해야 하므로 경고창 띄우고 return
    if (is_winui_created)
    {
        MessageBox(canvas_hwnd, L"WinUi 클래스 객체가 이미 생성되었습니다. WinUi 클래스는 반드시 하나만 존재해야 합니다.", L"클래스 생성 오류", MB_OK);
        return;
    }
    is_winui_created = true; //객체 생성되었다고 표시

    static_winui = this; // 이 객체의 주소를 저장
    this->draw = draw;
    main_ui_background = CreateSolidBrush(RGB(253, 253, 255)); // 배경 색 설정
    canvus_background = CreateSolidBrush(RGB(255, 255, 255)); // 배경 색 설정
}


void WinUi::createWinUIWindows(HWND hWnd)
{
    RECT rect;
    GetClientRect(hWnd, &rect);

    main_ui_hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME | WS_EX_TOPMOST,        // 스타일
        L"MainUIWindowClassType",                   // RegisterClass에서 등록한 클래스 이름
        nullptr,                                    // 창 상단 타이틀 바에 표시될 텍스트. 자식은 보통 없음
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN,    // 윈도우 스타일 (최소화/최대화/닫기 버튼이 있는 일반 창) 
        0, 0,                                       // 창이 나타날 처음 화면 위치 (x, y) - OS가 알아서 지정 
        rect.right, 100,                            // 창의 처음 가로, 세로 크기 (Width, Height) 
        hWnd,                                       // 부모 윈도우 핸들 
        (HMENU)MAIN_UI_ID,                          // 자식 윈도우 ID 
        hInst,                                      // 프로그램의 인스턴스 핸들 
        nullptr                                     // 추가 파라미터 
    );

    canvas_hwnd = CreateWindowExW(
        0,
        L"CanvasWindowClassType",                   // RegisterClass에서 등록한 클래스 이름
        nullptr, // 창 상단 타이틀 바에 표시될 텍스트. 
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN, // 윈도우 스타일 (최소화/최대화/닫기 버튼이 있는 일반 창) 
        0, 100, // 창이 나타날 처음 화면 위치 (X, Y) - OS가 알아서 지정 
        rect.right, rect.bottom - 100, // 창의 처음 가로, 세로 크기 (Width, Height) - OS가 알아서 지정 
        hWnd, // 부모 윈도우 핸들 (메인 창이므로 없음) 
        (HMENU)CANVAS_ID, // 메뉴 핸들 
        hInst, // 이 창을 만드는 프로그램의 인스턴스 핸들 
        nullptr // 추가 파라미터 
    );
}

void WinUi::resizeWinUIWindows(LPARAM lParam)
{
    int client_width = LOWORD(lParam);
    int client_height = HIWORD(lParam);
    int main_ui_height = client_height * 0.2;
    MoveWindow(canvas_hwnd, 0, main_ui_height, client_width, client_height - main_ui_height, TRUE);
    MoveWindow(main_ui_hwnd, 0, 0, client_width, main_ui_height, TRUE);
}

void WinUi::setInst(HINSTANCE hInst)
{
    this->hInst = hInst;
}




ATOM WinUi::MainUIRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 }; //등록하지 않은 값이 있으므로 미리 전체를 0으로 초기화


    wcex.cbSize = sizeof(WNDCLASSEX); // 구조체의 크기 설정 
    wcex.style = CS_HREDRAW | CS_VREDRAW; // 창의 크기가 변하면 다시 그리기, 입력 메시지 관련 설정
    wcex.lpfnWndProc = StaticMainUIWndProc; // 이 창에서 실행될 메시지 처리 함수 지정 
    wcex.cbClsExtra = 0; // 클래스 여유 메모리. 보통은 0
    wcex.cbWndExtra = 0; // 윈도우 여유 메모리. 보통은 0 
    wcex.hInstance = hInstance; // 인스턴스 핸들 

    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // 마우스 커서 스타일 
    wcex.hbrBackground = main_ui_background; // 창 배경색 
    wcex.lpszClassName = L"MainUIWindowClassType"; // 이 윈도우 창 이름 

    return RegisterClassExW(&wcex);
}


LRESULT CALLBACK WinUi::StaticMainUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return static_winui->MainUIWndProc(hWnd, message, wParam, lParam);
}


LRESULT CALLBACK WinUi::MainUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        makeTempButton(hWnd, hInst);
        button.setAllButtons();
    }
    break;
    case WM_LBUTTONDOWN:
    {
        int button_id = button.trackClick(lParam);
        switch (button_id)
        {
        case CUSTOM_BUTTON_RED:
        {
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(RED);
        }
        break;
        case CUSTOM_BUTTON_BLACK:
        {
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(BLACK);
        }
        break;
        default:
            break;
        }
    }
    break;
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // 메뉴 선택을 구문 분석합니다:

        //임시 버튼 이벤트 처리를 모아 놓은 switch문 입니다. 임시 버튼의 이벤트 처리는 모두 여기서 담당합니다.
        //충돌을 방지하기 위해 이 부분을 사용하여 테스트한 후에는 반드시 원상태로 복구해야 합니다.
        //원상태의 예시는 아래와 같습니다.
        //case ID_PEN_STYLE1:
        //(TAB)break;
        switch (wmId)
        {
        case ID_PEN_STYLE1:
        {
            draw->setIsPen(true);
            /// SOLIDPEN 
            draw->pen_style->selectPen(SOLIDPEN);
        }
        break;
        case ID_PEN_STYLE2:
        {
            draw->setIsPen(true);
            /// SPRAYPEN
            draw->pen_style->selectPen(SPRAYPEN);
        }
        break;
        case ID_PEN_STYLE3:
        {
            draw->setIsPen(true);
            /// BRUSHPEN
            draw->pen_style->selectPen(BRUSHPEN);
        }
        break;
        case ID_PEN_STYLE4:
        {
            draw->setIsPen(true);
            /// HIGHLIGHTERPEN
            draw->pen_style->selectPen(HIGHLIGHTERPEN);
        }
        break;
        case ID_PEN_STYLE5:
        {
            /// 전체 지우개
            ac.all_clear(canvas_hwnd, *draw);
        }
        break;
        case ID_PEN_COLOR1:
        {
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(BLACK);
        }
        break;
        case ID_PEN_COLOR2:
        {
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(RED);
        }
        break;
        case ID_PEN_COLOR3:
        {
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(GREEN);
        }
        break;
        case ID_PEN_COLOR4:
        {
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(BLUE);
        }
        break;
        case ID_PEN_COLOR5:
        {
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(YELLOW);
        }
        break;
        case ID_ERASER:
        {
            /// 획 지우개
            draw->setIsPen(false);

        }
        break;
        case ID_REPLAY:
        {
            /// 임시 주석 추후 적용 예정 
            /*int state = replay.getReplayState();
            switch (state) {
            case replayStateStop:
                replay.startReplay(draw, hWnd);
                break;
            case replayStatePlaying:
                replay.pauseReplay();
                break;
            case replayStatePaused:
                replay.startReplay(draw, hWnd);
                break;
            }*/
        }
        break;
        case ID_SAVE_FILE: {

            file.SaveFile(hWnd, *draw);
            break;
        }
        case ID_LOAD_FILE:
        {
            file.LoadFile(canvas_hwnd, *draw);
            break;
        }
        }

        switch (wmId)
        {
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_SIZE:
    {
        button.resizeAllButtons();
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        button.drawAllButtons(hdc);
        EndPaint(hWnd, &ps);
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}




ATOM WinUi::CanvasRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex = { 0 };


    wcex.cbSize = sizeof(WNDCLASSEX); // 구조체의 크기 설정 
    wcex.style = CS_HREDRAW | CS_VREDRAW; // 창의 크기가 변하면 다시 그리기, 입력 메시지 관련 설정
    wcex.lpfnWndProc = StaticCanvasWndProc; // 이 창에서 실행될 메시지 처리 함수 지정 
    wcex.cbClsExtra = 0; // 클래스 여유 메모리. 보통은 0
    wcex.cbWndExtra = 0; // 윈도우 여유 메모리. 보통은 0 
    wcex.hInstance = hInstance; // 인스턴스 핸들 

    wcex.hCursor = LoadCursor(nullptr, IDC_ARROW); // 마우스 커서 스타일 
    wcex.hbrBackground = canvus_background; // 창 배경색 
    wcex.lpszClassName = L"CanvasWindowClassType"; // 이 윈도우 창 이름 

    return RegisterClassExW(&wcex);
}


LRESULT CALLBACK WinUi::StaticCanvasWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return static_winui->CanvasWndProc(hWnd, message, wParam, lParam);
}


LRESULT CALLBACK WinUi::CanvasWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
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
        /// 펜 스타일에 따라 그리기 스타일 변경 ( penStyle 테스트 진행중 ... )
        draw->selectDrawStyle();
        draw->startDrawingLine(hWnd, lParam, draw->select_drawStyle);
        break;
    }
    case WM_MOUSEMOVE:
    {
        draw->drawingLine(hWnd, lParam, draw->select_drawStyle);
        break;
    }
    case WM_LBUTTONUP:
    {
        draw->endDrawingLine(hWnd, lParam, draw->select_drawStyle);
        break;
    }
    case WM_MOUSELEAVE:
    {
        draw->endDrawingLine(hWnd, lParam, draw->select_drawStyle);
        break;
    }
    case WM_MOUSEWHEEL:
    {

        int mouse_wheel_data = GET_WHEEL_DELTA_WPARAM(wParam);

        if (mouse_wheel_data > 0)
        {
            /// 휠을 위로 올렸을 때의 처리
            draw->pen_style->settingPenThicknessUp();
        }
        else
        {
            // 휠을 아래로 내렸을 때의 처리
            draw->pen_style->settingPenThicknessDown();
        }
        break;
    }
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
        draw->drawWindowLines(hWnd, hdc);
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
