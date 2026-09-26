#pragma once
///헤더 통합 파일을 넣기 전 임시 include문
#include <Windows.h>
#include <winuser.h>
#include "Draw.h"
#include "FileInOut.h"
#include "Delete.h"
#include "Replay.h"
#include "Button.h"

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

// 창 ID 메크로 상수
#define MAIN_UI_ID 1001
#define CANVAS_ID 1002

/**
* @file
* @brief 자식 창 관련 기능들을 구현한 헤더 파일.
* @details 자식 창 관련 기능들을 구현한 헤더 파일.
* @author challenjoy01
*/



void makeTempButton(HWND hWnd, HINSTANCE hInst);

/**
* @brief 자식 창 기능을 담당하는 클래스.
* @details 자식 창 기능을 담당하는 클래스.
* @author challenjoy01
*/
class WinUi
{
private:
	static WinUi* static_winui; ///< 클래스 내부의 static 함수에서 WinUi 클래스를 사용하기 위한 WinUi 클래스
	static bool is_winui_created; ///< WinUi가 이미 존재하는지 알려주는 플래그
	HINSTANCE hInst = nullptr; ///< 인스턴스 핸들
	HWND main_ui_hwnd = nullptr;
	HWND canvas_hwnd = nullptr;
	//----------------------------그림판 동작에 필요한 클래스들-----------------------//

	Draw* draw = nullptr; ///< 부모 창의 그리기 클래스 주솟값을 받아옴
	FileInOut file; ///< 파일입출력 클래스
	Replay replay; ///< 서명 재생 클래스
	AllDelete ac; ///< 지우기 클래스
	Button button;
	//--------------------------------MainUI용 변수들--------------------------------------//
	HBRUSH main_ui_background = nullptr; ///< MainUI 창 배경색 브러시 저장용 변수
	HBRUSH canvus_background = nullptr; ///< Canvus 창 배경색 브러시 저장용 변수

	/**
	* @brief MainUIRegisterClass에 등록할 WndProc
	* @details MainUIRegisterClass에 등록할 WndProc. 실제로 실행하는 WndProc를 리턴한다.
	* @param[in] hWnd 메시지를 받을 윈도우의 핸들
	* @param[in] message 발생한 메시지 종류
	* @param[in] wParam 메시지 부가정보
	* @param[in] lParam 메시지 부가정보
	* @return static_winui->MainUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 실제 실행되는 WndProc 본체
	* @note 이 함수 내에서 win->멤버클래스->멤버함수 형태로 만드는 것보다 일반 멤버 함수로 연결하는 것이 깔끔하다고 판단하여 이렇게 만들었습니다.
	* 이 함수 하나로 끝내고 싶다면 win->멤버클래스->멤버함수 형태로도 구현 가능합니다.
	* @author challenjoy01
	*/
	static LRESULT CALLBACK StaticMainUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	/**
	* @brief MainUIRegisterClass에 등록할 WndProc에 연결될 WndProc 실행부
	* @details MainUIRegisterClass에 등록할 WndProc에 연결될 WndProc 실행부. 실제로 실행하는 WndProc이다
	* @param[in] hWnd 메시지를 받을 윈도우의 핸들
	* @param[in] message 발생한 메시지 종류
	* @param[in] wParam 메시지 부가정보
	* @param[in] lParam 메시지 부가정보
	* @author challenjoy01
	*/
	LRESULT CALLBACK MainUIWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	/**
	* @brief CanvasRegisterClass에 등록할 WndProc
	* @details CanvasRegisterClass에 등록할 WndProc. 실제로 실행하는 WndProc를 리턴한다.
	* @param[in] hWnd 메시지를 받을 윈도우의 핸들
	* @param[in] message 발생한 메시지 종류
	* @param[in] wParam 메시지 부가정보
	* @param[in] lParam 메시지 부가정보
	* @return static_winui->CanvasWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 실제 실행되는 WndProc 본체
	* @note 이 함수 내에서 win->멤버클래스->멤버함수 형태로 만드는 것보다 일반 멤버 함수로 연결하는 것이 깔끔하다고 판단하여 이렇게 만들었습니다.
	* 이 함수 하나로 끝내고 싶다면 win->멤버클래스->멤버함수 형태로도 구현 가능합니다.
	* @author challenjoy01
	*/
	static LRESULT CALLBACK StaticCanvasWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK CanvasWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
public:
	WinUi(HINSTANCE hInst, Draw* draw);
	/**
	* @brief 필요한 창들을 생성하는 함수
	* @details 필요한 창들을 생성하는 함수
	* @author challenjoy01
	*/
	void createWinUIWindows(HWND hWnd);
	void resizeWinUIWindows(LPARAM lParam);
	/**
	* @brief 프로세스 종료 전 창을 포함한 자원 회수를 위해 실행할 함수
	* @details 프로세스 종료 전 창을 포함한 자원 회수를 위해 실행할 함수
	* @author challenjoy01
	*/
	void deleteWinUIWindows();








	/**
	* @brief 메인 UI 윈도우 창 형태 정의 및 창 등록 함수
	* @details 메인 UI 윈도우 창 형태 정의 및 창 등록 함수
	* @param[in] hInstance 인스턴스 핸들
	* @author challenjoy01
	*/
	ATOM MainUIRegisterClass(HINSTANCE hInstance);
	/**
	* @brief 그림 영역 윈도우 창 형태 정의 및 창 등록 함수
	* @details 그림 영역 윈도우 창 형태 정의 및 창 등록 함수
	* @param[in] hInstance 인스턴스 핸들
	* @author challenjoy01
	*/
	ATOM CanvasRegisterClass(HINSTANCE hInstance);

};


