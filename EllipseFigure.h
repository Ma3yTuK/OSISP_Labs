#pragma once

#include "BaseFigure.h"

class EllipseFigure : public BaseFigure
{
private:
    static const Color DEFAULT_BORDER_COLOR;

public:
    EllipseFigure(RectF Rect, Color color, Color borderColor = DEFAULT_BORDER_COLOR, Matrix* matrix = new Matrix());

    RectF GetRect() { return rect; }

    virtual void Draw(Graphics* graphics) override;
    virtual PointF* GetCenter() override;
    virtual void PlaceIn(RectF rect) override;
    virtual BOOL HitTest(PointF hitPoint) override;

protected:
    RectF rect;
};

