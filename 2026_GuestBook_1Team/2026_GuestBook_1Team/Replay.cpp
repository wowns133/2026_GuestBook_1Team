#include "WinUi.h"
#include "Replay.h"

/**
* @file
* @brief Replay 실행 함수
* @details 
*/

//~~~~~~~~~~~구조체에 펜 정보가 아직 안 들어갔으며 실행과 끝만 존재하는 버전입니다~~~~~~~//

void Replay::startReplay(Draw& draw, HWND targetHWnd) {

    if (replayState == replayStatePlaying) return;//이미 재생 중인 상태라면 중복 실행되지 않도록 합니다.

    //현재까지 그려진 객체를 가져옵니다.
    drawObj = &draw;
    hwnd = targetHWnd;
    isStopRequested = false;

    //리플레이가 실행중임을 나타냅니다.
    replayState = replayStatePlaying;

    //CreateThread를 실행합니다
    //자기 자신 객체 포인터(this)를 lpParam으로 전달합니다.
    HANDLE hThread = CreateThread(
        NULL,
        0,
        Replay::replayFunction,
        this,
        0,
        NULL
    );

    //스레드 핸들을 닫아줌으로써 자원 정리
    if (hThread != NULL) {
        CloseHandle(hThread);
    }
}

DWORD WINAPI Replay::replayFunction(LPVOID lpParam) {
    Replay* replayObj = (Replay*)lpParam;
    if (replayObj != nullptr) {
        replayObj->runReplay();
    }
    return 0;

}

void Replay::runReplay() {
    if (drawObj == nullptr || drawObj->pen_style == nullptr) return;
    
    //창을 가져옵니다.
    HDC hdc = GetDC(hwnd);
    Gdiplus::Graphics graphics(hdc);

    //처음(백지)부터 그리는 효과를 위해 창을 하얗게 칠합니다.
    graphics.Clear(Gdiplus::Color::White);

    Gdiplus::Pen* pen = drawObj->pen_style->getPen();

    if (pen != nullptr) {
        //HDC에 직접 실시간 선 긋기
        for (UINT i = 0; i < (UINT)drawObj->drawn_lines.size(); i++) {
            if (isStopRequested) break;

            const std::vector<DrawPointData>& currentLine = drawObj->drawn_lines[i];

            if (currentLine.size() < 2) continue;

            // 획과 획 사이 임의 딜레이 설정
            //Sleep(80);

            //실제 시간 반영 딜레이
            //획과 획 사이의 시간 반영
            if (i > 0 && !drawObj->drawn_lines[i - 1].empty()) {
                const DrawPointData& prevLineLastPoint = drawObj->drawn_lines[i - 1].back();
                const DrawPointData& currentLineFirstPoint = currentLine[0];

                ULONGLONG strokeDelay = 0;

                //이전 획의 끝 점과 현재 획의 시작 점 사이의 시간 반영
                if (currentLineFirstPoint.elapsed_time > prevLineLastPoint.elapsed_time) {
                    strokeDelay = currentLineFirstPoint.elapsed_time - prevLineLastPoint.elapsed_time;

                }
                //마우스 이동 시간차가 일정 시간을 넘어가거나 충족하지 못할 시 임의 시간으로 취급할 수 있도록 합니다.
                if (strokeDelay > 1000) {
                    strokeDelay = 40;
                }
                if (strokeDelay > 10) {
                    Sleep((DWORD)strokeDelay);
                }

            }

            //한 획 안에서 점과 점 사이 실제 그리기 속도 반영
            for (UINT j = 1; j < (UINT)currentLine.size(); j++) {
                if (isStopRequested) break;

                graphics.DrawLine(
                    pen,
                    (INT)currentLine[j - 1].point.x,
                    (INT)currentLine[j - 1].point.y,
                    (INT)currentLine[j].point.x,
                    (INT)currentLine[j].point.y
                );
                // 두 점 사이의 마우스 이동 시간차 계산
                ULONGLONG timeDiff = currentLine[j].elapsed_time - currentLine[j - 1].elapsed_time;
                if (timeDiff > 0) {
                    Sleep((DWORD)timeDiff);
                }

            }
        }
    }
    ReleaseDC(hwnd, hdc);

    //리플레이가 종료됨을 나타냅니다.
    replayState = replayStateStop;

    //Draw 클래스의 함수를 사용한 전체 화면 정상 갱신
    // InvalidateRect를 호출하여 WM_PAINT를 발생시키거나 drawWindowLines를 직접 수행합니다.
    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);
}

