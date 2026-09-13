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

    // 3. 맨 앞부분에 전체 선의 개수 기록
    size_t lineCount = drawObj.drawn_lines.size();             ///< 전체 선 개수 추출
    outFile.write((char*)&lineCount, sizeof(lineCount));    ///< 선 개수를 2진수 바이트 단위로 바이너리 저장

    // 4. 전체 선을 순회하며 데이터 블록 기록
    for (size_t i = 0; i < lineCount; ++i) {

        // 해당 선의 점 개수 기록
        size_t pointCount = drawObj.drawn_lines[i].size();               ///< 현재 선에 속한 점의 개수 추출
        outFile.write((char*)&pointCount, sizeof(pointCount)); ///< 점 개수를 바이너리로 저장

        if (pointCount > 0) {
            outFile.write((char*)drawObj.drawn_lines[i].data(), sizeof(DrawPointData) * pointCount);
        }
    }

    outFile.close();

    drawObj.ac_lines();
    InvalidateRect(hWnd, NULL, TRUE);                  ///< 화면 초기화
    return true;                                       ///< 저장 완료 반환
}

/**
 * @brief 바이너리 파일에서 그림 데이터를 한 방에 불러오는 함수 (탐색기 + 불러오기 통합)
 */
bool FileInOut::LoadFile(HWND hWnd, Draw& drawObj) {
    wchar_t filePath[MAX_PATH] = L"";

    // 1. 불러오기용 탐색기 창을 띄우고 취소하면 바로 종료
    if (!OpenFileDialog(hWnd, filePath, false)) {
        return false;
    }

    // 2. 바이너리 읽기 모드(std::ios::binary)로 파일 열기
    std::ifstream inFile(filePath, std::ios::binary);  ///< 선택한 경로의 바이너리 파일 열기

    if (!inFile.is_open()) {                           ///< 파일이 안 열릴 경우 예외 처리
        MessageBox(hWnd, L"파일을 읽을 수 없습니다.", L"에러", MB_OK);
        return false;
    }

    std::vector<std::vector<DrawPointData>> temp_lines; ///< 파일 데이터를 임시로 읽어올 벡터 생성
    size_t lineCount = 0;                              ///< 전체 선 개수를 담을 변수

    // 3. 맨 앞부분의 전체 선 개수 읽기
    inFile.read((char*)&lineCount, sizeof(lineCount));   ///< 저장했던 선 개수를 2진수 바이트 단위로 읽음
    if (inFile.fail()) {                               ///< 파일 읽기 실패 시 경고
        MessageBox(hWnd, L"파일 형식이 올바르지 않습니다.", L"불러오기 실패", MB_ICONERROR);
        inFile.close();
        return false;
    }

    temp_lines.resize(lineCount);                      ///< 선 개수만큼 벡터 공간 사전 확보

    // 4. 선 개수만큼 순회하며 점 덩어리 읽기
    for (size_t i = 0; i < lineCount; ++i) {
        size_t pointCount = 0;                         ///< 읽어올 점 개수 변수
        inFile.read((char*)&pointCount, sizeof(pointCount)); ///< 점 개수 바이너리로 읽기

        if (inFile.fail()) {
            MessageBox(hWnd, L"데이터를 읽는 중 오류가 발생했습니다.", L"불러오기 실패", MB_ICONERROR);
            inFile.close();
            return false;
        }

        temp_lines[i].resize(pointCount);

        if (pointCount > 0) {
            inFile.read((char*)temp_lines[i].data(), sizeof(DrawPointData) * pointCount); ///< 점 데이터 로드

            if (inFile.fail()) {
                MessageBox(hWnd, L"그림 데이터가 손상되었습니다.", L"불러오기 실패", MB_ICONERROR);
                inFile.close();
                return false;
            }
        }
    }

    inFile.close();                                     ///< 파일 스트림 닫기

    drawObj.drawn_lines = temp_lines;                  ///< 1. 좌표 데이터 복원
    drawObj.redrawAllLines(hWnd);                        ///< 2. 복원된 좌표를 비트맵(도화지)에 다시 그리기
    InvalidateRect(hWnd, NULL, TRUE);                  ///< 3. 화면 갱신 요청 (WM_PAINT 호출)

    return true;                                        ///< 불러오기 성공 반환
}
