#include "MainWindow.h"
#include "helper_functions.h"

#pragma comment(lib,"cabinet.lib")
#pragma comment(lib, "Shlwapi.lib")

const PCWSTR MainWindow::DEFAULT_CLASS_NAME = L"Graphics";
const float MainWindow::MARGIN_X = 6.0F;
const float MainWindow::MARGIN_Y = 6.0F;
const LPWSTR MainWindow::DEFAULT_EXTENSIONS = L"zip";
const COMDLG_FILTERSPEC MainWindow::FILE_TYPES[] = {
    { L"Archive (.zip)", L"*.zip" },
};

std::wstring wstr(L"C:\\Users\\Nazar\\OneDrive\\Рабочий стол\\");

MainWindow::MainWindow(PCWSTR CLASS_NAME) :
    BaseWindow<MainWindow>(CLASS_NAME)
{
}

MainWindow::~MainWindow()
{
}

LRESULT MainWindow::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CREATE:
        CreateLayout();
        return 0;

    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(m_hwnd, &ps);
        FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));
        EndPaint(m_hwnd, &ps);
        return 0;
    }

    case WM_SIZE:
        SetLayout();
        return 0;

    case WM_SIZING:
    {
        RECT* dragRc = (RECT*)lParam;

        RECT rcWindow;
        GetWindowRect(m_hwnd, &rcWindow);

        int WINDOW_HEIGHT_PIX = rcWindow.bottom - rcWindow.top;
        int WINDOW_WIDTH_PIX = rcWindow.right - rcWindow.left;

        switch (wParam)
        {
        case WMSZ_BOTTOM:
            dragRc->bottom = dragRc->top + WINDOW_HEIGHT_PIX;
            break;
            
        case WMSZ_BOTTOMLEFT:
            dragRc->bottom = dragRc->top + WINDOW_HEIGHT_PIX;
            dragRc->left = dragRc->right - WINDOW_WIDTH_PIX;
            break;

        case WMSZ_LEFT:
            dragRc->left = dragRc->right - WINDOW_WIDTH_PIX;
            break;

        case WMSZ_TOPLEFT:
            dragRc->left = dragRc->right - WINDOW_WIDTH_PIX;
            dragRc->top = dragRc->bottom - WINDOW_HEIGHT_PIX;
            break;

        case WMSZ_TOP:
            dragRc->top = dragRc->bottom - WINDOW_HEIGHT_PIX;
            break;

        case WMSZ_TOPRIGHT:
            dragRc->top = dragRc->bottom - WINDOW_HEIGHT_PIX;
            dragRc->right = dragRc->left + WINDOW_WIDTH_PIX;
            break;

        case WMSZ_RIGHT:
            dragRc->right = dragRc->left + WINDOW_WIDTH_PIX;
            break;


        case WMSZ_BOTTOMRIGHT:
            dragRc->right = dragRc->left + WINDOW_WIDTH_PIX;
            dragRc->bottom = dragRc->top + WINDOW_HEIGHT_PIX;
            break;
        }

        return TRUE;
    }

    case WM_COMMAND:
        if (HIWORD(wParam) == BN_CLICKED)
        {
            if ((HWND)lParam == compressButton)
            {
                compress();
            }
            else if ((HWND)lParam == decompressButton)
            {
                decompress();
            }
            return 0;
        }
        break;
    }
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void MainWindow::CreateLayout()
{
    compressButton = CreateWindow(L"BUTTON",
        L"Compress",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
    decompressButton = CreateWindow(L"BUTTON",
        L"Decompress",
        WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON | WS_BORDER,
        0,
        0,
        0,
        0,
        m_hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL);
}

void MainWindow::SetLayout()
{
    RECT rcClient;
    GetClientRect(m_hwnd, &rcClient);

    int MARGIN_XPix = DPIScale::DipXToPixels<int>(MARGIN_X);
    int MARGIN_YPix = DPIScale::DipYToPixels<int>(MARGIN_Y);
    int WINDOW_HEIGHT_PIX = rcClient.bottom - rcClient.top;
    int WINDOW_WIDTH_PIX = rcClient.right - rcClient.left;

    MoveWindow(compressButton,
        MARGIN_XPix,
        MARGIN_YPix,
        WINDOW_WIDTH_PIX - MARGIN_XPix * 2,
        (WINDOW_HEIGHT_PIX - MARGIN_YPix * 3) / 2,
        FALSE);

    MoveWindow(decompressButton,
        MARGIN_XPix,
        MARGIN_YPix * 2 + (WINDOW_HEIGHT_PIX - MARGIN_YPix * 3) / 2,
        WINDOW_WIDTH_PIX - MARGIN_XPix * 2,
        (WINDOW_HEIGHT_PIX - MARGIN_YPix * 3) / 2,
        FALSE);

    InvalidateRect(m_hwnd, NULL, FALSE);
}

bool MainWindow::compress()
{
    double result = 0;

    HANDLE decompressedFile = openFile();
    if (decompressedFile != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(decompressedFile, &fileSize) && fileSize.QuadPart < 0xffffffff)
        {
            DWORD realFileSize = fileSize.LowPart;
            HANDLE alertEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
            if (alertEvent != INVALID_HANDLE_VALUE)
            {
                OVERLAPPED overlapped = OVERLAPPED();
                overlapped.hEvent = alertEvent;
                LPVOID buffer = (LPVOID)malloc(realFileSize);
                if (buffer)
                {
                    BOOL readResult = ReadFile(decompressedFile, buffer, realFileSize, NULL, &overlapped);
                    bool isOverlapped = GetLastError() == ERROR_IO_PENDING;
                    if (readResult || isOverlapped)
                    {
                        COMPRESSOR_HANDLE compressor;
                        if (CreateCompressor(COMPRESS_ALGORITHM_MSZIP, NULL, &compressor))
                        {
                            bool readSuccessfull = true;
                            if (isOverlapped)
                            {
                                MsgWaitForMultipleObjects(1, &alertEvent, TRUE, INFINITE, QS_ALLINPUT);
                                DWORD numberOfBytesTransfered;
                                readSuccessfull = GetOverlappedResult(decompressedFile, &overlapped, &numberOfBytesTransfered, FALSE);
                            }
                            if (readSuccessfull)
                            {
                                SIZE_T compressBufferSize;
                                if (!Compress(compressor, buffer, realFileSize, NULL, 0, &compressBufferSize))
                                {
                                    DWORD compressError = GetLastError();
                                    if (compressError == ERROR_INSUFFICIENT_BUFFER)
                                    {
                                        DWORD ErrorCode = GetLastError();
                                        PBYTE compressBuffer = (PBYTE)malloc(compressBufferSize);
                                        if (compressBuffer)
                                        {
                                            SIZE_T compressDataSize;
                                            ULONGLONG StartTime = GetTickCount64();
                                            if (Compress(compressor, buffer, realFileSize, compressBuffer, compressBufferSize, &compressDataSize))
                                            {
                                                double duration = (GetTickCount64() - StartTime) / (double)1000;
                                                HANDLE compressedFile = saveFile(ARRAYSIZE(FILE_TYPES), FILE_TYPES, DEFAULT_EXTENSIONS);
                                                if (compressedFile != INVALID_HANDLE_VALUE)
                                                {
                                                    HANDLE alertEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
                                                    if (alertEvent != INVALID_HANDLE_VALUE)
                                                    {
                                                        /*HANDLE hMapFile;
                                                        hMapFile = CreateFileMapping(compressedFile, NULL, PAGE_READWRITE, 0, 0, NULL);
                                                        LPVOID lpMapAddress;
                                                        lpMapAddress = MapViewOfFile(hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
                                                        snprintf((char*)lpMapAddress, compressDataSize, (char*)compressBuffer);
                                                        UnmapViewOfFile(lpMapAddress);
                                                        CloseHandle(hMapFile);*/
                                                        
                                                        OVERLAPPED overlapped = OVERLAPPED();
                                                        overlapped.hEvent = alertEvent;
                                                        BOOL writeResult = WriteFile(compressedFile, compressBuffer, compressDataSize, NULL, &overlapped);
                                                        bool isOverlapped = GetLastError() == ERROR_IO_PENDING;
                                                        if (writeResult || isOverlapped)
                                                        {
                                                            bool writeSuccessfull = true;
                                                            if (isOverlapped)
                                                            {
                                                                MsgWaitForMultipleObjects(1, &alertEvent, TRUE, INFINITE, QS_ALLINPUT);
                                                                DWORD numberOfBytesTransfered;
                                                                writeSuccessfull = GetOverlappedResult(decompressedFile, &overlapped, &numberOfBytesTransfered, FALSE);
                                                            }
                                                            if (writeSuccessfull)
                                                            {
                                                                result = duration;
                                                            }
                                                        }
                                                    }
                                                    CloseHandle(compressedFile);
                                                }
                                            }
                                            free(compressBuffer);
                                        }
                                    }
                                }
                            }
                            CloseCompressor(compressor);
                        }
                    }
                    free(buffer);
                }
                CloseHandle(alertEvent);
            }
        }
        CloseHandle(decompressedFile);
    }

    return result;
}

