#include "WindowPaint.h"
#include <tchar.h>

ID2D1Factory          * pFactory = NULL;
ID2D1HwndRenderTarget * pRenderTarget = NULL;
ID2D1SolidColorBrush  * pBrush[pBrush_e::count];

int pFactoryInit(HWND hWnd) {
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        return -1;
    return 0;
}

static int pTargetAndBrushesInit(HWND hWnd) {
    if (pRenderTarget)
        return 0;

    RECT rect;
    GetClientRect(hWnd, &rect);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
    if (FAILED(pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
               D2D1::HwndRenderTargetProperties(hWnd, size), &pRenderTarget)))
        return -1;

    if (FAILED(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &pBrush[pBrush_e::roadSurface])))
        return -1;

    if (FAILED(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite), &pBrush[pBrush_e::roadDelim])))
        return -1;

    return 0;
}

template <class T> inline void SafeRelease(T** ptr) {
    if (*ptr) {
        (*ptr)->Release();
        *ptr = NULL;
    }
}

int pSafeRelease(void) {
    SafeRelease(&pFactory);
    SafeRelease(&pRenderTarget);
    for (int i = pBrush_e::begin; i < pBrush_e::count; ++i)
        SafeRelease(&pBrush[i]);

    return 0;
}

int PaintRoad(void);
int PaintCar(void);

int Paint(HWND hWnd) {
    if (pTargetAndBrushesInit(hWnd))
        return -1;

    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);

    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::AntiqueWhite));

    PaintRoad();
    PaintCar();

    HRESULT result = pRenderTarget->EndDraw();
    if (FAILED(result) || result == D2DERR_RECREATE_TARGET) {
        pSafeRelease();
    }

    EndPaint(hWnd, &ps);
    return 0;
}

int PaintRoad(void) {
    D2D1_SIZE_F size = pRenderTarget->GetSize();

    float roadBorderLeft = size.width * 0.075f;
    float roadBorderRight = size.width * 0.425f;
    float roadDelimiter = size.width * 0.250f;
    float roadDelimWidth = size.width * 0.0015f;
    float roadDrawLength = size.height;

    D2D1_RECT_F roadSurface = D2D1::RectF(roadBorderLeft, 0, roadBorderRight, roadDrawLength);
    pRenderTarget->FillRectangle(&roadSurface, pBrush[pBrush_e::roadSurface]);
    pRenderTarget->DrawLine(D2D1::Point2F(roadDelimiter, 0.0f), D2D1::Point2F(roadDelimiter, roadDrawLength), 
                            pBrush[pBrush_e::roadDelim], roadDelimWidth);

    return 0;
}

int PaintCar(void) {
    return 0;
}