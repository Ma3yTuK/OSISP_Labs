#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <CommCtrl.h>

#include <unordered_map>

#include "BaseWindow.h"
#include "DPIScale.h"
#include "ProcessNode.h"

class Initializer
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;

public:
    Initializer(PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME);

    void initialize(HWND m_hwnd);

    ~Initializer() { delete[] buffer; }

protected:
    HWND m_hwnd;
    std::deque<ProcessNode*> processes;
    void* buffer;
    size_t bufferSize;
};