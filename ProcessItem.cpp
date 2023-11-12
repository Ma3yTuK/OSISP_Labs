#include "ProcessItem.h"


LPCWSTR ProcessItem::NAME_PREFIX = L"Process ";



ProcessItem::ProcessItem(_SYSTEM_PROCESS_INFORMATION* info) : threads(), suspended(true)
{
	process = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)(info->UniqueProcessId));

	if (info->ImageName.Buffer)
		name = info->ImageName.Buffer;
	else
		name = NAME_PREFIX + std::to_wstring((DWORD)(info->UniqueProcessId));

	size_t si = sizeof(_SYSTEM_PROCESS_INFORMATION);

	_SYSTEM_THREAD_INFORMATION* threadsData = (_SYSTEM_THREAD_INFORMATION*)(info + 1);

	std::qsort(
		threadsData,
		info->NumberOfThreads,
		sizeof(_SYSTEM_THREAD_INFORMATION),
		[](const void* x, const void* y) {
			const _SYSTEM_THREAD_INFORMATION* arg1 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(x);
			const _SYSTEM_THREAD_INFORMATION* arg2 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(y);
			return (((DWORD)(arg1->ClientId.UniqueThread) > (DWORD)(arg2->ClientId.UniqueThread)) - ((DWORD)(arg1->ClientId.UniqueThread) < (DWORD)(arg2->ClientId.UniqueThread)));
		}
	);

	for (size_t i = 0; i < info->NumberOfThreads; ++i)
	{
		if (threadsData->WaitReason != 5)
			suspended = false;
		threads.emplace_back(threadsData++);
	}
}

ProcessItem::ProcessItem(const ProcessItem& obj) : threads(obj.threads)
{
	process = OpenProcess(THREAD_ALL_ACCESS, false, GetThreadId(obj.process));
}

ProcessItem::~ProcessItem()
{
	CloseHandle(process);
}

void ProcessItem::suspend()
{
	for (auto& thread : threads)
	{
		thread.suspend();
	}
	valid = false;
}

void ProcessItem::resume()
{
	for (auto & thread : threads)
	{
		thread.resume();
	}
	valid = false;
}

void ProcessItem::terminate()
{
	TerminateProcess(process, NULL);
	valid = false;
}