void Replay::stopReplay() {
    isStopRequested = true;
}








//~~~~~~~~~~구조체에 펜 정보가 들어갔으며 실행 일시정지 중지가 존재하는 버전입니다~~~~~~~~//
/*구조체 임의 조작 (in Draw.h)
struct DrawPointData
{
    POINT point; ///< 좌표를 저장하는 변수
    ULONGLONG elapsed_time; ///< 시간을 저장하는 변수
    bool is_pen = TRUE;///< 그리는 중인지 지우개 쓰는 중인지 판별. 그리기 TRUE, 지우개 FALSE

    ///test(임의의 구조체 정보 저장)
    // --- PenStyle에서 추출한 펜 정보 ---
    int select_pen;         ///< 펜 종류 (예: SOLIDPEN, DASH_PEN 등)
    float radius;           ///< 펜 두께 (또는 pen_width)
    int pen_color;          ///< 펜 색상 열거형/ID (예: BLACK, RED, BLUE 등)

    // (선택) 만약 Custom Color나 ARGB/반투명(형광펜)을 지원한다면 추가
    COLORREF custom_color;  ///< RGB 실재 색상 값
    BYTE alpha = 255;       ///< 투명도 (255: 불투명, 형광펜 등은 128 이하)
};
*/
/*정보 임의 조작에 따른 정보 삽입1(in Draw.cpp)
* void Draw::startDrawingLine(HWND hWnd, LPARAM lParam)
{
	//그리기 환경 제작
	draw_hdc = GetDC(hWnd); //hdc 발행. 사용 후 회수해야 함
	draw_hdc_graphics = new Gdiplus::Graphics(draw_hdc); //화면 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
	draw_hdc_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	GetClientRect(hWnd, &client_rect); //client_rect에 현재 작업영역 크기 구하기
	draw_bmp = new Gdiplus::Bitmap(client_rect.right, client_rect.bottom, PixelFormat32bppARGB); //더블 버퍼링 구현을 위한 Bitmap 객체 생성. 사용 후 삭제해야 함
	draw_bmp_graphics = new Gdiplus::Graphics(draw_bmp); //비트맵 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
	draw_bmp_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);				// 안티엘리어싱 적용. 선에서 계단 현상이 줄어든다.


	//선을 그리기 시작하는 좌표를 현재 좌표로 초기화
	previous_x = LOWORD(lParam); //lParam의 하위 16비트를 가져와 x좌표로 저장 
	previous_y = HIWORD(lParam); //lParam의 상위 16비트를 가져와 y좌표로 저장


	start_time = GetTickCount64(); //윈도우 창이 켜진 시점부터 흐르는 시간


	///임의의 정보(펜 구조체 정보) 삽입///
	// ★ 현재 pen_style 정보 가져와서 구조체 채우기
	DrawPointData startPt;
	startPt.point = { previous_x, previous_y };
	startPt.elapsed_time = 0;
	startPt.is_pen = true;
	startPt.select_pen = pen_style->getSelectPen();
	startPt.radius = pen_style->getRadius();
	startPt.pen_color = pen_style->getPenColor();

	drawn_line.push_back(startPt);    ... 이후의 코드는 원본과 동일합니다
*/

/*정보 임의 조작에 따른 정보 삽입2(in Draw.cpp)
void Draw::drawingLineRGB(HWND hWnd, LPARAM lParam)
{
    if (is_drawing)
    {
        //이동 후 현재 위치를 저장
        current_x = LOWORD(lParam);
        current_y = HIWORD(lParam);

        ULONGLONG elapsed_time = GetTickCount64() - start_time; // 경과 시간 계산

        ///임의의 정보 삽입(펜 구조체 정보)///
        // ★ 현재 pen_style 정보 가져와서 저장
        DrawPointData drawPt;
        drawPt.point = { current_x, current_y };
        drawPt.elapsed_time = elapsed_time;
        drawPt.is_pen = true;
        drawPt.select_pen = pen_style->getSelectPen();
        drawPt.radius = pen_style->getRadius();
        drawPt.pen_color = pen_style->getPenColor();

        drawn_line.push_back(drawPt);

        ... 이 후의 코드는 원본과 동일합니다
*/

