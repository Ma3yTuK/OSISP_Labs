#include "Initializer.h"

const PCWSTR Initializer::DEFAULT_CLASS_NAME = WC_TREEVIEW;

Initializer::Initializer(PCWSTR CLASS_NAME) : BaseWindow<Initializer>(CLASS_NAME), bufferSize(8192), processes()
{
    buffer = new void* [bufferSize];
}

LRESULT Initializer::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
}

void Initializer::initialize()
{
	std::vector<_SYSTEM_PROCESS_INFORMATION*> tmpp;
	NtQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, NULL);
	void* tmp = buffer;
	size_t offset;
	do
	{
		offset = ((_SYSTEM_PROCESS_INFORMATION*)tmp)->NextEntryOffset;
		tmpp.push_back((_SYSTEM_PROCESS_INFORMATION*)tmp);
		tmp = (void*)((LONG_PTR)tmp + offset);
	} while (offset != NULL);

	std::qsort(
		tmpp.data(),
		tmpp.size(),
		sizeof(ProcessNode),
		[](const void* x, const void* y) {
			const _SYSTEM_PROCESS_INFORMATION** arg1 = (const _SYSTEM_PROCESS_INFORMATION**)(x);
			const _SYSTEM_PROCESS_INFORMATION** arg2 = (const _SYSTEM_PROCESS_INFORMATION**)(y);
			return (int)(GetProcessId((*arg1)->UniqueProcessId) > GetProcessId((*arg2)->UniqueProcessId) - (int)(GetProcessId((*arg1)->UniqueProcessId) < GetProcessId((*arg2)->UniqueProcessId)));
		}
	);

	size_t i = 0, j = 0;
	ProcessNode* after = NULL;
	while (i < processes.size() && j < tmpp.size())
	{
		while (i < processes.size() && j < tmpp.size() && GetProcessId((tmpp[j])->UniqueProcessId) < GetProcessId(processes[i].getHandle()))
		{
			processes.emplace(processes.begin() + i, tmpp[j], m_hwnd, nullptr, after);
			after = &processes[i];
			j++;
			i++;
		}
		while (i < processes.size() && j < tmpp.size() && GetProcessId((tmpp[j])->UniqueProcessId) == GetProcessId(processes[i].getHandle()))
		{
			processes[i].update(tmpp[j]);
			after = &processes[i];
			j++;
			i++;
		}
		while (i < processes.size() && j < tmpp.size() && GetProcessId((tmpp[j])->UniqueProcessId) > GetProcessId(processes[i].getHandle()))
		{
			processes.erase(processes.begin() + i);
		}
	}

	while (i < processes.size())
	{
		processes.erase(processes.begin() + i);
	}

	while (j < tmpp.size())
	{
		processes.emplace_back(tmpp[j], m_hwnd, nullptr, after);
		after = &processes.back();
		j++;
	}
}