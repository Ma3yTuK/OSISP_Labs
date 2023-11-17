#include "MainWindow.h"
#include "KeyNode.h"
#include "ValueNode.h"
#include "resource.h"

#include <stdexcept>


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
    BaseWindow<MainWindow>(CLASS_NAME), selected(NULL)
{
}

MainWindow::~MainWindow()
{
}

void MainWindow::update()
{
    if (!selected)
    {
        SetWindowText(typeLabel, NULL);
        SetWindowText(valueLabel, NULL);
        EnableWindow(removeButton, FALSE);
        EnableWindow(updateButton, FALSE);
        EnableWindow(addButton, FALSE);
        return;
    }

    ValueNode* value = dynamic_cast<ValueNode*>(selected);

    if (value)
    {
        EnableWindow(removeButton, TRUE);
        EnableWindow(addButton, FALSE);

        DWORD type;
        DWORD valueMaxSize = 16384;
        BYTE* val = new BYTE[valueMaxSize];

        value->get(&type, val, &valueMaxSize);

        switch (type)
        {
        case REG_DWORD:
        {
            SetWindowText(typeLabel, L"Type: REG_DWORD");
            std::wstring tmp = std::to_wstring(*(DWORD*)val);
            SetWindowText(valueLabel, tmp.c_str());
            EnableWindow(updateButton, TRUE);
            break;
        }
        case REG_SZ:
            SetWindowText(typeLabel, L"Type: REG_SZ");
            SetWindowText(valueLabel, (LPWSTR)val);
            EnableWindow(updateButton, TRUE);
            break;
        default:
            SetWindowText(typeLabel, L"Type: unknown");
            SetWindowText(valueLabel, NULL);
            EnableWindow(updateButton, FALSE);
            break;
        }

        delete[] val;   
    }
    else
    {
        SetWindowText(typeLabel, L"Type: key");
        SetWindowText(valueLabel, NULL);
        EnableWindow(removeButton, TRUE);
        EnableWindow(updateButton, FALSE);
        EnableWindow(addButton, TRUE);
    }
}

//static HBRUSH hBrush = CreateSolidBrush(RGB(230, 2, 2));

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

    case WM_NOTIFY:
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
        break;

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
    tree = CreateWindow(WC_TREEVIEW,
        L"Tree view",
        WS_VISIBLE | WS_CHILD | WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_SHOWSELALWAYS,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    addButton = CreateWindow(L"BUTTON",
        L"Add",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    removeButton = CreateWindow(L"BUTTON",
        L"Remove",
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
        L"Edit value",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    typeLabel = CreateWindow(L"STATIC",
        NULL,
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    valueLabel = CreateWindow(L"STATIC",
        NULL,
        WS_VISIBLE | WS_CHILD | SS_LEFT,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);

    for (auto& i : KeyNode::PREDEFINED_NAMES)
    {
        KeyNode* key = new KeyNode(tree, i.first.c_str(), NULL);
        key->initialize(1);
    }

    update();
}

void MainWindow::SetLayout()
{
    RECT rcClient;
    GetClientRect(m_hwnd, &rcClient);

    int MARGIN_XPix = DPIScale::DipXToPixels<int>(MARGIN_X);
    int MARGIN_YPix = DPIScale::DipYToPixels<int>(MARGIN_Y);
    int WINDOW_HEIGHT_PIX = rcClient.bottom - rcClient.top;
    int WINDOW_WIDTH_PIX = rcClient.right - rcClient.left;

    MoveWindow(tree,
        MARGIN_XPix,
        MARGIN_YPix,
        WINDOW_WIDTH_PIX * 3 / 4 - MARGIN_XPix,
        WINDOW_HEIGHT_PIX - MARGIN_YPix,
        FALSE);

    MoveWindow(typeLabel,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        MARGIN_YPix,
        WINDOW_WIDTH_PIX / 4 - MARGIN_XPix * 2,
        WINDOW_HEIGHT_PIX / 8 - MARGIN_YPix * 2,
        FALSE);

    MoveWindow(valueLabel,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX / 4 - MARGIN_XPix * 2,
        WINDOW_HEIGHT_PIX / 8 - MARGIN_YPix * 2,
        FALSE);

    MoveWindow(addButton,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 1) / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX / 4 - MARGIN_XPix * 2,
        WINDOW_HEIGHT_PIX / 8 - MARGIN_YPix * 2,
        FALSE);

    MoveWindow(updateButton,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 2) / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX / 4 - MARGIN_XPix * 2,
        WINDOW_HEIGHT_PIX / 8 - MARGIN_YPix * 2,
        FALSE);

    MoveWindow(removeButton,
        WINDOW_WIDTH_PIX * 3 / 4 + MARGIN_XPix,
        WINDOW_HEIGHT_PIX * (8 - 3) / 8 + MARGIN_YPix,
        WINDOW_WIDTH_PIX / 4 - MARGIN_XPix * 2,
        WINDOW_HEIGHT_PIX / 8 - MARGIN_YPix * 2,
        FALSE);

    InvalidateRect(m_hwnd, NULL, FALSE);
}

BOOL CALLBACK MainWindow::addDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
    {
        SetDlgItemTextW(hwndDlg, IDC_ITEM_NAME, currentName.c_str());
        HWND comboBox = GetDlgItem(hwndDlg, IDC_TYPE);
        for (auto& i : types)
            ComboBox_AddString(comboBox, i.first.c_str());
        ComboBox_AddString(comboBox, DEFAULT_TYPE);
        SetDlgItemTextW(hwndDlg, IDC_TYPE, currentType.c_str());
    }

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            LPWSTR name = new WCHAR[MAX_NAME_SIZE];

            if (!GetDlgItemTextW(hwndDlg, IDC_ITEM_NAME, name, MAX_NAME_SIZE))
                *name = 0;

            currentName = name;

            delete[] name;

            LPWSTR type = new WCHAR[MAX_TYPE_SIZE];

            if (!GetDlgItemTextW(hwndDlg, IDC_TYPE, type, MAX_TYPE_SIZE))
                *type = 0;

            currentType = type;

            delete[] type;

            succeded = true;
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }

        case IDCANCEL:
        {
            succeded = false;
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
        }
    }
    return FALSE;
}

BOOL CALLBACK MainWindow::changeValueSzDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemTextW(hwndDlg, IDC_VALUE_SZ, currentSzValue.c_str());

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            LPWSTR value = new WCHAR[MAX_VALUE_SIZE];

            if (!GetDlgItemTextW(hwndDlg, IDC_VALUE_SZ, value, MAX_VALUE_SIZE))
                *value = 0;

            currentSzValue = value;

            delete[] value;

            succeded = true;
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }

        case IDCANCEL:
        {
            succeded = false;
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
        }
    }
    return FALSE;
}

BOOL CALLBACK MainWindow::changeValueDwordDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG:
        SetDlgItemInt(hwndDlg, IDC_VALUE_DWORD, currentDwordValue, TRUE);

    case WM_COMMAND:
        switch (LOWORD(wParam))
        {
        case IDOK:
        {
            BOOL success;
            currentDwordValue = GetDlgItemInt(hwndDlg, IDC_VALUE_DWORD, &success, TRUE);

            if (!success)
                currentDwordValue = 0;

            succeded = true;
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }

        case IDCANCEL:
        {
            succeded = false;
            EndDialog(hwndDlg, wParam);
            return TRUE;
        }
        }
    }
    return FALSE;
}