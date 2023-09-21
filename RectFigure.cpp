#include "RectFigure.h"

const D2D1_COLOR_F RectFigure::DEFAULT_BORDER_COLOR = D2D1::ColorF(D2D1::ColorF::Black);

RectFigure::RectFigure(D2D1_RECT_F rect, D2D1_COLOR_F color, D2D1_COLOR_F borderColor, D2D1::Matrix3x2F matrix) :
    BaseFigure(color, borderColor, matrix), rect(rect)
{
}

void RectFigure::Draw(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush)
{
    pRT->SetTransform(matrix);
    pBrush->SetColor(color);
    pRT->FillRectangle(rect, pBrush);
    pBrush->SetColor(borderColor);
    pRT->DrawRectangle(rect, pBrush, 1.0f);
    pRT->SetTransform(D2D1::Matrix3x2F::Identity());
}

void RectFigure::PlaceIn(D2D1_RECT_F rect)
{
    matrix = lastMatrix = D2D1::Matrix3x2F::Identity();
    this->rect = rect;
}

BOOL RectFigure::HitTest(D2D1_POINT_2F hitPoint)
{
    D2D1::Matrix3x2F invertedMatrix = matrix;
    invertedMatrix.Invert();
    hitPoint = invertedMatrix.TransformPoint(hitPoint);

    if (hitPoint.x > rect.left && hitPoint.x < rect.right && hitPoint.y > rect.top && hitPoint.y < rect.bottom)
        return true;
    return false;
}
