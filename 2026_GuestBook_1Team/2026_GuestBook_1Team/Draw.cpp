#pragma once
#include "Draw.h"
/**
* @file
* @brief 그리기 기능을 구현하는 파일.
* @details 그리기 기능을 구현한 파일. Draw.h의 함수들이 구현되어 있다.
*/

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
