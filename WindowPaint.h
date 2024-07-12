#pragma once

#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

namespace windowPaint {
    int factory_init(HWND);
    int safe_release(void);
    int car_relative_size_init(float relativeWidth, float relativeLength);
    int resize(HWND);
    int paint(HWND, float carPosition, float carAngle);
}   //  windowPaint


