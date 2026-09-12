#pragma once
#include "Draw.h"
#include <Windows.h>

#define replayStatePlaying 0 //재생중
#define replayStateStop 1    //정지(끝까지 완료)
#define replayStatePaused 2  //일시정지

class Replay {
private:
	const Draw* drawObj = nullptr;///<Draw 클래스 객체를 기억합니다.
	int replayState = 1;
	HWND hwnd = NULL;
	bool isStopRequested = false;///<사용자가 일시정지를 눌렀는지 여부를 나타내는 플래그
	static DWORD WINAPI replayFunction(LPVOID lpParam);//리플레이를 호출할 떄 사용할 스레드
	void runReplay();

public:
	void startReplay(Draw& draw, HWND targetHWnd);//replayFunction 스레드를 생성하고 시작하는 함수
	void stopReplay();

};

//~~~펜 정보 추가, 3가지 플래그가 구현되었을 때의 버전입니다~~~//
/*
class Replay {
private:
	const Draw* drawObj = nullptr;///<Draw 클래스 객체를 기억합니다.
	int replayState = replayStateStop;
	UINT pauseLineIdx = 0;///<일시정지 시점 기억용 변수(획)
	UINT pausePointIdx = 1;///<일시정지 시점 기억용 변수(점)
	HWND hwnd = NULL;
	bool isStopRequested = false;///<사용자가 일시정지를 눌렀는지 여부를 나타내는 플래그
	static DWORD WINAPI replayFunction(LPVOID lpParam);//재생중일 때 실행할 스레드
	void runReplay();
public:
	void startReplay(Draw& draw, HWND targetHWnd);//replayFunction 스레드를 생성하고 시작하는 함수
	void stopReplay();
	int getReplayState();
	void pauseReplay();
};
*/

//플래그 3개의 사용으로 2026_GuestBook_1Team.cpp에서의 wndProc의 변형이 필요합니다
/*        case ID_REPLAY:
        {
            int state = replay.getReplayState();
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
            }
        }
        break;
*/