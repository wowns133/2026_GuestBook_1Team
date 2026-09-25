#include "WinUi.h"
#include "Replay.h"
void Replay::startReplay(Draw & draw, HWND targetHWnd)
{
    if (replayState == replayStatePlaying)
        return;

    drawObj = &draw;
    hwnd = targetHWnd;
    replayState = replayStatePlaying;

    HANDLE hThread = CreateThread(
        NULL,
        0,
        Replay::replayFunction,
        this,
        0,
        NULL
    );

    if (hThread != NULL) {
        CloseHandle(hThread);
    }
}
DWORD WINAPI Replay::replayFunction(LPVOID lpParam)
{
    Replay* replayObj = (Replay*)lpParam;

    if (replayObj != nullptr)
    {
        replayObj->runReplay();
    }

    return 0;
}

void Replay::runReplay()
{
    if (drawObj == nullptr || drawObj->pen_style == nullptr)
        return;

    HDC hdc = GetDC(hwnd);
    Gdiplus::Graphics graphics(hdc);

    graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

    // 화면을 하얗게 지움
    graphics.Clear(Gdiplus::Color::White);

    const std::vector<DrawLineData>& linesData =
        drawObj->getDrawnLines();

    for (UINT i = 0; i < (UINT)linesData.size(); i++)
    {
        if (replayState==replayStateStop)
            break;

        const DrawLineData& currentLineData = linesData[i];
        const std::vector<DrawPointData>& points =
            currentLineData.point_data;

        if (points.empty())
            continue;

        int select_pen = currentLineData.select_pen_style;
        int pen_color = currentLineData.select_color;
        float radius = (float)points[0].radius;

        drawObj->pen_style->settingPenStyle(
            select_pen,
            radius,
            pen_color
        );

        Gdiplus::Pen* currentPen =
            drawObj->pen_style->getPen();

        // 획 사이 시간
        if (i > 0 && !linesData[i - 1].point_data.empty())
        {
            const DrawPointData& prevLineLastPoint =
                linesData[i - 1].point_data.back();

            const DrawPointData& currentLineFirstPoint =
                points[0];

            ULONGLONG strokeDelay = 0;

            if (currentLineFirstPoint.elapsed_time >
                prevLineLastPoint.elapsed_time)
            {
                strokeDelay =
                    currentLineFirstPoint.elapsed_time -
                    prevLineLastPoint.elapsed_time;
            }

            if (strokeDelay > 1000)
                strokeDelay = 40;

            if (strokeDelay > 10)
                Sleep((DWORD)strokeDelay);
        }

        // 정지 요청 확인
        if (replayState==replayStateStop)
            break;

        // 점 하나짜리 획
        if (points.size() == 1)
        {
            if (currentPen != nullptr)
            {
                graphics.DrawLine(
                    currentPen,
                    (float)points[0].point.x,
                    (float)points[0].point.y,
                    (float)points[0].point.x + 0.001f,
                    (float)points[0].point.y
                );
            }

            continue;
        }

        // 점들을 순서대로 그림
        for (UINT j = 1; j < (UINT)points.size(); j++)
        {
            if (replayState == replayStateStop)
                break;

            if (currentPen != nullptr)
            {
                graphics.DrawLine(
                    currentPen,
                    (float)points[j - 1].point.x,
                    (float)points[j - 1].point.y,
                    (float)points[j].point.x,
                    (float)points[j].point.y
                );
            }

            ULONGLONG timeDiff =
                points[j].elapsed_time -
                points[j - 1].elapsed_time;

            if (timeDiff > 0)
                Sleep((DWORD)timeDiff);
        }
    }

    ReleaseDC(hwnd, hdc);

    // 리플레이 종료
    replayState = replayStateStop;

    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);
}
void Replay::stopReplay() {
    replayState = replayStateStop;
}

int Replay::getReplayState() {
    return replayState;
}