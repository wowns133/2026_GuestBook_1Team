#include "Draw.h"


/**
* @file
* @brief 그리기 기능을 구현하는 파일.
* @details 그리기 기능을 구현한 파일. Draw.h의 함수들이 구현되어 있다.
* @author challenjoy01
* @warning startDrawingLine 내부에는 객체 생성 등 자원 해제가 필요한 부분이 존재합니다. startDrawingLine 실행 후에는
* 반드시 EndDrawingLine이 실행될 수 있도록 주의해야 합니다.
*/
#define COLOR1 255
#define COLOR2 255
#define COLOR3 255
#define COLOR4 0

void Draw::ac_lines()
{
	drawn_lines.clear(); //drawn_lines의 값을 비움(clear)
	drawn_line.clear();  //darawn_line의 값을 비움(clear)
	if (drawn_bmp_graphics != nullptr) {
		drawn_bmp_graphics->Clear(Gdiplus::Color(0, 0, 0, 0));
	}
}

const std::vector<std::vector<DrawPointData>>& Draw::getDrawnLines()
{
	return drawn_lines; //클래스 내부에 저장 된 데이터 원본의 주소 값(참조)을 반환
	//복사본을 만들지 않고 &(참조)를 넘겨 메모리 효율성을 올리기 위함
}

void Draw::sc_line(int index)
{
	if (index < drawn_lines.size()) //매개변수(index)가 배열의 총 크기보다 작은지 확인
		//sizeof()사용하면 배열의 갯수가 아닌 총 크기        .size()사용 해야 총 개수가 나옴
	{
		drawn_line.erase(drawn_line.begin() + index); //벡터의 시작점(begin) ~ 매개변수(index)만큼 이동 시키는
		//반복자(위치를 가르켜주는 객체or도구)를 구함
		//.erase를 사용해 해당 위치의 데이터를 삭제 시킴
	}
}

void Draw::redrawAllLines(HWND hWnd)
{
	HDC hdc = GetDC(hWnd); //hdc 생성
	Gdiplus::Graphics hdc_graphics(hdc); //hdc용 grapgics 생성
	
	//모니터 청소
	hdc_graphics.Clear(Gdiplus::Color(255, 255, 255, 255));
	//세번째 버퍼 청소
	drawn_bmp_graphics->Clear(Gdiplus::Color(0, 0, 0, 0));

	
	//세 번째 버퍼에 불러온 모든 데이터를 그리기
	for (size_t line_num = 0; line_num < drawn_lines.size(); line_num++)
	{
		//pen_style->settingPenStyle(스타일, 반지름, 색);
		if (drawn_lines[line_num].size() == 1) //점이 하나 뿐일 경우
		{
			drawn_bmp_graphics->DrawLine(pen_pointer,
				(float)drawn_lines[line_num][0].point.x,
				(float)drawn_lines[line_num][0].point.y,
				(float)drawn_lines[line_num][0].point.x + 0.001f,
				(float)drawn_lines[line_num][0].point.y
			);
			continue;
		}
		for (size_t point_num = 1; point_num < drawn_lines[line_num].size(); point_num++)
		{
			drawn_bmp_graphics->DrawLine(pen_pointer,
				(INT)drawn_lines[line_num][point_num - 1].point.x,
				(INT)drawn_lines[line_num][point_num - 1].point.y,
				(INT)drawn_lines[line_num][point_num].point.x,
				(INT)drawn_lines[line_num][point_num].point.y
			);
		}
	}

	if (drawn_line.size() == 1) //점이 하나 뿐일 경우
	{
		drawn_bmp_graphics->DrawLine(pen_pointer,
			(float)drawn_line[0].point.x,
			(float)drawn_line[0].point.y,
			(float)drawn_line[0].point.x + 0.001f,
			(float)drawn_line[0].point.y
		);
	}
	for (size_t point_num = 1; point_num < drawn_line.size(); point_num++)
	{
		drawn_bmp_graphics->DrawLine(pen_pointer,
			(INT)drawn_line[point_num - 1].point.x,
			(INT)drawn_line[point_num - 1].point.y,
			(INT)drawn_line[point_num].point.x,
			(INT)drawn_line[point_num].point.y
		);
	}
	hdc_graphics.DrawImage(drawn_bmp, 0, 0); // 화면에 미리 그려놓은 비트맵을 출력
	ReleaseDC(hWnd, hdc); //hdc 삭제
}



