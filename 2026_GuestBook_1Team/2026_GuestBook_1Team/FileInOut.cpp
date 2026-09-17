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
    ZeroMemory(&ofn, sizeof(OPENFILENAME)); ///< 구조체 0값으로 초기화

    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = filePath;                ///< 파일 경로 저장
    ofn.nMaxFile = MAX_PATH;                 ///< 파일 경로의 최대 길이 MAX_PATH는 260자까지 
    ofn.lpstrInitialDir = L"..\\2026_Draw_Data";    ///< 파일 탐색기 열릴때 경로 설정
    ofn.Flags = OFN_PATHMUSTEXIST;           ///< 존재하지 않는 경로 차단

    if (isSave) {
        ofn.Flags |= OFN_OVERWRITEPROMPT;   ///< 중복된 이름이면 덮어 쓸지 경고창 띄우는 옵션
        return GetSaveFileName(&ofn) != 0;  ///< 윈도우 기본 저장하기 창 호출. 사용자가 '저장' 누르면 true, '취소' 누르면 false 반환
    }
    else {
        ofn.Flags |= OFN_FILEMUSTEXIST;     ///< 이미 존재하는 파일만 고를 수 있게 하는 옵션
        return GetOpenFileName(&ofn) != 0;  ///< 열기 창 호출, 열기 true, 취소 false
    }
}

/**
 * @brief 그림 데이터를 바이너리 메모리 덤프 방식으로 저장 (탐색기 + 저장 통합)
 */
bool FileInOut::SaveFile(HWND hWnd, Draw& drawObj) {
    wchar_t filePath[MAX_PATH] = L"";

    // 1. 저장용 탐색기 창을 띄우고 취소하면 바로 종료
    if (!OpenFileDialog(hWnd, filePath, true)) {
        return false;
    }

    std::ofstream outFile(filePath, std::ios::binary);
    if (!outFile.is_open()) {                          ///< 권한이나 경로 문제로 파일 안 열리면
        MessageBox(hWnd, L"파일을 저장할 수 없습니다.", L"에러", MB_OK);
        return false;                                       ///< 저장 실패 처리
    }

    // 전체 선의 개수를 먼저 파일 맨 앞에 기록
    size_t lineCount = drawObj.drawn_lines_data.size();
    outFile.write((char*)&lineCount, sizeof(lineCount));

    // 선 개수만큼 하나씩 순회하며 저장 
    for (int i = 0; i < lineCount; i++)
    {
        // i번째 선을 하나 가져오기
        DrawLineData& line = drawObj.drawn_lines_data[i];


        // 이 선의 펜 설정값 저장 

        outFile.write((char*)&line.is_pen, sizeof(line.is_pen));                     // 펜인지 지우개인지
        outFile.write((char*)&line.select_pen_style, sizeof(line.select_pen_style)); // 펜 모양 (실선, 점선 등)
        outFile.write((char*)&line.select_color, sizeof(line.select_color));         // 펜 색상


        // 이 선이 가진 점의 개수 구해서 저장
        size_t pointCount = line.point_data.size();
        outFile.write((char*)&pointCount, sizeof(pointCount));

        // 점들의 좌표 데이터 전체 저장
        if (pointCount > 0) {
            outFile.write((char*)line.point_data.data(), sizeof(DrawPointData) * pointCount);
        }
    }

    outFile.close();

    drawObj.ac_lines();
    InvalidateRect(hWnd, NULL, TRUE);                  ///< 화면 초기화
    return true;                                       ///< 저장 완료 반환
}

/**
 * @brief 바이너리 파일에서 그림 데이터를 불러오는 함수
 */
bool FileInOut::LoadFile(HWND hWnd, Draw& drawObj) {
    wchar_t filePath[MAX_PATH] = L"";

    // 불러오기용 탐색기 창을 띄우고 취소하면 바로 종료
    if (!OpenFileDialog(hWnd, filePath, false)) {
        return false;
    }

    // 바이너리 읽기 모드(std::ios::binary)로 파일 열기
    std::ifstream inFile(filePath, std::ios::binary);  ///< 선택한 경로의 바이너리 파일 열기

    if (!inFile.is_open()) {                           ///< 파일이 안 열릴 경우 예외 처리
        MessageBox(hWnd, L"파일을 읽을 수 없습니다.", L"에러", MB_OK);
        return false;
    }

    // 맨 앞에 저장된 전체 선 개수 읽기
    size_t lineCount = 0;
    inFile.read((char*)&lineCount, sizeof(lineCount));
    if (inFile.fail()) {
        inFile.close();
        return false;
    }


    std::vector<DrawLineData> temp_lines(lineCount);    /// 읽어온 선 개수만큼 임시 저장 공간 생성

    // 5. 선 개수만큼 하나씩 읽기
    for (int i = 0; i < lineCount; i++)
    {
        DrawLineData& line = temp_lines[i];     /// 선을 채우기 위해 가져오기

        // 저장했던 순서대로 펜 설정값 3개 읽기
        inFile.read((char*)&line.is_pen, sizeof(line.is_pen));
        inFile.read((char*)&line.select_pen_style, sizeof(line.select_pen_style));
        inFile.read((char*)&line.select_color, sizeof(line.select_color));

        // 이 선의 점 개수 읽어오기

        size_t pointCount = 0;
        inFile.read((char*)&pointCount, sizeof(pointCount));


        // 점 개수만큼 벡터 크기를 늘린 뒤 좌표 채우기

        line.point_data.resize(pointCount); // 점을 담을 공간 생성

        if (pointCount > 0) {
            inFile.read((char*)line.point_data.data(), sizeof(DrawPointData) * pointCount);
        }
    }

    inFile.close(); // 파일 읽기 완료 후 닫기

    //불러온 데이터 복원 및 화면 다시 그리기
    drawObj.drawn_lines_data = temp_lines;
    drawObj.redrawAllLines(hWnd);
    InvalidateRect(hWnd, NULL, TRUE);

    return true;
}