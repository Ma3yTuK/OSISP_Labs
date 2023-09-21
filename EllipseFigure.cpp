#include "EllipseFigure.h"

const D2D1_COLOR_F EllipseFigure::DEFAULT_BORDER_COLOR = D2D1::ColorF(D2D1::ColorF::Black);

EllipseFigure::EllipseFigure(D2D1_ELLIPSE ellipse, D2D1_COLOR_F color, D2D1_COLOR_F borderColor, D2D1::Matrix3x2F matrix) :
    BaseFigure(color, borderColor, matrix), ellipse(ellipse)
{
}

void EllipseFigure::Draw(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush)
{
    pRT->SetTransform(matrix);
    pBrush->SetColor(color);
    pRT->FillEllipse(ellipse, pBrush);
    pBrush->SetColor(borderColor);
    pRT->DrawEllipse(ellipse, pBrush, 1.0f);
    pRT->SetTransform(D2D1::Matrix3x2F::Identity());
}

void EllipseFigure::PlaceIn(D2D1_RECT_F rect)
{
    matrix = lastMatrix = D2D1::Matrix3x2F::Identity();
    D2D1_POINT_2F center = D2D1::Point2F((rect.right + rect.left) / 2, (rect.bottom + rect.top) / 2);
    FLOAT radiusX = (rect.right - rect.left) / 2;
    FLOAT radiusY = (rect.bottom - rect.top) / 2;
    ellipse = D2D1::Ellipse(center, radiusX, radiusY);
}

BOOL EllipseFigure::HitTest(D2D1_POINT_2F hitPoint)
{
    D2D1::Matrix3x2F invertedMatrix = matrix;
    invertedMatrix.Invert();
    hitPoint = invertedMatrix.TransformPoint(hitPoint);

    const float a = ellipse.radiusX;
    const float b = ellipse.radiusY;
    const float x1 = hitPoint.x - ellipse.point.x;
    const float y1 = hitPoint.y - ellipse.point.y;
    const float d = ((x1 * x1) / (a * a)) + ((y1 * y1) / (b * b));
    return d <= 1.0f;
}
