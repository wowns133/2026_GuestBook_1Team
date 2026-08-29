#include "FileInOut.h"
#include <commdlg.h>
#include <fstream>
#include "Draw.h"

/**
 * @brief 윈도우 파일 탐색기 창을 띄워주는 함수
 * @details 저장/불러오기를 위한 파일 선택 창을 켬
 * @return 정상적으로 선택 시 true, 취소 시 false 반환
 */
bool FileInOut::OpenFileDialog(HWND hWnd, wchar_t* filePath, bool isSave) {
    OPENFILENAME ofn;
    ZeroMemory(&ofn, sizeof(OPENFILENAME)); // 구조체 초기화

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = filePath;
    ofn.nMaxFile = MAX_PATH;
    ofn.lpstrInitialDir = L"..\\file\\"; // 기본 폴더
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST; // 잘못된 경로 방지

    if (isSave) {
        return GetSaveFileName(&ofn) != 0;
    }
    else {
        return GetOpenFileName(&ofn) != 0;
    }
}

/**
 * @brief 그림 데이터를 파일로 저장하는 함수
 */
bool FileInOut::SaveFile(HWND hWnd, const wchar_t* path, Draw& drawObj) {
    std::wofstream outFile(path);

    if (!outFile.is_open()) {
        MessageBox(hWnd, L"파일을 저장할 수 없습니다.", L"에러", MB_OK);
        return false;
    }

    // 맨 윗줄에 전체 선의 개수 기록
    outFile << drawObj.drawn_lines.size() << L"\n";

    // 전체 선을 순회하며 기록
    for (const auto& line : drawObj.drawn_lines) {

        // 해당 선의 점 개수 기록
        outFile << line.size() << L"\n";

        // 점 데이터 기록 (X, Y, 시간, 펜상태)
        for (const auto& ptData : line) {
            outFile << ptData.point.x << L" "
                << ptData.point.y << L" "
                << ptData.elapsed_time << L" "
                << ptData.is_pen << L"\n";
        }
    }

    outFile.close();
    return true;
}

/**
 * @brief 파일에서 그림 데이터를 불러오는 함수
 * @details 에러 발생 시 기존 그림을 보호하고 불러오기를 취소함
 */
bool FileInOut::LoadFile(HWND hWnd, const wchar_t* path, Draw& drawObj) {
    std::wifstream inFile(path);

    if (!inFile.is_open()) {
        MessageBox(hWnd, L"파일을 읽을 수 없습니다.", L"에러", MB_OK);
        return false;
    }

    std::vector<std::vector<DrawPointData>> temp_lines; // 임시 저장소
    size_t line_count = 0;

    // 전체 선 개수 읽기
    inFile >> line_count;

    // 예외처리: 파일이 비었거나 숫자가 아니면 중단
    if (inFile.fail()) {
        MessageBox(hWnd, L"파일 형식이 올바르지 않습니다.", L"불러오기 실패", MB_ICONERROR);
        inFile.close();
        return false;
    }

    temp_lines.resize(line_count);

    // 선 개수만큼 반복
    for (size_t i = 0; i < line_count; ++i) {
        size_t point_count = 0;

        // 현재 선의 점 개수 읽기
        inFile >> point_count;

        // 예외처리: 점 개수 읽기 실패 시 중단
        if (inFile.fail()) {
            MessageBox(hWnd, L"데이터를 읽는 중 오류가 발생했습니다.", L"불러오기 실패", MB_ICONERROR);
            inFile.close();
            return false;
        }

        temp_lines[i].resize(point_count);

        // 점 개수만큼 데이터 읽기
        for (size_t j = 0; j < point_count; ++j) {
            inFile >> temp_lines[i][j].point.x
                >> temp_lines[i][j].point.y
                >> temp_lines[i][j].elapsed_time
                >> temp_lines[i][j].is_pen;

            // 예외처리: 데이터에 문자가 섞이거나 끊기면 중단
            if (inFile.fail()) {
                MessageBox(hWnd, L"그림 데이터가 손상되었습니다.", L"불러오기 실패", MB_ICONERROR);
                inFile.close();
                return false;
            }
        }
    }

    inFile.close();

    // 에러 없이 통과하면 실제 객체에 덮어쓰기
    drawObj.drawn_lines = temp_lines;

    // 화면 새로고침 요청
    InvalidateRect(hWnd, NULL, TRUE);

    return true;
}