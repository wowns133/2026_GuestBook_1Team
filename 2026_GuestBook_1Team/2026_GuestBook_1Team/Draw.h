#pragma once

#include <Windows.h>
#include <unknwn.h>			// framework.h 내부 #define WIN32_LEAN_AND_MEAN로 인해 gdiplus에 필요한 몇몇 헤더들이 제외되어 직접 넣음 
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <vector>

/// 펜 스타일 임시 헤더파일 
#include "PenStyle.h"

///펜 모드 define
#define PEN_MODE_ER 0
#define PEN_MODE_NORMAL_PEN 1
#define PEN_MODE_ARGB_PEN 2

/**
* @file
* @brief 그리기 기능을 구현하는 헤더 파일.
* @details 그리기 기능을 구현한 헤더 파일.
* @todo
* 1. 수정된 자료 구조에 맞게 저장하는 부분 등 수정, 점 하나일 때 그리는 거 제대로 작동 안함. end쪽으로 옮기는거 검토
* 2. redrawAllLines함수 선마다 적절한 스타일 적용되도록 만들기, DrawPointData 구조체 정보 추가하기
* 3. 서명 시작, 끝 함수 추가
* 4. pen 담당자에게 질문 : pen_pointer 없애는 코드 없어도 문제 없는지, pen_pointer에 주소 넘기는 건 한번만 하면 되는지
* 4. drawWindowLines 함수 최적화
* 5. 포인터와 new로 객체 생성하는 부분이 다수 존재하므로 스마트 포인터 사용을 고려해보기
* 6. 상하 크기조절 시 이상현상
* 7. 추후 가능하면 삼중 버퍼링의 전체 그림 버퍼에 대한 메모리 관리를 좀 더 효율적으로 바꾸기
* @warning
* 1. startDrawingLine 내부에는 객체 생성 등 자원 해제가 필요한 부분이 존재합니다. startDrawingLine 실행 후에는
* 반드시 EndDrawingLine이 실행될 수 있도록 주의해야 합니다.
* 2. Draw.h 내부에 선언한 Gdiplus 관련 많은 핵심 요소들은(데이터 타입, 단순 구조체 등 일부 요소 제외한 모든 요소)
* GdiplusStartup이 실행되기 전에는 정상적으로 만들어지지 않습니다.
* Draw.h 내부에 선언한 요소들이 gdiPlusStart 함수가 실행된 후 선언될 방법은 없으므로(Draw 객체가 만든 뒤에 gdiPlusStart 함수를 실행하기에)
* Draw.h 내부에 선언할 객체는 포인터로 선언한 후 nullptr를 넣어 놓고 추후 함수 내부에서 new로 초기화해야 합니다.
* Draw.h 외부 함수로 GdiplusStartup을 실행한 뒤 Draw 객체를 선언할 생각이라면 Draw.h 내부에서도 자유롭게 선언할 수 있습니다.
* GdiplusStartup, GdiplusShutdown을 생성자, 소멸자에 배정하는 것도 고려해 볼 만한 방법입니다.
* @author challenjoy01
*/





/**
* @brief 점의 데이터를 저장하기 위한 구조체
* @details
* @author challenjoy01
*/
struct DrawPointData
{
	POINT point; ///< 좌표를 저장하는 변수
	ULONGLONG elapsed_time; ///< 시간을 저장하는 변수
	float radius; ///< 반지름을 저장하는 변수
};
/**
* @brief 선의 데이터를 저장하기 위한 구조체
* @details
* @author challenjoy01
*/
struct DrawLineData
{
	bool is_pen = TRUE; ///< 그리는 중인지 지우개 쓰는 중인지 판별. 그리기 TRUE, 지우개 FALSE
	int select_pen_style; ///< 펜 스타일 정보를 저장하는 변수
	int select_color; ///< 펜 색 정보를 저장하는 변수
	std::vector<DrawPointData> point_data;
};


/**
* @brief 그리기 기능을 담당하는 클래스.
* @details 그리기 기능을 담당하는 클래스.
* @author challenjoy01
*/
class Draw
{
private:
	int previous_x = 0; ///< 선을 그릴 때 기준이 되는 좌표. 이전 x 좌표
	int previous_y = 0; ///< 선을 그릴 때 기준이 되는 좌표. 이전 y 좌표
	int current_x = 0; ///< 선을 그릴 때 기준이 되는 좌표. 현재 x 좌표
	int current_y = 0; ///< 선을 그릴 때 기준이 되는 좌표. 현재 y 좌표

