#include "MainWindow.h"
#include "helper_functions.h"

const PCWSTR MainWindow::DEFAULT_CLASS_NAME = L"Graphics";
const Mode MainWindow::DEFAULT_MODE = Mode::SelectMode;
const Figure MainWindow::DEFAULT_FIGURE = Figure::Ellipse;
const D2D1_COLOR_F MainWindow::DEFAULT_COLOR = D2D1::ColorF(D2D1::ColorF::Black);

extern HWND scene_handle;
extern WNDPROC scene_proc;
extern HWND control_handle;
extern WNDPROC control_proc;

MainWindow::MainWindow(Mode mode, Figure figure, D2D1_COLOR_F color, PCWSTR CLASS_NAME) :
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
        if (FAILED(D2D1CreateFactory(
            D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory)))
        {
            return -1;  // Fail CreateWindowEx.
        }
        DPIScale::Initialize();
        CreateLayout();
        return 0;

    case WM_SIZE:
        SetLayout();
        return 0;

    case WM_DESTROY:
        SafeRelease(&pFactory);
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
    sceneControl = new SceneControl(&mode, &figure, &color);
    sceneControl->Create(L"Scene control", WS_CHILD | WS_BORDER | WS_VISIBLE, m_hwnd);
    control_handle = sceneControl->Window();
    control_proc = SceneControl::WindowProc;

    graphicsScene = new GraphicsScene(&mode, &figure, &color, pFactory);
    graphicsScene->Create(L"Scene", WS_CHILD | WS_BORDER | WS_VISIBLE, m_hwnd);
    scene_handle = graphicsScene->Window();
    scene_proc = GraphicsScene::WindowProc;
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
