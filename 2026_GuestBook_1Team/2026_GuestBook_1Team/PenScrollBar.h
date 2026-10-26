#pragma once
#include <Windows.h>
#include "PenStyle.h"

// 창 ID 메크로 상수
#define PEN_SCROLL_ID 1003 ///< PenScroll 창 상수 ID


/**
* @file
* @brief 펜 두께를 조절하는 스크롤바 창을 구현한 헤더 파일.
* @details 펜 두께를 조절하는 스크롤바 창을 구현한 헤더 파일.
* @author challenjoy01
*/


/**
* @brief 스크롤바 창 기능을 담당하는 클래스.
* @details 스크롤바 창 기능을 담당하는 클래스.
* @author challenjoy01
*/
class PenScrollBar
{
private:
	PenStyle* pen_style; ///< 펜 스타일 변경을 위해 펜 스타일 객체 주소를 받아오는 포인터

	static PenScrollBar* static_pen_scroll; ///< 클래스 내부의 static 함수에서 PenScrollBar 클래스를 사용하기 위한 PenScrollBar 클래스
	static bool is_pen_scroll_created; ///< PenScrollBar가 이미 존재하는지 알려주는 플래그
	HINSTANCE hInst = nullptr; ///< 인스턴스 핸들

	// 스크롤바 관련 스타일 변수
	int pen_scroll_wnd_x = 600;													///< penscroll 창 x 위치
	int pen_scroll_wnd_y = 20;													///< penscroll 창 y 위치

	int pen_scroll_bar_height = 10;												///< 스크롤바의 높이
	int pen_max_width = 25;														///< 펜의 최대 반지름
	int pen_scroll_bar_ratio = 8;												///< 스크롤바 비율 (50 * pen_scroll_bar_ratio 형식)
	int pen_scroll_bar_width = pen_max_width * pen_scroll_bar_ratio;			///< 스크롤바의 너비
	int pen_scroll_button_width = 30;											///< 스크롤바 버튼 너비
	
	int pen_scroll_wnd_height = 80;						///< penscroll 창 높이
	int pen_scroll_wnd_width = pen_scroll_bar_width + pen_scroll_button_width;	///< penscroll 창 너비
	int is_click = 0;
	int scroll_button_x = 0;

	HBRUSH pen_scroll_background = nullptr; ///< PenScroll 창 배경색 브러시 저장용 변수
	//HBRUSH pen_scroll_background = nullptr; ///< PenScroll 버튼색 브러시 저장용 변수
	HWND pen_scroll_hwnd = nullptr; ///< PenScroll 창 핸들


	//------------마우스 추적------------//
	bool is_tracking_mouse = false; ///< 마우스가 추적 중인지를 나타내는 변수 true이면 추적 중, false이면 추적 중이 아님을 나타냄
	TRACKMOUSEEVENT* track_mouse; ///< 마우스의 움직임을 추적하기 위해 사용하는 구조체

	/**
	* @brief 마우스가 창을 벗어나는 이벤트 감지를 시작하는 이벤트
	* @details 마우스가 창을 벗어나는 이벤트 감지를 시작하는 이벤트
	* @author challenjoy01
	*/
	void setTrackMouseEvent(HWND hWnd);


	//-----------------스크롤바 함수-----------------//
	/**
	* @brief 스크롤바 위치 정보 수정 및 펜 두께 변경 함수
	* @details 스크롤바 위치 정보 수정 및 펜 두께 변경 함수
	* @author challenjoy01
	*/
	void setScrollButton(HWND hWnd, LPARAM lParam);
	/**
	* @brief 스크롤바 버튼 그리기 함수
	* @details 스크롤바 버튼 그리기 함수
	* @author challenjoy01
	*/
	void drawScrollButton(HDC hdc);

	//------------------WndProc 함수---------------//
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
	static LRESULT CALLBACK StaticPenScrollWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	/**
	* @brief PenScrollRegisterClass에 등록할 WndProc에 연결될 WndProc 실행부
	* @details PenScrollRegisterClass에 등록할 WndProc에 연결될 WndProc 실행부. 실제로 실행하는 WndProc이다
	* @param[in] hWnd 메시지를 받을 윈도우의 핸들
	* @param[in] message 발생한 메시지 종류
	* @param[in] wParam 메시지 부가정보
	* @param[in] lParam 메시지 부가정보
	* @author challenjoy01
	*/
	LRESULT CALLBACK PenScrollWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
protected:
public:
	PenScrollBar();
	/**
	* @brief 스크롤바 윈도우 창 형태 정의 및 창 등록 함수
	* @details 스크롤바 윈도우 창 형태 정의 및 창 등록 함수
	* @param[in] hInstance 인스턴스 핸들
	* @author challenjoy01
	*/
	ATOM PenScrollRegisterClass(HINSTANCE hInstance);
	/**
	* @brief 필요한 창들을 생성하는 함수.
	* @details 필요한 창들을 생성하는 함수. 상위 창의 WndProc의 WM_CREATE에서 실행하는 것을 추천한다.
	* @author challenjoy01
	*/
	void createPenScrollWindows(HWND hWnd);
	/**
	* @brief 인스턴스 핸들 초기화 함수.
	* @details 인스턴스 핸들 초기화 함수. 상위 창의 인스턴스 핸들 초기화 함수 내부에서 실행하는 것을 추천한다.
	* @author challenjoy01
	*/
	void setInst(HINSTANCE hInst);
	/**
	* @brief 펜 스타일 포인터 초기화 함수
	* @details 펜 두께 변환을 위한 펜 스타일 포인터 초기화 함수. 상위 창의 WndProc의 WM_CREATE에서 실행하는 것을 추천한다.
	* @author challenjoy01
	*/
	void setPenStyle(PenStyle* pen_style);
};