bool MainWindow::decompress()
{
    double result = 0;

    HANDLE compressedFile = openFile(ARRAYSIZE(FILE_TYPES), FILE_TYPES, DEFAULT_EXTENSIONS);
    if (compressedFile != INVALID_HANDLE_VALUE)
    {
        LARGE_INTEGER fileSize;
        if (GetFileSizeEx(compressedFile, &fileSize) && fileSize.QuadPart < 0xffffffff)
        {
            DWORD realFileSize = fileSize.LowPart;
            HANDLE alertEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
            if (alertEvent != INVALID_HANDLE_VALUE)
            {
                OVERLAPPED overlapped = OVERLAPPED();
                overlapped.hEvent = alertEvent;
                LPVOID buffer = (LPVOID)malloc(realFileSize);
                if (buffer)
                {
                    BOOL readResult = ReadFile(compressedFile, buffer, realFileSize, NULL, &overlapped);
                    bool isOverlapped = GetLastError() == ERROR_IO_PENDING;
                    if (readResult || isOverlapped)
                    {
                        DECOMPRESSOR_HANDLE decompressor;
                        if (CreateDecompressor(COMPRESS_ALGORITHM_MSZIP, NULL, &decompressor))
                        {
                            bool readSuccessfull = true;
                            if (isOverlapped)
                            {
                                MsgWaitForMultipleObjects(1, &alertEvent, TRUE, INFINITE, QS_ALLINPUT);
                                DWORD numberOfBytesTransfered;
                                readSuccessfull = GetOverlappedResult(compressedFile, &overlapped, &numberOfBytesTransfered, FALSE);
                            }
                            if (readSuccessfull)
                            {
                                SIZE_T decompressBufferSize;
                                if (!Decompress(decompressor, buffer, realFileSize, NULL, 0, &decompressBufferSize))
                                {
                                    DWORD decompressError = GetLastError();
                                    if (decompressError == ERROR_INSUFFICIENT_BUFFER)
                                    {
                                        DWORD ErrorCode = GetLastError();
                                        PBYTE decompressBuffer = (PBYTE)malloc(decompressBufferSize);
                                        if (decompressBuffer)
                                        {
                                            SIZE_T decompressDataSize;
                                            ULONGLONG StartTime = GetTickCount64();
                                            if (Decompress(decompressor, buffer, realFileSize, decompressBuffer, decompressBufferSize, &decompressDataSize))
                                            {
                                                double duration = (GetTickCount64() - StartTime) / (double)1000;
                                                HANDLE decompressedFile = saveFile();
                                                if (decompressedFile != INVALID_HANDLE_VALUE)
                                                {
                                                    HANDLE alertEvent = CreateEventA(NULL, FALSE, FALSE, NULL);
                                                    if (alertEvent != INVALID_HANDLE_VALUE)
                                                    {
                                                        OVERLAPPED overlapped = OVERLAPPED();
                                                        overlapped.hEvent = alertEvent;
                                                        BOOL writeResult = WriteFile(decompressedFile, decompressBuffer, decompressDataSize, NULL, &overlapped);
                                                        bool isOverlapped = GetLastError() == ERROR_IO_PENDING;
                                                        if (writeResult || isOverlapped)
                                                        {
                                                            bool writeSuccessfull = true;
                                                            if (isOverlapped)
                                                            {
                                                                MsgWaitForMultipleObjects(1, &alertEvent, TRUE, INFINITE, QS_ALLINPUT);
                                                                DWORD numberOfBytesTransfered;
                                                                writeSuccessfull = GetOverlappedResult(compressedFile, &overlapped, &numberOfBytesTransfered, FALSE);
                                                            }
                                                            if (writeSuccessfull)
                                                            {
                                                                result = duration;
                                                            }
                                                        }
                                                    }
                                                    CloseHandle(decompressedFile);
                                                }
                                            }
                                            free(decompressBuffer);
                                        }
                                    }
                                }
                            }
                            CloseCompressor(decompressor);
                        }
                    }
                    free(buffer);
                }
                CloseHandle(alertEvent);
            }
        }
        CloseHandle(compressedFile);
    }

    return result;
}

