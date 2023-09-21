#pragma once

#include "BaseFigure.h"

class RectFigure : public BaseFigure
{
private:
    static const D2D1_COLOR_F DEFAULT_BORDER_COLOR;

public:
    RectFigure(D2D1_RECT_F rect, D2D1_COLOR_F color, D2D1_COLOR_F borderColor = DEFAULT_BORDER_COLOR, D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F::Identity());

    void SetEllipse(D2D1_RECT_F rect) { this->rect = rect; }

    D2D1_RECT_F GetRect() { return rect; }

    virtual void Draw(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush) override;
    virtual D2D1_POINT_2F GetCenter() override { return matrix.TransformPoint(D2D1::Point2F((rect.right + rect.left) / 2, (rect.top + rect.bottom) / 2)); }
    virtual void PlaceIn(D2D1_RECT_F rect) override;
    virtual BOOL HitTest(D2D1_POINT_2F hitPoint) override;

protected:
    D2D1_RECT_F rect;
};
