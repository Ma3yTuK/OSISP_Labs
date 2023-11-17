#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <compressapi.h>
#include <shlwapi.h>
#include <string>
#include <map>

#include "BaseWindow.h"
#include "DPIScale.h"
#include "BaseNode.h"

class MainWindow : public BaseWindow<MainWindow>
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;
    static const float MARGIN_X;
    static const float MARGIN_Y;
    static const int MAX_NAME_SIZE;
    static const int MAX_VALUE_SIZE;
    static const int MAX_TYPE_SIZE;
    static const std::map<std::wstring, DWORD> types;
    static LPCWSTR DEFAULT_TYPE;
    static std::wstring currentName;
    static DWORD currentDwordValue;    
    static std::wstring currentSzValue;
    static std::wstring currentType;
    static bool succeded;

public:
    MainWindow(PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME);
    ~MainWindow();

    void update();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    void CreateLayout();
    void SetLayout();

    static BOOL CALLBACK addDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK changeValueSzDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);
    static BOOL CALLBACK changeValueDwordDialogProc(HWND hwndDlg, UINT message, WPARAM wParam, LPARAM lParam);

    HWND addButton;
    HWND removeButton;
    HWND updateButton;

    HWND typeLabel;
    HWND valueLabel;

    BaseNode* selected;

    HWND tree;
};

