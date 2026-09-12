#include "WinUi.h"
#include "Replay.h"

/**
* @file
* @brief Replay 실행 함수
* @details 
*/

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