/*정보 임의 조작에 따른 정보 삽입3(in Draw.cpp)
void Draw::drawingLineARGB(HWND hWnd, LPARAM lParam)
{
    if (is_drawing)
    {
        //이동 후 현재 위치를 저장
        current_x = LOWORD(lParam);
        current_y = HIWORD(lParam);

        ULONGLONG elapsed_time = GetTickCount64() - start_time; // 경과 시간 계산


        ///임의의 정보 저장(펜 구조체 정보 삽입)///
        // ★ 현재 pen_style 정보 가져와서 저장
        DrawPointData drawPt;
        drawPt.point = { current_x, current_y };
        drawPt.elapsed_time = elapsed_time;
        drawPt.is_pen = true;
        drawPt.select_pen = pen_style->getSelectPen();
        drawPt.radius = pen_style->getRadius();
        drawPt.pen_color = pen_style->getPenColor();

        drawn_line.push_back(drawPt);

        line_path->AddLine(previous_x, previous_y, current_x, current_y);

        ... 이후의 코드는 원본과 동일합니다
*/

/*
void Replay::startReplay(Draw& draw, HWND targetHWnd) {

    if (replayState == replayStatePlaying) return;//이미 재생 중인 상태라면 중복 실행되지 않도록 합니다.

    //현재까지 그려진 객체를 가져옵니다.
    drawObj = &draw;
    hwnd = targetHWnd;
    isStopRequested = false;

    //CreateThread를 실행합니다
    //자기 자신 객체 포인터(this)를 lpParam으로 전달합니다.
    HANDLE hThread = CreateThread(
        NULL,
        0,
        Replay::replayFunction,
        this,
        0,
        NULL
    );

    //스레드 핸들을 닫아줌으로써 자원 정리
    if (hThread != NULL) {
        CloseHandle(hThread);
    }
}

DWORD WINAPI Replay::replayFunction(LPVOID lpParam) {
    Replay* replayObj = (Replay*)lpParam;
    if (replayObj != nullptr) {
        replayObj->runReplay();
    }
    return 0;

}

void Replay::runReplay(){
    if (drawObj == nullptr || drawObj->pen_style == nullptr) return;



    //처음(백지)부터 그리는 효과를 위해 창을 하얗게 칠합니다.
    //graphics.Clear(Gdiplus::Color::White);

    //정지상태에서 새로 시작될 때만 인덱스를 리셋하고 화면을 하얗게 지웁니다.
    //일시정지상태에서 재개할 때는 이전 pauseLineIdx, pausePointIdx를 유지합니다.
    if (replayState == replayStateStop) {
        pauseLineIdx = 0;
        pausePointIdx = 1;

        HDC hdc = GetDC(hwnd);
        Gdiplus::Graphics graphics(hdc);
        graphics.Clear(Gdiplus::Color::White);
        ReleaseDC(hwnd, hdc);
    }

    // 재생 상태로 전환
    replayState = replayStatePlaying;

    //창을 가져옵니다.
    HDC hdc = GetDC(hwnd);
    Gdiplus::Graphics graphics(hdc);
    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);



    //HDC에 직접 실시간 선 긋기
    for (UINT i = (UINT)pauseLineIdx; i < (UINT)drawObj->drawn_lines.size(); i++) {
        if (isStopRequested) break;  //정지 요청 시 중단됩니다.

        // 재생 도중 외부에서 pauseReplay()가 호출되어 일시정지 상태가 된 경우
        if (replayState == replayStatePaused) {
            pauseLineIdx = i; // 현재 획 위치 저장
            break;
        }

        //i번째 선의 데이터를 가져옵니다.
        std::vector<DrawPointData> currentLine = drawObj->drawn_lines[i];
        if (currentLine.size() < 2) continue;

        //첫번째 획에서의 펜 정보는 첫번째 점에 저장된 펜 정보로 재설정합니다.
        int select_pen = currentLine[0].select_pen;
        float radius = currentLine[0].radius;
        int pen_color = currentLine[0].pen_color;

        drawObj->pen_style->settingPenStyle(select_pen, radius, pen_color);
        Gdiplus::Pen* currentPen = drawObj->pen_style->getPen();

        // 획과 획 사이 임의 딜레이 설정
        // Sleep(80);

        //실제 시간 반영 딜레이
        //획과 획 사이의 시간 반영(일시정지 후 첫 획이 아닐 때만 적용)
        if (i > (UINT)pauseLineIdx && i > 0 && !drawObj->drawn_lines[i - 1].empty()) {
            const DrawPointData& prevLineLastPoint = drawObj->drawn_lines[i - 1].back();
            const DrawPointData& currentLineFirstPoint = currentLine[0];

            ULONGLONG strokeDelay = 0;

            //이전 획의 끝 점과 현재 획의 시작 점 사이의 시간 반영
            if (currentLineFirstPoint.elapsed_time > prevLineLastPoint.elapsed_time) {
                strokeDelay = currentLineFirstPoint.elapsed_time - prevLineLastPoint.elapsed_time;

            }
            //마우스 이동 시간차가 일정 시간을 넘어가거나 충족하지 못할 시 임의 시간으로 취급할 수 있도록 합니다.
            if (strokeDelay > 1000) {
                strokeDelay = 40;
            }
            if (strokeDelay > 10) {
                Sleep((DWORD)strokeDelay);
            }

        }
        // 일시정지했던 점(pausePointIdx)부터 시작
        UINT startJ = (i == (UINT)pauseLineIdx) ? (UINT)pausePointIdx : 1;

        //점 처리 루프
        // ★ 단 하나의 점 처리 루프만 실행
        for (UINT j = startJ; j < (UINT)currentLine.size(); j++) {
            if (isStopRequested) break;

            // 점을 그리던 도중 일시정지 요청을 받은 경우
            if (replayState == replayStatePaused) {
                pauseLineIdx = i;  // 현재 획 위치 저장
                pausePointIdx = j; // 현재 점 위치 저장
                break;
            }

            if (currentPen != nullptr) {
                graphics.DrawLine(
                    currentPen,
                    (INT)currentLine[j - 1].point.x,
                    (INT)currentLine[j - 1].point.y,
                    (INT)currentLine[j].point.x,
                    (INT)currentLine[j].point.y
                );
            }
            //두 점 사이의 마우스 이동 시간차 계산
            ULONGLONG timeDiff = currentLine[j].elapsed_time - currentLine[j - 1].elapsed_time;
            if (timeDiff > 0) {
                Sleep((DWORD)timeDiff);
            }

        }
        // 한 획이 무사히 끝나면 다음 획은 1번 점부터 시작하도록 초기화
        pausePointIdx = 1;

        // 점 루프 탈출 사유가 일시정지인 경우 획 루프도 탈출
        if (replayState == replayStatePaused) break;
    }

    ReleaseDC(hwnd, hdc);

    //리플레이가 종료됨을 나타냅니다.
    //replayState = replayStateStop;

    //일시정지가 아닌 상태로 종료되었으면(끝까지 재생했거나 완전 정지된 경우) 정지 상태로 전환
    if (replayState != replayStatePaused) {
        replayState = replayStateStop;
        pauseLineIdx = 0;
        pausePointIdx = 1;

        //전체 화면 정상 갱신
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);

        //Draw 클래스의 함수를 사용한 전체 화면 정상 갱신
        //InvalidateRect를 호출하여 WM_PAINT를 발생시키거나 drawWindowLines를 직접 수행합니다.
        InvalidateRect(hwnd, NULL, FALSE);
        UpdateWindow(hwnd);
    }
}

void Replay::stopReplay() {
    isStopRequested = true;
}

int Replay::getReplayState() {
    return replayState;
}

void Replay::pauseReplay() {
    if (replayState == replayStatePlaying) {
        replayState = replayStatePaused;
    }
}
*/
