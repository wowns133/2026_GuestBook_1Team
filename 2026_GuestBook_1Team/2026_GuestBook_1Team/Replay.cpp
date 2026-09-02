#include "WinUi.h"
#include "Replay.h"

/**
* @file
* @brief Replay 실행 함수
* @details 
* @todo
* 1. 간단 리플레이만 넣을 것인가 리플레이 확장(타임랩스)도 구현할 것인가
* 2. 리플레이 확장 시 업데이트된 파일 내 모든 획을 기준으로
* 실행,일시정지,완료 를 설정할 수 있도록 한다
*/
void Replay::startReplay(Draw& draw, HWND targetHWnd) {

    //현재까지 그려진 객체를 가져옵니다.
    drawObj = &draw;
    hwnd = targetHWnd;

    //리플레이가 실행중임을 나타냅니다.
    replayState = replayStatePlaying;

    //창을 가져옵니다.
    HDC hdc = GetDC(hwnd);
    Gdiplus::Graphics graphics(hdc);

    //처음(백지)부터 그리는 효과를 위해 창을 하얗게 칠합니다.
    graphics.Clear(Gdiplus::Color::White);

    Gdiplus::Pen* pen = draw.pen_style->getPen();


    //HDC에 직접 실시간 선 긋기
    for (size_t i = 0; i < draw.drawn_lines.size(); i++) {
        const std::vector<DrawPointData>& currentLine = draw.drawn_lines[i];

        if (currentLine.size() < 2) continue;

        // 획과 획 사이 임의 딜레이 설정
        //실제 시간의 형태로도 구현 가능
        Sleep(80);

        for (size_t j = 1; j < currentLine.size(); j++) {
            graphics.DrawLine(
                pen,
                (INT)currentLine[j - 1].point.x,
                (INT)currentLine[j - 1].point.y,
                (INT)currentLine[j].point.x,
                (INT)currentLine[j].point.y
            );

            // 마우스 이동 시간차 계산
            ULONGLONG timeDiff = currentLine[j].elapsed_time - currentLine[j - 1].elapsed_time;

            //마우스 이동 시간차가 일정 시간을 넘어가거나 충족하지 못할 시 임의 시간으로 취급할 수 있도록 합니다.
            if (timeDiff > 40) timeDiff = 40;
            if (timeDiff < 10) timeDiff = 10;

            Sleep((DWORD)timeDiff);
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
//DWORD Replay::replayFunction(LPVOID lpParam) {
//	drawObj = &draw;
//	replayFunction* arg = (DrawPoint*)lpParam;
//}
