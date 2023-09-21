#include "BaseFigure.h"

const D2D1_COLOR_F BaseFigure::DEFAULT_BORDER_COLOR = D2D1::ColorF(D2D1::ColorF::Black);

BaseFigure::BaseFigure(D2D1_COLOR_F color, D2D1_COLOR_F borderColor, D2D1::Matrix3x2F matrix) :
    color(color), borderColor(borderColor), matrix(matrix)
{
    SaveTransform();
}

void BaseFigure::Translate(D2D1_SIZE_F size)
{
    lastMatrix = matrix;
    matrix = lastMatrix * D2D1::Matrix3x2F::Translation(size);
}

void BaseFigure::Rotate(FLOAT angle, D2D1_POINT_2F center)
{
    matrix = lastMatrix = matrix;
    matrix = lastMatrix * D2D1::Matrix3x2F::Rotation(angle, center);
}

void BaseFigure::Scale(D2D1_SIZE_F size, D2D1_POINT_2F center)
{
    lastMatrix = matrix;
    matrix = lastMatrix * D2D1::Matrix3x2F::Scale(size, center);
}

void BaseFigure::RevertTransform()
{
    matrix = lastMatrix;
}

void BaseFigure::SaveTransform()
{
    lastMatrix = matrix;
}
