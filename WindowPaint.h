#pragma once

#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

enum pBrush_e {
	begin = 0,
	roadSurface = begin,
	roadDelim,
	count,
	bad = -1
};

int pFactoryInit(HWND);
int pSafeRelease(void);

int Paint(HWND);