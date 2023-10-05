#include "RectFigure.h"

const Color RectFigure::DEFAULT_BORDER_COLOR = Color(0, 0, 0);

RectFigure::RectFigure(RectF rect, Color color, Color borderColor, Matrix* matrix) :
    BaseFigure(color, borderColor, matrix), rect(rect)
{
}

void RectFigure::Draw(Graphics* graphics)
{
    graphics->SetTransform(matrix);

    pen->SetColor(borderColor);
    graphics->DrawRectangle(pen, rect);
    pen->SetColor(color);
    graphics->FillRectangle(pen->GetBrush(), rect);

    graphics->ResetTransform();
}

PointF* RectFigure::GetCenter()
{
    PointF* result = new PointF((rect.GetRight() + rect.GetLeft()) / 2, (rect.GetTop() + rect.GetBottom()) / 2);
    matrix->TransformPoints(result);
    return result;
}

void RectFigure::PlaceIn(RectF rect)
{
    matrix->Reset();
    lastMatrix->Reset();
    this->rect = rect;
}

BOOL RectFigure::HitTest(PointF hitPoint)
{
    Matrix* invertedMatrix = matrix->Clone();
    invertedMatrix->Invert();
    invertedMatrix->TransformPoints(&hitPoint);

    if (hitPoint.X > rect.GetLeft() && hitPoint.X < rect.GetRight() && hitPoint.Y > rect.GetTop() && hitPoint.Y < rect.GetBottom())
        return true;
    return false;
}
