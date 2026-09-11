#pragma once

#include <Windows.h>
#include <unknwn.h>			// framework.h 내부 #define WIN32_LEAN_AND_MEAN로 인해 gdiplus에 필요한 몇몇 헤더들이 제외되어 직접 넣음 
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

/// selectPenColor()함수에서 사용되는 상수 
#define BLACK					1		///< 검은색
#define RED						2		///< 빨간색
#define GREEN					3		///< 초록색
#define BLUE					4		///< 파랑색
#define YELLOW					5		///< 노란색

/// 펜 스타일을 정하는데 사용되는 상수
#define SOLIDPEN				101		///< 기본펜
#define SPRAYPEN				102		///< 스프레이
#define BRUSHPEN				103		///< 붓 펜
#define HIGHLIGHTERPEN			104		///< 형광펜

class PenColor 
{
public:
	int select_color;
	BYTE color_a;				///< 펜 색깔 alpha값 조정 변수
	BYTE color_r;				///< 펜 색깔 red값 조정 변수
	BYTE color_g;				///< 펜 색깔 green값 조정 변수
	BYTE color_b;				///< 펜 색깔 blue값 조정 변수

	PenColor();
};

/**
* @file
* @brief 펜 스타일을 구현하는 파일
*/
class PenStyle 
{
private:
	int select_pen;				///< 펜 스타일 선택 변수
	float radius;				///< 펜 두께 반지름 변수
	PenColor pen_color;
	Gdiplus::Pen pen;			///< gdiplus 기본펜 색상 및 두께 지정 클래스 변수
	

public:
	/// PenStyle 생성자
	PenStyle();

	/// 펜 선택 함수
	void selectPen(int pen);

	/// 펜 색상 선택 함수
	void selectPenColor(int color);

	/// 펜 두께 올리기 함수
	void settingPenThicknessUp();

	/// 펜 두께 내리기 함수
	void settingPenThicknessDown();

	/// 펜 스타일에 따라 두께와 색깔 모두 한번에 지정하는 함수
	void settingPenStyle(	int parameter_select_pen,	///< pen의 스타일 값을 전달한다. (select_pen변수의 값)
							int parameter_radius,		///< pen의 반지름을 전달한다. (radius변수의 값)
							int pen_color);				///< pen의 색상 클래스 변수를 전달한다. (pen_color 변수의 값)

	/// 기본 펜 함수
	void solidPen();

	/// 스프레이 펜 함수
	void sprayPen();

	/// 붓 펜 함수
	void brushPen();

	/// 형광펜 함수
	void highlighterPen();

	

	/// 펜 포인터 getter
	Gdiplus::Pen* getPen();

	/// 펜 스타일 getter
	int getSelectPen();

	/// 펜 반지름 getter
	float getRadius();

	/// 펜 색상 getter 
	int getPenColor();

	/*/// 펜 각 색상 별 getter
	BYTE getColorA();
	BYTE getColorR();
	BYTE getColorG();
	BYTE getColorB();*/
};