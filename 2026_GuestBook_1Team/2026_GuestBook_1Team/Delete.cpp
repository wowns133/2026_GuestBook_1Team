#include "Delete.h"
#pragma comment(lib, "Gdiplus.lib")
using namespace Gdiplus;

void Delete::setDeleteMode(bool state)
{
	delete_mode = state;
}

bool Delete::getDeleteMode()
{
	return delete_mode;
}

Gdiplus::Color Delete::getDeleteColor()
{
	return Gdiplus::Color(255, 0, 255, 0);
}


Gdiplus::Pen* Delete::getEraserPen()
{
	return new Gdiplus::Pen(Gdiplus::Color(0, 0, 0, 0), 14.0f);
}