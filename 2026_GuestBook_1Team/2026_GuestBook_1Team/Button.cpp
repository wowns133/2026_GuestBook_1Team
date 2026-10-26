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
	new_button.image_path = nullptr;

	//데이터 입력이 완료된 객체를 벡터에 저장
	button.push_back(new_button);
}

void Button::setImageButton(int button_id, LPCWSTR button_name, int x, int y, int width, int height, LPCWSTR image_path)
{
	for (int button_index = 0; button_index < button.size(); button_index++)
	{
		if (button_id == button[button_index].button_id)
		{
			return;
		}
	}

	ButtonData new_button = { 0 };

	new_button.button_id = button_id;
	new_button.button_name = button_name;
	new_button.x = x;
	new_button.y = y;
	new_button.width = width;
	new_button.height = height;
	new_button.button_style = BUTTON_STYLE_IMAGE;
	new_button.image_path = image_path;

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

	case BUTTON_STYLE_IMAGE:
		drawImageButton(hdc_graphics, button_index);
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

	// ★ 2. 색상 버튼은 selected_color_id와 같을 때만 테두리 표시
	if (button[button_index].button_id == selected_color_id)
	{
		Gdiplus::Pen selectPen(Gdiplus::Color(255, 0, 120, 255), 3);
		hdc_graphics.DrawEllipse(&selectPen,
			button[button_index].x - 3,
			button[button_index].y - 3,
			button[button_index].width + 6,
			button[button_index].height + 6
		);
	}
}

// 3. 이미지 그리기 함수 (신규 추가)
void Button::drawImageButton(Gdiplus::Graphics& hdc_graphics, int button_index)
{
	if (button[button_index].image_path == nullptr) return;

	Gdiplus::Image image(button[button_index].image_path);
	if (image.GetLastStatus() == Gdiplus::Ok)
	{
		hdc_graphics.DrawImage(&image,
			button[button_index].x,
			button[button_index].y,
			button[button_index].width,
			button[button_index].height
		);
	}


	int current_id = button[button_index].button_id;
	bool is_selected = false;

	if (current_id >= CUSTOM_BUTTON_PEN_SOLID && current_id <= CUSTOM_BUTTON_PEN_HIGHLIGHT)
	{
		// 펜 버튼: 펜 모드이면서 내가 선택된 펜일 때만
		is_selected = (!is_eraser_mode && current_id == selected_pen_type_id);
	}
	else if (current_id == CUSTOM_BUTTON_ERASER_STROKE)
	{
		// 한 획 지우개: 지우개 모드일 때만
		is_selected = is_eraser_mode;
	}

	if (is_selected)
	{
		Gdiplus::Pen selectPen(Gdiplus::Color(255, 0, 120, 255), 2);
		hdc_graphics.DrawRectangle(&selectPen,
			button[button_index].x - 2,
			button[button_index].y - 2,
			button[button_index].width + 4,
			button[button_index].height + 4
		);
	}
}

void Button::setAllButtons(HWND hWnd)
{
	RECT rect;
	GetClientRect(hWnd, &rect);
	int button_size = 40;
	int client_height = rect.bottom - rect.top;
	int button_y = (client_height - button_size) / 2;

	setImageButton(
		CUSTOM_BUTTON_PEN_SOLID,
		L"기본펜", 20, 40, 40, 40,
		L"assets\\pen_solid.png"
	);
	setImageButton(CUSTOM_BUTTON_PEN_SPRAY, L"스프레이", 80, 40, 40, 40, L"assets/pen_spray.png");
	setImageButton(CUSTOM_BUTTON_PEN_BRUSH, L"붓펜", 120, 40, 40, 40, L"assets/pen_brush.png");
	setImageButton(CUSTOM_BUTTON_PEN_HIGHLIGHT, L"형광펜", 180, 40, 40, 40, L"assets/pen_highlight.png");

	setImageButton(CUSTOM_BUTTON_ERASER_STROKE, L"한획지우기", 740, 40, 40, 40, L"assets/eraser_stroke.png");
	setImageButton(CUSTOM_BUTTON_ERASER_ALL, L"전체지우기", 800, 40, 40, 40, L"assets/eraser_all.png");

	setImageButton(CUSTOM_BUTTON_PLAY, L"재생", 950, 40, 40, 40, L"assets/play.png");
	setImageButton(CUSTOM_BUTTON_STOP, L"정지", 1000, 40, 40, 40, L"assets/stop.png");

	setImageButton(CUSTOM_BUTTON_SAVE, L"저장", 1130, 40, 40, 40, L"assets/save.png");
	setImageButton(CUSTOM_BUTTON_LOAD, L"불러오기", 1200, 40, 40, 40, L"assets/load.png");

	// setButton으로 버튼을 등록합니다.
	// button id는 겹치면 안됩니다.
	setButton(
		CUSTOM_BUTTON_RED,
		L"빨간색",
		350, 40, 40, 40,
		Gdiplus::Color(0, 0, 0, 0), 0,
		Gdiplus::Color(255, 255, 0, 0),
		BUTTON_STYLE_ELLIPSE_SHADOW
	);
	setButton(
		CUSTOM_BUTTON_BLACK,
		L"검은색",
		300, 40, 40, 40,
		Gdiplus::Color(0, 0, 0, 0), 0,
		Gdiplus::Color(255, 0, 0, 0),
		BUTTON_STYLE_ELLIPSE_SHADOW
	);
	setButton(
		CUSTOM_BUTTON_GREEN,
		L"초록색",
		400, 40, 40, 40,
		Gdiplus::Color(0, 0, 0, 0), 0,
		Gdiplus::Color(255, 0, 200, 0),
		BUTTON_STYLE_ELLIPSE_SHADOW
	);
	setButton(
		CUSTOM_BUTTON_BLUE,
		L"파란색",
		450, 40, 40, 40,
		Gdiplus::Color(0, 0, 0, 0), 0,
		Gdiplus::Color(255, 0, 0, 255),
		BUTTON_STYLE_ELLIPSE_SHADOW
	);
	setButton(
		CUSTOM_BUTTON_YELLOW,
		L"노란색",
		500, 40, 40, 40,
		Gdiplus::Color(0, 0, 0, 0), 0,
		Gdiplus::Color(255, 255, 255, 0),
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

void Button::resizeAllButtons(HWND hWnd)
{
	RECT rect; // RECT 자료형 선언
	GetClientRect(hWnd, &rect); // hWnd 창의 클라이언트 크기를 rect에 저장
	int button_height = 40; // 버튼의 높이
	int client_height = rect.bottom - rect.top; // 클라이언트 높이
	int button_y = (client_height - button_height) / 2; //창 전체 높이에서 버튼 높이를 뺀 위아래 여백 길이

	for (int button_index = 0; button_index < button.size(); button_index++)
	{
		button[button_index].y = button_y;
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


