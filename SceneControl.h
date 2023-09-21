#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <CommCtrl.h>

#include <unordered_map>

#include "settings.h"
#include "BaseWindow.h"
#include "DPIScale.h"
#include "resource.h"

class SceneControl : public BaseWindow<SceneControl>
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;
    static const float MARGIN_X;
    static const float MARGIN_Y;
    static const float DEFAULT_PICKER_WIDTH;
    static const float DEFAULT_WINDOW_HEIGHT;
    static const float DEFAULT_BUTTON_WIDTH;
    static const wchar_t* const MODE_NAMES[];
    static const wchar_t* const FIGURE_NAMES[];
    static const COLORREF BUTTON_COLORS[];

public:
    SceneControl(Mode* mode = NULL, Figure* figure = NULL, D2D1_COLOR_F* color = NULL, PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME, float windowHeight = DEFAULT_WINDOW_HEIGHT, float buttonWidth = DEFAULT_BUTTON_WIDTH, float pickerWidth = DEFAULT_PICKER_WIDTH);
    ~SceneControl() { DeleteObject(brush); }

    HRESULT CreateGraphicsResources();
    void DiscardGraphicsResources();

    float GetWindowHeight() { return windowHeight; }
    int GetRealWindowHeight();

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    void CreateLayout();
    void SetLayout();

    Mode* mode;
    Figure* figure;
    D2D1_COLOR_F* color;

    float windowHeight;
    float buttonWidth;
    float pickerWidth;

    std::unordered_map<HWND, COLORREF> buttons;

    HBRUSH brush;

    HWND modePicker;
    HWND figurePicker;

    bool tracking;
    TRACKMOUSEEVENT trackingStruct;
};
