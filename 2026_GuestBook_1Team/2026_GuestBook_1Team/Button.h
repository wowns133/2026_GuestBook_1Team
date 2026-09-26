#pragma once
#include <Windows.h>
#include <winuser.h>
#include <unknwn.h>			// framework.h 내부 #define WIN32_LEAN_AND_MEAN로 인해 gdiplus에 필요한 몇몇 헤더들이 제외되어 직접 넣음 
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")
#include <vector>


#define BUTTON_STYLE_ELLIPSE 1
#define BUTTON_STYLE_RECT 2
#define BUTTON_STYLE_ELLIPSE_SHADOW 3
#define BUTTON_STYLE_RECT_SHADOW 4

#define CUSTOM_BUTTON_RED 101
#define CUSTOM_BUTTON_BLACK 102

/**
* @file
* @brief 커스텀 버튼 관련 기능들을 구현한 헤더 파일.
* @details 커스텀 버튼 관련 기능들을 구현한 헤더 파일.
* @author challenjoy01
*/

/**
* @brief 커스텀 버튼의 정보를 저장하는 구조체.
* @details 커스텀 버튼의 정보를 저장하는 구조체.
* @author challenjoy01
*/
struct ButtonData
{
	int button_id;
	LPCWSTR button_name;
	int x;
	int y;
	int width;
	int height;
	Gdiplus::Color pen_color;
	int pen_width;
	Gdiplus::Color brush_color;
	int button_style;
};

/**
* @brief 커스텀 버튼 기능을 담당하는 클래스.
* @details 커스텀 버튼 기능을 담당하는 클래스.
* @author challenjoy01
*/
class Button
{
private:
	std::vector<ButtonData> button; ///< 모든 버튼의 데이터를 저장할 벡터
	int mouse_x = 0; ///< 마우스의 x좌표
	int mouse_y = 0; ///< 마우스의 y좌표


	void setButton(int button_id, LPCWSTR button_name, int x, int y, int width, int height, Gdiplus::Color pen_color, int pen_width, Gdiplus::Color brush_color, int button_style);
	ButtonData getButtonById(int button_id);
	void drawButton(HDC hdc, int button_index);
	void drawEllipseShadowButton(Gdiplus::Graphics& hdc_graphics, int button_index);

protected:
public:
	void setAllButtons();
	void drawAllButtons(HDC hdc);
	void resizeAllButtons();

	int trackClick(LPARAM lParam);
};

