#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>

class BaseFigure
{
private:
    static const D2D1_COLOR_F DEFAULT_BORDER_COLOR;

public:
    BaseFigure(D2D1_COLOR_F color, D2D1_COLOR_F borderColor = DEFAULT_BORDER_COLOR, D2D1::Matrix3x2F matrix = D2D1::Matrix3x2F::Identity());

    void Translate(D2D1_SIZE_F size);
    void Rotate(FLOAT angle, D2D1_POINT_2F center);
    void Scale(D2D1_SIZE_F size, D2D1_POINT_2F center);
    void RevertTransform();
    void SaveTransform();

    void SetColor(D2D1_COLOR_F color) { this->color = color; }
    void SetBorderColor(D2D1_COLOR_F borderColor) { this->borderColor = borderColor; }
    void SetMatrix(D2D1::Matrix3x2F matrix) { this->matrix = matrix; }

    D2D1_COLOR_F GetColor() { return color; }
    D2D1_COLOR_F GetBorderColor() { return borderColor; }
    D2D1::Matrix3x2F GetMatrix() { return matrix; }

    virtual void Draw(ID2D1RenderTarget* pRT, ID2D1SolidColorBrush* pBrush) = 0;
    virtual D2D1_POINT_2F GetCenter() = 0;
    virtual void PlaceIn(D2D1_RECT_F rect) = 0;
    virtual BOOL HitTest(D2D1_POINT_2F hitPoint) = 0;

protected:
    D2D1_COLOR_F    color;
    D2D1_COLOR_F    borderColor;
    D2D1::Matrix3x2F matrix;
    D2D1::Matrix3x2F lastMatrix;
};
