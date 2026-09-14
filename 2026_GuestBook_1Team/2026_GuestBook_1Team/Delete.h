#pragma once
#include <Windows.h>


class AllDelete
{
private:
	
public:
	void all_clear(HWND hWnd, Draw& draw);
};


class SingleDelete
{
public:
	void single_clear(HWND hWnd, Draw& draw, LPARAM lParam);
};