#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <compressapi.h>
#include <shlwapi.h>
#include <string>

#include "BaseWindow.h"
#include "DPIScale.h"

class MainWindow : public BaseWindow<MainWindow>
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;
    static const float MARGIN_X;
    static const float MARGIN_Y;

public:
    MainWindow(PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME);
    ~MainWindow();

    void update();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    void CreateLayout();
    void SetLayout();

    Initializer initializer;
    HWND suspendButton;
    HWND resumeButton;
    HWND terminateButton;
    HWND updateButton;

    HWND sleepLabel;

    HWND tree;

    BaseItem* selected;
};

