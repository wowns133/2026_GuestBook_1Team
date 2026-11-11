#pragma once
#include <Windows.h>
#include "PenStyle.h"
// gdiplus 사용을 위한 전처리문
#include <unknwn.h>			// framework.h 내부 #define WIN32_LEAN_AND_MEAN로 인해 gdiplus에 필요한 몇몇 헤더들이 제외되어 직접 넣음 
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include "Draw.h"
#include "Replay.h"


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
	static PenScrollBar* static_pen_scroll; ///< 클래스 내부의 static 함수에서 PenScrollBar 클래스를 사용하기 위한 PenScrollBar 클래스
	static bool is_pen_scroll_created; ///< PenScrollBar가 이미 존재하는지 알려주는 플래그
	HINSTANCE hInst = nullptr; ///< 인스턴스 핸들
	HWND pen_scroll_hwnd = nullptr; ///< PenScroll 창 핸들

	HBRUSH pen_scroll_background = nullptr; ///< PenScroll 창 배경색 브러시 저장용 변수
	
	//----------------------------------각종 크기 설정 관련 변수----------------------------------//
	
	// 현재 펜 표시 관련 변수
	int pen_sample_width = 250; ///< 현재 펜의 상태를 보여줄 영역의 너비

	// 스크롤바 관련 스타일 변수
	int pen_scroll_wnd_x = 600;													///< penscroll 창 x 위치
	int pen_scroll_wnd_y = 20;													///< penscroll 창 y 위치

	float wnd_start_x_ratio = 0.35;											///< penscroll 창이 시작하는 x위치가 부모 창의 몇%인지
	float wnd_end_x_ratio = 0.65;											///< penscroll 창이 끝나는 x위치가 부모 창의 몇%인지

	int pen_scroll_bar_height = 10;												///< 스크롤바의 높이
	int pen_max_radius = 25;													///< 펜의 최대 반지름
	int pen_min_radius = 5;														///< 펜의 최소 반지름
	int pen_scroll_bar_ratio = 8;												///< 스크롤바 배율 (50 * pen_scroll_bar_ratio 형식)
	int pen_scroll_bar_width = (pen_max_radius - pen_min_radius) * pen_scroll_bar_ratio;			///< 스크롤바의 너비
	int pen_scroll_button_width = 30;											///< 스크롤바 버튼 너비
	
	int pen_scroll_wnd_height = 80;												///< penscroll 창 높이
	int pen_scroll_wnd_width = (pen_scroll_bar_width + pen_scroll_button_width) + 5 + pen_sample_width; ///< penscroll 창 너비(스크롤바가 차지할 범위 + 여유공간 + 펜 샘플 공간 너비)
	int is_click = 0;															///< 클릭 중인지 나타내는 플래그 변수. 0 - 클릭 중 아님, 1 - 클릭 중
	int scroll_button_x = 0;													///< 스크롤바 버튼의 x 좌표


	//------------------------------비트맵 관련 맴버 변수--------------------------//
	Gdiplus::Bitmap* scroll_bar_bitmap = nullptr; ///< 더블 버퍼링 구현을 위한 Bitmap 객체 생성. 사용 후 삭제해야 함
	Gdiplus::Graphics* scroll_bar_bitmap_graphics = nullptr; ///< 비트맵 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함

	//-----------------------------펜 샘플 관련 맴버 변수----------------------------//
	Draw* draw_pointer = nullptr; ///< 현재 사용 중인 Draw 객체를 가져오기 위한 포인터
	//PenStyle* pen_style; ///< 펜 스타일 변경을 위해 펜 스타일 객체 주소를 받아오는 포인터
	//Gdiplus::Pen* pen_pointer = nullptr; ///< 현재 펜을 가져오기 위한 펜 포인터


	//------------마우스 추적------------//
	bool is_tracking_mouse = false; ///< 마우스가 추적 중인지를 나타내는 변수 true이면 추적 중, false이면 추적 중이 아님을 나타냄

	Replay* replay = nullptr; ///<WinUi에 생성된 Replay 객체의 주소를 저장하는 포인터. 리플레이 중 스크롤바 클릭을 처리하는 용도

	/**
	* @brief 마우스가 창을 벗어나는 이벤트 감지를 시작하는 이벤트
	* @details 마우스가 창을 벗어나는 이벤트 감지를 시작하는 이벤트
	* @author challenjoy01
	*/
	void setTrackMouseEvent(HWND hWnd);


	//------------------비트맵 관리 함수----------------//
	/**
	* @brief 비트맵 생성, 크기 변경 함수
	* @details 비트맵이 없을 땐 비트맵을 생성하고, 있을 경우는 크기를 변경하는 함수. WM_SIZE에서 실행하는 것을 상정한 함수이다.
	* @param[in] lParam WM_SIZE에서 사용했을 경우 창 높이, 너비가 담겨 있는 변수. LOWORD(lParam) -> 너비, HIWORD(lParam) -> 높이
	* @author challenjoy01
	*/
	void setupBitmap(LPARAM lParam);
	/**
	* @brief 비트맵 삭제 함수
	* @details 비트맵 삭제 함수. WM_DESTROY에서 실행하는 것을 상정한 함수이다.
	* @author challenjoy01
	*/
	void deleteBitmap();

	

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
	void drawScrollBar();
	
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
	* @brief 스크롤바 객체에 필요한 포인터를 초기화하는 함수
	* @details 필요한 포인터를 초기화하는 함수. Draw 객체 포인터를 초기화한다. 상위 창의 WndProc의 WM_CREATE에서 실행하는 것을 추천한다.
	* @author challenjoy01
	*/
	void setPointers(Draw* draw_pointer, Replay* replay);
	/**
	* @brief 스크롤바 창 핸들 getter
	* @details 스크롤바 창 핸들 getter
	* @author challenjoy01
	*/
	HWND getScrollBarHandle();

	//--------------------펜 샘플 관련 함수--------------//
	/**
	* @brief 펜 샘플 그리기 함수
	* @details 펜 샘플 그리기 함수
	* @author challenjoy01
	*/
	void drawPenSample();
	/**
	* @brief 펜 스크롤바 창 크기 조절 함수
	* @details 펜 스크롤바 창 크기 조절 함수
	* @author challenjoy01
	*/
	void resizePenScrollBar(HWND hWnd);
	/**
	* @brief 재생 중 버튼 조작에 대응하기 위한 함수
	* @details 재생 중 버튼 조작에 대응하기 위한 함수
	* @warning 이런 식의 코드는 좋지 않은 코드이니 이런 식으로 만들 일 없도록 하세요.
	* @author challenjoy01
	*/
	void tempSetScrollBar(LPARAM lParam);
	void tempDrawScrollBar(HDC hdc);
};

