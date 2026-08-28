#pragma once

#include <Windows.h>
#include <unknwn.h>			// framework.h 내부 #define WIN32_LEAN_AND_MEAN로 인해 gdiplus에 필요한 몇몇 헤더들이 제외되어 직접 넣음 
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <vector>


/**
* @file
* @brief 그리기 기능을 구현하는 헤더 파일.
* @details 그리기 기능을 구현한 헤더 파일.
* @todo 
* 1. 화면 안에서 클릭한 후 화면 밖에서 땠을 때 다시 화면 안으로 돌아오면 화면 밖에서 생긴 이벤트를 감지할 수 없는 문제 해결
* 2. 그릴 때 시간 측정하는 기능 추가
* 3. drawingLine에는 임시 펜이 적용되어 있음. 추후 수정
* 4. 빠르게 그렸을 때 선이 각지게 그려지는 문제 해결
* 5. drawWindowLines 함수 더블 버퍼링 버전으로 변경
* 6. 포인터와 new로 객체 생성하는 부분이 다수 존재하므로 스마트 포인터 사용을 고려해보기
* 7. 상하 크기조절 시 이상현상
* @warning startDrawingLine 내부에는 객체 생성 등 자원 해제가 필요한 부분이 존재합니다. startDrawingLine 실행 후에는 
* 반드시 EndDrawingLine이 실행될 수 있도록 주의해야 합니다.
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
	bool is_pen = TRUE;///< 그리는 중인지 지우개 쓰는 중인지 판별. 그리기 TRUE, 지우개 FALSE
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

	
	//------------------------GDI+---------------------//
	ULONG_PTR drawing_token; ///< GDI 토큰 핸들

	HDC draw_hdc; ///< Draw 클래스 멤버 변수로 선언된 HDC
	Gdiplus::Graphics* draw_hdc_graphics = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 Graphics. hdc를 삭제하기 전 반드시 파괴해야 한다.
	Gdiplus::Bitmap* draw_bmp = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 Bitmap
	Gdiplus::Graphics* draw_bmp_graphics = nullptr; ///< Draw 클래스 멤버 포인터 변수로 선언된 Graphics. Bitmap를 삭제하기 전 반드시 파괴해야 한다.

	RECT client_rect; ///< 작업 영역의 크기를 저장하기 위한 RECT. right가 너미, bottom이 높이.


protected:

public:
	std::vector<std::vector<DrawPointData>> drawn_lines; ///< 선들의 집합을 저장하는 vector. 즉 모든 선을 저장하는 vector
	std::vector<DrawPointData> drawn_line; ///< 그려진 점들의 집합을 저장하는 vector. 즉 하나의 선을 저장하는 vector
	ULONGLONG start_time; ///< 



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
	void startDrawingLine(HWND hWnd, LPARAM lParam);
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
	void drawingLine(HWND hWnd, LPARAM lParam);
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
	void endDrawingLine(HWND hWnd, LPARAM lParam);
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
	* @brief GDI+ 사용 시작하는 함수
	* @details GDI+ 사용을 시작하는 함수이다.
	* GdiplusStartup를 실행한다. 프로그램 시작 시 한 번 사용하고, 마지막 프로그램 종료 직전 GdiplusShutdown 함수를 실행해야 한다.
	* @author challenjoy01
	*/
	void gdiPlusStart();

	/**
	* @brief GDI+ 사용 종료하는 함수
	* @details GDI+ 사용을 종료하는 함수이다.
	* GdiplusShutDown를 실행한다. 프로그램 종료 전 한 번 사용한다.
	* @author challenjoy01
	*/
	void gdiPlusEnd();

	


};