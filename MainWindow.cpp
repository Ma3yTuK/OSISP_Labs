#include "MainWindow.h"

const PCWSTR MainWindow::DEFAULT_CLASS_NAME = L"Graphics";
const float MainWindow::MARGIN_X = 6.0F;
const float MainWindow::MARGIN_Y = 6.0F;

MainWindow::MainWindow(PCWSTR CLASS_NAME) :
    BaseWindow<MainWindow>(CLASS_NAME), initializer(), selected(NULL)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::selectionChanged()
{
    if (selected->isSuspended())
    {
        SetWindowText(sleepLabel, TEXT("State: suspended"));
        EnableWindow(suspendButton, FALSE);
        EnableWindow(resumeButton, TRUE);
    }
    else
    {
        SetWindowText(sleepLabel, TEXT("State: active"));
        EnableWindow(suspendButton, TRUE);
        EnableWindow(resumeButton, FALSE);
    }

    InvalidateRect(m_hwnd, NULL, FALSE);
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        CreateLayout();
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(m_hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
        SetLayout();
        return 0;

    case WM_SIZING:
    {
        RECT* dragRc = (RECT*)lParam;

        RECT rcWindow;
        GetWindowRect(m_hwnd, &rcWindow);

        int WINDOW_HEIGHT_PIX = rcWindow.bottom - rcWindow.top;
        int WINDOW_WIDTH_PIX = rcWindow.right - rcWindow.left;

        switch (wParam)
        {
        case WMSZ_BOTTOM:
            dragRc->bottom = dragRc->top + WINDOW_HEIGHT_PIX;
            break;

        case WMSZ_BOTTOMLEFT:
            dragRc->bottom = dragRc->top + WINDOW_HEIGHT_PIX;
            dragRc->left = dragRc->right - WINDOW_WIDTH_PIX;
            break;

        case WMSZ_LEFT:
            dragRc->left = dragRc->right - WINDOW_WIDTH_PIX;
            break;

        case WMSZ_TOPLEFT:
            dragRc->left = dragRc->right - WINDOW_WIDTH_PIX;
            dragRc->top = dragRc->bottom - WINDOW_HEIGHT_PIX;
            break;

        case WMSZ_TOP:
            dragRc->top = dragRc->bottom - WINDOW_HEIGHT_PIX;
            break;

        case WMSZ_TOPRIGHT:
            dragRc->top = dragRc->bottom - WINDOW_HEIGHT_PIX;
            dragRc->right = dragRc->left + WINDOW_WIDTH_PIX;
            break;

        case WMSZ_RIGHT:
            dragRc->right = dragRc->left + WINDOW_WIDTH_PIX;
            break;


        case WMSZ_BOTTOMRIGHT:
            dragRc->right = dragRc->left + WINDOW_WIDTH_PIX;
            dragRc->bottom = dragRc->top + WINDOW_HEIGHT_PIX;
            break;
        }

        return TRUE;
    }

    case WM_NOTIFY:
    {
        NMHDR* notification = (NMHDR*)lParam;
        if (notification->code == TVN_SELCHANGED)
        {
            selected = (BaseItem*)((NMTREEVIEWW*)lParam)->itemNew.lParam;
            selectionChanged();
            return 0;
        }
        break;
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            if ((HWND)lParam == suspendButton)
            {
                selected->suspend();
                initializer->initialize();
            }
            else if ((HWND)lParam == resumeButton)
            {
                selected->resume();
                initializer->initialize();
            }
            else if ((HWND)lParam == terminateButton)
            {
                selected->terminate();
                initializer->initialize();
            }
            else
            {
                initializer->initialize();
            }
            return 0;
        }
        break;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::CreateLayout()
{
    initializer->Create(L"View",
        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES,
        m_hwnd
        );
    suspendButton = CreateWindow(L"BUTTON",
        L"Compress",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    resumeButton = CreateWindow(L"BUTTON",
        L"Decompress",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    terminateButton = CreateWindow(L"BUTTON",
        L"Decompress",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    updateButton = CreateWindow(L"BUTTON",
        L"Decompress",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    sleepLabel = CreateWindow(NULL,
        L"STATIC",
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
}

void MainWindow::SetLayout()
{
    RECT rcClient;
    GetClientRect(m_hwnd, &rcClient);

    int MARGIN_XPix = DPIScale::DipXToPixels<int>(MARGIN_X);
    int MARGIN_YPix = DPIScale::DipYToPixels<int>(MARGIN_Y);
    int WINDOW_HEIGHT_PIX = rcClient.bottom - rcClient.top;
    int WINDOW_WIDTH_PIX = rcClient.right - rcClient.left;

    MoveWindow(initializer->Window(),
        MARGIN_XPix,
        MARGIN_YPix,
        WINDOW_WIDTH_PIX * 3 / 4 - MARGIN_XPix,
        WINDOW_HEIGHT_PIX - MARGIN_YPix,
        FALSE);

    MoveWindow(sleepLabel,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        MARGIN_YPix,
        WINDOW_WIDTH_PIX - MARGIN_XPix,
        WINDOW_HEIGHT_PIX / 8 - MARGIN_YPix,
        FALSE);

    MoveWindow(suspendButton,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 1) / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX - MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 0) / 8 - MARGIN_YPix,
        FALSE);

    MoveWindow(resumeButton,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 2) / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX - MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 1) / 8 - MARGIN_YPix,
        FALSE);

    MoveWindow(terminateButton,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 3) / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX - MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 2) / 8 - MARGIN_YPix,
        FALSE);

    MoveWindow(updateButton,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 4) / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX - MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 3) / 8 - MARGIN_YPix,
        FALSE);

    InvalidateRect(m_hwnd, NULL, FALSE);
}
