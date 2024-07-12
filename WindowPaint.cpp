#include "WindowPaint.h"
#include <tchar.h>

ID2D1Factory          * factory = NULL;
ID2D1HwndRenderTarget * renderTarget = NULL;
ID2D1SolidColorBrush  * brush[windowPaint::brush_e::count];

namespace roadPaintSize {
    constexpr float relativeBorderLeft = 0.075f;
    constexpr float relativeBorderRight = 0.425f;
    constexpr float relativeDelimPos = (relativeBorderLeft + relativeBorderRight) / 2;
    constexpr float relativeDelimWidth = 0.008f;
    constexpr float relativeDelimDashLength = 0.35f;

    float borderLeft;
    float borderRight;
    float delimPos;
    float delimWidth;
    float length;

    static void resize(void);
}   //  roadPaintSize

int windowPaint::factory_init(HWND hWnd) {
    if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory)))
        return -1;
    return 0;
}

static int target_and_brushes_init(HWND hWnd) {
    if (renderTarget)
        return 0;

    RECT rect;
    GetClientRect(hWnd, &rect);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
    if (FAILED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
               D2D1::HwndRenderTargetProperties(hWnd, size), &renderTarget)))
        return -1;

    roadPaintSize::resize();

    if (FAILED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &brush[windowPaint::brush_e::roadSurface])))
        return -1;

    if (FAILED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite), &brush[windowPaint::brush_e::roadDelim])))
        return -1;

    return 0;
}

int windowPaint::resize(HWND hWnd) {
    if (!renderTarget)
        return 0;

    RECT rect;
    GetClientRect(hWnd, &rect);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
    if (FAILED(renderTarget->Resize(size)))
        return -1;

    roadPaintSize::resize();

    return 0;
}

template <class T> inline void safe_release_ptr(T** ptr) {
    if (*ptr) {
        (*ptr)->Release();
        *ptr = NULL;
    }
}

int windowPaint::safe_release(void) {
    safe_release_ptr(&factory);
    safe_release_ptr(&renderTarget);
    for (int i = brush_e::begin; i < brush_e::count; ++i)
        safe_release_ptr(&brush[i]);

    return 0;
}

int paint_road(void);
int paint_car(void);

int windowPaint::paint(HWND hWnd) {
    if (target_and_brushes_init(hWnd))
        return -1;

    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);

    renderTarget->BeginDraw();
    renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::AntiqueWhite));

    paint_road();
    paint_car();

    HRESULT result = renderTarget->EndDraw();
    if (FAILED(result) || result == D2DERR_RECREATE_TARGET) {
        safe_release();
    }

    EndPaint(hWnd, &ps);
    return 0;
}

static void roadPaintSize::resize(void) {
    D2D1_SIZE_F size = renderTarget->GetSize();

    roadPaintSize::borderLeft = size.width * roadPaintSize::relativeBorderLeft;
    roadPaintSize::borderRight = size.width * roadPaintSize::relativeBorderRight;
    roadPaintSize::delimPos = size.width * roadPaintSize::relativeDelimPos;
    roadPaintSize::delimWidth = size.width * roadPaintSize::relativeDelimWidth;
    roadPaintSize::length = size.height;
}

static inline void paint_road_surface(void) {
    D2D1_RECT_F roadSurface = D2D1::RectF(roadPaintSize::borderLeft, 0, roadPaintSize::borderRight, roadPaintSize::length);
    renderTarget->FillRectangle(&roadSurface, brush[windowPaint::brush_e::roadSurface]);
}

static inline void paint_road_delim_solid(float from, float to) {
    renderTarget->DrawLine(D2D1::Point2F(roadPaintSize::delimPos, from), 
                           D2D1::Point2F(roadPaintSize::delimPos, to),
                           brush[windowPaint::brush_e::roadDelim], roadPaintSize::delimWidth);
}

static inline void paint_road_delim_dash(float from, float to) {
    D2D1_SIZE_U pixelSize = renderTarget->GetPixelSize();
}

int paint_road(void) {
    D2D1_SIZE_F size = renderTarget->GetSize();
    paint_road_surface();
    paint_road_delim_solid(0, roadPaintSize::length);

    return 0;
}

int paint_car(void) {
    return 0;
}