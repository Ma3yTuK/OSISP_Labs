#include "GraphicsScene.h"
#include "helper_functions.h"

const PCWSTR GraphicsScene::DEFAULT_CLASS_NAME = L"Graphics";
const Color GraphicsScene::DEFAULT_BORDER_COLOR = Color(0, 0, 0);
const Color GraphicsScene::DEFAULT_SELECTION_COLOR = Color(255, 0, 0);
const float GraphicsScene::DEFAULT_FIGURE_SIZE = 2.0F;

GraphicsScene::GraphicsScene(Mode* mode, Figure* figure, Color* color, PCWSTR CLASS_NAME, Color borderColor, Color selectionColor) :
    BaseWindow<GraphicsScene>(CLASS_NAME), mode(mode), figure(figure), color(color),
    borderColor(borderColor), selectionColor(selectionColor), ptMouse(), selection(figures.end()),
    tracking(false), trackingStruct{ sizeof(trackingStruct), NULL, NULL, NULL }
{
}

std::shared_ptr<BaseFigure> GraphicsScene::Selection()
{
    if (selection == figures.end())
    {
        return nullptr;
    }
    else
    {
        return (*selection);
    }
}

void GraphicsScene::ClearSelection()
{
    if (Selection())
    {
        Selection()->SetBorderColor(DEFAULT_BORDER_COLOR);
    }
    selection = figures.end();
}

BOOL GraphicsScene::Select(PointF hitPoint)
{
    ClearSelection();
    for (auto i = figures.rbegin(); i != figures.rend(); ++i)
    {
        if ((*i)->HitTest(hitPoint))
        {
            auto tmp = *i;
            *i = figures.back();
            figures.back() = tmp;
            selection = --figures.end();
            Selection()->SetBorderColor(DEFAULT_SELECTION_COLOR);
            return TRUE;
        }
    }
    return FALSE;
}

void GraphicsScene::InsertFigure(float dipX, float dipY)
{
    ClearSelection();
    switch (*figure)
    {
    case Figure::Ellipse:
    {
        ptMouse = PointF(dipX, dipY);
        RectF rect = RectF(dipX, dipY, DEFAULT_FIGURE_SIZE, DEFAULT_FIGURE_SIZE);
        selection = figures.insert(
            figures.end(),
            std::shared_ptr<BaseFigure>(new EllipseFigure(rect, *color, DEFAULT_SELECTION_COLOR)));
        break;
    }
    case Figure::Rect:
    {
        ptMouse = PointF(dipX, dipY);
        RectF rect = RectF(dipX, dipY, DEFAULT_FIGURE_SIZE, DEFAULT_FIGURE_SIZE);
        selection = figures.insert(
            figures.end(),
            std::shared_ptr<BaseFigure>(new RectFigure(rect, *color, DEFAULT_SELECTION_COLOR)));
    }
    }
}

void GraphicsScene::ColorChanged()
{
    if ((*mode == Mode::SelectMode) && Selection())
        Selection()->SetColor(*color);
    InvalidateRect(m_hwnd, NULL, FALSE);
}

/*void GraphicsScene::Resize()
{
    if (pRenderTarget != NULL)
    {
        RECT rc;
        GetClientRect(m_hwnd, &rc);

        D2D1_SIZE_U size = D2D1::SizeU(rc.right, rc.bottom);

        pRenderTarget->Resize(size);

        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}*/