void Draw::startDrawingLine(HWND hWnd, LPARAM lParam, int pen_mode)
{
	switch (pen_mode)
	{
		case PEN_MODE_ER:
		{

		}
		break;
		default:
		{
			//그리기 환경 제작
			draw_hdc = GetDC(hWnd); //hdc 발행. 사용 후 회수해야 함
			draw_hdc_graphics = new Gdiplus::Graphics(draw_hdc); //화면 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
			draw_hdc_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

			GetClientRect(hWnd, &client_rect); //client_rect에 현재 작업영역 크기 구하기
			draw_bmp = new Gdiplus::Bitmap(client_rect.right, client_rect.bottom, PixelFormat32bppARGB); //더블 버퍼링 구현을 위한 Bitmap 객체 생성. 사용 후 삭제해야 함
			draw_bmp_graphics = new Gdiplus::Graphics(draw_bmp); //비트맵 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
			draw_bmp_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);				// 안티엘리어싱 적용. 선에서 계단 현상이 줄어든다.


			//선을 그리기 시작하는 좌표를 현재 좌표로 초기화
			previous_x = LOWORD(lParam); //lParam의 하위 16비트를 가져와 x좌표로 저장 
			previous_y = HIWORD(lParam); //lParam의 상위 16비트를 가져와 y좌표로 저장


			start_time = GetTickCount64(); //윈도우 창이 켜진 시점부터 흐르는 시간

			//현재 좌표를 선의 시작점으로 저장
			drawn_line.push_back({ { previous_x, previous_y }, 0, 5 });

			//그리는 중이라고 플래그 표시
			is_drawing = true;

			//창 벗어남을 추적하기 위해 마우스 추적 시작
			setTrackMouseEvent(hWnd);


			// 시작점 그리기
			draw_hdc_graphics->DrawLine(pen_pointer, (float)previous_x, (float)previous_y, (float)previous_x + 0.001f, (float)previous_y);

			// 선의 점 정보를 담는 클래스 생성
			//pen_pointer->SetLineJoin(Gdiplus::LineJoinRound);
			line_path = new Gdiplus::GraphicsPath();
		}
		break;
	}//switch(pen_mode)
	
}


void Draw::drawingLine(HWND hWnd, LPARAM lParam, int pen_mode)
{
	if (is_drawing)
	{
		switch (pen_mode)
		{
			case PEN_MODE_ER:
			{
				/// 지우개용 공간
			}
			break;
			case PEN_MODE_NORMAL_PEN:
			{
				drawingLineRGB(hWnd, lParam);
			}
			break;
			case PEN_MODE_ARGB_PEN:
			{
				drawingLineARGB(hWnd, lParam);
			}
			break;
			default:
			{
				break;
			}
		}
	}
}


void Draw::drawingLineRGB(HWND hWnd, LPARAM lParam)
{
	if (is_drawing)
	{
		//이동 후 현재 위치를 저장
		current_x = LOWORD(lParam);
		current_y = HIWORD(lParam);

		ULONGLONG elapsed_time = GetTickCount64() - start_time; // 경과 시간 계산

		//현재 위치 좌표를 벡터에 저장
		drawn_line.push_back({ { current_x, current_y }, elapsed_time, is_pen });

		//--------------비트맵에 그리기--------------//
		draw_bmp_graphics->Clear(Gdiplus::Color(0, 0, 0, 0));
		draw_bmp_graphics->DrawLine(pen_pointer, previous_x, previous_y, current_x, current_y);	// 선 긋기 함수 실행

		//-----------------국소 범위 화면 갱신-----------------//
		drawSectionImage(draw_hdc_graphics, draw_bmp, previous_x, previous_y, current_x, current_y);

		previous_x = current_x; //현재 위치를 이동 전 좌표 변수에 저장
		previous_y = current_y; //현재 위치를 이동 전 좌표 변수에 저장
	}
}


