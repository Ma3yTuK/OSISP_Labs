#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <CommCtrl.h>

#include <unordered_map>

#include "settings.h"
#include "BaseWindow.h"
#include "DPIScale.h"
#include "resource.h"
#include "BaseFigure.h"
#include "ProcessNode.h"

class Initializer : public BaseWindow<Initializer>
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;

public:
    Initializer(PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;
    void initialize();

    ~Initializer() { delete[] buffer; }

protected:
    std::vector<ProcessNode> processes;
    void* buffer;
    size_t bufferSize;
};