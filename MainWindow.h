#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <compressapi.h>
#include <shlwapi.h>
#include <string>

#include "settings.h"
#include "BaseWindow.h"
#include "DPIScale.h"
#include "resource.h"
#include "GraphicsScene.h"
#include "SceneControl.h"

class MainWindow : public BaseWindow<MainWindow>
{
private:
    static const PCWSTR DEFAULT_CLASS_NAME;
    static const float MARGIN_X;
    static const float MARGIN_Y;
    static const LPWSTR DEFAULT_EXTENSIONS;
    static const COMDLG_FILTERSPEC FILE_TYPES[];

public:
    MainWindow(PCWSTR CLASS_NAME = DEFAULT_CLASS_NAME);
    ~MainWindow();

    bool compress();
    bool decompress();

    HANDLE openFile(UINT fileTypesSize = 0, const COMDLG_FILTERSPEC* fileTypes = NULL, LPWSTR defaultExtension = NULL);
    HANDLE saveFile(UINT fileTypesSize = 0, const COMDLG_FILTERSPEC* fileTypes = NULL, LPWSTR defaultExtension = NULL);

    virtual LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) override;

protected:
    void CreateLayout();
    void SetLayout();

    HWND compressButton;
    HWND decompressButton;

    LPCWSTR current_path;
};

