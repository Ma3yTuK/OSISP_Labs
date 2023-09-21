#pragma once

#include "BaseFigure.h"

class EllipseFigure : public BaseFigure
{
private:
    static const D2D1_COLOR_F DEFAULT_BORDER_COLOR;

public:
    EllipseFigure(D2D1_ELLIPSE ellipse, D2D1_COLOR_F color, D2D1_COLOR_F borderColor = DEFAULT_BORDER_COLOR, D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F::Identity());

    void SetEllipse(D2D1_ELLIPSE ellipse) { this->ellipse = ellipse; }

    D2D1_ELLIPSE GetEllipse() { return ellipse; }

    virtual void Draw(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush) override;
    virtual D2D1_POINT_2F GetCenter() override { return matrix.TransformPoint(ellipse.point); }
    virtual void PlaceIn(D2D1_RECT_F rect) override;
    virtual BOOL HitTest(D2D1_POINT_2F hitPoint) override;

protected:
    D2D1_ELLIPSE ellipse;
};

