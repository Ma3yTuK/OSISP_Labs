#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>

#include "settings.h"
#include "BaseWindow.h"
#include "DPIScale.h"
#include "resource.h"
#include "GraphicsScene.h"
#include "SceneControl.h"

class MainWindow : public BaseWindow<MainWindow>
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;
    static const Mode DEFAULT_MODE;
    static const Figure DEFAULT_FIGURE;
    static const D2D1_COLOR_F DEFAULT_COLOR;

public:
    MainWindow(Mode mode = DEFAULT_MODE, Figure figure = DEFAULT_FIGURE, D2D1_COLOR_F color = DEFAULT_COLOR, PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME);
    ~MainWindow();

    Mode* GetMode() { return &mode; }
    Figure* GetFigure() { return &figure; }
    D2D1_COLOR_F* GetColor() { return &color; }
    ID2D1Factory* GetFactory() { return pFactory; }
    HWND GetScene() { return graphicsScene->Window(); }

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    void CreateLayout();
    void SetLayout();

    Mode mode;
    Figure figure;
    D2D1_COLOR_F color;
    ID2D1Factory* pFactory;

    SceneControl* sceneControl;
    GraphicsScene* graphicsScene;
};

