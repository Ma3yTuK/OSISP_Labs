#include "MainWindow.h"
#include "helper_functions.h"

const PCWSTR MainWindow::DEFAULT_CLASS_NAME = L"Graphics";
const Mode MainWindow::DEFAULT_MODE = Mode::SelectMode;
const Figure MainWindow::DEFAULT_FIGURE = Figure::Ellipse;
const Color MainWindow::DEFAULT_COLOR = Color(0, 0, 0);

extern HWND scene_handle;
extern WNDPROC scene_proc;
extern HWND control_handle;
extern WNDPROC control_proc;

MainWindow::MainWindow(Mode mode, Figure figure, Color color, PCWSTR CLASS_NAME) :
    BaseWindow<MainWindow>(CLASS_NAME), mode(mode), figure(figure), color(color), sceneControl(NULL), graphicsScene(NULL)
{
}

MainWindow::~MainWindow()
{
    if (sceneControl)
        delete sceneControl;
    if (graphicsScene)
        delete graphicsScene;
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        DPIScale::Initialize();
        CreateLayout();
        return 0;

    case WM_SIZE:
        SetLayout();
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_MODE_CHANGED:
        PostMessage(sceneControl->Window(), uMsg, wParam, lParam);
        PostMessage(graphicsScene->Window(), uMsg, wParam, lParam);
        return 0;

    case WM_FIGURE_CHANGED:
        PostMessage(sceneControl->Window(), uMsg, wParam, lParam);
        PostMessage(graphicsScene->Window(), uMsg, wParam, lParam);
        return 0;

    case WM_COLOR_CHANGED:
        PostMessage(sceneControl->Window(), uMsg, wParam, lParam);
        PostMessage(graphicsScene->Window(), uMsg, wParam, lParam);
        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::CreateLayout()
{
    modePicker = CreateWindowEx(0,
        WC_COMBOBOX,
        NULL,
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        0, 0, 0, 0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    figurePicker = CreateWindowEx(0,
        WC_COMBOBOX,
        NULL,
        CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE,
        0, 0, 0, 0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    for (auto& i : MODE_NAMES)
    {
        SendMessage(modePicker, CB_ADDSTRING, 0, (LPARAM)i);
    }
    SendMessage(modePicker, CB_SETCURSEL, (WPARAM)*mode, 0);

    for (auto& i : FIGURE_NAMES)
    {
        SendMessage(figurePicker, CB_ADDSTRING, 0, (LPARAM)i);
    }
    SendMessage(figurePicker, CB_SETCURSEL, (WPARAM)*figure, 0);

    for (auto& i : BUTTON_COLORS)
    {
        buttons[CreateWindow(L"BUTTON",
            NULL,
            WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | BS_OWNERDRAW | WS_BORDER,
            0,
            0,
            0,
            0,
            m_hwnd,
            NULL,
            GetModuleHandle(NULL),
            NULL)] = i;
    }
}

void MainWindow::SetLayout()
{
    RECT rcClient;
    GetClientRect(m_hwnd, &rcClient);

    MoveWindow(sceneControl->Window(),
        rcClient.left,
        rcClient.top,
        rcClient.right,
        sceneControl->GetRealWindowHeight(),
        FALSE);

    RECT rcControl;
    GetClientRect(sceneControl->Window(), &rcControl);

    MoveWindow(graphicsScene->Window(),
        rcClient.left,
        rcControl.bottom,
        rcClient.right,
        rcClient.bottom - rcControl.bottom,
        FALSE);

    InvalidateRect(m_hwnd, NULL, FALSE);
}
