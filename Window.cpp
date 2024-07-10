#include "Window.h"
#include "WindowPaint.h"
#include "framework.h"
#include "resource.h"
#include <d2d1.h>
#pragma comment(lib, "d2d1")

//  То, что пришлось вынести из-за вызовов в static методе
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);

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
    pSafeRelease();
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
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
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

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
    {
        if (pFactoryInit(hWnd))
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
        Paint(hWnd);
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case VK_LEFT:
    case VK_RIGHT:
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

