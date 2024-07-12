#pragma once

#include <Windows.h>
#include "MathDriving.h"

constexpr short unsigned int MAX_LOADSTRING = 100;

class Window {
public:
    Window(_In_ HINSTANCE     hInstance,
           _In_opt_ HINSTANCE hPrevInstance,
           _In_ LPWSTR        lpCmdLine,
           _In_ int           nCmdShow);
    ~Window(void);
    int Init(void);

private:
    //  AppEntry parameters 
    HINSTANCE hInstance;
    HINSTANCE hPrevInstance;
    LPWSTR lpCmdLine;
    int nCmdShow;

    //  Window/Class Title
    WCHAR szTitle[MAX_LOADSTRING];
    WCHAR szWindowClass[MAX_LOADSTRING];

    //  Window Initializtion
    int ClassInit(void) const;
    int HandlerInit(void) const;
    int MsgLoop(void) const;

    //
    world::World World;
};

