#include "SceneControl.h"
#include "helper_functions.h"

const PCWSTR SceneControl::DEFAULT_CLASS_NAME = L"Graphics";
const float SceneControl::MARGIN_X = 6.0F;
const float SceneControl::MARGIN_Y = 6.0F;
const float SceneControl::DEFAULT_PICKER_WIDTH = 200.0F;
const float SceneControl::DEFAULT_WINDOW_HEIGHT = 60.0F;
const float SceneControl::DEFAULT_BUTTON_WIDTH = 70.0F;

const wchar_t* const SceneControl::MODE_NAMES[] = { // must follow the same order as enum
    L"Draw",
    L"Select",
    L"Drag",
    L"Scale",
    L"Rotate"
};
const wchar_t* const SceneControl::FIGURE_NAMES[] = { // must follow the same order as enum
    L"Ellipse",
    L"Rect"
};

const COLORREF SceneControl::BUTTON_COLORS[] = {
    0x00000000,
    0x00FF0000,
    0x0000FF00,
    0x000000FF,
    0x00FFFF00,
    0x0000FFFF,
    0x00FF00FF,
    0x00FFFFFF
};

SceneControl::SceneControl(Mode* mode, Figure* figure, D2D1_COLOR_F* color, PCWSTR CLASS_NAME, float windowHeight, float buttonWidth, float pickerWidth) :
    BaseWindow<SceneControl>(CLASS_NAME), mode(mode), figure(figure), color(color), modePicker(NULL), figurePicker(NULL), windowHeight(windowHeight), buttonWidth(buttonWidth), pickerWidth(pickerWidth), buttons(), brush(NULL), tracking(false), trackingStruct{ sizeof(trackingStruct), NULL, NULL, NULL }
{
}

LRESULT SceneControl::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
    {
        if (!GetParent(m_hwnd))
        {
            DPIScale::Initialize();
        }
        CreateLayout();

        RECT rcWindow;
        GetWindowRect(m_hwnd, &rcWindow);
        MoveWindow(m_hwnd,
            rcWindow.left,
            rcWindow.top,
            rcWindow.right - rcWindow.left,
            GetRealWindowHeight(),
            TRUE);

        trackingStruct.hwndTrack = m_hwnd;

        return 0;
    }

    case WM_SIZE:
        SetLayout();
        return 0;

    case WM_SIZING:
    {
        RECT* dragRc = (RECT*)lParam;

        if (wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOMRIGHT)
        {
            dragRc->bottom = dragRc->top + GetRealWindowHeight();
        }
        else if (wParam == WMSZ_TOP || wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOPRIGHT)
        {
            dragRc->top = dragRc->bottom - GetRealWindowHeight();
        }
        return TRUE;
    }

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(m_hwnd, &ps);
        return 0;
    }

    case WM_MOUSEMOVE:
        if (!tracking)
        {
            trackingStruct.dwFlags = TME_HOVER | TME_LEAVE;
            TrackMouseEvent(&trackingStruct);
            tracking = true;
        }
        return 0;

    case WM_MOUSEHOVER:
        SetFocus(m_hwnd);
        trackingStruct.dwFlags = TME_LEAVE;
        TrackMouseEvent(&trackingStruct);
        return 0;

    case WM_MOUSELEAVE:
        tracking = false;
        return 0;

    case WM_COMMAND:
        if (HIWORD(wParam) == CBN_SELCHANGE)
        {
            HWND parentWND = GetParent(m_hwnd);
            if (!parentWND)
            {
                parentWND = m_hwnd;
            }

            int ItemIndex = SendMessage((HWND)lParam, (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);

            if ((HWND)lParam == modePicker)
            {
                *mode = (Mode)ItemIndex;
                PostMessage(parentWND, WM_MODE_CHANGED, NULL, NULL);
                return 0;
            }
            else if ((HWND)lParam == figurePicker)
            {
                *figure = (Figure)ItemIndex;
                PostMessage(parentWND, WM_FIGURE_CHANGED, NULL, NULL);
                return 0;
            }
        }
        else if (HIWORD(wParam) == BN_CLICKED)
        {
            HWND parentWND = GetParent(m_hwnd);
            if (!parentWND)
            {
                parentWND = m_hwnd;
            }
            COLORREF colorRef = buttons[(HWND)lParam];
            *color = D2D1::ColorF(GetRValue(colorRef), GetGValue(colorRef), GetBValue(colorRef));
            PostMessage(parentWND, WM_COLOR_CHANGED, NULL, NULL);

            return 0;
        }
        break;

    case WM_CTLCOLORBTN:
    {
        DeleteObject(brush);
        brush = CreateSolidBrush(buttons[(HWND)lParam]);
        return (LRESULT)brush;
    }

    case WM_MODE_CHANGED:
        SendMessage(modePicker, CB_SETCURSEL, (WPARAM)*mode, 0);
        return 0;

    case WM_FIGURE_CHANGED:
        SendMessage(figurePicker, CB_SETCURSEL, (WPARAM)*figure, 0);
        return 0;

    case WM_COLOR_CHANGED:
        return 0;
    }

    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

int SceneControl::GetRealWindowHeight()
{
    RECT adjustedWindowHeightRect = { 0, 0, 0, DPIScale::DipYToPixels<int>(windowHeight) };
    AdjustWindowRect(&adjustedWindowHeightRect, GetWindowStyle(m_hwnd), (BOOL)GetMenu(m_hwnd));
    return adjustedWindowHeightRect.bottom - adjustedWindowHeightRect.top;
}

void SceneControl::CreateLayout()
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

void SceneControl::SetLayout()
{
    RECT rcClient;
    GetClientRect(m_hwnd, &rcClient);

    int MARGIN_XPix = DPIScale::DipXToPixels<int>(MARGIN_X);
    int MARGIN_YPix = DPIScale::DipYToPixels<int>(MARGIN_Y);
    int pickerWidthPix = DPIScale::DipXToPixels<int>(pickerWidth);

    MoveWindow(modePicker,
        MARGIN_XPix,
        MARGIN_YPix,
        pickerWidthPix,
        1000,
        FALSE);

    RECT rcMode;
    GetClientRect(modePicker, &rcMode);

    MoveWindow(figurePicker,
        MARGIN_XPix,
        rcClient.bottom - MARGIN_YPix - rcMode.bottom,
        pickerWidthPix,
        1000,
        FALSE);

    int i = 0;
    for (auto& it : buttons)
    {
        MoveWindow(it.first,
            rcClient.right - (MARGIN_XPix + buttonWidth) * (1 + i / 2),
            MARGIN_YPix * (1 - i % 2) + (rcClient.bottom - MARGIN_YPix - rcMode.bottom) * (i % 2),
            buttonWidth,
            rcMode.bottom,
            FALSE);
        ++i;
    }

    InvalidateRect(m_hwnd, NULL, FALSE);
}
