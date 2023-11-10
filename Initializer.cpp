#include "Initializer.h"

const PCWSTR Initializer::DEFAULT_CLASS_NAME = L"Hard";

Initializer::Initializer(PCWSTR CLASS_NAME) : BaseWindow<Initializer>(CLASS_NAME), bufferSize(8192)
{
    buffer = new void* [bufferSize];
}

LRESULT Initializer::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void Initializer::initialize()
{
    processes.clear();
    NtQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, NULL);
    void* tmp = buffer;
    size_t offset;
    BaseNode* after = NULL;
    do
    {
        offset = ((_SYSTEM_PROCESS_INFORMATION*)tmp)->NextEntryOffset;
        processes.emplace_back((_SYSTEM_PROCESS_INFORMATION*)tmp, m_hwnd, NULL, after);
        after = &processes.back();
        tmp = tmp + offset;
    } while (offset != NULL);
}