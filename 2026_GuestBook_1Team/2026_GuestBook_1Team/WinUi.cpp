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
    int main_ui_height = (rect.bottom - rect.top) * MAIN_UI_HEIGHT_RATIO;
    int canvus_ui_height = (rect.bottom - rect.top) - main_ui_height;

    main_ui_hwnd = CreateWindowExW(
        WS_EX_DLGMODALFRAME,        // 입체적인 이중 테두리를 적용한 대화 상자 형태의 프레임 스타일
        L"MainUIWindowClassType",                   // RegisterClass에서 등록한 클래스 이름
        nullptr,                                    // 창 상단 타이틀 바에 표시될 텍스트. 자식은 보통 없음
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,    // 윈도우 스타일 (순서대로 -> 자식 창 지정 | 화면에 즉시 표시 | 자식 창의 영역은 제외하고 그리기 | 겹쳐진 형제 창의 영역에 그리지 못하도록 제한)
        0, 0,                                       // 창이 나타날 처음 화면 위치 (x, y) - OS가 알아서 지정 
        rect.right, main_ui_height,                            // 창의 처음 가로, 세로 크기 (Width, Height) 
        hWnd,                                       // 부모 윈도우 핸들 
        (HMENU)MAIN_UI_ID,                          // 자식 윈도우 ID 
        hInst,                                      // 프로그램의 인스턴스 핸들 
        nullptr                                     // 추가 파라미터 
    );

    canvas_hwnd = CreateWindowExW(
        0,
        L"CanvasWindowClassType",                   // RegisterClass에서 등록한 클래스 이름
        nullptr, // 창 상단 타이틀 바에 표시될 텍스트. 
        WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS, // 윈도우 스타일 (순서대로 -> 자식 창 지정 | 화면에 즉시 표시 | 자식 창의 영역은 제외하고 그리기 | 겹쳐진 형제 창의 영역에 그리지 못하도록 제한) 
        0, 100, // 창이 나타날 처음 화면 위치 (X, Y) - OS가 알아서 지정 
        rect.right, canvus_ui_height, // 창의 처음 가로, 세로 크기 (Width, Height) - OS가 알아서 지정 
        hWnd, // 부모 윈도우 핸들 (메인 창이므로 없음) 
        (HMENU)CANVAS_ID, // 메뉴 핸들 
        hInst, // 이 창을 만드는 프로그램의 인스턴스 핸들 
        nullptr // 추가 파라미터 
    );

    // 화면 보호기 창 생성
    screen_saver_hwnd = screen_saver.createScreenSaverWindow(hWnd);
    // screen_saver 객체의 내부 핸들 변수 세팅
    screen_saver.setOtherWindowsHandle(root_window_hwnd, main_ui_hwnd, canvas_hwnd);
    screen_saver.setReplay(&replay);
    replay.setMainUiHandle(main_ui_hwnd);
}

