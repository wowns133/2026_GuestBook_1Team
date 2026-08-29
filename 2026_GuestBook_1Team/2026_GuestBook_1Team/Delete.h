#pragma once
#include <vector>

// 매크로 충돌 방지 및 GDI+ 종속성 해결을 위한 필수 선언
#define NOMINMAX
#include <Windows.h>
#include <objidl.h>
#include <gdiplus.h>

class Delete
{
private:
	bool delete_mode = false; 

public:
	void setDeleteMode(bool state);
	bool getDeleteMode(); 

	Gdiplus::Color getDeleteColor();
	
	// GDI+ 펜 반환으로 변경
	Gdiplus::Pen* getEraserPen(); 
};
