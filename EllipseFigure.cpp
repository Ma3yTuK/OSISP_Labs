#include "EllipseFigure.h"

const Color EllipseFigure::DEFAULT_BORDER_COLOR = Color(0, 0, 0);

EllipseFigure::EllipseFigure(RectF rect, Color color, Color borderColor, Matrix* matrix) :
    BaseFigure(color, borderColor, matrix), rect(rect)
{
}

void EllipseFigure::Draw(Graphics* graphics)
{
    graphics->SetTransform(matrix);

    pen->SetColor(borderColor);
    graphics->DrawEllipse(pen, rect);
    pen->SetColor(color);
    graphics->FillEllipse(pen->GetBrush(), rect);

    graphics->ResetTransform();
}

PointF* EllipseFigure::GetCenter() 
{ 
    PointF* result = new PointF((rect.GetRight() + rect.GetLeft()) / 2, (rect.GetTop() + rect.GetBottom()) / 2);
    matrix->TransformPoints(result);
    return result;
}

void EllipseFigure::PlaceIn(RectF rect)
{
    matrix->Reset();
    lastMatrix->Reset();
    this->rect = rect;
}

BOOL EllipseFigure::HitTest(PointF hitPoint)
{
    Matrix* invertedMatrix = matrix->Clone();
    invertedMatrix->Invert();
    invertedMatrix->TransformPoints(&hitPoint);

    const float a = rect.Width / 2;
    const float b = rect.Height / 2;
    const float x1 = hitPoint.X - (rect.GetLeft() + rect.GetRight()) / 2;
    const float y1 = hitPoint.Y - (rect.GetTop() + rect.GetBottom()) / 2;
    const float d = ((x1 * x1) / (a * a)) + ((y1 * y1) / (b * b));
    return d <= 1.0f;
}
