#include "Button.h"


void Button::setButton(int button_id, LPCWSTR button_name, int x, int y, int width, int height, Gdiplus::Color pen_color, int pen_width, Gdiplus::Color brush_color, int button_style)
{
	for (int button_index = 0; button_index < button.size(); button_index++) //이미 같은 버튼 id가 있다면 실패하도록 제한
	{
		if (button_id == button[button_index].button_id)
		{
			return;
		}
	}
	//임시 버튼 데이터 객체 생성
	ButtonData new_button;
	new_button = { 0 }; //입력하지 않은 데이터가 있을 때를 대비해 0으로 초기화

	//데이터 입력
	new_button.button_id = button_id;
	new_button.button_name = button_name;
	new_button.x = x;
	new_button.y = y;
	new_button.width = width;
	new_button.height = height;
	new_button.pen_color = pen_color; //펜을 사용하지 않는다면 Alpha 값을 0으로
	new_button.pen_width = pen_width; //펜을 사용하지 않는다면 pen_width를 0으로
	new_button.brush_color = brush_color;
	new_button.button_style = button_style;

	//데이터 입력이 완료된 객체를 벡터에 저장
	button.push_back(new_button);
}

ButtonData Button::getButtonById(int button_id)
{
	for (int button_index = 0; button_index < button.size(); button_index++)
	{
		if (button[button_index].button_id == button_id)
		{
			return button[button_index];
		}
	}
}

void Button::drawButton(HDC hdc, int button_index)
{
	// 그리기 환경 설정
	//HDC hdc = GetDC(hWnd);
	Gdiplus::Graphics hdc_graphics(hdc);
	hdc_graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);

	//펜 스타일에 따라 맞는 버튼 그리기 함수를 실행
	switch (button[button_index].button_style)
	{
	case BUTTON_STYLE_ELLIPSE_SHADOW:
	{
		drawEllipseShadowButton(hdc_graphics, button_index);
	}
	break;
	case BUTTON_STYLE_RECT_SHADOW:
	{

	}
	break;
	default:
		break;
	}

	//hde 해제
	//ReleaseDC(hWnd, hdc);
}

void Button::drawEllipseShadowButton(Gdiplus::Graphics& hdc_graphics, int button_index)
{
	//버튼 브러시 생성
	Gdiplus::SolidBrush brush(
		Gdiplus::Color(
		button[button_index].brush_color.GetAlpha(),
		button[button_index].brush_color.GetRed(), 
		button[button_index].brush_color.GetGreen(), 
		button[button_index].brush_color.GetBlue()
		)
	);
	//그림자 브러시 생성
	Gdiplus::SolidBrush shadow_brush(Gdiplus::Color(150, 50, 50, 50));

	//그림자 생성
	hdc_graphics.FillEllipse(&shadow_brush, 
		button[button_index].x + 2, 
		button[button_index].y + 2, 
		button[button_index].width, 
		button[button_index].height
	);
	//버튼 배경 채우기
	hdc_graphics.FillEllipse(&brush,
		button[button_index].x,
		button[button_index].y,
		button[button_index].width,
		button[button_index].height
	);
	
	//외곽선 그리기
	//Alpha 값이 0 또는 pen_width가 0일 경우 외곽선을 만들지 않는다고 판단.
	if (button[button_index].pen_color.GetAlpha() != 0 && button[button_index].pen_width != 0)
	{
		//버튼 외곽선 펜 생성
		Gdiplus::Pen pen(
			Gdiplus::Color(
				button[button_index].pen_color.GetAlpha(),
				button[button_index].pen_color.GetRed(),
				button[button_index].pen_color.GetGreen(),
				button[button_index].pen_color.GetBlue()
			), button[button_index].pen_width
		);

		//버튼 외곽선 생성
		hdc_graphics.DrawEllipse(&pen,
			button[button_index].x,
			button[button_index].y,
			button[button_index].width,
			button[button_index].height
		);
	}
}


void Button::setAllButtons()
{
	// setButton으로 버튼을 등록합니다.
	// button id는 겹치면 안됩니다.
	setButton(
		CUSTOM_BUTTON_RED, 
		L"빨간색",
		600, 20, 30, 30,
		Gdiplus::Color(0, 0, 0, 0), 0,
		Gdiplus::Color(255, 255, 0, 0),
		BUTTON_STYLE_ELLIPSE_SHADOW
		);
	setButton(
		CUSTOM_BUTTON_BLACK, 
		L"검은색",
		640, 20, 30, 30,
		Gdiplus::Color(0, 0, 0, 0), 0,
		Gdiplus::Color(255, 0, 0, 0),
		BUTTON_STYLE_ELLIPSE_SHADOW
	);
}


void Button::drawAllButtons(HDC hdc)
{
	// 벡터에 등록된 버튼을 그리기
	for (int button_index = 0; button_index < button.size(); button_index++)
	{
		drawButton(hdc, button_index);
	}
}

int Button::trackClick(LPARAM lParam)
{
	mouse_x = LOWORD(lParam);
	mouse_y = HIWORD(lParam);

	for (int button_num = 0; button_num < button.size(); button_num++)
	{
		if (mouse_x >= button[button_num].x &&
			mouse_x <= button[button_num].x + button[button_num].width &&
			mouse_y >= button[button_num].y &&
			mouse_y <= button[button_num].y + button[button_num].height)
		{
			return button[button_num].button_id;
		}
	}
	return -1;
}


