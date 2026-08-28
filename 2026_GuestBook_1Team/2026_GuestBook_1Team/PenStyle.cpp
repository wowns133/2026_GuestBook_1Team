#pragma once

#include "PenStyle.h"

/**
* @file
* @brief PenStyle 클래스 생성자 
* 클래스 생성 시 검은색과 10px의 두께로 생성되며, 펜의 끝부분을 둥글게 만든다.
*/
PenStyle::PenStyle() : pen(Gdiplus::Color(255, 0, 0, 0), 10.0f) 
{
	thickness = 10.0f;
	color_a = 255;
	color_r = 0;
	color_g = 0;
	color_b = 0;
	pen.SetStartCap(Gdiplus::LineCapRound);
	pen.SetEndCap(Gdiplus::LineCapRound);
}

/**
* @file
* @brief 펜 색깔을 구현하는 함수이며, color 매개변수로 들어온 값을 확인하여 
* 각 색깔에 필요한 alpha, red, green, blue값을 각각 지정한다.
*/
void PenStyle::selectPenColor(int color) 
{
	switch(color)
	{
	case BLACK:
	{
		color_a = 255;
		color_r = 0;
		color_g = 0;
		color_b = 0;
	}
		break;

	case RED:
	{
		color_a = 255;
		color_r = 255;
		color_g = 0;
		color_b = 0;
	}
		break;

	case GREEN:
	{
		color_a = 255;
		color_r = 0;
		color_g = 255;
		color_b = 0;
	}
		break;

	case BLUE:
	{
		color_a = 255;
		color_r = 0;
		color_g = 0;
		color_b = 255;
	}
		break;

	case YELLOW:
	{
		color_a = 255;
		color_r = 255;
		color_g = 255;
		color_b = 0;
	}
		break;
	}
	
	/// 색 변수 조정 후 pen 함수 호출하여 색 변경
	solidPen();
}

/**
* @file
* @brief 펜 두께를 올리는 함수 
* 마우스 휠을 올리는 이벤트가 발생 시 올라간다.
*/
void PenStyle::settingPenThicknessUp() {

	// 펜 두께 내리기
	thickness++;


	/// 펜 두께가 50px 보다 올라가지 않도록 조정
	if (thickness >= 50)
	{
		thickness = 50;
	}

	/// 펜 두께 조절 후 solidPen함수를 불러와서 두께 재설정
	solidPen();


}

/// 펜 두께 내리기 함수
void PenStyle::settingPenThicknessDown() {

	/// 펜 두께 내리기
	thickness--;

	/// 펜 두께가 10px 보다 내려가지 않도록 조정
	if (thickness <= 10)
	{
		thickness = 10;
	}

	/// 펜 두께 조절 후 solidPen함수를 불러와서 두께 재설정
	solidPen();
}

void PenStyle::solidPen() 
{
	/// 색 변경 
	pen.SetColor(Gdiplus::Color(color_a, color_r, color_g, color_b));

	/// 두께 변경
	pen.SetWidth(thickness);
}



Gdiplus::Pen* PenStyle::getPen() 
{
	return &pen;
}