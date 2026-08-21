#pragma once
///헤더 통합 파일을 넣기 전 임시 include문
#include <Windows.h>
#include <winuser.h>

///임시 버튼 define
#define ID_PEN_STYLE1 3001
#define ID_PEN_STYLE2 3002
#define ID_PEN_STYLE3 3003
#define ID_PEN_STYLE4 3004
#define ID_PEN_STYLE5 3005
#define ID_PEN_COLOR1 3006
#define ID_PEN_COLOR2 3007
#define ID_PEN_COLOR3 3008
#define ID_PEN_COLOR4 3009
#define ID_PEN_COLOR5 3010
#define ID_ERASER 3011
#define ID_REPLAY 3012
#define ID_SAVE_FILE 3013
#define ID_LOAD_FILE 3014



void makeTempButton(HWND hWnd, HINSTANCE hInst);