void Draw::drawingLineARGB(HWND hWnd, LPARAM lParam)
{
	if (is_drawing)
	{
		//이동 후 현재 위치를 저장
		current_x = LOWORD(lParam);
		current_y = HIWORD(lParam);

		ULONGLONG elapsed_time = GetTickCount64() - start_time; // 경과 시간 계산


		drawn_line.push_back({ { current_x, current_y }, elapsed_time, is_pen }); // 이번 점에 대한 데이터를 데이터 저장용 구조체에 저장
		line_path->AddLine(previous_x, previous_y, current_x, current_y); // 이번에 그릴 부분을 그리기용 path에 저장

		//--------------비트맵에 그리기--------------//
		draw_bmp_graphics->Clear(Gdiplus::Color(255, 255, 255, 255)); //그리기용 버퍼를 초기화
		drawSectionImage(draw_bmp_graphics, drawn_bmp, previous_x, previous_y, current_x, current_y); // 이번 선 전까지 그어진 모든 선을 그리기용 버퍼에 그리기
		draw_bmp_graphics->DrawPath(pen_pointer, line_path);	// 이번 선을 그리기용 버퍼에 그리기

		//-----------------국소 범위 화면 출력-----------------//
		drawSectionImage(draw_hdc_graphics, draw_bmp, previous_x, previous_y, current_x, current_y); //그리기용 버퍼에사 국소 범위만을 화면에 출력

		previous_x = current_x; //현재 위치를 이동 전 좌표 변수에 저장
		previous_y = current_y; //현재 위치를 이동 전 좌표 변수에 저장
	}
}







void Draw::endDrawingLine(HWND hWnd, LPARAM lParam, int pen_mode)
{
	switch (pen_mode)
	{
		case PEN_MODE_ER:
		{

		}
		break;
		case PEN_MODE_NORMAL_PEN:
		{
			for (size_t point_num = 1; point_num < drawn_line.size(); point_num++)
			{
				drawn_bmp_graphics->DrawLine(pen_pointer,
					(INT)drawn_line[point_num - 1].point.x,
					(INT)drawn_line[point_num - 1].point.y,
					(INT)drawn_line[point_num].point.x,
					(INT)drawn_line[point_num].point.y
				);
			}
		}
		break;
		case PEN_MODE_ARGB_PEN:
		{
			drawn_bmp_graphics->DrawPath(pen_pointer, line_path);	// 전체 그림 버퍼에 선 긋기 함수 실행
		}
		break;
		default:
		{
			break;
		}
	}


	drawn_lines.push_back(drawn_line); //다 그린 선을 선을 모아놓은 벡터에 저장
	drawn_line.clear(); //선 벡터 비우기
	is_drawing = false; //그리기 끝났으므로 플래그를 false로 변경


	//-------------------마우스 추적 완료 처리------------------//
	is_tracking_mouse = false;
	//delete track_mouse;


	/*--------------------포인터 처리-------------------*/
	delete draw_bmp_graphics;
	draw_bmp_graphics = nullptr;

	delete draw_bmp;
	draw_bmp = nullptr;

	delete line_path;
	line_path = nullptr;

	delete draw_hdc_graphics; //화면 출력을 담당할 Graphics 객체 파괴
	draw_hdc_graphics = nullptr;
	ReleaseDC(hWnd, draw_hdc); //hdc 파괴

	
	
	

	
}


