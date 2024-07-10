// ForwardTest.cpp : Определяет точку входа для приложения.
//

#include "ForwardTest.h"
#include <Windows.h>
#include "Window.h"

int APIENTRY wWinMain(_In_ HINSTANCE    hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR        lpCmdLine,
                     _In_ int           nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    Window BalkanDrivingWindow(hInstance, hPrevInstance, lpCmdLine, nCmdShow);
    return BalkanDrivingWindow.Init();
}