HANDLE MainWindow::openFile(UINT fileTypesSize, const COMDLG_FILTERSPEC* fileTypes, LPWSTR defaultExtension)
{
    HANDLE result = INVALID_HANDLE_VALUE;

    IFileDialog* fileDialog = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&fileDialog));
    if (SUCCEEDED(hr))
    {
        if (fileTypes)
        {
            hr = fileDialog->SetFileTypes(fileTypesSize, fileTypes);
        }
        if (SUCCEEDED(hr))
        {
            if (defaultExtension)
            {
                hr = fileDialog->SetDefaultExtension(defaultExtension);
            }
            if (SUCCEEDED(hr))
            {
                hr = fileDialog->Show(NULL);
                if (SUCCEEDED(hr))
                {
                    IShellItem* shellItem;
                    hr = fileDialog->GetResult(&shellItem);
                    if (SUCCEEDED(hr))
                    {
                        PWSTR filePath = NULL;
                        hr = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
                        if (SUCCEEDED(hr))
                        {
                            result = CreateFile(filePath, GENERIC_READ, NULL, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
                            current_path = PathFindFileNameW(filePath);
                        }
                        shellItem->Release();
                    }
                }
                fileDialog->Release();
            }
        }
    }
    return result;
}

HANDLE MainWindow::saveFile(UINT fileTypesSize, const COMDLG_FILTERSPEC* fileTypes, LPWSTR defaultExtension)
{
    HANDLE result = INVALID_HANDLE_VALUE;

    IFileDialog* fileDialog = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileSaveDialog,
        NULL,
        CLSCTX_INPROC_SERVER,
        IID_PPV_ARGS(&fileDialog));
    if (SUCCEEDED(hr))
    {
        if (fileTypes)
        {
            hr = fileDialog->SetFileTypes(fileTypesSize, fileTypes);
        }
        if (SUCCEEDED(hr))
        {
            if (defaultExtension)
            {
                hr = fileDialog->SetDefaultExtension(defaultExtension);
            }
            if (SUCCEEDED(hr))
            {
                //hr = fileDialog->Show(NULL);
                if (SUCCEEDED(hr))
                {
                    //IShellItem* shellItem;
                    //hr = fileDialog->GetResult(&shellItem);
                    if (SUCCEEDED(hr))
                    {
                        //PWSTR filePath = NULL;
                        //hr = shellItem->GetDisplayName(SIGDN_FILESYSPATH, &filePath);
                        if (SUCCEEDED(hr))
                        {
                            std::wstring path = wstr + current_path;
                            if (fileTypesSize)
                                path += L".zip";
                            else
                                for (int i = 0; i < 4; i++)
                                    path.pop_back();
                            result = CreateFile(path.c_str(), GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
                        }
                        //shellItem->Release();
                    }
                }
            }
        }
        fileDialog->Release();
    }
    return result;
}