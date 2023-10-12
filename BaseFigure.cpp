#include "BaseFigure.h"

const Color BaseFigure::DEFAULT_BORDER_COLOR = Color(0, 0, 0);

BaseFigure::BaseFigure(Color color, Color borderColor, Matrix* matrix) :
    color(color), borderColor(borderColor), matrix(matrix), lastMatrix(NULL), pen(new Pen(borderColor, 3.0F))
{
    SaveTransform();
}

void BaseFigure::Translate(PointF size)
{
    delete lastMatrix;
    lastMatrix = matrix->Clone();
    matrix->Translate(size.X, size.Y, MatrixOrderAppend);
}

void BaseFigure::Rotate(FLOAT angle, PointF center)
{
    delete lastMatrix;
    lastMatrix = matrix->Clone();
    matrix->RotateAt((REAL)angle, center, MatrixOrderAppend);
}

void BaseFigure::Scale(PointF size)
{
    delete lastMatrix;
    lastMatrix = matrix->Clone();
    PointF* center = GetCenter();
    matrix->Translate(-center->X, -center->Y, MatrixOrderAppend);
    matrix->Scale(size.X, size.Y, MatrixOrderAppend);
    matrix->Translate(center->X, center->Y, MatrixOrderAppend);
}

void BaseFigure::RevertTransform()
{
    delete matrix;
    matrix = lastMatrix->Clone();
}

void BaseFigure::SaveTransform()
{
    delete lastMatrix;
    lastMatrix = matrix->Clone();
}
