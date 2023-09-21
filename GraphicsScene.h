#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>

#include <memory>
#include <list>

#include "settings.h"
#include "BaseFigure.h"
#include "EllipseFigure.h"
#include "RectFigure.h"
#include "BaseWindow.h"
#include "DPIScale.h"
#include "resource.h"

class GraphicsScene : public BaseWindow<GraphicsScene>
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;
    static const D2D1_COLOR_F DEFAULT_BORDER_COLOR;
    static const D2D1_COLOR_F DEFAULT_SELECTION_COLOR;
    static const float DEFAULT_FIGURE_SIZE;

public:
    GraphicsScene(Mode* mode = NULL, Figure* figure = NULL, D2D1_COLOR_F* color = NULL, ID2D1Factory* pFactory = NULL,
        PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME, D2D1_COLOR_F borderColor = DEFAULT_BORDER_COLOR, D2D1_COLOR_F selectionColor=DEFAULT_SELECTION_COLOR);

    std::shared_ptr<BaseFigure> Selection();
    void ClearSelection();
    BOOL Select(D2D1_POINT_2F hitPoint);

    HRESULT CreateGraphicsResources();
    void DiscardGraphicsResources();

    void InsertFigure(float dipX, float dipY);
    void ColorChanged();

    void Resize();
    void OnPaint();
    void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
    void OnLButtonUp();
    void OnMouseMove(int pixelX, int pixelY, DWORD flags);
    void OnKeyDown(UINT vkey);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    D2D1_COLOR_F borderColor;
    D2D1_COLOR_F selectionColor;

    ID2D1Factory* pFactory;
    Mode* mode;
    Figure* figure;
    D2D1_COLOR_F* color;

    ID2D1HwndRenderTarget* pRenderTarget;
    ID2D1SolidColorBrush* pBrush;
    D2D1_POINT_2F ptMouse;

    std::list<std::shared_ptr<BaseFigure>> figures;
    std::list<std::shared_ptr<BaseFigure>>::iterator selection;

    bool tracking;
    TRACKMOUSEEVENT trackingStruct;
};
