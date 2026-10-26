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
    //draw 객체가 존재하는지 확인
    if (drawObj == nullptr)
    {
        replayState = replayStateStop;
        return;
    }

    //pen_style이 존재하는지 확인
    if (drawObj->pen_style == nullptr)
    {
        replayState = replayStateStop;
        return;
    }

    //Canvas 윈도우 핸들 확인
    if (hwnd == NULL)
    {
        replayState = replayStateStop;
        return;
    }

    RECT rect;

    //Canvas 클라이언트 영역 크기 가져오기
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

    //지금까지 완료한 replay를 저장하는 bitmap
    replay_canvas_bmp =
        new Bitmap(
            width,
            height,
            PixelFormat32bppARGB
        );

    replay_canvas_graphics =
        new Graphics(replay_canvas_bmp);

    replay_canvas_graphics->SetSmoothingMode(
        SmoothingModeAntiAlias
    );

    //흰색 배경에서 시작
    replay_canvas_graphics->Clear(
        Color::White
    );

    //현재 replay 프레임을 만드는 bitmap
    replay_bmp =
        new Bitmap(
            width,
            height,
            PixelFormat32bppARGB
        );

    replay_bmp_graphics =
        new Graphics(replay_bmp);

    replay_bmp_graphics->SetSmoothingMode(
        SmoothingModeAntiAlias
    );

    replay_bmp_graphics->Clear(
        Color::White
    );

    //canvas HDC를 가져옴
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

    //실제 Canvas 화면에 그리는 Graphics
    Graphics graphics(hdc);

    graphics.SetSmoothingMode(
        SmoothingModeAntiAlias
    );

    //초기 화면
    graphics.DrawImage(
        replay_canvas_bmp,
        0,
        0
    );

    //Spray 시드 초기화
    srand((unsigned)time(NULL));

    //전체 획 데이터
    const std::vector<DrawLineData>& linesData =
        drawObj->getDrawnLines();

    //획 기준 replay
    for (size_t i = 0; i < linesData.size(); i++)
    {
        if (replayState == replayStateStop)
            break;

        const DrawLineData& currentLineData =
            linesData[i];

        const std::vector<DrawPointData>& points =
            currentLineData.point_data;

        if (points.empty())
            continue;

        //현재 획의 pen 확인
        int select_pen =
            currentLineData.select_pen_style;

        int pen_color =
            currentLineData.select_color;

        float radius =
            points[0].radius;


        drawObj->pen_style->settingPenStyle(
            select_pen,
            radius,
            pen_color
        );


        Pen* currentPen =
            drawObj->pen_style->getPen();

        SolidBrush* currentBrush =
            drawObj->pen_style->getBrush();

        //획과 획 사이 시간 보정
        if (i > 0 && !linesData[i - 1].point_data.empty())
        {
            const DrawPointData& previousLastPoint =
                linesData[i - 1].point_data.back();

            const DrawPointData& currentFirstPoint =
                points[0];

            ULONGLONG strokeDelay = 0;

            if (currentFirstPoint.elapsed_time > previousLastPoint.elapsed_time)
            {
                strokeDelay = currentFirstPoint.elapsed_time - previousLastPoint.elapsed_time;
            }

            //획과 획 사이의 긴 공백은 최대 100ms
            if (strokeDelay > 100)
            {
                strokeDelay = 100;
            }

            if (strokeDelay > 0)
            {
                Sleep((DWORD)strokeDelay);
            }
        }

        if (replayState == replayStateStop)
            break;

        //점
        if (points.size() == 1)
        {
            if (currentPen != nullptr)
            {
                // 현재 화면 구성
                replay_bmp_graphics->Clear(
                    Color::White
                );

                replay_bmp_graphics->DrawImage(
                    replay_canvas_bmp,
                    0,
                    0
                );

                replay_bmp_graphics->DrawLine(
                    currentPen,

                    (float)points[0].point.x,
                    (float)points[0].point.y,

                    (float)points[0].point.x + 0.001f,
                    (float)points[0].point.y
                );

                // 화면 출력
                graphics.DrawImage(
                    replay_bmp,
                    0,
                    0
                );

                // 완료된 그림에 확정
                replay_canvas_graphics->DrawLine(
                    currentPen,

                    (float)points[0].point.x,
                    (float)points[0].point.y,

                    (float)points[0].point.x + 0.001f,
                    (float)points[0].point.y
                );
            }

            continue;
        }

        //일반 Pen
        if (select_pen != SPRAYPEN && select_pen != HIGHLIGHTERPEN)
        {
            if (currentPen != nullptr)
            {
                //현재 획을 그리기 시작할 때
                //완료된 그림을 한 번 복사
                replay_bmp_graphics->Clear(
                    Color::White
                );

                replay_bmp_graphics->DrawImage(
                    replay_canvas_bmp,
                    0,
                    0
                );

                //현재 획의 점들을 하나씩 재생
                for (size_t j = 1; j < points.size(); j++)
                {
                    if (replayState == replayStateStop)
                        break;

                    // 새로 추가된 선분 하나만 그림
                    replay_bmp_graphics->DrawLine(
                        currentPen,

                        (float)points[j - 1].point.x,
                        (float)points[j - 1].point.y,

                        (float)points[j].point.x,
                        (float)points[j].point.y
                    );

                    //현재 프레임 출력
                    graphics.DrawImage(
                        replay_bmp,
                        0,
                        0
                    );

                    //점과 점 사이 시간
                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time > points[j - 1].elapsed_time)
                    {
                        timeDiff = points[j].elapsed_time - points[j - 1].elapsed_time;
                    }


                    //기존 Replay 속도 보정
                    timeDiff = (ULONGLONG)(timeDiff * 0.18);


                    if (timeDiff > 0)
                    {
                        Sleep((DWORD)timeDiff);
                    }
                }

                if (replayState == replayStateStop)
                    break;

                //획이 끝났으므로 완료된 Canvas에 확정
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
                    currentPen,
                    &finalPath
                );
            }

            continue;
        }

        //Highlighter
        // Draw.cpp와 동일한 방식으로 처리
        // 획이 끝나면 replay_canvas_bmp에 한 번 확정
        if (select_pen == HIGHLIGHTERPEN)
        {
            if (currentPen != nullptr)
            {
                GraphicsPath replayPath;


                // 현재 획 시작 시
                // 완료된 그림을 현재 프레임에 복사
                replay_bmp_graphics->Clear(
                    Color::White
                );

                replay_bmp_graphics->DrawImage(
                    replay_canvas_bmp,
                    0,
                    0
                );

                //현재 획 Replay
                for (size_t j = 1; j < points.size(); j++)
                {
                    if (replayState == replayStateStop)
                        break;


                    //현재 점까지 Path 추가
                    replayPath.AddLine(
                        (float)points[j - 1].point.x,
                        (float)points[j - 1].point.y,

                        (float)points[j].point.x,
                        (float)points[j].point.y
                    );

                    replay_bmp_graphics->Clear(
                        Color::White
                    );

                    //지금까지 완성된 그림
                    replay_bmp_graphics->DrawImage(
                        replay_canvas_bmp,
                        0,
                        0
                    );

                    //현재 그리고 있는 획 전체
                    replay_bmp_graphics->DrawPath(
                        currentPen,
                        &replayPath
                    );

                    //현재 프레임 출력
                    graphics.DrawImage(
                        replay_bmp,
                        0,
                        0
                    );

                    //점과 점 사이 시간
                    ULONGLONG timeDiff = 0;

                    if (points[j].elapsed_time > points[j - 1].elapsed_time)
                    {
                        timeDiff = points[j].elapsed_time - points[j - 1].elapsed_time;
                    }

                    timeDiff = (ULONGLONG)(timeDiff * 0.18);

                    if (timeDiff > 0)
                    {
                        Sleep((DWORD)timeDiff);
                    }
                }


                if (replayState == replayStateStop)
                    break;

                replay_canvas_graphics->DrawPath(
                    currentPen,
                    &replayPath
                );
            }

            continue;
        }

        //Spray Pen
        if (select_pen == SPRAYPEN)
        {
            for (size_t j = 1; j < points.size(); j++)
            {
                if (replayState == replayStateStop)
                    break;


                if (currentBrush != nullptr)
                {
                    int sprayRadius =
                        (int)points[j].radius;


                    //현재 프레임 새로 구성
                    replay_bmp_graphics->Clear(
                        Color::White
                    );

                    //지금까지 완료된 그림
                    replay_bmp_graphics->DrawImage(
                        replay_canvas_bmp,
                        0,
                        0
                    );

                    // 현재 점 Spray
                    for (int k = 0; k < sprayRadius * 4; k++)
                    {
                        int rand_x = rand() % (sprayRadius * 2 + 1) - sprayRadius;

                        int rand_y = rand() % (sprayRadius * 2 + 1) - sprayRadius;


                        // 원 바깥 제외
                        if (rand_x * rand_x + rand_y * rand_y > sprayRadius * sprayRadius)
                        {
                            continue;
                        }

                        int x = points[j].point.x + rand_x;

                        int y = points[j].point.y + rand_y;


                        replay_bmp_graphics->FillEllipse(
                            currentBrush,
                            x,
                            y,
                            2,
                            2
                        );
                    }


                    //현재 프레임 출력
                    graphics.DrawImage(
                        replay_bmp,
                        0,
                        0
                    );

                    // 완료된 그림에 Spray 확정
                    for (int k = 0; k < sprayRadius * 4; k++)
                    {
                        int rand_x = rand() % (sprayRadius * 2 + 1) - sprayRadius;

                        int rand_y = rand() % (sprayRadius * 2 + 1) - sprayRadius;

                        if (rand_x * rand_x + rand_y * rand_y > sprayRadius * sprayRadius)
                        {
                            continue;
                        }

                        int x = points[j].point.x + rand_x;

                        int y = points[j].point.y + rand_y;

                        replay_canvas_graphics->FillEllipse(
                            currentBrush,
                            x,
                            y,
                            2,
                            2
                        );
                    }
                }

                //점과 점 사이 시간
                ULONGLONG timeDiff = 0;

                if (points[j].elapsed_time > points[j - 1].elapsed_time)
                {
                    timeDiff = points[j].elapsed_time - points[j - 1].elapsed_time;
                }

                timeDiff = (ULONGLONG)(timeDiff * 0.18);

                if (timeDiff > 0)
                {
                    Sleep((DWORD)timeDiff);
                }
            }
        }
    }

    ReleaseDC(hwnd, hdc);

    //Bitmap 메모리 정리
    delete replay_bmp_graphics;
    replay_bmp_graphics = nullptr;

    delete replay_bmp;
    replay_bmp = nullptr;

    delete replay_canvas_graphics;
    replay_canvas_graphics = nullptr;

    delete replay_canvas_bmp;
    replay_canvas_bmp = nullptr;

    replayState = replayStateStop;

    // 실제 Canvas의 WM_PAINT를 통해 원본 그림을 다시 표시
    InvalidateRect(
        hwnd,
        NULL,
        FALSE
    );

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