void Draw::drawWindowLines(HWND hWnd, HDC hdc)
{
	Gdiplus::Graphics hdc_graphics(hdc); //hdc용 grapgics 생성
	hdc_graphics.DrawImage(drawn_bmp, 0, 0); // 화면에 전체 선 비트맵을 출력
}




















void Draw::gdiPlusStart()
{
	/// Gdi+ 시작
	Gdiplus::GdiplusStartupInput gdi_plus_start_up_input;
	Gdiplus::GdiplusStartup(&drawing_token, &gdi_plus_start_up_input, NULL);

	/// 
	drawn_bmp = new Gdiplus::Bitmap(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), PixelFormat32bppARGB); //삼중 버퍼링 구현을 위한 Bitmap 객체 생성. 사용 후 삭제해야 함
	drawn_bmp_graphics = new Gdiplus::Graphics(drawn_bmp); //비트맵 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
	drawn_bmp_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);	// 안티엘리어싱 적용. 선에서 계단 현상이 줄어든다.

	/// pen_style 테스트
	pen_style = new PenStyle();
	pen_pointer = pen_style->getPen();
}

void Draw::gdiPlusEnd()
{
	/// 삼중 버퍼 해제
	delete drawn_bmp_graphics;
	delete drawn_bmp;
	drawn_bmp_graphics = nullptr;
	drawn_bmp = nullptr;

	/// pen_style 테스트
	delete pen_style;
	pen_style = nullptr;

	Gdiplus::GdiplusShutdown(drawing_token);
}

void Draw::drawSectionImage(Gdiplus::Graphics* output_graphics, Gdiplus::Bitmap* input_bitmap, int previous_x, int previous_y, int current_x, int current_y)
{
	pen_width = (int)pen_style->getRadius() + 5;

	draw_image_area_start_x = min(previous_x, current_x) - pen_width;
	draw_image_area_start_y = min(previous_y, current_y) - pen_width;
	draw_image_area_width = max(previous_x, current_x) - draw_image_area_start_x + pen_width;
	draw_image_area_height = max(previous_y, current_y) - draw_image_area_start_y + pen_width;

	// 시작점이 0보다 적게 될 경우 0으로 설정
	if (draw_image_area_start_x < 0)
	{
		draw_image_area_start_x = 0;
	}
	if (draw_image_area_start_y < 0)
	{
		draw_image_area_start_y = 0;
	}

	// draw_bmp의 새로 그을 선의 좌상단부터 width x height 크기만큼만 화면에 그리기
	output_graphics->DrawImage(
		input_bitmap,	// 가져올 화면(비트맵)
		draw_image_area_start_x,		// 화면에 그려질 시작점(좌상단) x좌표
		draw_image_area_start_y,      // 화면에 그려질 시작점(좌상단) y좌표
		draw_image_area_start_x,		// 비트맵에서 가져올 시작점(좌상단) x좌표
		draw_image_area_start_y,      // 비트맵에서 가져올 시작점(좌상단) y좌표
		draw_image_area_width,		// 가져올 너비
		draw_image_area_height,     // 가져올 높이
		Gdiplus::UnitPixel // 단위(픽셀)
	);
}

void Draw::setTrackMouseEvent(HWND hWnd)
{
	if (!is_tracking_mouse)
	{
		track_mouse = new TRACKMOUSEEVENT; //구조체 초기화
		track_mouse->cbSize = sizeof(TRACKMOUSEEVENT);
		track_mouse->dwFlags = TME_LEAVE; //창 벗어남을 감시함
		track_mouse->hwndTrack = hWnd;

		if (TrackMouseEvent(track_mouse)) //TrackMouseEvent 함수는 마우스 이벤트 추적을 시작하는 함수
		{
			is_tracking_mouse = true; //추척을 시작했을 경우 플래그 true
		}
		delete track_mouse;
	}
}


void Draw::setIsPen(bool is_pen)
{
	this->is_pen = is_pen;
	return;
}

