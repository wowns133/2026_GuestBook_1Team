#pragma once
#include <Windows.h>
#include <vector>

class Draw;

/**
 * @brief 파일 입출력(저장 및 불러오기)을 담당하는 클래스
 */
class FileInOut {
private:

public:
    /**
     * @brief 파일 탐색기 다이얼로그 창을 띄우는 함수
     */
    bool OpenFileDialog(HWND hWnd, wchar_t* filePath, bool isSave);

    /**
     * @brief 그림 데이터를 파일에 저장하는 함수
     */
    bool SaveFile(HWND hWnd, const wchar_t* path, Draw& drawObj);

    /**
     * @brief 파일에서 그림 데이터를 불러오는 함수
     */
    bool LoadFile(HWND hWnd, const wchar_t* path, Draw& drawObj);
};