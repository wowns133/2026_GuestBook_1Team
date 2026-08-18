#pragma once
#include <vector>
#include <Windows.h>
/**
* @file
* @brief 그리기 기능을 구현하는 헤더 파일.
* @details 그리기 기능을 구현한 헤더 파일.
* @todo 1. 화면 안에서 클릭한 후 화면 밖에서 땠을 때 다시 화면 안으로 돌아오면 화면 밖에서 생긴 이벤트를 감지할 수 없는 문제 해결
* 2. 그릴 때 시간 측정하는 기능 추가
*/


/**
* @brief 그리기 기능을 담당하는 클래스.
* @details 그리기 기능을 담당하는 클래스.
*/
class Draw
{
private:
	int previous_x = 0; ///< 선을 그릴 때 기준이 되는 좌표. 이전 x 좌표
	int previous_y = 0; ///< 선을 그릴 때 기준이 되는 좌표. 이전 y 좌표
	int current_x = 0; ///< 선을 그릴 때 기준이 되는 좌표. 현재 x 좌표
	int current_y = 0; ///< 선을 그릴 때 기준이 되는 좌표. 현재 y 좌표

	bool is_drawing = false;///< 마우스 클릭 상태인지 표시하는 플래그 변수

	std::vector<std::vector<POINT>> drawn_lines; ///< 선들의 집합을 저장하는 vector. 즉 모든 선을 저장하는 vector
	std::vector<POINT> drawn_line;///< 그려진 점들의 집합을 저장하는 vector. 즉 하나의 선을 저장하는 vector
protected:

public:
	/**
	* @brief 선 그리기 시작 함수
	* @details 선 그리기를 시작할 때 시작하는 시점에 실행되는 함수. 마우스 관련 메시지 식별자에서 사용하는 함수이다.
	* @param[in] hWnd 창 고유 ID
	* @param[in] lParam x, y 좌표가 담긴 변수(마우스 메시지가 아닐 경우 lParam 값이 다를 수 있음)
	*/
	void startDrawingLine(HWND hWnd, LPARAM lParam);
	/**
	* @brief 선 그리기 함수
	* @details 선을 그리고 그린 선에 대한 좌표값을 저장하는 함수. 마우스 관련 메시지 식별자에서 사용하는 함수이다. 
	startDrawingLine 함수가 먼저 실행되지 않으면 호출 되어도 동작하지 않고, 시작한 뒤에 endDrawingLine 함수를 실행하지 않으면 호출될 때마다 계속 동작한다.
	* @param[in] hWnd 창 고유 ID
	* @param[in] lParam x, y 좌표가 담긴 변수(마우스 메시지가 아닐 경우 lParam 값이 다를 수 있음)
	*/
	void drawingLine(HWND hWnd, LPARAM lParam);
	/**
	* @brief 선 그리기 종료 함수
	* @details 선을 그리기를 종료하고, 그린 선을 저장한다. 마우스 관련 메시지 식별자에서 사용하는 함수이다.
	* @param[in] hWnd 창 고유 ID
	* @param[in] lParam x, y 좌표가 담긴 변수(마우스 메시지가 아닐 경우 lParam 값이 다를 수 있음)
	*/
	void endDrawingLine(HWND hWnd, LPARAM lParam);
	/**
	* @brief 화면 전체 선 다시 그리기 함수
	* @details 화면 전체의 선을 다시 그리는 함수이다. 함수 외부에서 HDC 변수가 먼저 선언되었으며, 
	함수 실행 후 외부에서 HDC 반환한다는 전제 하에 만들어진 함수이다.
	* @param[in] hWnd 창 고유 ID
	* @param[in] hdc 그리기 작업을 수행할 디바이스 컨텍스트(DC) 핸들
	*/
	void drawWindowLines(HWND hWnd, HDC hdc);
};