/// 펜 스타일에 따라 그리기 스타일 정하는 함수 (PenStyle test 진행중..)
void Draw::selectDrawStyle()
{
	if (/*is_Pen ==*/ true) {
		/// select_pen 변수 받아오기
		switch (pen_style->getSelectPen()) {
		case SOLIDPEN:
		{
			select_drawStyle = PEN_MODE_NORMAL_PEN;
		}
		break;
		case SPRAYPEN:
		{

		}
		break;
		case BRUSHPEN:
		{

		}
		break;
		case HIGHLIGHTERPEN:
		{
			select_drawStyle = PEN_MODE_ARGB_PEN;
		}
		break;
		}
	}
	else {
		select_drawStyle = PEN_MODE_ER;
	}


	/// 변수 값 확인용
	/*wchar_t buffer[100];
	swprintf_s(buffer, L"getSelectPen = %d\nselect_drawStyle = %d\n", pen_style->getSelectPen(), select_drawStyle);
	OutputDebugString(buffer);*/
}

/// select_drawStyle변수와 pen_width 변수 초기값 설정 (PenStyle test 진행중..)
//Draw::Draw()
//{
//	select_drawStyle = 1;
//	//pen_width = 15;
//}

/// pen_width 변수를 펜 클래스 변수의 radius값에 따라 변경되도록 하는 함수(PenStyle test 진행중..)
//void Draw::penWidthChange()
//{
//	int radius = (int)pen_style->getRadius();
//	pen_width = radius + 5;
//	
//
//	/// 변수 값 확인용
//	/*wchar_t buffer[100];
//	swprintf_s(buffer, L"getRadius = %d\npen_width = %d\n", radius, pen_width);
//	OutputDebugString(buffer);*/
//}





// 더블 버퍼링을 적용하지 않은 단순 선 그리기 함수들
// void startDrawingLine(HWND hWnd, LPARAM lParam);
// void drawingLine(HWND hWnd, LPARAM lParam);
// void endDrawingLine(HWND hWnd, LPARAM lParam);
// void Draw::drawWindowLines(HWND hWnd, HDC hdc)
// 위 네 함수의 이전 버전입니다.
/*
void Draw::startDrawingLine(HWND hWnd, LPARAM lParam)
{
	//선을 그리기 시작하는 좌표를 현재 좌표로 초기화
	previous_x = LOWORD(lParam); //lParam의 하위 16비트를 가져와 x좌표로 저장
	previous_y = HIWORD(lParam); //lParam의 상위 16비트를 가져와 y좌표로 저장

	//현재 좌표를 선의 시작점으로 저장
	drawn_line.push_back({ previous_x, previous_y });

	//그리는 중이라고 플래그 표시
	is_drawing = true;
}

void Draw::drawingLine(HWND hWnd, LPARAM lParam)
{
	if (is_drawing)
	{
		HDC hdc = GetDC(hWnd); //HDC 발행

		//이동 후 현재 위치를 저장
		current_x = LOWORD(lParam);
		current_y = HIWORD(lParam);

		//현재 위치 좌표를 벡터에 저장
		drawn_line.push_back({ current_x, current_y });

		MoveToEx(hdc, previous_x, previous_y, NULL); //이전 좌표로 이동
		LineTo(hdc, current_x, current_y); //현재 좌표로 선 긋기

		previous_x = current_x; //현재 위치를 이동 전 좌표 변수에 저장
		previous_y = current_y; //현재 위치를 이동 전 좌표 변수에 저장

		ReleaseDC(hWnd, hdc); //HDC 반납
	}
}

void Draw::endDrawingLine(HWND hWnd, LPARAM lParam)
{
	drawn_lines.push_back(drawn_line); //다 그린 선을 선을 모아놓은 벡터에 저장
	drawn_line.clear(); //선 벡터 비우기
	is_drawing = false; //그리기 끝났으므로 플래그를 false로 변경
}
void Draw::drawWindowLines(HWND hWnd, HDC hdc)
{
	for (size_t line_num = 0; line_num < drawn_lines.size(); line_num++)
	{
		for (size_t point_num = 1; point_num < drawn_lines[line_num].size(); point_num++)
		{
			MoveToEx(hdc, drawn_lines[line_num][point_num - 1].x, drawn_lines[line_num][point_num - 1].y, NULL);
			LineTo(hdc, drawn_lines[line_num][point_num].x, drawn_lines[line_num][point_num].y);
		}
	}

	for (size_t point_num = 1; point_num < drawn_line.size(); point_num++)
	{
		MoveToEx(hdc, drawn_line[point_num - 1].x, drawn_line[point_num - 1].y, NULL);
		LineTo(hdc, drawn_line[point_num].x, drawn_line[point_num].y);
	}
}
*/
