void WinUi::resizeWinUIWindows(HWND hWnd, LPARAM lParam)
{
    //----------------------------------------사이즈 계산-------------------------------------//

    // 현재 창이 위치한 모니터 핸들 가져오기 (어느 모니터에도 없으면 가장 가까운 모니터 선택)
    // HMONITOR = 모니터의 핸들
    // MonitorFromWindow(hwnd, MONITOR_DEFAULTTONEAREST) = 지정한 창이(hWnd) 속해 있는 모니터의 핸들을 가져오는 함수
    // MONITOR_DEFAULTTONEAREST = 창이 완전히 화면 밖에 있거나 모니터 사이 애매한 위치에 있을 때 가장 가까운 모니터를 선택하도록 설정
    HMONITOR monitor_handle = MonitorFromWindow(hWnd, MONITOR_DEFAULTTONEAREST);

    // 모니터 정보 구조체 선언 후 일단 0으로 초기화
    // MONITORINFO = 모니터의 정보를 담는 구조체
    MONITORINFO monitorInfo = { 0 };

    // GetMonitorInfo 함수 실행 시 이 구조체가 MONITORINFO인지 MONITORINFOEX인지 판단하기 위한 구조체 크기 저장
    // 이 과정이 없으면 GetMonitorInfo 함수는 크기 정보가 올바르지 않다고 판단해 실패한다(return 0).
    monitorInfo.cbSize = sizeof(MONITORINFO);

    // 모니터 크기를 저장할 변수 선언 및 초기화
    int monitor_width = 0;                      ///< 현재 모니터의 너비
    int monitor_height = 0;                     ///< 현재 모니터의 높이

    // 모니터 정보 가져오기
    // GetMonitorInfo(monitor_handle, &monitorInfo) = 특정 모니터의(monitor_handle) 실제 사양과 좌표 정보를 구해 모니터 정보 구조체에(monitorInfo) 저장하는 함수
    if (GetMonitorInfo(monitor_handle, &monitorInfo)) {
        // monitorInfo.rcMonitor = 모니터 전체의 좌표 정보를 담은 RECT 구조체
        // 모니터의 전체 해상도 크기 계산 (큰 좌표 - 작은 좌표)
        monitor_width = monitorInfo.rcMonitor.right - monitorInfo.rcMonitor.left;
        monitor_height = monitorInfo.rcMonitor.bottom - monitorInfo.rcMonitor.top;
    }

    int monitor_x = monitorInfo.rcMonitor.left; ///< 현재 모니터의 좌상단 x 좌표
    int monitor_y = monitorInfo.rcMonitor.top;  ///< 현재 모니터의 좌상단 y 좌표

    // 현재 client의 크기 구하기
    int client_width = LOWORD(lParam); // client 가로 길이
    int client_height = HIWORD(lParam); // client 세로 길이


    //--------------------------창 재배치--------------------------//

    // 메인 UI와 캔버스를 위아래로 배치
    int main_ui_height = client_height * MAIN_UI_HEIGHT_RATIO;
    MoveWindow(canvas_hwnd, 0, main_ui_height, client_width, client_height - main_ui_height, TRUE);
    MoveWindow(main_ui_hwnd, 0, 0, client_width, main_ui_height, TRUE);

    // 화면 보호기 창의 크기를 화면 전체로 재조정(여러 개의 모니터에 대응 가능)
    // WS_POPUP 창이므로 x, y의 기준이 주 모니터의 좌상단입니다. 0, 0이 아닌 현재 윈도우의 좌상단을 x, y값으로 만들어야 한다.
    MoveWindow(screen_saver_hwnd, monitor_x, monitor_y, monitor_width, monitor_height, TRUE);


    // 화면 보호기 창의 z-order을 맨 위로
    SetWindowPos(screen_saver_hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    // 메인 UI 창의 z-order을 화면 보호기 창의 밑으로
    SetWindowPos(main_ui_hwnd, screen_saver_hwnd, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    // 캔버스 창의 z-order을 맨 아래로
    SetWindowPos(canvas_hwnd, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // 비트맵 크기 재조정
    draw->resizeBitmap(client_width, client_height);

    // 스크롤바 재조정
    pen_scroll.resizePenScrollBar(main_ui_hwnd);
}

void WinUi::resetScreenSaverTimer()
{
    screen_saver.resetScreenSaverTimer();
}

void WinUi::setInst(HINSTANCE hInst)
{
    this->hInst = hInst;
    pen_scroll.setInst(hInst);
    screen_saver.setInst(hInst);
}

void WinUi::setRootWindowHandle(HWND hWnd)
{
    this->root_window_hwnd = hWnd;
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
        //makeTempButton(hWnd, hInst);
        button.setAllButtons(hWnd);

        //펜 두께 스크롤바 함수 세팅
        pen_scroll.createPenScrollWindows(hWnd);
        pen_scroll.setPointers(draw, &replay);

        // 화면 보호기 타이머 최초 설정
        SetTimer(hWnd, SCREEN_SAVER_TIMER_ID, 1000, NULL);
    }
    break;
    case WM_REPLAY_ENDED:
    {
        // 재생 끝난 직후에 펜 스타일 세팅할 수 있도록
        if (draw->is_play_end == 1)
        {
            if (replay.getReplayState() == replayStateStop || replay.getReplayState() == replayStatePause)
            {
                // 펜 스타일 변경 실행 완료
                draw->is_play_end = 0;
                // 저장해둔 스타일 변경
                draw->pen_style->settingPenStyle(draw->temp_select_pen, draw->temp_radius, draw->temp_pen_color);
                // 스크롤바와 펜 샘플 화면 갱신
                InvalidateRect(pen_scroll.getScrollBarHandle(), NULL, FALSE);
                KillTimer(main_ui_hwnd, PEN_CHECK_TIMER_ID);
            }
        }
    }
    break;
    case WM_TIMER:
    {
        if (wParam == SCREEN_SAVER_TIMER_ID)
        {
            // 타이머 처리 함수
            screen_saver.screenSaverTimer();
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        // 화면보호기 실행까지의 시간 초기화
        screen_saver.resetScreenSaverTimer();

        // 버튼 클릭을 추적해 클릭한 버튼이 있으면 버튼 ID를 저장
        int button_id = button.trackClick(lParam);

        // 버튼이 무엇인지에 따라 알맞게 처리
        if (button_id == -1) break;

        // Replay 실행 중 다른 기능을 선택했다면 Replay 종료
        //if (replay.getReplayState() == replayStatePlaying &&
        //    button_id != CUSTOM_BUTTON_PLAY)
        //{
        //    replay.stopReplay();
        //}
        // button_id가 -1이 아니다 == 버튼을 눌렀다
        // 펜 스타일이 바뀌는 버튼을 누른 것이라면 스크롤바 최신화
        if (button_id != -1 &&
            button_id != CUSTOM_BUTTON_PLAY &&
            button_id != CUSTOM_BUTTON_STOP &&
            button_id != CUSTOM_BUTTON_ERASER_STROKE &&
            button_id != CUSTOM_BUTTON_ERASER_ALL &&
            button_id != CUSTOM_BUTTON_SAVE &&
            button_id != CUSTOM_BUTTON_LOAD)
        {
            // 스크롤바와 펜 샘플 화면 갱신
            InvalidateRect(pen_scroll.getScrollBarHandle(), NULL, FALSE);
        }


        if (button_id >= CUSTOM_BUTTON_PEN_SOLID && button_id <= CUSTOM_BUTTON_PEN_HIGHLIGHT)
        {
            button.setSelectedPenType(button_id);
            button.setEraserMode(false);     // 펜을 고르면 펜 모드
        }
        else if (button_id >= CUSTOM_BUTTON_BLACK && button_id <= CUSTOM_BUTTON_YELLOW)
        {
            button.setSelectedColor(button_id);
            button.setEraserMode(false);     // 색을 고르면 펜 모드로 복귀
        }
        else if (button_id == CUSTOM_BUTTON_ERASER_STROKE)
        {
            button.setEraserMode(true);
        }

        InvalidateRect(hWnd, NULL, FALSE);


        switch (button_id)
        {
        case CUSTOM_BUTTON_PEN_SOLID:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_select_pen = SOLIDPEN;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPen(SOLIDPEN);
        }
        break;
        case CUSTOM_BUTTON_PEN_SPRAY:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_select_pen = SPRAYPEN;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPen(SPRAYPEN);
        }
        break;
        case CUSTOM_BUTTON_PEN_BRUSH:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_select_pen = BRUSHPEN;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPen(BRUSHPEN);
        }
        break;
        case CUSTOM_BUTTON_PEN_HIGHLIGHT:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_select_pen = HIGHLIGHTERPEN;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPen(HIGHLIGHTERPEN);
        }
        break;

        // --- 2. 지우개 종류 (2종) ---
        case CUSTOM_BUTTON_ERASER_STROKE: // 획 지우기
        {
            draw->setIsPen(false);
        }
        break;
        case CUSTOM_BUTTON_ERASER_ALL: // 전체 지우기
        {
            // Replay 실행 중 지우기 했다면 Replay 중단
            if (replay.getReplayState() != replayStateStop)
            {
                replay.stopReplay();
            }

            ac.all_clear(canvas_hwnd, *draw);
            InvalidateRect(canvas_hwnd, NULL, TRUE); // 지운 후 캔버스 갱신
        }
        break;
        // --- 3. 리플레이 (2종) ---
        case CUSTOM_BUTTON_PLAY:
        {
            // 이미 재생 중이라면 재실행 방지
            if (replay.getReplayState() == replayStatePlaying)
            {
                break;
            }
                
            // 기존 펜 스타일 저장
            draw->temp_select_pen = draw->pen_style->getSelectPen();
            draw->temp_radius = draw->pen_style->getRadius();
            draw->temp_pen_color = draw->pen_style->getPenColor();
            // 플레이 시작했다는 플래그
            draw->is_play_end = 1;

            // 리플레이 시작
            replay.startReplay(*draw, canvas_hwnd);

            SetTimer(hWnd, PEN_CHECK_TIMER_ID, 100, NULL);

        }
        break;
        case CUSTOM_BUTTON_STOP:
        {


            replay.stopReplay();
            // 재생 대상 창 갱신
            if (canvas_hwnd != NULL)
            {
                InvalidateRect(canvas_hwnd, NULL, FALSE);
            }
        }
        break;
        case CUSTOM_BUTTON_SAVE:
        {
            // 기존 리플레이가 실행 중이었는지 확인
            bool wasReplayPlaying = (replay.getReplayState() == replayStatePlaying);

            // 기존 리플레이 일시정지
            if (wasReplayPlaying)
                replay.pauseReplay();
            if (!file.SaveFile(canvas_hwnd, *draw))
            {
                if (wasReplayPlaying)
                    replay.resumeReplay();

                break;
            }
            //file.SaveFile(canvas_hwnd, *draw);

            replay.stopReplay();
        }
        break;
        case CUSTOM_BUTTON_LOAD:
        {
            std::wstring filePath;

            // 기존 리플레이가 실행 중이었는지 확인
            bool wasReplayPlaying = (replay.getReplayState() == replayStatePlaying);

            // 기존 리플레이 일시정지
            if (wasReplayPlaying)
                replay.pauseReplay();
            //draw->clearDrawnBitmap();

            // 파일 선택 창 열기
            if (!file.SelectLoadFile(canvas_hwnd, filePath))
            {
                // 파일 선택 취소 시 기존 리플레이 재개
                if (wasReplayPlaying)
                    replay.resumeReplay();

                break;
            }

            // 새로운 파일을 선택했다면 기존 리플레이 스레드 종료
            replay.stopReplay();
            
            bool isLoaded = file.LoadFile(filePath, *draw);

            if (isLoaded)
            {
                // 기존 원본 비트맵도 흰색으로 초기화
                //draw->clearDrawnBitmap();
                // 기존 펜 스타일 저장
                if (!wasReplayPlaying)
                {
                    draw->temp_select_pen = draw->pen_style->getSelectPen();
                    draw->temp_radius = draw->pen_style->getRadius();
                    draw->temp_pen_color = draw->pen_style->getPenColor();
                }
                // 플레이 시작했다는 플래그
                draw->is_play_end = 1;

                // 새로운 파일 리플레이
                replay.startReplay(*draw, canvas_hwnd);
                SetTimer(hWnd, PEN_CHECK_TIMER_ID, 100, NULL);

            }
        }
        break;
        case CUSTOM_BUTTON_RED:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_pen_color = RED;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(RED);
        }
        break;
        case CUSTOM_BUTTON_BLACK:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_pen_color = BLACK;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(BLACK);
        }
        break;
        case CUSTOM_BUTTON_GREEN:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_pen_color = GREEN;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(GREEN);
        }
        break;
        case CUSTOM_BUTTON_BLUE:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_pen_color = BLUE;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(BLUE);
        }
        break;
        case CUSTOM_BUTTON_YELLOW:
        {
            if (replay.getReplayState() == replayStatePlaying)
            {
                draw->setIsPen(true);
                draw->temp_pen_color = YELLOW;
                break;
            }
            draw->setIsPen(true);
            draw->pen_style->selectPenColor(YELLOW);
        }
        break;
        default:
            break;
        }
    }
    break;
    // 마우스 움직임이 있을 때, 화면 보호기 실행까지의 시간을 초기화
    case WM_KEYDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MOUSEMOVE:
    {
        // 화면보호기 실행까지의 시간 초기화
        screen_saver.resetScreenSaverTimer();
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
            file.SaveFile(canvas_hwnd, *draw);
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
        button.resizeAllButtons(hWnd);
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
    case WM_TIMER:
    {
        /// 붓 펜 한곳에 머무를 시 두께 늘리기
        if (wParam == BRUSH_TIMER)
        {
            /// 타이머 플래그 변수가 true일 시 두께 늘리기
            if (draw->pen_style->getTimerFlag())
            {
                draw->pen_style->brushAnchorPointThickUp();
                wchar_t buffer[100];
                swprintf_s(buffer, L"현재 반지름: %lf\n", draw->pen_style->getBrushPenRadius());

                OutputDebugString(buffer);
                draw->updateBrushAnchorPoint(hWnd, lParam);
            }
        }
    }
    break;
    case WM_LBUTTONDOWN:
    {
        // Replay 실행 중이면 먼저 종료시키기
        if (replay.getReplayState() == replayStatePlaying)
        {
            replay.stopReplay();
            InvalidateRect(canvas_hwnd, NULL, FALSE);
            UpdateWindow(canvas_hwnd);
            draw->pen_style->settingPenStyle(draw->temp_select_pen, draw->temp_radius, draw->temp_pen_color);

        }
        // 화면보호기 실행까지의 시간 초기화
        screen_saver.resetScreenSaverTimer();

        if (draw->is_pen == false)
        {
                draw->temp_select_pen = draw->pen_style->getSelectPen();
                draw->temp_radius = draw->pen_style->getRadius();
                draw->temp_pen_color = draw->pen_style->getPenColor();
        }

        /// 펜 스타일에 따라 그리기 스타일 변경 ( penStyle 테스트 진행중 ... )
        draw->selectDrawStyle();
        draw->startDrawingLine(hWnd, lParam, draw->select_drawStyle);
        break;
    }
    case WM_MOUSEMOVE:
    {
        // 화면보호기 실행까지의 시간 초기화
        screen_saver.resetScreenSaverTimer();

        draw->drawingLine(hWnd, lParam, draw->select_drawStyle);
        break;
    }
    case WM_LBUTTONUP:
    {
        // 화면보호기 실행까지의 시간 초기화
        screen_saver.resetScreenSaverTimer();
        int was_drawing = false;
        if (draw->is_drawing == true)
        {
            was_drawing = true;
        }

        draw->endDrawingLine(hWnd, lParam, draw->select_drawStyle);

        /// 붓 펜 한곳에 머무른 뒤 마우스 땠을 시 플래그 변수 false 지정
        draw->pen_style->setTimerFlag(false);
        if (was_drawing == true)
        {
            was_drawing = false;
            if (draw->is_pen == false && replay.getReplayState() == replayStateStop)
            {
                draw->pen_style->settingPenStyle(draw->temp_select_pen, draw->temp_radius, draw->temp_pen_color);
            }
        }
        break;
    }
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_KEYDOWN:
    {
        // 화면보호기 실행까지의 시간 초기화
        screen_saver.resetScreenSaverTimer();
    }
    break;
    case WM_MOUSELEAVE:
    {
        int was_drawing = false;
        if (draw->is_drawing == true)
        {
            was_drawing = true;
        }

        draw->endDrawingLine(hWnd, lParam, draw->select_drawStyle);
        if (was_drawing == true)
        {
            was_drawing = false;
            if (draw->is_pen == false && replay.getReplayState() == replayStateStop)
            {
                draw->pen_style->settingPenStyle(draw->temp_select_pen, draw->temp_radius, draw->temp_pen_color);
            }
        }
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

        if (replay.getReplayState() == replayStateStop)
        //{
            draw->drawWindowLines(hWnd, hdc);
        //}

        EndPaint(hWnd, &ps);
    }
    break;
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

void WinUi::setRegisterClass(HINSTANCE hInstance)
{
    MainUIRegisterClass(hInstance); // 메인 UI창 등록
    CanvasRegisterClass(hInstance); // 캔버스 창 등록
    screen_saver.ScreenSaverRegisterClass(hInstance); // 화면 보호기 창 등록
    pen_scroll.PenScrollRegisterClass(hInstance); // 스크롤바 창 등록
}