void GraphicsScene::OnPaint()
{
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);
    POINT lpSize = { ps.rcPaint.right - ps.rcPaint.left, ps.rcPaint.bottom - ps.rcPaint.top };
    POINT pSize = lpSize;
    LPtoDP(hdc, &pSize, 1);
    HDC hdcBits = CreateCompatibleDC(hdc);
    HBITMAP bitMap = CreateCompatibleBitmap(hdc, pSize.x, pSize.y);
    SelectObject(hdcBits, bitMap);
    Graphics graphics(hdcBits);
    graphics.Clear(Color(200, 200, 200));
    for (auto i = figures.begin(); i != figures.end(); ++i)
    {
        (*i)->Draw(&graphics);
    }
    BitBlt(hdc, ps.rcPaint.left, ps.rcPaint.top, lpSize.x, lpSize.y, hdcBits, 0, 0, SRCCOPY);
    EndPaint(m_hwnd, &ps);
    DeleteDC(hdc);
    DeleteDC(hdcBits);
    DeleteObject(bitMap);
    /*HRESULT hr = CreateGraphicsResources();
    if (SUCCEEDED(hr))
    {
        PAINTSTRUCT ps;
        BeginPaint(m_hwnd, &ps);

        pRenderTarget->BeginDraw();

        pRenderTarget->Clear(D2D1::ColorF(D2D1::ColorF::SkyBlue));

        for (auto i = figures.begin(); i != figures.end(); ++i)
        {
            (*i)->Draw(pRenderTarget, pBrush);
        }

        hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
        {
            DiscardGraphicsResources();
        }
        EndPaint(m_hwnd, &ps);
    }*/
}

void GraphicsScene::OnLButtonDown(int pixelX, int pixelY, DWORD flags)
{
    const float dipX = pixelX;// DPIScale::PixelsToDipsX(pixelX);
    const float dipY = pixelY;// DPIScale::PixelsToDipsY(pixelY);

    POINT pt = { pixelX, pixelY };
    ptMouse = { dipX, dipY };

    if (DragDetect(m_hwnd, pt))
    {
        SetCapture(m_hwnd);
        switch (*mode)
        {
        case Mode::DrawMode:
            InsertFigure(dipX, dipY);
            break;
        }
    }
    else
    {
        if (*mode == Mode::SelectMode)
        {
            Select(ptMouse);
        }
    }
    InvalidateRect(m_hwnd, NULL, FALSE);
}

void GraphicsScene::OnLButtonUp()
{
    if (Selection())
    {
        Selection()->SaveTransform();
    }
    ReleaseCapture();
}


void GraphicsScene::OnMouseMove(int pixelX, int pixelY, DWORD flags)
{
    const float dipX = pixelX;//DPIScale::PixelsToDipsX(pixelX);
    const float dipY = pixelY;//DPIScale::PixelsToDipsY(pixelY);

    if ((flags & MK_LBUTTON) && Selection())
    {
        switch (*mode)
        {
        case Mode::DrawMode:
        {
            float left;
            float width;
            float top;
            float height;

            if (ptMouse.X > dipX)
            {
                left = dipX;
                width = ptMouse.X - dipX;
            }
            else
            {
                left = ptMouse.X;
                width = dipX - ptMouse.X;
            }

            if (ptMouse.Y > dipY)
            {
                top = dipY;
                height = ptMouse.Y - dipY;
            }
            else
            {
                top = ptMouse.Y;
                height = dipY - ptMouse.Y;
            }

            Selection()->PlaceIn(RectF(left, top, width, height));
            break;
        }
        case Mode::DragMode:
        {
            Selection()->Translate({ dipX - ptMouse.X, dipY - ptMouse.Y });
            ptMouse = { dipX, dipY };
            break;
        }
        {
        case Mode::ScaleMode:
            Selection()->RevertTransform();
            PointF* center = Selection()->GetCenter();
            PointF size = { abs((dipX - center->X) / (ptMouse.X - center->X)), abs((dipY - center->Y) / (ptMouse.Y - center->Y)) };
            Selection()->Scale(size.X > size.Y ? size.X : size.Y);
            delete center;
            break;
        }
        case Mode::RotateMode:
        {
            PointF* center = Selection()->GetCenter();
            FLOAT ax = ptMouse.X - center->X;
            FLOAT ay = ptMouse.Y - center->Y;
            FLOAT bx = dipX - center->X;
            FLOAT by = dipY - center->Y;
            FLOAT aLengthSquare = ax * ax + ay * ay;
            FLOAT bLengthSquare = bx * bx + by * by;
            FLOAT angle = ToDegrees((ax * by - ay * bx) / sqrtf(aLengthSquare * bLengthSquare));
            Selection()->Rotate(angle, *center);
            ptMouse = { dipX, dipY };
            delete center;
            break;
        }
        }
        InvalidateRect(m_hwnd, NULL, FALSE);
    }
}

