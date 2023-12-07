#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <Winuser.h>

#include <list>
#include <memory>

using namespace std;

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Ntdll")
#pragma comment(lib, "Comctl32")
#pragma comment(lib, "Ws2_32.lib")

#include "BaseWindow.h"
#include "MainWindow.h"

#include <objidl.h>
#include <gdiplus.h>
#include <winternl.h>
#include <CommCtrl.h>

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR, int nCmdShow)
{
    //Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    //ULONG_PTR gdiplusToken;
    //Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
    MainWindow win = MainWindow();
    //Mode mode = Mode::SelectMode;
    //Figure figure = Figure::Ellipse;
    //D2D1_COLOR_F color = D2D1::ColorF(D2D1::ColorF::Black);
    //SceneControl win = SceneControl(&mode, &figure, &color);
    //GraphicsScene win = GraphicsScene(&mode, &figure, &color);

    if (!win.Create(L"Draw Circles",
        WS_OVERLAPPEDWINDOW,
        NULL,
        NULL,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        800,
        600))
    {
        return 0;
    }
    //SetWindowsHookEx(WH_GETMESSAGE, GetMsgProcCustom, NULL, GetCurrentThreadId());

    //HACCEL hAccel1 = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL1));
    //HACCEL hAccel2 = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCEL2));
    
    ShowWindow(win.Window(), nCmdShow);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        //if (!TranslateAccelerator(GetFocus(), hAccel1, &msg) && !TranslateAccelerator(GetFocus(), hAccel2, &msg))
        //{
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        //}
    }
    return 0;
}