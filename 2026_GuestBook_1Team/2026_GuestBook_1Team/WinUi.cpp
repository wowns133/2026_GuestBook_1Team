#pragma once
#include "WinUi.h"


void makeTempButton(HWND hWnd, HINSTANCE hInst)
{
    // --- PEN STYLE (1 ~ 5) ---
    CreateWindowW(L"BUTTON", L"solid", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        10, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE1, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"S2", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        50, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE2, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"S3", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        90, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE3, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"S4", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        130, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE4, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"S5", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        170, 10, 30, 30, hWnd, (HMENU)ID_PEN_STYLE5, hInst, nullptr);

    // --- PEN COLOR (1 ~ 5) ---
    CreateWindowW(L"BUTTON", L"black", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        210, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR1, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"red", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        250, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR2, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"green", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        290, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR3, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"blue", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        330, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR4, hInst, nullptr);
    CreateWindowW(L"BUTTON", L"yellow", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        370, 10, 30, 30, hWnd, (HMENU)ID_PEN_COLOR5, hInst, nullptr);

    // ---기능 버튼들 ---
    CreateWindowW(L"BUTTON", L"ER", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        410, 10, 30, 30, hWnd, (HMENU)ID_ERASER, hInst, nullptr); // 지우개
    CreateWindowW(L"BUTTON", L"PLAY", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        450, 10, 30, 30, hWnd, (HMENU)ID_REPLAY, hInst, nullptr); // 재생
    CreateWindowW(L"BUTTON", L"SAVE", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        490, 10, 30, 30, hWnd, (HMENU)ID_SAVE_FILE, hInst, nullptr); // 저장
    CreateWindowW(L"BUTTON", L"LOAD", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
        530, 10, 30, 30, hWnd, (HMENU)ID_LOAD_FILE, hInst, nullptr); // 열기
}

