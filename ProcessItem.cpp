#include "ProcessItem.h"


#pragma comment(lib, "Ntdll")


ProcessItem::ProcessItem(_SYSTEM_PROCESS_INFORMATION* info) : threads(), suspended(true)
{
	process = OpenProcess(PROCESS_ALL_ACCESS, false, (DWORD)info->UniqueProcessId);
	name = info->ImageName.Buffer;

	_SYSTEM_THREAD_INFORMATION* threadsData = (_SYSTEM_THREAD_INFORMATION*)(info + 1);

	std::qsort(
		threadsData,
		info->NumberOfThreads,
		sizeof(_SYSTEM_THREAD_INFORMATION),
		[](const void* x, const void* y) {
			const _SYSTEM_THREAD_INFORMATION* arg1 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(x);
			const _SYSTEM_THREAD_INFORMATION* arg2 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(y);
			return (arg1->ClientId.UniqueThread > arg2->ClientId.UniqueThread) - (arg1->ClientId.UniqueThread < arg2->ClientId.UniqueThread);
		}
	);

	threads.reserve(info->NumberOfThreads);
	for (size_t i = 0; i < info->NumberOfThreads; ++i)
	{
		if (threadsData->WaitReason != 5)
			suspended = false;
		threads.emplace_back(threadsData++);
	}
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

void ProcessItem::update(_SYSTEM_PROCESS_INFORMATION* info)
{
	if (GetProcessId(process) != (DWORD)info->UniqueProcessId)
	{
		CloseHandle(process);
		process = OpenProcess(THREAD_ALL_ACCESS, false, (DWORD)info->UniqueProcessId);
		name = info->ImageName.Buffer;
	}

	_SYSTEM_THREAD_INFORMATION* threadsData = (_SYSTEM_THREAD_INFORMATION*)(info + 1);

	std::qsort(
		threadsData,
		info->NumberOfThreads,
		sizeof(_SYSTEM_THREAD_INFORMATION),
		[](const void* x, const void* y) {
			const _SYSTEM_THREAD_INFORMATION* arg1 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(x);
			const _SYSTEM_THREAD_INFORMATION* arg2 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(y);
			return (arg1->ClientId.UniqueThread > arg2->ClientId.UniqueThread) - (arg1->ClientId.UniqueThread < arg2->ClientId.UniqueThread);
		}
	);

	suspended = true;
	threads.clear();
	for (size_t i = 0; i < info->NumberOfThreads; ++i)
	{
		if (threadsData->WaitReason != 5)
			suspended = false;
		threads.emplace_back(threadsData++);
	}

	valid = true;
}