	bool is_drawing = false;///< 마우스 클릭 상태인지 표시하는 플래그 변수

	ULONGLONG start_time; ///< 선을 그릴 때 시작이 되는 지점(기준점)


	//------------------------GDI+---------------------//
	ULONG_PTR drawing_token; ///< GDI 토큰 핸들

	// 화면 출력용
	HDC draw_hdc; ///< Draw 클래스 멤버 변수로 선언된 HDC
	Gdiplus::Graphics* draw_hdc_graphics = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 Graphics. hdc를 삭제하기 전 반드시 파괴해야 한다.

	// 선 그리기용
	Gdiplus::Bitmap* draw_bmp = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 Bitmap. 실시간으로 그리는 도중 버퍼 용도로 사용된다 
	Gdiplus::Graphics* draw_bmp_graphics = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 실시간 버퍼용 Graphics. Bitmap를 삭제하기 전 반드시 파괴해야 한다.

	// 전체 그림 보관용
	Gdiplus::Bitmap* drawn_bmp = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 Bitmap. 지금까지 그려진 완성된 선 전체를 온전히 보관하는 비트맵
	Gdiplus::Graphics* drawn_bmp_graphics = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 지금까지 그려진 완성된 선 전체를 온전히 보관하는 비트맵용 Graphics. Bitmap를 삭제하기 전 반드시 파괴해야 한다.

	RECT client_rect; ///< 작업 영역의 크기를 저장하기 위한 RECT. right가 너미, bottom이 높이.


	//--------------국소 부위 화면 갱신을 위한 변수-------------//
	int draw_image_area_start_x = 0; ///< 화면을 갱신할 범위의 좌상단 x좌표
	int draw_image_area_start_y = 0; ///< 화면을 갱신할 범위의 좌상단 y좌표
	int draw_image_area_width = 0; ///< 화면을 갱신할 범위의 너비
	int draw_image_area_height = 0; ///< 화면을 갱신할 범위의 높이
	int pen_width = 15; ///< 현재 펜의 반지름 + 5


	//------------마우스 추적------------//
	bool is_tracking_mouse = false; ///< 마우스가 추적 중인지를 나타내는 변수 true이면 추적 중, false이면 추적 중이 아님을 나타냄
	TRACKMOUSEEVENT* track_mouse; ///< 마우스의 움직임을 추적하기 위해 사용하는 구조체


	//----------------------------Pen-----------------------------//
	Gdiplus::Pen* pen_pointer;


protected:

public:
	PenStyle* pen_style; ///< 펜 스타일 

	bool is_pen = true; ///< 펜인지 지우개인지 구별하기 위한 변수. true일때 펜, false일때 지우개
	std::vector<std::vector<DrawPointData>> drawn_lines; ///< 선들의 집합을 저장하는 vector. 즉 모든 선을 저장하는 vector
	std::vector<DrawPointData> drawn_line; ///< 그려진 점들의 집합을 저장하는 vector. 즉 하나의 선을 저장하는 vector

	std::vector<DrawLineData> drawn_lines_data;

	Gdiplus::GraphicsPath* line_path; ///< DrawPath로 그리기 위한 점들의 정보를 담은 GraphicsPath 클래스 객체. 점 좌표와 역할에 대한 정보가 들어간다.

	int select_drawStyle = 1;            ///< 그리기 상수 정하는 변수 (PenStyle test 진행중...)

	//-------------------------데이터 관리 함수---------------------//
	/**
	* @brief 화면에 그려진 모든 선 데이터를 초기화 시킴
	* @details 누적된 벡터(drawn_lines, drawn_line)를 완전히 비워 화면을 지운 상태로 되돌림
	*/
	void ac_lines();
	/**
	* @brief 화면에 그려진 모든 선 데이터를 가져옴
	* @details 누적된 벡터(drawn_lines)에 저장된 모든 선 데이터를 참조 형태로 반환함.
	* 원본 데이터를 복사하지 않고 가져와 메모리 사용을 줄임.
	*/
	const std::vector<DrawLineData>& getDrawnLines();
	/**
	* @brief 지정한 번호의 선을 삭제함
	* @details 전달받은 인덱스(index)에 해당하는 선 데이터를 drawn_lines에서 삭제함
	*/
	void sc_line(int index);


