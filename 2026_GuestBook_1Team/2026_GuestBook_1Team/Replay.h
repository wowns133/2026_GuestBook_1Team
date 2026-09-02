#pragma once
#include "Draw.h"
#include <Windows.h>

#define replayStatePlaying 0 //재생중
#define replayStateStop 1    //정지(끝까지 완료)
#define replayStatePaused 2  //일시정지

class Replay {
private:
	//static DWORD WINAPI replayFunction(LPVOID lpParam);//재생중일 때 실행할 스레드
	const Draw* drawObj = nullptr;///<Draw 클래스 객체를 기억합니다.
	int replayState = 0;
	HWND hwnd = NULL;

public:
	void startReplay(Draw& draw, HWND targetHWnd);//Draw를 최초로 넘겨받을 때 사용됩니다


};