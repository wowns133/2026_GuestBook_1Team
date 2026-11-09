#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include "Replay.h"
#include "FileInOut.h"
#include "Draw.h"

#define SCREEN_SAVER_WINDOW_ID 1010 ///< 화면 보호기 창 ID
#define SCREEN_SAVER_TIMER_ID 10 ///< 화면 보호기 타이머 ID
#define SCREEN_SAVER_REPLAY_TIMER_ID 2 ///<리플레이 재생 타이머 ID

#define SCREEN_SAVE_TIME_LIMIT 30 ///< 화면 보호기 실행까지의 걸리는 시간(초)

/**
* @file
* @brief 화면 보호기 창과 기능을 구현한 헤더 파일.
* @details 화면 보호기 창과 기능을 구현한 헤더 파일.
* @warning
* 1. 스레드를 여러 개로 나눌 경우 reset_screen_saver_timer 함수와 screen_saver_timer 함수의 동시동작이 발생해 screen_saver_time에 동시 접근이 발생할 수
* 있습니다. 스레드를 나누는 경우 임계 영역을 만들어야 합니다.
* @author challenjoy01
*/


/**
* @brief 화면 보호기 창과 기능을 담당하는 클래스.
* @details 화면 보호기 창과 기능을 담당하는 클래스.
* @author challenjoy01
*/
class ScreenSaver
{
private:
	Replay* replay = nullptr;///<현재 사용중인 리플레이 객체를 사용할 수 있도록 합니다
	std::vector<std::wstring> fileList;///<파일 이름 목록을 저장하는 벡터
	int currentFileIndex = 0;///<현재 재생할 파일의 인덱스
	Draw screenSaverDraw;
	FileInOut fileInOut;
	void startScreenSaverReplay();
	static ScreenSaver* static_screen_saver; ///< 클래스 내부의 static 함수에서 ScreenSaver 클래스를 사용하기 위한 ScreenSaver 클래스 포인터
	static bool is_screen_saver_created; ///< ScreenSaver가 이미 존재하는지 알려주는 플래그
	HINSTANCE hInst = nullptr; ///< 인스턴스 핸들
	HBRUSH screen_saver_background = nullptr; ///< PenScroll 창 배경색 브러시 저장용 변수

	HWND screen_saver_hwnd = nullptr; ///< 화면 보호기 창 핸들

	HWND root_window_hwnd = nullptr; ///< 최상위 부모 창 핸들
	HWND main_ui_hwnd = nullptr; ///< 메인 ui 창 핸들
	HWND canvas_hwnd = nullptr; ///< 캔버스 창 핸들

	int screen_saver_time = SCREEN_SAVE_TIME_LIMIT; ///< 화면 보호기 실행까지의 시간(초)
	int is_first_mouse_move = 1; ///< 창 생성 후 첫 번째 mousemove 이벤트인지 판단하기 위한 변수. 1 - 첫 실행, 0 or under - 첫 실행이 아님
	int screen_on_mouse_x = 0; ///< 화면 보호기가 켜진 순간의 마우스 x 좌표
	int screen_on_mouse_y = 0; ///< 화면 보호기가 켜진 순간의 마우스 x 좌표
	int is_screen_saver_on = 0; ///< 창이 켜져 있는지를 나타내는 플래그. 0 - 창 꺼짐, 1 - 창 켜짐
	//------------------WndProc 함수---------------//
	/**
	* @brief 화면 보호기 실행 종료 함수.
	* @details 화면 보호기를 숨기고, 화면 보호기 실행까지의 타이머를 다시 실행하는 함수.
	* @author challenjoy01
	*/
	void hideScreenSaver();

	/**
	* @brief PenScrollRegisterClass에 등록할 WndProc
	* @details PenScrollRegisterClass에 등록할 WndProc. 실제로 실행하는 WndProc를 리턴한다.
	* @param[in] hWnd 메시지를 받을 윈도우의 핸들
	* @param[in] message 발생한 메시지 종류
	* @param[in] wParam 메시지 부가정보
	* @param[in] lParam 메시지 부가정보
	* @return static_pen_scroll->PenScrollWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) 실제 실행되는 WndProc 본체
	* @note 이 함수 내에서 win->멤버클래스->멤버함수 형태로 만드는 것보다 일반 멤버 함수로 연결하는 것이 깔끔하다고 판단하여 이렇게 만들었습니다.
	* 이 함수 하나로 끝내고 싶다면 win->멤버클래스->멤버함수 형태로도 구현 가능합니다.
	* @author challenjoy01
	*/
	static LRESULT CALLBACK StaticScreenSaverWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	/**
	* @brief PenScrollRegisterClass에 등록할 WndProc에 연결될 WndProc 실행부
	* @details PenScrollRegisterClass에 등록할 WndProc에 연결될 WndProc 실행부. 실제로 실행하는 WndProc이다
	* @param[in] hWnd 메시지를 받을 윈도우의 핸들
	* @param[in] message 발생한 메시지 종류
	* @param[in] wParam 메시지 부가정보
	* @param[in] lParam 메시지 부가정보
	* @author challenjoy01
	*/
	LRESULT CALLBACK ScreenSaverWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
public:
	ScreenSaver();

	/**
	* @brief 스크롤바 윈도우 창 형태 정의 및 창 등록 함수
	* @details 스크롤바 윈도우 창 형태 정의 및 창 등록 함수
	* @param[in] hInstance 인스턴스 핸들
	* @author challenjoy01
	*/
	ATOM ScreenSaverRegisterClass(HINSTANCE hInstance);

	/**
	* @brief 필요한 창들을 생성하는 함수.
	* @details 필요한 창들을 생성하는 함수. 상위 창의 WndProc의 WM_CREATE에서 실행하는 것을 추천한다.
	* @param[in] hWnd 상위 창 핸들
	* @author challenjoy01
	*/
	HWND createScreenSaverWindow(HWND hWnd);
	/**
	* @brief 인스턴스 핸들 초기화 함수.
	* @details 인스턴스 핸들 초기화 함수. 상위 창의 인스턴스 핸들 초기화 함수 내부에서 실행하는 것을 추천한다.
	* @param[in] hInstance 인스턴스 핸들
	* @author challenjoy01
	*/
	void setInst(HINSTANCE hInst);
	/**
	* @brief 다른 창들의 핸들 초기화 함수.
	* @details 다른 창들의 핸들 초기화 함수.
	* @param[in] main_ui_hwnd 메인 UI 창의 핸들
	* @param[in] canvas_hwnd 캔버스 창의 핸들
	* @author challenjoy01
	*/
	void setOtherWindowsHandle(HWND root_window_hwnd, HWND main_ui_hwnd, HWND canvas_hwnd);
	/**
	* @brief 화면 보호기 WM_TIMER 동작 함수.
	* @details 화면 보호기 실행까지의 남은 시간을 줄이고, 남은 시간이 없다면 화면 보호기를 실행과 기타 처리를 맡는 함수.
	* @author challenjoy01
	*/
	void screenSaverTimer();
	/**
	* @brief 화면 보호기 실행까지의 시간을 초기화하는 함수.
	* @details 화면 보호기 실행까지의 시간을 초기화하는 함수.
	* @author challenjoy01
	*/
	void resetScreenSaverTimer();

	void setReplay(Replay* replay);
};

