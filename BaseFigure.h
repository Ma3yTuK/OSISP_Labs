#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>

#include <gdiplus.h>

using namespace Gdiplus;

class BaseFigure
{
private:
    static const Color DEFAULT_BORDER_COLOR;

public:
    BaseFigure(Color color, Color borderColor = DEFAULT_BORDER_COLOR, Matrix* matrix = new Matrix());

    void Translate(PointF size);
    void Rotate(FLOAT angle, PointF center);
    void Scale(FLOAT size);
    void RevertTransform();
    void SaveTransform();

    void SetColor(Color color) { this->color = color; }
    void SetBorderColor(Color borderColor) { this->borderColor = borderColor; }
    void SetMatrix(Matrix* matrix) { delete this->matrix; this->matrix = matrix; }

    Color GetColor() { return color; }
    Color GetBorderColor() { return borderColor; }
    Matrix* GetMatrix() { return matrix->Clone(); }

    virtual void Draw(Graphics* graphics) = 0;
    virtual PointF* GetCenter() = 0;
    virtual void PlaceIn(RectF rect) = 0;
    virtual BOOL HitTest(PointF hitPoint) = 0;

protected:
    Pen* pen;
    Color color;
    Color borderColor;
    Matrix* matrix;
    Matrix* lastMatrix;
};
