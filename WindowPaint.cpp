#include "WindowPaint.h"
#include <tchar.h>

namespace windowPaint {
    constexpr float FLOAT_UNINITIALIZED = 0.0f;

    ID2D1Factory          * factory = NULL;
    ID2D1HwndRenderTarget * renderTargetMain = NULL;
    ID2D1HwndRenderTarget * renderTargetCar = NULL;

    namespace road {
        enum brush_e {
            brush_begin = 0,
            brush_surface = brush_begin,
            brush_delim,
            brush_count,
            brush_bad = -1
        };

        ID2D1SolidColorBrush * brush[brush_count];
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
            if (!renderTargetMain)
                return -1;

            D2D1_SIZE_F targetSize = renderTargetMain->GetSize();

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
            renderTargetMain->FillRectangle(&size::surfaceRect, brush[brush_surface]);
        }

        static inline void paint_delim_solid(float from, float to) {
            renderTargetMain->DrawLine(D2D1::Point2F(size::delim, from),
                D2D1::Point2F(size::delim, to), brush[brush_delim],
                size::delimWidth);
        }

        static int paint(void) {
            if (!renderTargetMain)
                return -1;
            paint_surface();
            paint_delim_solid(size::surfaceRect.top, size::surfaceRect.bottom);
            return 0;
        }

    }   //  road

    namespace car {
        enum brush_e {
            brush_begin = 0,
            brush_car = brush_begin,
            brush_count,
            brush_bad = -1
        };
        ID2D1SolidColorBrush * brush[brush_count];

        namespace size {
            float relativeLength = FLOAT_UNINITIALIZED;
            float relativeWidth = FLOAT_UNINITIALIZED;
            float length = FLOAT_UNINITIALIZED;
            float width = FLOAT_UNINITIALIZED;
        }   //  size

        static void resize(void);
        static int paint(float carPosition, float carAngle) {
            D2D1_RECT_F roadRect = road::size::surfaceRect;
            float roadWidth = roadRect.right - roadRect.left;

            float carLengthHalf = size::length / 2.0f;
            float carWidthHalf = size::width / 2.0f;

            float carPaintPositionX = roadWidth * carPosition + roadRect.left;
            float carPaintPositionY = roadRect.bottom * 0.5f;

            float carLeft = carPaintPositionX - carWidthHalf;
            float carTop = carPaintPositionY + carLengthHalf;
            float carRight = carPaintPositionX + carWidthHalf;
            float carBottom = carPaintPositionY - carLengthHalf;

            D2D1_RECT_F car = D2D1::RectF(carLeft, carTop, carRight, carBottom);
            renderTargetMain->FillRectangle(&car, brush[brush_car]);

            return 0;
        }

        static void resize(void) {
            D2D1_SIZE_F renderSize = renderTargetMain->GetSize();
            D2D1_RECT_F roadRect = road::size::surfaceRect;
            float roadWidth = roadRect.right - roadRect.left;
            size::length = roadWidth * size::relativeLength;
            size::width = roadWidth * size::relativeWidth;
        }
    }   //  car

    int factory_init(HWND hWnd) {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory)))
            return -1;
        return 0;
    }

    int car_size_init(float relative_length, float relative_width) {
        car::size::relativeLength = relative_length;
        car::size::relativeWidth = relative_width;
        return 0;
    }

    static int target_brushes_and_size_init(HWND hWnd) {
        if (renderTargetMain)
            return 0;

        RECT rect;
        GetClientRect(hWnd, &rect);

        D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
        if (FAILED(factory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
            D2D1::HwndRenderTargetProperties(hWnd, size), &renderTargetMain)))
            return -1;

        if (FAILED(renderTargetMain->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &road::brush[road::brush_surface])))
            return -1;

        if (FAILED(renderTargetMain->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite), &road::brush[road::brush_delim])))
            return -1;

        if (FAILED(renderTargetMain->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &car::brush[car::brush_car])))
            return -1;

        road::resize();
        car::resize();

        return 0;
    }

    int resize(HWND hWnd) {
        if (!renderTargetMain)
            return 0;

        RECT rect;
        GetClientRect(hWnd, &rect);

        D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
        if (FAILED(renderTargetMain->Resize(size)))
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

        renderTargetMain->BeginDraw();
        renderTargetMain->Clear(D2D1::ColorF(D2D1::ColorF::AntiqueWhite));

        road::paint();
        car::paint(carPosition, carAngle);

        HRESULT result = renderTargetMain->EndDraw();
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
        safe_release_ptr(&renderTargetMain);
        safe_release_ptr(&renderTargetCar);
        for (int i = road::brush_begin; i < road::brush_count; ++i)
            safe_release_ptr(&road::brush[i]);
        for (int i = car::brush_begin; i < car::brush_count; ++i)
            safe_release_ptr(&car::brush[i]);
        return 0;
    }
}   //  windowPaint