	//------------------------------그리기 함수----------------------//
	void redrawAllLines(HWND hWnd);
	/**
	* @brief 더블 버퍼링이 적용된 선 그리기 시작 함수. 선 그리기에 필요한 각종 값 설정 및 객체 생성을 맡는다.
	* @details 선 그리기를 시작할 때 시작하는 시점에 실행되는 함수. 마우스 관련 메시지 식별자에서 사용하는 함수이다.
	* 핵심 동작은 아래와 같다.
	* 1. 비트맵 생성
	* 2. 비트맵에 그리기를 담당할 Graphics 객체 생성
	* 3. hdc 생성
	* 4. hdc에 그리기를 담당할 Graphics 객체 생성
	* 5. 이전 좌표 변수에 현재 좌표를 저장
	* 6. 현재 좌표를 새로운 선 vector에 저장
	* 7. 그리는 중이라고 플래그 표시
	* @param[in] hWnd 창 고유 핸들
	* @param[in] lParam x, y 좌표가 담긴 변수(마우스 메시지가 아닐 경우 lParam 값이 다를 수 있음)
	* @warning startDrawingLine 내부에는 객체 생성 등 자원 해제가 필요한 부분이 존재합니다. startDrawingLine 실행 후에는
	* 반드시 EndDrawingLine이 실행될 수 있도록 주의해야 합니다.
	* @author challenjoy01
	*/
	void startDrawingLine(HWND hWnd, LPARAM lParam, int pen_mode);
	/**
	* @brief 더블 버퍼링이 적용된 선 그리기 함수. startDrawingLine 함수가 선행되어야 정상 작동한다. 실행 후 반드시 endDrawingLine 함수를 실행해야 한다.
	* @details 선을 그리고 그린 선에 대한 좌표값을 저장하는 함수. 그리기 동작에는 더블 버퍼링 기법이 적용되어 있다. 마우스 관련 메시지 식별자에서 사용하는 함수이다.
	* startDrawingLine 함수가 먼저 실행되지 않으면 호출 되어도 동작하지 않고, 시작한 뒤에 endDrawingLine 함수를 실행하지 않으면 호출될 때마다 계속 동작한다.
	* 핵심 동작은 아래와 같다.(startDrawingLine으로 인해 플래그가 바뀌었을 경우에만 실행되는 동작이다.)
	* 1. 펜 생성 및 설정
	* 2. 현재 위치 변수에 현재 위치를 저장
	* 3. 현재 위치를 선 벡터에 저장
	* 4. 비트맵에 선 그리기
	* 5. 이전 좌표 변수에 현재 좌표를 저장
	* 6. 비트맵을 화면에 출력
	* @param[in] hWnd 창 고유 핸들
	* @param[in] lParam x, y 좌표가 담긴 변수(마우스 메시지가 아닐 경우 lParam 값이 다를 수 있음)
	* @author challenjoy01
	*/
	void drawingLine(HWND hWnd, LPARAM lParam, int pen_mode);
	/**
	* @brief 더블 버퍼링이 적용된 선 그리기 함수. startDrawingLine 함수가 선행되어야 정상 작동한다. 실행 후 반드시 endDrawingLine 함수를 실행해야 한다.
	* @details 선을 그리고 그린 선에 대한 좌표값을 저장하는 함수. 그리기 동작에는 더블 버퍼링 기법이 적용되어 있다. 마우스 관련 메시지 식별자에서 사용하는 함수이다.
	* startDrawingLine 함수가 먼저 실행되지 않으면 호출 되어도 동작하지 않고, 시작한 뒤에 endDrawingLine 함수를 실행하지 않으면 호출될 때마다 계속 동작한다.
	* 핵심 동작은 아래와 같다.(startDrawingLine으로 인해 플래그가 바뀌었을 경우에만 실행되는 동작이다.)
	* 1. 펜 생성 및 설정
	* 2. 현재 위치 변수에 현재 위치를 저장
	* 3. 현재 위치를 선 벡터에 저장
	* 4. 비트맵에 선 그리기
	* 5. 이전 좌표 변수에 현재 좌표를 저장
	* 6. 비트맵을 화면에 출력
	* @param[in] hWnd 창 고유 핸들
	* @param[in] lParam x, y 좌표가 담긴 변수(마우스 메시지가 아닐 경우 lParam 값이 다를 수 있음)
	* @author challenjoy01
	*/
	void drawingLineRGB(HWND hWnd, LPARAM lParam);

