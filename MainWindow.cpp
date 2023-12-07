#include "MainWindow.h"

#include <stdexcept>
#include <CommCtrl.h>


const PCWSTR MainWindow::DEFAULT_CLASS_NAME = L"Graphics";
const float MainWindow::MARGIN_X = 6.0F;
const float MainWindow::MARGIN_Y = 6.0F;
const int MainWindow::MAX_NAME_SIZE = 256;
const int MainWindow::MAX_TYPE_SIZE = 16;
const int MainWindow::MAX_VALUE_SIZE = 16384;
LPCWSTR MainWindow::DEFAULT_TYPE = L"key";

std::wstring MainWindow::currentName = std::wstring();
DWORD MainWindow::currentDwordValue = NULL;
std::wstring MainWindow::currentSzValue = std::wstring();
std::wstring MainWindow::currentType = std::wstring();
bool MainWindow::succeded = false;

const std::map<std::wstring, DWORD> MainWindow::types = {
    { L"REG_SZ", REG_SZ },
    { L"REG_DWORD", REG_DWORD }
};

MainWindow::MainWindow(PCWSTR CLASS_NAME) :
    BaseWindow<MainWindow>(CLASS_NAME)
{
}

MainWindow::~MainWindow()
{
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

    case WM_INITDIALOG:
        InitCommonControls();

    case WM_SIZE:
        SetLayout();
        return 0;

    /*case WM_SIZING:
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
    }*/

    /*case WM_NOTIFY:
    {
        NMHDR* notification = (NMHDR*)lParam;
        switch (((NMHDR*)lParam)->code)
        {
        case TVN_SELCHANGED:
            selected = (BaseNode*)((LPNMTREEVIEW)lParam)->itemNew.lParam;
            update();
            return 0;

        case TVN_ITEMEXPANDED:
        {
            NMTREEVIEW* item = (NMTREEVIEW*)(lParam);
            KeyNode* key = (KeyNode*)(item)->itemNew.lParam;
            if (KeyNode::PREDEFINED_NAMES.find(key->getName()) == KeyNode::PREDEFINED_NAMES.end())
                key->initialize(1);
            break;
        }
        }

        break;
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            if ((HWND)lParam == addButton)
            {
                currentName.clear();
                currentType = DEFAULT_TYPE;

                DialogBoxW(NULL, MAKEINTRESOURCEW(IDD_ADD_DIALOG), m_hwnd, (DLGPROC)addDialogProc);
                if (succeded)
                {
                    if (currentType == DEFAULT_TYPE)
                    {
                        try
                        {
                            KeyNode* key = new KeyNode(tree, currentName.c_str(), (KeyNode*)selected);
                        }
                        catch (std::invalid_argument)
                        {
                        }
                    }
                    else
                    {
                        try
                        {
                            ValueNode* value = new ValueNode(tree, currentName.c_str(), (KeyNode*)selected);
                            value->set(types.at(currentType), NULL, NULL);
                        }
                        catch (std::invalid_argument)
                        {
                        }
                    }
                    ((KeyNode*)selected)->initialize(1);
                }

                update();
            }
            else if ((HWND)lParam == removeButton)
            {
                selected->remove();
                delete selected;

                selected = nullptr;

                update();
            }
            else if ((HWND)lParam == updateButton)
            {
                DWORD type; 
                ((ValueNode*)selected)->get(&type, NULL, NULL);

                if (type == REG_SZ)
                {
                    DialogBoxW(NULL, MAKEINTRESOURCEW(IDD_CHANGE_SZ_DIALOG), m_hwnd, (DLGPROC)changeValueSzDialogProc);
                    if (succeded)
                    {
                        ((ValueNode*)selected)->set(type, (BYTE*)currentSzValue.c_str(), currentSzValue.size() + 1);
                    }
                }
                else if (type == REG_DWORD)
                {
                    DialogBoxW(NULL, MAKEINTRESOURCEW(IDD_CHANGE_DWORD_DIALOG), m_hwnd, (DLGPROC)changeValueDwordDialogProc);
                    if (succeded)
                    {
                        ((ValueNode*)selected)->set(type, (BYTE*)&currentDwordValue, sizeof(DWORD));
                    }
                }

                update();
            }

            return 0;
        }
        break;*/

    /*case WM_CTLCOLORSTATIC:
    {
        HDC hdcStatic = (HDC)wParam;
        SetTextColor(hdcStatic, RGB(0, 0, 0));
        SetBkColor(hdcStatic, RGB(230, 2, 2));
        return (INT_PTR)hBrush;
    }*/
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::CreateLayout()
{
    input = CreateWindow(L"EDIT",
        NULL,
        WS_VISIBLE | WS_CHILD | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    sendButton = CreateWindow(L"BUTTON",
        L"Send",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    chat = CreateWindow(L"STATIC",
        NULL,
        WS_VISIBLE | WS_CHILD | SS_LEFT | WS_BORDER,
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

    MoveWindow(chat,
        MARGIN_XPix,
        MARGIN_YPix,
        WINDOW_WIDTH_PIX - 2 * MARGIN_XPix,
        WINDOW_HEIGHT_PIX * 3 / 4 - MARGIN_YPix,
        FALSE);

    MoveWindow(input,
        MARGIN_XPix,
        MARGIN_YPix + WINDOW_HEIGHT_PIX * 3 / 4,
        WINDOW_WIDTH_PIX * 3 / 4 - MARGIN_XPix,
        WINDOW_HEIGHT_PIX * 1 / 4 - 2 * MARGIN_YPix,
        FALSE);

    MoveWindow(sendButton,
        MARGIN_XPix + WINDOW_WIDTH_PIX * 3 / 4,
        MARGIN_YPix + WINDOW_HEIGHT_PIX * 3 / 4,
        WINDOW_WIDTH_PIX * 1 / 4 - 2 * MARGIN_XPix,
        WINDOW_HEIGHT_PIX * 1 / 4 - 2 * MARGIN_YPix,
        FALSE);

    InvalidateRect(m_hwnd, NULL, FALSE);
}