#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <compressapi.h>
#include <shlwapi.h>
#include <string>
#include <map>

#include "BaseWindow.h"
#include "DPIScale.h"

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

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    void CreateLayout();
    void SetLayout();

    HWND chat;
    HWND sendButton;
    HWND input;
};

