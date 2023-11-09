#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>

#include <list>
#include <memory>

using namespace std;

#pragma comment(lib, "d2d1")

#include "BaseWindow.h"
#include "EllipseFigure.h"
#include "resource.h"
#include "GraphicsScene.h"
#include "SceneControl.h"
#include "MainWindow.h"

#include <objidl.h>
#include <gdiplus.h>

#pragma comment (lib,"Gdiplus.lib")

HWND scene_handle;
WNDPROC scene_proc;
HWND control_handle;
WNDPROC control_proc;

LRESULT CALLBACK GetMsgProcCustom(
    _In_ int    code,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
)
{
    UINT Msg = ((MSG*)lParam)->message;
    if (code >= 0 && (Msg == WM_MODE_CHANGED || Msg == WM_FIGURE_CHANGED || Msg == WM_COLOR_CHANGED))
    {
        control_proc(control_handle, Msg, wParam, lParam);
        scene_proc(scene_handle, Msg, wParam, lParam);
        return 0;
    }

    return CallNextHookEx(NULL, code, wParam, lParam);
}

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

    if (!win.Create(L"Draw Circles", WS_OVERLAPPEDWINDOW))
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