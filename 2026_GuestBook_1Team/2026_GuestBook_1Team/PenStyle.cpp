#pragma once

#include "PenStyle.h"



PenColor::PenColor()
{
	color_a = 255;
	color_r = 0;
	color_g = 0;
	color_b = 0;
}

/**
* @file
* @brief PenStyle 클래스 생성자 
* 클래스 생성 시 기본펜 스타일로 적용되며
* 색깔은 검은색 두께는 10px의 두께로 생성된다.
* 펜의 끝부분은 둥글게 만든다.
*/
PenStyle::PenStyle()  
	:
	select_pen(SOLIDPEN),
	radius(5.0f),
	pen(Gdiplus::Color(255, 0, 0, 0), (radius * 2))
{
	pen.SetStartCap(Gdiplus::LineCapRoundAnchor);
	pen.SetEndCap(Gdiplus::LineCapRoundAnchor);
}

/**
* @file
* @brief 펜 스타일을 선택하는 함수이며
* select_pen 변수로 펜 스타일의 상수를 받아서 결정한다.
*/
void PenStyle::selectPen(int pen) 
{
	select_pen = pen;
	switch (select_pen) {

	case SOLIDPEN:
	{
		solidPen();
	}
	break;

	case SPRAYPEN:
	{
		sprayPen();
	}
	break;

	case BRUSHPEN:
	{
		brushPen();
	}
	break;

	case HIGHLIGHTERPEN:
	{
		highlighterPen();
	}
	break;
	}
}

/**
* @file
* @brief 펜 색깔을 구현하는 함수이며, color 매개변수로 들어온 값을 확인하여 
* 각 색깔에 필요한 alpha, red, green, blue값을 각각 지정한다.
*/
void PenStyle::selectPenColor(int select_color) 
{
	pen_color.select_color = select_color;
	switch(select_color)
	{
	case BLACK:
	{
	
		pen_color.color_a = 255;
		pen_color.color_r = 0;
		pen_color.color_g = 0;
		pen_color.color_b = 0;
	}
		break;

	case RED:
	{
		pen_color.color_a = 255;
		pen_color.color_r = 255;
		pen_color.color_g = 0;
		pen_color.color_b = 0;
	}
		break;

	case GREEN:
	{
		pen_color.color_a = 255;
		pen_color.color_r = 0;
		pen_color.color_g = 255;
		pen_color.color_b = 0;
	}
		break;

	case BLUE:
	{
		pen_color.color_a = 255;
		pen_color.color_r = 0;
		pen_color.color_g = 0;
		pen_color.color_b = 255;
	}
		break;

	case YELLOW:
	{
		pen_color.color_a = 255;
		pen_color.color_r = 255;
		pen_color.color_g = 255;
		pen_color.color_b = 0;
	}
		break;
	}
	
	/// 색 변수 조정 후 펜 스타일에 맞는 함수를 호출하여 색 변경
	selectPen(select_pen);
}

/**
* @file
* @brief 펜 두께를 올리는 함수 
* 마우스 휠을 올리는 이벤트가 발생 시 올라간다.
*/
void PenStyle::settingPenThicknessUp() 
{

	// 펜 두께 내리기
	radius++;


	/// 펜 두께가 50px 보다 올라가지 않도록 조정
	if (radius >= 25)
	{
		radius = 25;
	}

	/// 펜 두께 조절 후 펜 스타일에 맞는 함수를 호출하여 두께 재설정
	selectPen(select_pen);
}

/**
* @file
* @brief 펜 두께를 내리는 함수
* 마우스 휠을 내리는 이벤트가 발생 시 내려간다.
*/
void PenStyle::settingPenThicknessDown() 
{

	/// 펜 두께 내리기
	radius--;

	/// 펜 두께가 10px 보다 내려가지 않도록 조정
	if (radius <= 5)
	{
		radius = 5;
	}

	/// 펜 두께 조절 후 펜 스타일에 맞는 함수를 호출하여 두께 재설정
	selectPen(select_pen);
}


