#include "Initializer.h"
#include <algorithm>

const PCWSTR Initializer::DEFAULT_CLASS_NAME = WC_TREEVIEW;

Initializer::Initializer(PCWSTR CLASS_NAME) : m_hwnd(NULL), bufferSize(1048576), processes()
{
    buffer = new void* [bufferSize];
}


void Initializer::initialize(HWND m_hwnd)
{
	this->m_hwnd = m_hwnd;
	std::vector<_SYSTEM_PROCESS_INFORMATION*> tmpp;
	ULONG size;
	NtQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, &size);
	size_t si = sizeof(_SYSTEM_PROCESS_INFORMATION);
	void* tmp = buffer;

	size_t offset;
	do
	{
		offset = ((_SYSTEM_PROCESS_INFORMATION*)tmp)->NextEntryOffset;
		tmpp.push_back((_SYSTEM_PROCESS_INFORMATION*)tmp);
		tmp = (void*)((size_t)tmp + offset);
	} while (offset != NULL);

	std::sort(tmpp.begin(),
		tmpp.end(),
		[](_SYSTEM_PROCESS_INFORMATION* const &x, _SYSTEM_PROCESS_INFORMATION* const &y) {
			return (int)((DWORD)(x->UniqueProcessId) > (DWORD)(y->UniqueProcessId) - (int)((DWORD)(x->UniqueProcessId) < (DWORD)(y->UniqueProcessId)));
		}
	);

	size_t i = 0, j = 0;
	ProcessNode* after = NULL;
	while (i < processes.size() && j < tmpp.size())
	{
		while (i < processes.size() && j < tmpp.size() && (DWORD)((tmpp[j])->UniqueProcessId) < processes[i]->getId())
		{
			processes.emplace(processes.begin() + i, new ProcessNode(tmpp[j], m_hwnd, nullptr, after));
			after = processes[i];
			j++;
			i++;
		}
		while (i < processes.size() && j < tmpp.size() && (DWORD)((tmpp[j])->UniqueProcessId) == processes[i]->getId())
		{
			processes[i]->update(tmpp[j]);
			after = processes[i];
			j++;
			i++;
		}
		while (i < processes.size() && j < tmpp.size() && (DWORD)((tmpp[j])->UniqueProcessId) > processes[i]->getId())
		{
			delete processes[i];
			processes.erase(processes.begin() + i);
		}
	}

	while (i < processes.size())
	{
		delete processes[i];
		processes.erase(processes.begin() + i);
	}

	while (j < tmpp.size())
	{
		processes.emplace_back(new ProcessNode(tmpp[j], m_hwnd, nullptr, after));
		after = processes.back();
		j++;
	}
}