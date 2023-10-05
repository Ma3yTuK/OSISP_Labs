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
    static const Color DEFAULT_BORDER_COLOR;
    static const Color DEFAULT_SELECTION_COLOR;
    static const float DEFAULT_FIGURE_SIZE;

public:
    GraphicsScene(Mode* mode = NULL, Figure* figure = NULL, Color* color = NULL, PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME, Color borderColor = DEFAULT_BORDER_COLOR, Color selectionColor=DEFAULT_SELECTION_COLOR);

    std::shared_ptr<BaseFigure> Selection();
    void ClearSelection();
    BOOL Select(PointF hitPoint);

    void InsertFigure(float dipX, float dipY);
    void ColorChanged();

    //void Resize();
    void OnPaint();
    void OnLButtonDown(int pixelX, int pixelY, DWORD flags);
    void OnLButtonUp();
    void OnMouseMove(int pixelX, int pixelY, DWORD flags);
    void OnKeyDown(UINT vkey);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    Color borderColor;
    Color selectionColor;

    Mode* mode;
    Figure* figure;
    Color* color;

    PointF ptMouse;

    std::list<std::shared_ptr<BaseFigure>> figures;
    std::list<std::shared_ptr<BaseFigure>>::iterator selection;

    bool tracking;
    TRACKMOUSEEVENT trackingStruct;
};
