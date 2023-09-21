#pragma once

#include <d2d1.h>

class DPIScale
{
    static float scaleX;
    static float scaleY;

public:
    static void Initialize()
    {
        FLOAT dpi = GetDpiForSystem();
        scaleX = dpi / 96.0f;
        scaleY = dpi / 96.0f;
    }

    template <typename T>
    static float PixelsToDipsX(T x)
    {
        return static_cast<float>(x) / scaleX;
    }

    template <typename T>
    static float PixelsToDipsY(T y)
    {
        return static_cast<float>(y) / scaleY;
    }

    template <typename T>
    static T DipXToPixels(float x)
    {
        return static_cast<T>(x) * scaleX;
    }

    template <typename T>
    static T DipYToPixels(float y)
    {
        return static_cast<T>(y) * scaleY;
    }
};