/**
* @file
* @brief 기본 펜 스타일 함수
* 기본펜 선택 시 기본펜으로 적용된다.
*/
void PenStyle::solidPen() 
{
	/// 펜 끝 둥글게 만들기
	pen.SetStartCap(Gdiplus::LineCapRoundAnchor);
	pen.SetEndCap(Gdiplus::LineCapRoundAnchor);

	/// 색 변경 
	pen.SetColor(Gdiplus::Color(pen_color.color_a, pen_color.color_r, pen_color.color_g, pen_color.color_b));

	/// 두께 변경
	pen.SetWidth((radius * 2));
}

/**
* @file
* @brief 스프레이 펜 스타일 함수
* 스프레이펜 선택 시 스프레이로 적용된다.
*/
void PenStyle::sprayPen() 
{
	/// 미적용
}

/**
* @file
* @brief 붓 펜 스타일 함수
* 붓펜 선택 시 스프레이로 적용된다.
*/
void PenStyle::brushPen() 
{
	/// 미적용
}

/**
* @file
* @brief 형광펜 스타일 하뭇
* 형광펜 선택 시 스프레이로 적용된다.
*/
void PenStyle::highlighterPen() 
{

	/// 형광펜 처럼 각지도록 나오게 변경
	pen.SetStartCap(Gdiplus::LineCapSquare);		///< LineCapSquare : 끝을 더 연장시켜서 끝이 직각으로 나오도록함.
	pen.SetEndCap(Gdiplus::LineCapSquare);
	pen.SetLineJoin(Gdiplus::LineJoinBevel);

	/// 색 변경 
	pen.SetColor(Gdiplus::Color((pen_color.color_a/8), pen_color.color_r, pen_color.color_g, pen_color.color_b));
	
	/// 두께 변경
	pen.SetWidth((radius * 2));
}


/**
* @file
* @brief Gdiplus pen 포인터 getter
* 현재 pen 클래스의 정보를 반환한다.
* @return pen 클래스 변수의 주소를 반환한다.
*/
Gdiplus::Pen* PenStyle::getPen() 
{
	return &pen;
}

/**
* @file
* @brief 펜 스타일 선택 변수 getter
* 현재 선택 된 펜 스타일을 반환한다.
* @return select_pen 변수를 반환한다.
*/
int PenStyle::getSelectPen() 
{
	return select_pen;
}

/**
* @file
* @brief 펜 반지름 getter
* 현재 펜 반지름을 반환한다.
* @return radius 변수를 반환한다.
*/
float PenStyle::getRadius() 
{
	return radius;
}

/**
* @file
* @brief 펜 색상 getter
* 현재 선택된 펜 색상을 반환한다.
* @return PenColor의 멤버변수 select_color 변수를 반환한다.
*/
int PenStyle::getPenColor() 
{
	return pen_color.select_color;
}


/**
* @file
* @brief 펜 스타일에 따라 두께와 색깔 모두 한번에 지정하는 함수
* selectPenColor함수 실행 시 펜 색깔과 두께 모두 변경된다.
*/
void PenStyle::settingPenStyle
(	int parameter_select_pen,			
	float parameter_radius,				
	int pen_color					
) 
{
	select_pen = parameter_select_pen;		///< select_pen 멤버변수를 매개변수로 받아온 값으로 변경한다.
	radius = parameter_radius;				///< radius 멤버변수를 매개변수로 받아온 값으로 변경한다.

	selectPenColor(pen_color);
}

///**
//* @file
//* @brief 펜 색상 getter
//* 현재 펜 색상의 alpha 값을 반환한다.
//* @return color_a 변수를 반환한다.
//*/
//BYTE PenStyle::getColorA() {
//	return pen_color.color_a;
//}
//
///**
//* @file
//* @brief 펜 색상 getter
//* 현재 펜 색상의 red 값을 반환한다.
//* @return color_r 변수를 반환한다.
//*/
//BYTE PenStyle::getColorR() {
//	return pen_color.color_r;
//}
//
///**
//* @file
//* @brief 펜 색상 getter
//* 현재 펜 색상의 green 값을 반환한다.
//* @return color_g 변수를 반환한다.
//*/
//BYTE PenStyle::getColorG() {
//	return pen_color.color_g;
//}
//
///**
//* @file
//* @brief 펜 색상 getter
//* 현재 펜 색상의 blue 값을 반환한다.
//* @return color_b 변수를 반환한다.
//*/
//BYTE PenStyle::getColorB() {
//	return pen_color.color_b;
//}