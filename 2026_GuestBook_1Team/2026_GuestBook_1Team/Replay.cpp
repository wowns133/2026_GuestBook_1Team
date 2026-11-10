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

    //이전 스레드의 핸들 정리
    if (replayThreadHandle != NULL)
    {
        WaitForSingleObject(replayThreadHandle, INFINITE);
        CloseHandle(replayThreadHandle);
        replayThreadHandle = NULL;
    }

    drawObj = &draw;
    hwnd = targetHWnd;

    replayState = replayStatePlaying;

    // Replay Thread 생성
    replayThreadHandle = CreateThread(
        NULL,
        0,
        Replay::replayFunction,
        this,
        0,
        NULL
    );

    if (replayThreadHandle == NULL)
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

    graphics.Clear(Color::White);

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
            if (select_pen != HIGHLIGHTERPEN)
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

            if (strokeDelay > 300)
                strokeDelay = 200;

            if (strokeDelay > 0)
                Sleep((DWORD)strokeDelay);
        }

        if (replayState == replayStateStop)
            break;

        // 점 하나만 있는 획
        if (points.size() == 1)
        {
            int x = points[0].point.x;
            int y = points[0].point.y;

            replay_bmp_graphics->Clear(Color::White);

            // 스프레이 펜
            if (select_pen == SPRAYPEN && currentBrush != nullptr)
            {
                int sprayRadius = (int)points[0].radius;

                /// 난수 배열 행 인덱스 받기
                int spray_index = points[0].spray_index;

                std::vector<Point> sprayPoints;

                for (int k = 0; k < sprayRadius * 4; k++)
                {
                    int rand_x = drawObj->pen_style->getSprayRandomNumberX()[spray_index][k]
                        % (sprayRadius * 2 + 1) - sprayRadius;

                    int rand_y = drawObj->pen_style->getSprayRandomNumberY()[spray_index][k]
                        % (sprayRadius * 2 + 1) - sprayRadius;

                    if (rand_x * rand_x + rand_y * rand_y >
                        sprayRadius * sprayRadius)
                    {
                        continue;
                    }

                    sprayPoints.push_back(Point(x + rand_x, y + rand_y));
                }

                // 기존 누적 그림 복사
                drawObj->drawSectionImage(
                    replay_bmp_graphics,
                    replay_canvas_bmp,
                    x - sprayRadius,
                    y - sprayRadius,
                    x + sprayRadius,
                    y + sprayRadius
                );

                // 스프레이 입자 그리기
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

                // 화면 출력
                drawObj->drawSectionImage(
                    &graphics,
                    replay_bmp,
                    x - sprayRadius,
                    y - sprayRadius,
                    x + sprayRadius,
                    y + sprayRadius
                );
            }
            else
            {
                // 기존 누적 그림 복사
                drawObj->drawSectionImage(
                    replay_bmp_graphics,
                    replay_canvas_bmp,
                    x, y, x, y
                );

                // 붓펜
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
                // 기존 일반 펜의 점 처리와 동일
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

                // 현재 점 주변 화면 출력
                drawObj->drawSectionImage(
                    &graphics,
                    replay_bmp,
                    x, y, x, y
                );
            }

            continue;
        }

        // 일반 펜
        if (select_pen == SOLIDPEN)
        {
            if (currentPen != nullptr)
            {
                replay_bmp_graphics->Clear(Color::White);

                // 획 시작 시점에만 전체 누적 캔버스를 복사한다.
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

                    drawObj->drawSectionImage(
                        &graphics,
                        replay_bmp,
                        points[j - 1].point.x,
                        points[j - 1].point.y,
                        points[j].point.x,
                        points[j].point.y
                    );

                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time >
                        points[j - 1].elapsed_time)
                    {
                        timeDiff =
                            points[j].elapsed_time -
                            points[j - 1].elapsed_time;
                    }

                    timeDiff = (ULONGLONG)timeDiff;

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
            for (size_t j = 1; j < points.size(); j++)
            {
                if (replayState == replayStateStop)
                    break;

                float previous_x = (float)points[j - 1].point.x;
                float previous_y = (float)points[j - 1].point.y;
                float previous_radius = points[j - 1].radius;

                float current_x = (float)points[j].point.x;
                float current_y = (float)points[j].point.y;
                float current_radius = points[j].radius;

                //붓펜 반지름에 따른 복사 영역 크기 확보
                // 두 점 중 더 큰 반지름을 기준으로 영역 계산
                float maxRadius = previous_radius > current_radius
                    ? previous_radius : current_radius;

                int brushMargin = (int)maxRadius + 2;

                int left = (int)min(previous_x, current_x) - brushMargin;
                int top = (int)min(previous_y, current_y) - brushMargin;
                int right = (int)max(previous_x, current_x) + brushMargin;
                int bottom = (int)max(previous_y, current_y) + brushMargin;

                // 비트맵 영역을 벗어나지 않도록 제한
                int bmpWidth = replay_canvas_bmp->GetWidth();
                int bmpHeight = replay_canvas_bmp->GetHeight();

                if (left < 0) left = 0;
                if (top < 0) top = 0;
                if (right > bmpWidth) right = bmpWidth;
                if (bottom > bmpHeight) bottom = bmpHeight;

                int sectionWidth = right - left;
                int sectionHeight = bottom - top;

                if (sectionWidth <= 0 || sectionHeight <= 0)
                    continue;

                replay_bmp_graphics->Clear(Color::White);

                // 기존에 그려진 부분을 임시 비트맵에 복사
                replay_bmp_graphics->DrawImage(
                    replay_canvas_bmp,
                    left, top,
                    left, top,
                    sectionWidth, sectionHeight,
                    UnitPixel
                );

                // 이전 점과 현재 점 사이의 붓펜 그리기
                drawObj->fillBrushLine(
                    replay_bmp_graphics,
                    previous_x, previous_y, previous_radius,
                    current_x, current_y, current_radius
                );

                drawObj->fillBrushLine(
                    replay_canvas_graphics,
                    previous_x, previous_y, previous_radius,
                    current_x, current_y, current_radius
                );

                // 계산한 영역만 화면에 출력
                graphics.DrawImage(
                    replay_bmp,
                    left, top,
                    left, top,
                    sectionWidth, sectionHeight,
                    UnitPixel
                );

                ULONGLONG timeDiff = 0;

                if (points[j].elapsed_time > points[j - 1].elapsed_time)
                {
                    timeDiff = points[j].elapsed_time - points[j - 1].elapsed_time;
                }

                if (timeDiff > 0)
                    Sleep((DWORD)timeDiff);
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

                    drawObj->drawSectionImage(
                        replay_bmp_graphics,
                        replay_canvas_bmp,
                        points[j - 1].point.x,
                        points[j - 1].point.y,
                        points[j].point.x,
                        points[j].point.y
                    );

                    replay_bmp_graphics->DrawPath(
                        currentPen, &replayPath
                    );

                    drawObj->drawSectionImage(
                        &graphics,
                        replay_bmp,
                        points[j - 1].point.x,
                        points[j - 1].point.y,
                        points[j].point.x,
                        points[j].point.y
                    );

                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time >
                        points[j - 1].elapsed_time)
                    {
                        timeDiff =
                            points[j].elapsed_time -
                            points[j - 1].elapsed_time;
                    }

                    timeDiff = (ULONGLONG)timeDiff;

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
                // 점 하나로 구성된 스프레이 획
                if (points.size() == 1)
                {
                    int sprayRadius = (int)points[0].radius;
                    int x = points[0].point.x;
                    int y = points[0].point.y;

                    /// 난수 배열 행 인덱스 받기
                    int spray_index = points[0].spray_index;

                    std::vector<Point> sprayPoints;

                    for (int k = 0; k < sprayRadius * 4; k++)
                    {
                        int rand_x = drawObj->pen_style->getSprayRandomNumberX()[spray_index][k]
                            % (sprayRadius * 2 + 1) - sprayRadius;

                        int rand_y = drawObj->pen_style->getSprayRandomNumberY()[spray_index][k]
                            % (sprayRadius * 2 + 1) - sprayRadius;

                        if (rand_x * rand_x + rand_y * rand_y >
                            sprayRadius * sprayRadius)
                            continue;

                        sprayPoints.push_back(Point(x + rand_x, y + rand_y));
                    }

                    replay_bmp_graphics->Clear(Color::White);

                    drawObj->drawSectionImage(
                        replay_bmp_graphics,
                        replay_canvas_bmp,
                        x - sprayRadius,
                        y - sprayRadius,
                        x + sprayRadius,
                        y + sprayRadius
                    );

                    for (size_t k = 0; k < sprayPoints.size(); k++)
                    {
                        replay_bmp_graphics->FillEllipse(
                            currentBrush, sprayPoints[k].X, sprayPoints[k].Y, 2, 2
                        );

                        replay_canvas_graphics->FillEllipse(
                            currentBrush, sprayPoints[k].X, sprayPoints[k].Y, 2, 2
                        );
                    }

                    drawObj->drawSectionImage(
                        &graphics,
                        replay_bmp,
                        x - sprayRadius,
                        y - sprayRadius,
                        x + sprayRadius,
                        y + sprayRadius
                    );
                }
                else
                {
                    for (size_t j = 1; j < points.size(); j++)
                    {
                        if (replayState == replayStateStop)
                            break;

                        int sprayRadius = (int)points[j].radius;
                        int x = points[j].point.x;
                        int y = points[j].point.y;

                        /// 난수 배열 행 인덱스 받기
                        int spray_index = points[j].spray_index;

                        // 현재 점의 랜덤 점을 먼저 생성
                        std::vector<Point> sprayPoints;

                        for (int k = 0; k < sprayRadius * 4; k++)
                        {
                            int rand_x = drawObj->pen_style->getSprayRandomNumberX()[spray_index][k]
                                % (sprayRadius * 2 + 1) - sprayRadius;

                            int rand_y = drawObj->pen_style->getSprayRandomNumberY()[spray_index][k]
                                % (sprayRadius * 2 + 1) - sprayRadius;

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

                        drawObj->drawSectionImage(
                            replay_bmp_graphics,
                            replay_canvas_bmp,
                            x - sprayRadius,
                            y - sprayRadius,
                            x + sprayRadius,
                            y + sprayRadius
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

                        drawObj->drawSectionImage(
                            &graphics,
                            replay_bmp,
                            x - sprayRadius,
                            y - sprayRadius,
                            x + sprayRadius,
                            y + sprayRadius
                        );

                        ULONGLONG timeDiff = 0;

                        if (points[j].elapsed_time >
                            points[j - 1].elapsed_time)
                        {
                            timeDiff =
                                points[j].elapsed_time -
                                points[j - 1].elapsed_time;
                        }

                        timeDiff = (ULONGLONG)timeDiff;

                        if (timeDiff > 0)
                            Sleep((DWORD)timeDiff);
                    }
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


    // 원본 그림으로 화면 복원
    //drawObj->redrawAllLines(hwnd);

    // 재생 전 펜 설정 복구
    drawObj->pen_style->settingPenStyle(
        savedPen,
        savedRadius,
        savedColor
    );

    //InvalidateRect(hwnd, NULL, FALSE);
    //UpdateWindow(hwnd);
    replayState = replayStateStop;
}

void Replay::stopReplay()
{
    replayState = replayStateStop;

    if (replayThreadHandle != NULL)
    {
        WaitForSingleObject(replayThreadHandle, INFINITE);

        CloseHandle(replayThreadHandle);
        replayThreadHandle = NULL;
    }
}

int Replay::getReplayState()
{
    return replayState;
}

HANDLE Replay::getReplayThreadHandle()
{
    return replayThreadHandle;
}