	void drawingLineARGB(HWND hWnd, LPARAM lParam);
	/**
	* @brief 더블 버퍼링이 적용된 선 그리기 종료 함수
	* @details 선 그리기를 종료하고, 그린 선을 저장하며, 사용이 끝난 객체를 삭제한다. 마우스 관련 메시지 식별자에서 사용하는 함수이다.
	* 핵심 동작은 아래와 같다.
	* 1. 그리기를 마친 선을 저장하고 선 벡터의 내용물을 비운다.
	* 2. 그리기를 완료했다고 플래그를 표시
	* 3. HDC, 비트맵, 객체 파괴
	* @param[in] hWnd 창 고유 핸들
	* @param[in] lParam x, y 좌표가 담긴 변수(마우스 메시지가 아닐 경우 lParam 값이 다를 수 있음)
	* @author challenjoy01
	*/
	void endDrawingLine(HWND hWnd, LPARAM lParam, int pen_mode);
	/**
	* @brief 화면 전체 선 다시 그리기 함수
	* @details 화면 전체의 선을 다시 그리는 함수이다. 함수 외부에서 HDC 변수가 먼저 선언되었으며,
	함수 실행 후 외부에서 HDC 반환한다는 전제 하에 만들어진 함수이다.
	* @param[in] hWnd 창 고유 ID
	* @param[in] hdc 그리기 작업을 수행할 디바이스 컨텍스트(DC) 핸들
	* @author challenjoy01
	*/
	void drawWindowLines(HWND hWnd, HDC hdc);


	//------------------------GDI+ START, END 함수---------------------//
	/**
	* @brief GDI+ 사용 시작하는 함수. 또한 그려진 그림 전체를 보관하는 버퍼와 펜 객체를 생성한다.
	* @details GDI+ 사용을 시작하는 함수이다. 동시에 그려진 그림 전체를 보관하는 버퍼와 펜 객체를 생성한다.
	* GdiplusStartup를 실행한다. 프로그램 시작 시 한 번 사용하고, 마지막 프로그램 종료 직전 GdiplusShutdown 함수를 실행해야 한다.
	* @author challenjoy01
	*/
	void gdiPlusStart();
	/**
	* @brief GDI+ 사용 종료하는 함수. 또한 gdiPlusStart 함수에서 생성한 버퍼, 펜 객체를 해제한다.
	* @details GDI+ 사용을 종료하는 함수이다. 또한 gdiPlusStart 함수에서 생성한 버퍼, 펜 객체를 해제한다.
	* GdiplusShutDown를 실행한다. 프로그램 종료 전 한 번 사용한다.
	* @author challenjoy01
	*/
	void gdiPlusEnd();


	//-------------------------화면 국소 범위 출력 함수----------------//

	void drawSectionImage(Gdiplus::Graphics* output_graphics, Gdiplus::Bitmap* input_bitmap, int previous_x, int previous_y, int current_x, int current_y);


	//-------------------------마우스 추적 세팅 함수-----------------//
	/**
	* @brief 마우스가 창을 벗어나는 이벤트 감지를 시작하는 이벤트
	* @details GDI+ 사용을 종료하는 함수이다.
	* GdiplusShutDown를 실행한다. 프로그램 종료 전 한 번 사용한다.
	* @author challenjoy01
	*/
	void setTrackMouseEvent(HWND hWnd);


	//------------------------멤버 변수 값 세팅 함수--------------------------------//
	void setIsPen(bool is_pen);
	/**
	* @brief 펜 스타일에 따라 그리기 스타일 정하는 함수 (PenStyle test 진행중..)
	*/
	void selectDrawStyle();

	/// select_drawStyle변수와 pen_width 변수 초기값 설정 (PenStyle test 진행중..)
	//Draw();

	/// pen_width 변수를 펜 클래스 변수의 radius값에 따라 변경되도록 하는 함수(PenStyle test 진행중..)
	//void penWidthChange();

};