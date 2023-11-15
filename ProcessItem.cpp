#include "ProcessItem.h"


LPCWSTR ProcessItem::NAME_PREFIX = L"Process ";



ProcessItem::ProcessItem(_SYSTEM_PROCESS_INFORMATION* info) : threads(), suspended(true)
{
	process_id = (DWORD)(info->UniqueProcessId);

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
			return ((int)((DWORD)(arg1->ClientId.UniqueThread) > (DWORD)(arg2->ClientId.UniqueThread)) - (int)((DWORD)(arg1->ClientId.UniqueThread) < (DWORD)(arg2->ClientId.UniqueThread)));
		}
	);

	for (size_t i = 0; i < info->NumberOfThreads; ++i)
	{
		if (threadsData->WaitReason != 5)
			suspended = false;
		threads.emplace_back(new ThreadItem(threadsData++));
	}
}

ProcessItem::~ProcessItem()
{
	for (auto& thread : threads)
		delete thread;
	//CloseHandle(process);
}

bool ProcessItem::suspend()
{
	valid = false;
	for (auto& thread : threads)
	{
		if (thread->suspend() < 0)
			return true;
	}
}

bool ProcessItem::resume()
{
	valid = false;
	for (auto & thread : threads)
	{
		if (thread->resume() < 0)
			return true;
	}
}

bool ProcessItem::terminate()
{
	valid = false;
	if (HANDLE process = OpenProcess(THREAD_ALL_ACCESS, false, process_id))
	{
		DWORD ret = TerminateProcess(process, NULL);
		CloseHandle(process);
		return ret == 0;
	}
}