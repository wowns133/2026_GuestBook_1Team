#pragma once
#include "Draw.h"
#include <Windows.h>

#define replayStatePlaying 0 // 재생중
#define replayStateStop 1    // 정지(및 바로 빠져나오기)

class Replay {
private:
    Draw* drawObj = nullptr; ///< Draw 클래스 객체를 기억합니다.
    int replayState = replayStateStop;
    HWND hwnd = NULL;

    static DWORD WINAPI replayFunction(LPVOID lpParam); // 재생중일 때 실행할 스레드
    void runReplay();

public:
    void startReplay(Draw& draw, HWND targetHWnd); // replayFunction 스레드를 생성하고 시작하는 함수
    void stopReplay();
    int getReplayState();
};