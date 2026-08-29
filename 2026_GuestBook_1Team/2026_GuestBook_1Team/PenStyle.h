#pragma once

#include <Windows.h>
#include <unknwn.h>			// framework.h 내부 #define WIN32_LEAN_AND_MEAN로 인해 gdiplus에 필요한 몇몇 헤더들이 제외되어 직접 넣음 
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")

/// selectPenColor()함수에서 사용되는 상수 
#define BLACK 1
#define RED 2
#define GREEN 3
#define BLUE 4
#define YELLOW 5
/**
* @file
* @brief 펜 스타일을 구현하는 파일
*/
class PenStyle {
private:
	float thickness;		///< 펜 두께 변수
	BYTE color_a;		///< 펜 색깔 alpha값 조정 변수
	BYTE color_r;		///< 펜 색깔 red값 조정 변수
	BYTE color_g;		///< 펜 색깔 green값 조정 변수
	BYTE color_b;		///< 펜 색깔 blue값 조정 변수

	Gdiplus::Pen pen;

public:
	/// PenStyle 생성자
	PenStyle();

	/// 펜 색상 선택 함수
	void selectPenColor(int color);

	/// 펜 두께 올리기 함수
	void settingPenThicknessUp();

	/// 펜 두께 내리기 함수
	void settingPenThicknessDown();

	/// 기본 펜 함수
	void solidPen();

	/// 펜 포인터 getter
	Gdiplus::Pen* getPen();

	//void 
};