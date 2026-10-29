#include "Replay.h"

#include <Windows.h>
#include <gdiplus.h>
#include <cstdlib>
#include <ctime>

using namespace Gdiplus;

void Replay::startReplay(Draw& draw, HWND targetHWnd)
{
    if (replayState == replayStatePlaying)
        return;

    drawObj = &draw;
    hwnd = targetHWnd;

    replayState = replayStatePlaying;

    //Replay Thread 생성
    HANDLE hThread = CreateThread(
        NULL,
        0,
        Replay::replayFunction,
        this,
        0,
        NULL
    );

    if (hThread != NULL)
    {
        CloseHandle(hThread);
    }
    else
    {
        // Thread 생성 실패
        replayState = replayStateStop;
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
    if (drawObj == nullptr ||
        drawObj->pen_style == nullptr ||
        hwnd == NULL)
    {
        replayState = replayStateStop;
        return;
    }

    int savedPen = drawObj->pen_style->getSelectPen();
    float savedRadius = drawObj->pen_style->getRadius();
    int savedColor = drawObj->pen_style->getPenColor();

    RECT rect;

    if (!GetClientRect(hwnd, &rect))
    {
        replayState = replayStateStop;
        return;
    }

    int width = rect.right - rect.left;
    int height = rect.bottom - rect.top;

    if (width <= 0 || height <= 0)
    {
        replayState = replayStateStop;
        return;
    }

    replay_canvas_bmp =
        new Bitmap(width, height, PixelFormat32bppARGB);

    replay_canvas_graphics =
        new Graphics(replay_canvas_bmp);

    replay_canvas_graphics->SetSmoothingMode(
        SmoothingModeAntiAlias
    );

    replay_canvas_graphics->Clear(Color::White);

    replay_bmp =
        new Bitmap(width, height, PixelFormat32bppARGB);

    replay_bmp_graphics =
        new Graphics(replay_bmp);

    replay_bmp_graphics->SetSmoothingMode(
        SmoothingModeAntiAlias
    );

    replay_bmp_graphics->Clear(Color::White);

    HDC hdc = GetDC(hwnd);

    if (hdc == NULL)
    {
        delete replay_bmp_graphics;
        replay_bmp_graphics = nullptr;

        delete replay_bmp;
        replay_bmp = nullptr;

        delete replay_canvas_graphics;
        replay_canvas_graphics = nullptr;

        delete replay_canvas_bmp;
        replay_canvas_bmp = nullptr;

        replayState = replayStateStop;
        return;
    }

    Graphics graphics(hdc);

    graphics.SetSmoothingMode(SmoothingModeAntiAlias);

    graphics.DrawImage(replay_canvas_bmp, 0, 0);

    srand((unsigned)time(NULL));

    const std::vector<DrawLineData>& linesData =
        drawObj->getDrawnLines();

    for (size_t i = 0; i < linesData.size(); i++)
    {
        if (replayState == replayStateStop)
            break;

        const DrawLineData& currentLineData = linesData[i];

        const std::vector<DrawPointData>& points =
            currentLineData.point_data;

        if (points.empty())
            continue;

        int select_pen = currentLineData.select_pen_style;
        int pen_color = currentLineData.select_color;

        drawObj->pen_style->settingPenStyle(
            select_pen,
            points[0].radius,
            pen_color
        );

        Pen* currentPen = drawObj->pen_style->getPen();
        SolidBrush* currentBrush = drawObj->pen_style->getBrush();
        if (currentPen != nullptr)
        {
            if(select_pen != HIGHLIGHTERPEN)
            currentPen->SetLineJoin(Gdiplus::LineJoinRound);
        }

        // 획과 획 사이 시간 보정
        if (i > 0 && !linesData[i - 1].point_data.empty())
        {
            const DrawPointData& previousLastPoint =
                linesData[i - 1].point_data.back();

            const DrawPointData& currentFirstPoint =
                points[0];

            ULONGLONG strokeDelay = 0;

            if (currentFirstPoint.elapsed_time >
                previousLastPoint.elapsed_time)
            {
                strokeDelay =
                    currentFirstPoint.elapsed_time -
                    previousLastPoint.elapsed_time;
            }

            if (strokeDelay > 100)
                strokeDelay = 100;

            if (strokeDelay > 0)
                Sleep((DWORD)strokeDelay);
        }

        if (replayState == replayStateStop)
            break;

        // 점 하나만 있는 획
        if (points.size() == 1)
        {
            replay_bmp_graphics->Clear(Color::White);

            replay_bmp_graphics->DrawImage(
                replay_canvas_bmp, 0, 0
            );

            int x = points[0].point.x;
            int y = points[0].point.y;

            if (select_pen == BRUSHPEN && currentBrush != nullptr)
            {
                int radius = (int)points[0].radius;

                replay_bmp_graphics->FillEllipse(
                    currentBrush,
                    x - radius,
                    y - radius,
                    radius * 2,
                    radius * 2
                );

                replay_canvas_graphics->FillEllipse(
                    currentBrush,
                    x - radius,
                    y - radius,
                    radius * 2,
                    radius * 2
                );
            }
            else if (currentPen != nullptr)
            {
                replay_bmp_graphics->DrawLine(
                    currentPen,
                    (float)x,
                    (float)y,
                    (float)x + 0.001f,
                    (float)y
                );

                replay_canvas_graphics->DrawLine(
                    currentPen,
                    (float)x,
                    (float)y,
                    (float)x + 0.001f,
                    (float)y
                );
            }

            graphics.DrawImage(replay_bmp, 0, 0);
            continue;
        }

        // 일반 펜
        if (select_pen == SOLIDPEN)
        {
            if (currentPen != nullptr)
            {
                replay_bmp_graphics->Clear(Color::White);

                replay_bmp_graphics->DrawImage(
                    replay_canvas_bmp, 0, 0
                );

                for (size_t j = 1; j < points.size(); j++)
                {
                    if (replayState == replayStateStop)
                        break;

                    replay_bmp_graphics->DrawLine(
                        currentPen,
                        (float)points[j - 1].point.x,
                        (float)points[j - 1].point.y,
                        (float)points[j].point.x,
                        (float)points[j].point.y
                    );

                    graphics.DrawImage(replay_bmp, 0, 0);

                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time >
                        points[j - 1].elapsed_time)
                    {
                        timeDiff =
                            points[j].elapsed_time -
                            points[j - 1].elapsed_time;
                    }

                    timeDiff = (ULONGLONG)(timeDiff * 0.18);

                    if (timeDiff > 0)
                        Sleep((DWORD)timeDiff);
                }

                if (replayState == replayStateStop)
                    break;

                GraphicsPath finalPath;

                for (size_t j = 1; j < points.size(); j++)
                {
                    finalPath.AddLine(
                        (float)points[j - 1].point.x,
                        (float)points[j - 1].point.y,
                        (float)points[j].point.x,
                        (float)points[j].point.y
                    );
                }

                replay_canvas_graphics->DrawPath(
                    currentPen, &finalPath
                );
            }

            continue;
        }

        // 붓펜
        if (select_pen == BRUSHPEN)
        {
            if (currentBrush != nullptr)
            {
                for (size_t j = 1; j < points.size(); j++)
                {
                    if (replayState == replayStateStop)
                        break;

                    int x = points[j].point.x;
                    int y = points[j].point.y;
                    int radius = (int)points[j].radius;

                    replay_bmp_graphics->Clear(Color::White);

                    replay_bmp_graphics->DrawImage(
                        replay_canvas_bmp, 0, 0
                    );

                    // 저장된 반지름으로 원을 그림
                    replay_bmp_graphics->FillEllipse(
                        currentBrush,
                        x - radius,
                        y - radius,
                        radius * 2,
                        radius * 2
                    );

                    graphics.DrawImage(replay_bmp, 0, 0);

                    replay_canvas_graphics->FillEllipse(
                        currentBrush,
                        x - radius,
                        y - radius,
                        radius * 2,
                        radius * 2
                    );

                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time >
                        points[j - 1].elapsed_time)
                    {
                        timeDiff =
                            points[j].elapsed_time -
                            points[j - 1].elapsed_time;
                    }

                    timeDiff = (ULONGLONG)(timeDiff * 0.18);

                    if (timeDiff > 0)
                        Sleep((DWORD)timeDiff);
                }
            }

            continue;
        }

        // 형광펜
        if (select_pen == HIGHLIGHTERPEN)
        {
            if (currentPen != nullptr)
            {
                GraphicsPath replayPath;

                for (size_t j = 1; j < points.size(); j++)
                {
                    if (replayState == replayStateStop)
                        break;

                    replayPath.AddLine(
                        (float)points[j - 1].point.x,
                        (float)points[j - 1].point.y,
                        (float)points[j].point.x,
                        (float)points[j].point.y
                    );

                    replay_bmp_graphics->Clear(Color::White);

                    replay_bmp_graphics->DrawImage(
                        replay_canvas_bmp, 0, 0
                    );

                    replay_bmp_graphics->DrawPath(
                        currentPen, &replayPath
                    );

                    graphics.DrawImage(replay_bmp, 0, 0);

                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time >
                        points[j - 1].elapsed_time)
                    {
                        timeDiff =
                            points[j].elapsed_time -
                            points[j - 1].elapsed_time;
                    }

                    timeDiff = (ULONGLONG)(timeDiff * 0.18);

                    if (timeDiff > 0)
                        Sleep((DWORD)timeDiff);
                }

                if (replayState == replayStateStop)
                    break;

                replay_canvas_graphics->DrawPath(
                    currentPen, &replayPath
                );
            }

            continue;
        }

        // 스프레이 펜
        if (select_pen == SPRAYPEN)
        {
            if (currentBrush != nullptr)
            {
                for (size_t j = 1; j < points.size(); j++)
                {
                    if (replayState == replayStateStop)
                        break;

                    int sprayRadius = (int)points[j].radius;
                    int x = points[j].point.x;
                    int y = points[j].point.y;

                    // 현재 점의 랜덤 점을 먼저 생성
                    std::vector<Point> sprayPoints;

                    for (int k = 0; k < sprayRadius * 4; k++)
                    {
                        int rand_x =
                            rand() % (sprayRadius * 2 + 1)
                            - sprayRadius;

                        int rand_y =
                            rand() % (sprayRadius * 2 + 1)
                            - sprayRadius;

                        if (rand_x * rand_x + rand_y * rand_y >
                            sprayRadius * sprayRadius)
                        {
                            continue;
                        }

                        sprayPoints.push_back(
                            Point(x + rand_x, y + rand_y)
                        );
                    }

                    replay_bmp_graphics->Clear(Color::White);

                    replay_bmp_graphics->DrawImage(
                        replay_canvas_bmp, 0, 0
                    );

                    for (size_t k = 0; k < sprayPoints.size(); k++)
                    {
                        replay_bmp_graphics->FillEllipse(
                            currentBrush,
                            sprayPoints[k].X,
                            sprayPoints[k].Y,
                            2,
                            2
                        );

                        replay_canvas_graphics->FillEllipse(
                            currentBrush,
                            sprayPoints[k].X,
                            sprayPoints[k].Y,
                            2,
                            2
                        );
                    }

                    graphics.DrawImage(replay_bmp, 0, 0);

                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time >
                        points[j - 1].elapsed_time)
                    {
                        timeDiff =
                            points[j].elapsed_time -
                            points[j - 1].elapsed_time;
                    }

                    timeDiff = (ULONGLONG)(timeDiff * 0.18);

                    if (timeDiff > 0)
                        Sleep((DWORD)timeDiff);
                }
            }
        }
    }

    ReleaseDC(hwnd, hdc);

    delete replay_bmp_graphics;
    replay_bmp_graphics = nullptr;

    delete replay_bmp;
    replay_bmp = nullptr;

    delete replay_canvas_graphics;
    replay_canvas_graphics = nullptr;

    delete replay_canvas_bmp;
    replay_canvas_bmp = nullptr;

    replayState = replayStateStop;

    // 원본 그림으로 화면 복원
    drawObj->redrawAllLines(hwnd);

    // 재생 전 펜 설정 복구
    drawObj->pen_style->settingPenStyle(
        savedPen,
        savedRadius,
        savedColor
    );

    InvalidateRect(hwnd, NULL, FALSE);
    UpdateWindow(hwnd);
}

void Replay::stopReplay()
{
    replayState = replayStateStop;
}

int Replay::getReplayState()
{
    return replayState;
}