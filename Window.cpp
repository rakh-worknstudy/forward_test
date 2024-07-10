#include "Window.h"
#include "framework.h"
#include "resource.h"

//  То, что пришлось вынести из-за вызовов в static методе
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
HWND hWnd = NULL;

ID2D1Factory          *pFactory      = NULL;
ID2D1HwndRenderTarget *pRenderTarget = NULL;

ID2D1SolidColorBrush *pBrush_roadSurface = NULL;
ID2D1SolidColorBrush *pBrush_roadDelim   = NULL;

int ResourcesInit(void);
int ResourcesClean(void);

int PaintRoad(void);
int Paint(void);

Window::Window(_In_ HINSTANCE hInstance,
               _In_opt_ HINSTANCE hPrevInstance,
               _In_ LPWSTR lpCmdLine,
               _In_ int nCmdShow) {
   this->hInstance = hInstance;
   this->hPrevInstance = hPrevInstance;
   this->lpCmdLine = lpCmdLine;
   this->nCmdShow = nCmdShow;

   LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
   LoadStringW(hInstance, IDC_FORWARDTEST, szWindowClass, MAX_LOADSTRING);
}

Window::~Window(void) {
    ResourcesClean();
}

int Window::Init(void) {
    if (ClassInit())
        return -1;
    if (HandlerInit())
        return -1;

    return MsgLoop();
}

int Window::ClassInit(void) const {
    WNDCLASSEXW windowClassEx;

    windowClassEx.cbSize = sizeof(WNDCLASSEX);
    windowClassEx.style = CS_HREDRAW | CS_VREDRAW;
    windowClassEx.lpfnWndProc = WindowProc;
    windowClassEx.cbClsExtra = 0;
    windowClassEx.cbWndExtra = 0;
    windowClassEx.hInstance = hInstance;
    windowClassEx.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_FORWARDTEST));
    windowClassEx.hCursor = LoadCursor(nullptr, IDC_ARROW);
    windowClassEx.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    windowClassEx.lpszMenuName = MAKEINTRESOURCEW(IDC_FORWARDTEST);
    windowClassEx.lpszClassName = szWindowClass;
    windowClassEx.hIconSm = LoadIcon(windowClassEx.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    if (!RegisterClassExW(&windowClassEx)) {
        MessageBox(NULL, _T("Window::ClassInit(): RegisterClassExW() failed"), _T("ERROR"), MB_OK);
        return -1;
    }
    return 0;
}

int Window::HandlerInit(void) const {
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
                         CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd) {
        MessageBox(NULL, _T("Window::HanlerInit(): CreateWindow() failed"), _T("ERROR"), MB_OK);
        return -1;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return 0;
}

int Window::MsgLoop(void) const {
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_FORWARDTEST));

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int)msg.wParam;
}


int ResourcesInit(void) {
    if (pRenderTarget)
        return 0;

    RECT rect;
    GetClientRect(hWnd, &rect);

    D2D1_SIZE_U size = D2D1::SizeU(rect.right, rect.bottom);
    if (FAILED(pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
                                                D2D1::HwndRenderTargetProperties(hWnd, size),
                                                &pRenderTarget))) {
        MessageBox(NULL, _T("Window::ResourceInit(): pFactory->CreateHwndRenderTarget() failed"), _T("ERROR"), MB_OK);
        return -1;
    }

    if (FAILED(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::DarkGray), &pBrush_roadSurface))) {
        MessageBox(NULL, _T("Window::ResourceInit(): pRenderTarget->CreateSolidColorBrush() failed"), _T("ERROR"), MB_OK);
        return -1;
    }

    if (FAILED(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::GhostWhite), &pBrush_roadDelim))) {
        MessageBox(NULL, _T("Window::ResourceInit(): pRenderTarget->CreateSolidColorBrush() failed"), _T("ERROR"), MB_OK);
        return -1;
    }

    return 0;
}

template <class T> inline void SafeRelease(T** ptr) {
    if (*ptr) {
        (*ptr)->Release();
        *ptr = NULL;
    }
}

int ResourcesClean(void) {
    SafeRelease(&pRenderTarget);
    SafeRelease(&pBrush_roadSurface);
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
            return 0;
        return 1;
    }
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Разобрать выбор в меню:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
        Paint();
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
        break;
    case WM_KEYUP:
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

int PaintRoad(void) {
    D2D1_SIZE_F size = pRenderTarget->GetSize();

    float roadBorderLeft = size.width * 0.075f;
    float roadBorderRight = size.width * 0.425f;
    float roadDelimiter = size.width * 0.250f;
    float roadDrawLength = size.height;
    float roadBrushWidth = size.width * 0.0025f;

    D2D1_RECT_F roadSurface = D2D1::RectF(roadBorderLeft, 0, roadBorderRight, roadDrawLength);
    pRenderTarget->FillRectangle(&roadSurface, pBrush_roadSurface);
    pRenderTarget->DrawLine(D2D1::Point2F(roadDelimiter, 0.0f), D2D1::Point2F(roadDelimiter, roadDrawLength), pBrush_roadDelim, roadBrushWidth);

    return 0;
}

int Paint(void) {
    if (ResourcesInit())
        return -1;

    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);

    pRenderTarget->BeginDraw();
    pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::AntiqueWhite));

    PaintRoad();

    HRESULT result = pRenderTarget->EndDraw();
    if (FAILED(result) || result == D2DERR_RECREATE_TARGET) {
        ResourcesClean();
    }

    EndPaint(hWnd, &ps);
    return 0;
}