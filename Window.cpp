#include "Window.h"
#include "WindowPaint.h"
#include "MathDriving.h"
#include "WindowKeyState.h"

#include "framework.h"
#include "resource.h"
#include <d2d1.h>
#pragma comment(lib, "d2d1")

//  ƒл€ окон и отрисовки были выбраны WinApi и Direct2D по общим соображени€м:
//  - ѕо заданию проект в Visual Studio -> веро€тно, Windows
//  - Direct2 лучше/интереснее стандартных битмап

//  »з-за трудностей работы с WindowProc как static методом (иначе не подать в WINCLASSEX)
//  часть кода была вынесена в global scope ради выполнени€ задачи. ¬озможно, объект
//  класса Window получилось бы доставать из параметров WindowProc, но с этим возникли
//  трудности, а т.к. врем€ ограничено было выбрано быстрое решение.

//  »з подобных соображений окно сделано статическим: минимального стандартного размера.

world::World World = world::World();

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
    windowPaint::safe_release();
}

int Window::Init(void) {
    if (ClassInit())
        return -1;
    if (HandlerInit())
        return -1;

    return MsgLoop();
}

int Window::ClassInit(void) const {
    WNDCLASSEXW windowClassEx = {0};

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
    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU ,
                         CW_USEDEFAULT, 0, 640, 480, nullptr, nullptr, hInstance, nullptr);

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
    if (message == WM_CREATE) {
        if (windowPaint::factory_init(hWnd))
            return 0;
        if (windowPaint::car_size_init(World.GetCarLength(), World.GetCarWidth()))
            return 0;
        SetTimer(hWnd, 7777777, 20, NULL);
        return 1;
    }

    switch (message) {
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        // –азобрать выбор в меню:
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
    case WM_SIZE: {
        if (windowPaint::resize(hWnd))
            DestroyWindow(hWnd);
        break;
    }
    case WM_TIMER: {
        unsigned key_state = 0;
        if (GetAsyncKeyState(VK_UP))
            key_state |= windowKeyState::UP;
        if (GetAsyncKeyState(VK_DOWN))
            key_state |= windowKeyState::DOWN;
        if (GetAsyncKeyState(VK_LEFT))
            key_state |= windowKeyState::LEFT;
        if (GetAsyncKeyState(VK_RIGHT))
            key_state |= windowKeyState::RIGHT;
        World.Drive(key_state, 20);

        if (!World.StateChanged())
            break;
    }
    case WM_PAINT: {
        float pos_x = World.GetCarPositionX();
        float pos_y = World.GetCarPositionY();
        int times_y = World.GetCarYTimesLast();
        float angle = World.GetCarAngle();
        windowPaint::paint(hWnd, pos_x, pos_y, times_y, angle);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

