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
    static const int BUFF_SIZE;
    static wchar_t BUFF[];

public:
    MainWindow(PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME);
    ~MainWindow();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    void CreateLayout();
    void SetLayout();

    std::wstring currentChat;

    HWND chat;
    HWND sendButton;
    HWND input;
};

