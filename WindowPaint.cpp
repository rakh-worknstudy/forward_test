#include "WindowPaint.h"
#include <tchar.h>

namespace windowPaint {
    constexpr float FLOAT_UNINITIALIZED = 0.0f;

    enum brush_e {
        begin = 0,
        roadSurface = begin,
        roadDelim,
        car,
        count,
        bad = -1
    };

    enum renderTarget_e {
        begin = 0,
        road = begin,
        car,
        count,
        bad = -1
    };

    ID2D1Factory          * factory = NULL;
    ID2D1HwndRenderTarget * renderTarget[renderTarget_e::count];
    ID2D1SolidColorBrush  * brush[brush_e::count];

    namespace road {
        namespace size {
            constexpr float RELATIVE_LEFT = 0.075f;
            constexpr float RELATIVE_RIGHT = 0.425f;
            constexpr float RELATIVE_DELIM = (RELATIVE_LEFT + RELATIVE_RIGHT) / 2;
            constexpr float RELATIVE_DELIM_WIDTH = 0.008f;

            D2D1_RECT_F surfaceRect = D2D1::RectF(FLOAT_UNINITIALIZED, FLOAT_UNINITIALIZED, 
                                                  FLOAT_UNINITIALIZED, FLOAT_UNINITIALIZED);

            float delim = FLOAT_UNINITIALIZED;
            float delimWidth = FLOAT_UNINITIALIZED;
        }   //  size

        static int resize(void);
        static int paint(void);

        static int resize(void) {
            if (!renderTarget[renderTarget_e::road])
                return -1;

            D2D1_SIZE_F targetSize = renderTarget[renderTarget_e::road]->GetSize();

            static constexpr float top = 0.0f;
            float bottom = targetSize.height;
            float left = targetSize.width * size::RELATIVE_LEFT;
            float right = targetSize.width * size::RELATIVE_RIGHT;
            size::surfaceRect = D2D1::RectF(left, top, right, bottom);

            size::delim = targetSize.width * size::RELATIVE_DELIM;
            size::delimWidth = targetSize.width * size::RELATIVE_DELIM_WIDTH;

            return 0;
        }

        static inline void paint_surface(void) {
            renderTarget[renderTarget_e::road]->FillRectangle(&size::surfaceRect, brush[brush_e::roadSurface]);
        }

        static inline void paint_delim_solid(float from, float to) {
            renderTarget[renderTarget_e::road]->DrawLine(D2D1::Point2F(size::delim, from),
                                                         D2D1::Point2F(size::delim, to),
                                                         brush[windowPaint::brush_e::roadDelim],
                                                         size::delimWidth);
        }

        static int paint(void) {
            if (!renderTarget[renderTarget_e::road])
                return -1;
            paint_surface();
            paint_delim_solid(size::surfaceRect.top, size::surfaceRect.bottom);
            return 0;
        }

    }   //  road

    namespace car {
        namespace size {
            constexpr float relativePosY = 0.5f;
            float relativeWidth = FLOAT_UNINITIALIZED;
            float relativeLength = FLOAT_UNINITIALIZED;
            float width = FLOAT_UNINITIALIZED;
            float length = FLOAT_UNINITIALIZED;
        }   //  size

        static void resize(void);
    }   //  car

    int factory_init(HWND hWnd) {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory)))
            return -1;
        return 0;
    }

    int car_relative_size_init(float relativeWidth, float relativeLength) {
        car::size::relativeWidth = relativeWidth;
        car::size::relativeLength = relativeLength;
        return 0;
    }

    int resize(HWND hWnd) {
        if (!renderTarget[renderTarget_e::road])
            return 0;

        RECT rect;
        GetClientRect(hWnd, &rect);

        D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
        if (FAILED(renderTarget[renderTarget_e::road]->Resize(size)))
            return -1;

        road::resize();
        car::resize();

        return 0;
    }

    int paint(HWND hWnd, float carPosition, float carAngle) {
        if (target_brushes_and_size_init(hWnd))
            return -1;

        PAINTSTRUCT ps;
        BeginPaint(hWnd, &ps);

        renderTarget->BeginDraw();
        renderTarget->Clear(D2D1::ColorF(D2D1::ColorF::AntiqueWhite));

        paint_road();
        paint_car(carPosition, carAngle);

        HRESULT result = renderTarget->EndDraw();
        if (FAILED(result) || result == D2DERR_RECREATE_TARGET) {
            safe_release();
        }

        EndPaint(hWnd, &ps);
        return 0;
    }

    template <class T> inline void safe_release_ptr(T** ptr) {
        if (*ptr) {
            (*ptr)->Release();
            *ptr = NULL;
        }
    }

    int safe_release(void) {
        safe_release_ptr(&factory);
        safe_release_ptr(&renderTarget);
        for (int i = brush_e::begin; i < brush_e::count; ++i)
            safe_release_ptr(&brush[i]);

        return 0;
    }
}   //  windowPaint
ID2D1Factory          * factory = NULL;
ID2D1HwndRenderTarget * renderTarget = NULL;
ID2D1SolidColorBrush  * brush[windowPaint::brush_e::count];
//  ----------------------------------------------------------------


int windowPaint::car_relative_size_init(float relativeWidth, float relativeLength) {
    paintSize::car::relativeWidth = relativeWidth;
    paintSize::car::relativeLength = relativeLength;
    return 0;
}

static int target_brushes_and_size_init(HWND hWnd) {
    if (renderTarget)
        return 0;

    RECT rect;
    GetClientRect(hWnd, &rect);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
    if (FAILED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
        D2D1::HwndRenderTargetProperties(hWnd, size), &renderTarget)))
        return -1;

    if (FAILED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &brush[windowPaint::brush_e::roadSurface])))
        return -1;

    if (FAILED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite), &brush[windowPaint::brush_e::roadDelim])))
        return -1;

    if (FAILED(renderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &brush[windowPaint::brush_e::car])))
        return -1;

    paintSize::road::resize();
    paintSize::car::resize();

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

    paintSize::road::resize();
    paintSize::car::resize();

    return 0;
}

//  ----------------------------------------------------------------

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


static void paintSize::car::resize(void) {
    D2D1_SIZE_F renderSize = renderTarget->GetSize();

    paintSize::car::width = (paintSize::road::borderRight - paintSize::road::borderLeft) / paintSize::car::relativeWidth;
    paintSize::car::length = paintSize::car::width * paintSize::car::relativeLength;
}

int paint_car(float carPosition, float carAngle) {

    float carPaintPositionX = (paintSize::road::borderRight - paintSize::road::borderLeft) * carPosition +
                             paintSize::road::borderLeft;
    float carPaintPositionY = paintSize::road::length * paintSize::car::relativePosY;

    float carHalfLength = paintSize::car::length / 2.0f;
    float carHalfWidth = paintSize::car::width / 2.0f;

    float carLeft = carPaintPositionX - carHalfWidth;
    float carTop = carPaintPositionY + carHalfLength;
    float carRight = carPaintPositionX + carHalfWidth;
    float carBottom = carPaintPositionY - carHalfLength;

    D2D1_RECT_F car = D2D1::RectF(carLeft, carTop, carRight, carBottom);
    renderTarget->FillRectangle(&car, brush[windowPaint::brush_e::car]);

    return 0;
}