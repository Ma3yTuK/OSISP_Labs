#include "ThreadItem.h"


LPCWSTR ThreadItem::NAME_PREFIX = L"Thread ";


ThreadItem::ThreadItem(_SYSTEM_THREAD_INFORMATION* info)
{
	thread = OpenThread(THREAD_ALL_ACCESS, false, (DWORD)(info->ClientId.UniqueThread));
	name = NAME_PREFIX + std::to_wstring((DWORD)(info->ClientId.UniqueThread));
	suspended = info->WaitReason == 5;
}

ThreadItem::ThreadItem(const ThreadItem& obj) : name(obj.name), suspended(obj.suspended)
{
	thread = OpenThread(THREAD_ALL_ACCESS, false, GetThreadId(obj.thread));
}

ThreadItem::~ThreadItem()
{
	CloseHandle(thread);
}

void ThreadItem::suspend()
{
	SuspendThread(thread);
	valid = false;
}

void ThreadItem::resume()
{
	while (ResumeThread(thread) > 1);
	valid = false;
}

void ThreadItem::terminate()
{
	TerminateThread(thread, NULL);
	valid = false;
}

bool ThreadItem::update(_SYSTEM_THREAD_INFORMATION* info)
{
	bool updated = false;

	if ((info->WaitReason == 5) != suspended)
	{
		suspended = info->WaitReason == 5;
		updated = true;
	}

	valid = true;

	return updated;
}