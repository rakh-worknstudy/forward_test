#pragma once

#include <Windows.h>
#include <d2d1.h>
#pragma comment(lib, "d2d1")

namespace windowPaint {
    enum brush_e {
        begin = 0,
        roadSurface = begin,
        roadDelim,
        count,
        bad = -1
    };
    int factory_init(HWND);
    int safe_release(void);
    int resize(HWND);
    int paint(HWND);
}   //  windowPaint


