#pragma once

#include <Windows.h>

namespace windowPaint {
    int factory_init(HWND);
    int safe_release(void);
    int car_size_init(float relative_length, float relative_width);
    int resize(HWND);
    int paint(HWND hWnd, float carPositionX, float carPositionY, unsigned carTimesY, float carAngle, UINT8 drive_state);
}   //  windowPaint


