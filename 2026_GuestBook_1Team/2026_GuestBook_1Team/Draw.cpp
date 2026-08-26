#include "Draw.h"

/**
* @file
* @brief 그리기 기능을 구현하는 파일.
* @details 그리기 기능을 구현한 파일. Draw.h의 함수들이 구현되어 있다.
* @author challenjoy01
* @warning startDrawingLine 내부에는 객체 생성 등 자원 해제가 필요한 부분이 존재합니다. startDrawingLine 실행 후에는 
* 반드시 EndDrawingLine이 실행될 수 있도록 주의해야 합니다.
*/


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

void Draw::gdiPlusStart()
{
	Gdiplus::GdiplusStartupInput gdi_plus_start_up_input;
	Gdiplus::GdiplusStartup(&drawing_token, &gdi_plus_start_up_input, NULL);
}

void Draw::gdiPlusEnd()
{
	Gdiplus::GdiplusShutdown(drawing_token);
}


void Draw::startDrawingLine(HWND hWnd, LPARAM lParam)
{
	//그리기 환경 제작
	draw_hdc = GetDC(hWnd); //hdc 발행. 사용 후 회수해야 함
	draw_hdc_graphics = new Gdiplus::Graphics(draw_hdc); //화면 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함
	GetClientRect(hWnd, &client_rect); //client_rect에 현재 작업영역 크기 구하기
	draw_bmp = new Gdiplus::Bitmap(client_rect.right, client_rect.bottom, PixelFormat32bppARGB); //더블 버퍼링 구현을 위한 Bitmap 객체 생성. 사용 후 삭제해야 함
	draw_bmp_graphics = new Gdiplus::Graphics(draw_bmp); //비트맵 출력을 담당할 Graphics 객체 생성. 사용 후 삭제해야 함

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
	if(is_drawing)
	{
	/*------------------임시 펜 조작-----------------------*/
	Gdiplus::Pen pen(Gdiplus::Color(255, 0, 0, 0), 1.0f); 
	pen.SetStartCap(Gdiplus::LineCapRound);
	pen.SetEndCap(Gdiplus::LineCapRound);

	//이동 후 현재 위치를 저장
	current_x = LOWORD(lParam);
	current_y = HIWORD(lParam);

	//현재 위치 좌표를 벡터에 저장
	drawn_line.push_back({ current_x, current_y });

	/*--------------비트맵에 그리기--------------*/
	draw_bmp_graphics->SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);				// 안티엘리어싱 적용. 선에서 계단 현상이 줄어든다.
	draw_bmp_graphics->DrawLine(&pen, previous_x, previous_y, current_x, current_y);	// 선 긋기 함수 실행

	previous_x = current_x; //현재 위치를 이동 전 좌표 변수에 저장
	previous_y = current_y; //현재 위치를 이동 전 좌표 변수에 저장

	draw_hdc_graphics->DrawImage(draw_bmp, 0, 0); // 화면에 미리 그려놓은 비트맵을 출력
	}
}

void Draw::endDrawingLine(HWND hWnd, LPARAM lParam)
{
	drawn_lines.push_back(drawn_line); //다 그린 선을 선을 모아놓은 벡터에 저장
	drawn_line.clear(); //선 벡터 비우기
	is_drawing = false; //그리기 끝났으므로 플래그를 false로 변경


	/*--------------------포인터 처리-------------------*/
	delete draw_bmp_graphics;
	delete draw_bmp;

	delete draw_hdc_graphics; //화면 출력을 담당할 Graphics 객체 파괴
	ReleaseDC(hWnd, draw_hdc); //hdc 파괴

	draw_bmp_graphics = nullptr;
	draw_bmp = nullptr;
	draw_hdc_graphics = nullptr;
}



// 더블 버퍼링을 적용하지 않은 단순 선 그리기 함수들
// void startDrawingLine(HWND hWnd, LPARAM lParam);
// void drawingLine(HWND hWnd, LPARAM lParam);
// void endDrawingLine(HWND hWnd, LPARAM lParam);
// 위 세 함수의 이전 버전입니다.
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
*/

