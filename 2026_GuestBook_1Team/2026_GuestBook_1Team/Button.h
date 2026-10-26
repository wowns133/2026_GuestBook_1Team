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
#define BUTTON_STYLE_IMAGE 5

// 1. 펜 종류 (4종)
#define CUSTOM_BUTTON_PEN_SOLID     101  ///< 기본펜
#define CUSTOM_BUTTON_PEN_SPRAY     102  ///< 스프레이
#define CUSTOM_BUTTON_PEN_BRUSH     103  ///< 붓펜
#define CUSTOM_BUTTON_PEN_HIGHLIGHT 104  ///< 형광펜

// 2. 지우개 종류 (2종)
#define CUSTOM_BUTTON_ERASER_STROKE 105  ///< 한 획 지우기
#define CUSTOM_BUTTON_ERASER_ALL    106  ///< 전체 지우기

// 3. 리플레이 (2종)
#define CUSTOM_BUTTON_PLAY          107  ///< 재생
#define CUSTOM_BUTTON_STOP          108  ///< 정지

// 4. 저장 및 불러오기 (2종)
#define CUSTOM_BUTTON_SAVE          109  ///< 저장하기
#define CUSTOM_BUTTON_LOAD          110  ///< 불러오기

// 5. 색상 버튼 (5종)
#define CUSTOM_BUTTON_BLACK         201  ///< 검은색
#define CUSTOM_BUTTON_RED           202  ///< 빨간색
#define CUSTOM_BUTTON_GREEN         203  ///< 초록색
#define CUSTOM_BUTTON_BLUE          204  ///< 파란색
#define CUSTOM_BUTTON_YELLOW        205  ///< 노란색

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
	LPCWSTR image_path;
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

	int selected_pen_type_id = CUSTOM_BUTTON_PEN_SOLID;  // 펜 종류 (4종)
	int selected_color_id = CUSTOM_BUTTON_BLACK;      // 색 종류 (5종)
	bool is_eraser_mode = false;                    // 도구 모드 (false: 펜, true: 한 획 지우개)

	int active_func_id = -1;


	void setButton(int button_id, LPCWSTR button_name, int x, int y, int width, int height, Gdiplus::Color pen_color, int pen_width, Gdiplus::Color brush_color, int button_style);
	void setImageButton(int button_id, LPCWSTR button_name, int x, int y, int width, int height, LPCWSTR image_path); ///< 이미지 버튼 등록 함수 추가
	ButtonData getButtonById(int button_id);
	void drawButton(HDC hdc, int button_index);
	void drawEllipseShadowButton(Gdiplus::Graphics& hdc_graphics, int button_index);
	void drawImageButton(Gdiplus::Graphics& hdc_graphics, int button_index); ///< 이미지 버튼 그리기 함수 추가

protected:
public:
	void setAllButtons(HWND hWnd);
	void drawAllButtons(HDC hdc);
	void resizeAllButtons(HWND hWnd);

	int trackClick(LPARAM lParam);


	void setSelectedPenType(int id) { selected_pen_type_id = id; }
	void setSelectedColor(int id) { selected_color_id = id; }
	void setEraserMode(bool on) { is_eraser_mode = on; }

};

