#pragma once

#include "Draw.h"
#include <Windows.h>
#include <gdiplus.h>

#define replayStatePlaying 0
#define replayStateStop 1
#define replayStatePause 2

class Replay
{
private:
    Draw* drawObj = nullptr;

    int replayState = replayStateStop;

    HWND hwnd = NULL;

    // 지금까지 Replay가 완료한 그림
    Gdiplus::Bitmap* replay_canvas_bmp = nullptr;
    Gdiplus::Graphics* replay_canvas_graphics = nullptr;

    // 현재 Replay 중인 획을 임시로 합성하는 Bitmap
    Gdiplus::Bitmap* replay_bmp = nullptr;
    Gdiplus::Graphics* replay_bmp_graphics = nullptr;

    static DWORD WINAPI replayFunction(LPVOID lpParam);

    HANDLE replayThreadHandle = NULL;

    void runReplay();

public:
    void startReplay(Draw& draw, HWND targetHWnd);
    void stopReplay();
    int getReplayState();
    void pauseReplay();
    void resumeReplay();
    bool waitReplayDelay(DWORD delay);
    HANDLE getReplayThreadHandle();
};