void GraphicsScene::OnKeyDown(UINT vkey)
{
    switch (vkey)
    {
    case VK_DELETE:
        if ((*mode == Mode::SelectMode) && Selection())
        {
            figures.erase(selection);
            selection = figures.end();
            ClearSelection();
            InvalidateRect(m_hwnd, NULL, FALSE);
        }
        break;
    }
}

LRESULT GraphicsScene::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        if (!GetParent(m_hwnd))
        {
            DPIScale::Initialize();
        }
        trackingStruct.hwndTrack = m_hwnd;
        return 0;

    case WM_DESTROY:
        if (!GetParent(m_hwnd))
        {
            PostQuitMessage(0);
        }
        return 0;

    case WM_PAINT:
        OnPaint();
        return 0;

    case WM_SIZE:
        InvalidateRect(m_hwnd, NULL, FALSE);
        //Resize();
        return 0;

    case WM_LBUTTONDOWN:
        OnLButtonDown(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        return 0;

    case WM_LBUTTONUP:
        OnLButtonUp();
        return 0;

    case WM_MOUSEMOVE:
        if (tracking)
        {
            OnMouseMove(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), (DWORD)wParam);
        }
        else
        {
            trackingStruct.dwFlags = TME_HOVER | TME_LEAVE;
            TrackMouseEvent(&trackingStruct);
            tracking = true;
        }
        return 0;

    case WM_MOUSEHOVER:
        SetFocus(m_hwnd);
        trackingStruct.dwFlags = TME_LEAVE;
        TrackMouseEvent(&trackingStruct);
        return 0;

    case WM_MOUSELEAVE:
        tracking = false;
        return 0;

    case WM_KEYDOWN:
        OnKeyDown((UINT)wParam);
        return 0;

    case WM_COMMAND:
        if (~GetKeyState(VK_LBUTTON) & 0x8000)
        {
            HWND parentWND = GetParent(m_hwnd);
            if (!parentWND)
            {
                parentWND = m_hwnd;
            }

            switch (LOWORD(wParam))
            {
            case ID_DRAW_MODE:
                *mode = Mode::DrawMode;
                PostMessage(m_hwnd, WM_MODE_CHANGED, NULL, NULL);
                return 0;

            case ID_SELECT_MODE:
                *mode = Mode::SelectMode;
                PostMessage(m_hwnd, WM_MODE_CHANGED, NULL, NULL);
                return 0;

            case ID_DRAG_MODE:
                *mode = Mode::DragMode;
                PostMessage(m_hwnd, WM_MODE_CHANGED, NULL, NULL);
                return 0;

            case ID_SCALE_MODE:
                *mode = Mode::ScaleMode;
                PostMessage(m_hwnd, WM_MODE_CHANGED, NULL, NULL);
                return 0;

            case ID_ROTATE_MODE:
                *mode = Mode::RotateMode;
                PostMessage(m_hwnd, WM_MODE_CHANGED, NULL, NULL);
                return 0;


            case ID_ELLIPSE:
                if (*mode == Mode::DrawMode)
                {
                    *figure = Figure::Ellipse;
                    PostMessage(m_hwnd, WM_FIGURE_CHANGED, NULL, NULL);
                }
                return 0;

            case ID_RECT:
                if (*mode == Mode::DrawMode)
                {
                    *figure = Figure::Rect;
                    PostMessage(m_hwnd, WM_FIGURE_CHANGED, NULL, NULL);
                }
                return 0;
            }
        }
        break;

    case WM_MODE_CHANGED:
        return 0;

    case WM_FIGURE_CHANGED:
        return 0;

    case WM_COLOR_CHANGED:
        ColorChanged();
        return 0;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}
