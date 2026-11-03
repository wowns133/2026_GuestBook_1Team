#pragma once
///헤더 통합 파일을 넣기 전 임시 include문
#include <Windows.h>
#include <winuser.h>
#include "Draw.h"
#include "FileInOut.h"
#include "Delete.h"
#include "Replay.h"
#include "Button.h"
#include "PenScrollBar.h"
#include "ScreenSaver.h"


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

#define MAIN_UI_HEIGHT_RATIO 0.15 ///< 메인 UI창이 전체 높이 중 차지하는 비율을 정의한 매크로 상수

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
	HWND root_window_hwnd = nullptr; ///< 최상위 부모 창 핸들
	HWND main_ui_hwnd = nullptr; ///< 메인 UI 창의 핸들
	HWND canvas_hwnd = nullptr; ///< 캔버스 창의 핸들
	HWND screen_saver_hwnd = nullptr; ///< 화면 보호기 창의 핸들

	//----------------------------그림판 동작에 필요한 클래스들-----------------------//

	Draw* draw = nullptr; ///< 부모 창의 그리기 클래스 주솟값을 받아옴
	FileInOut file; ///< 파일입출력 클래스
	Replay replay; ///< 서명 재생 클래스
	AllDelete ac; ///< 지우기 클래스
	Button button; ///< 버튼 클래스
	PenScrollBar pen_scroll; ///< 펜 두께 조절 스크롤바 창 클래스
	ScreenSaver screen_saver; ///< 화면 보호기 창 클래스
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
protected:
public:
	WinUi(Draw* draw);
	/**
	* @brief 모든 윈도우 창 형태 정의 및 창 등록 함수
	* @details 모든 윈도우 창 형태 정의 및 창 등록 함수
	* @param[in] hInstance 인스턴스 핸들
	* @author challenjoy01
	*/
	void setRegisterClass(HINSTANCE hInstance);
	/**
	* @brief 필요한 창들을 생성하는 함수
	* @details 필요한 창들을 생성하는 함수. WM_CREATE 내부에서 실행하는 것을 추천합니다.
	* @note 이 클래스의 바로 위 창이 최상위 창일 경우에는 screen_saver.setOtherWindowsHandle(root_window_hwnd, main_ui_hwnd, canvas_hwnd); 대신
	* screen_saver.setOtherWindowsHandle(hWnd, main_ui_hwnd, canvas_hwnd);로 사용하는 것이 안정적입니다.
	* 이 창의 부모가 최상위 창이 아닐 가능성이 있을 경우 root_window_hwnd로 사용해야 하며, root_window_hwnd를 사용할 경우 setRootWindowHandle 함수와의 순서에
	* 주의해야 합니다.
	* @author challenjoy01
	*/
	void createWinUIWindows(HWND hWnd);
	/**
	* @brief 창 크기 변화를 담당하는 함수
	* @details 창 크기 변화를 담당하는 함수
	* @param[in] hInstance 인스턴스 핸들
	* @author challenjoy01
	*/
	void resizeWinUIWindows(LPARAM lParam);
	/**
	* @brief 프로세스 종료 전 창을 포함한 자원 회수를 위해 실행할 함수
	* @details 프로세스 종료 전 창을 포함한 자원 회수를 위해 실행할 함수
	* @author challenjoy01
	*/
	void deleteWinUIWindows();

	/**
	* @brief 화면 보호기 실행까지의 시간을 초기화하는 함수.
	* @details 화면 보호기 실행까지의 시간을 초기화하는 함수.
	* @author challenjoy01
	*/
	void resetScreenSaverTimer();

	//-------------------------데이터 세팅하는 함수들--------------------//
	/**
	* @brief 인스턴스 핸들 초기화 함수.
	* @details 인스턴스 핸들 초기화 함수.
	* @param[in] hInst 인스턴스 핸들
	* @author challenjoy01
	*/
	void setInst(HINSTANCE hInst);
	/**
	* @brief 최상위 부모 창 핸들 멤버 변수 초기화 함수.
	* @details 최상위 부모 창 핸들 변수 초기화 함수. 
	* @param[in] hWnd 최상위 부모 창 핸들
	* @warning 이 함수를 실행할 때는 주의할 점이 몇 가지 있습니다
	1. createWinUiWindows 함수보다 선행되어 실행되어야 합니다.
	2. createWinUiWindows 함수가 WM_CREATE에서 실행하도록 만들 경우, 이 함수를 WM_CREATE에서 createWinUiWindows 함수보다 선행되어 실행해야 합니다. 이유는 아래와 같습니다.
		- CreateWindow로 창을 만들 경우 HWND를 반환하기 전에 WndProc에 진입하여 WM_CREATE를 포함한 각종 메시지들을 호출합니다. 
		- HWND 반환 전에 WM_CREATE 메시지가 발생하므로 HWND hWnd = CreateWindow(); 뒤에 이 함수를 실행하면 이 함수보다 createWinUiWindows 함수가 먼저 실행됩니다.
		- createWinUiWindows 함수가 먼저 실행되면 root_window_hwnd가 초기화되지 않은 상태로 createWinUiWindows가 실행되어 일부 기능이 동작하지 않습니다.
	* @author challenjoy01
	*/
	void setRootWindowHandle(HWND hWnd);
};