// 더블 버퍼링을 적용하지 않은 단순 선 그리기 함수들
// void startDrawingLine(HWND hWnd, LPARAM lParam);
// void drawingLine(HWND hWnd, LPARAM lParam);
// void endDrawingLine(HWND hWnd, LPARAM lParam);
// void Draw::drawWindowLines(HWND hWnd, HDC hdc)
// 위 네 함수의 이전 버전입니다.
/*
void Draw::startDrawingLine(HWND hWnd, LPARAM lParam)
{
	//선을 그리기 시작하는 좌표를 현재 좌표로 초기화
	previous_x = LOWORD(lParam); //lParam의 하위 16비트를 가져와 x좌표로 저장
	previous_y = HIWORD(lParam); //lParam의 상위 16비트를 가져와 y좌표로 저장

	//현재 좌표를 선의 시작점으로 저장
	drawn_line.push_back({ previous_x, previous_y });

	//그리는 중이라고 플래그 표시
	is_drawing = true;
}

void Draw::drawingLine(HWND hWnd, LPARAM lParam)
{
	if (is_drawing)
	{
		HDC hdc = GetDC(hWnd); //HDC 발행

		//이동 후 현재 위치를 저장
		current_x = LOWORD(lParam);
		current_y = HIWORD(lParam);

		//현재 위치 좌표를 벡터에 저장
		drawn_line.push_back({ current_x, current_y });

		MoveToEx(hdc, previous_x, previous_y, NULL); //이전 좌표로 이동
		LineTo(hdc, current_x, current_y); //현재 좌표로 선 긋기

		previous_x = current_x; //현재 위치를 이동 전 좌표 변수에 저장
		previous_y = current_y; //현재 위치를 이동 전 좌표 변수에 저장

		ReleaseDC(hWnd, hdc); //HDC 반납
	}
}

void Draw::endDrawingLine(HWND hWnd, LPARAM lParam)
{
	drawn_lines.push_back(drawn_line); //다 그린 선을 선을 모아놓은 벡터에 저장
	drawn_line.clear(); //선 벡터 비우기
	is_drawing = false; //그리기 끝났으므로 플래그를 false로 변경
}
void Draw::drawWindowLines(HWND hWnd, HDC hdc)
{
	for (size_t line_num = 0; line_num < drawn_lines.size(); line_num++)
	{
		for (size_t point_num = 1; point_num < drawn_lines[line_num].size(); point_num++)
		{
			MoveToEx(hdc, drawn_lines[line_num][point_num - 1].x, drawn_lines[line_num][point_num - 1].y, NULL);
			LineTo(hdc, drawn_lines[line_num][point_num].x, drawn_lines[line_num][point_num].y);
		}
	}

	for (size_t point_num = 1; point_num < drawn_line.size(); point_num++)
	{
		MoveToEx(hdc, drawn_line[point_num - 1].x, drawn_line[point_num - 1].y, NULL);
		LineTo(hdc, drawn_line[point_num].x, drawn_line[point_num].y);
	}
}
*/

