#include "ThreadItem.h"


LPCWSTR ThreadItem::NAME_PREFIX = L"Thread ";


ThreadItem::ThreadItem(_SYSTEM_THREAD_INFORMATION* info)
{
	thread_id = (DWORD)(info->ClientId.UniqueThread);
	name = NAME_PREFIX + std::to_wstring(thread_id);
	suspended = info->WaitReason == 5;
}

ThreadItem::~ThreadItem()
{
	//CloseHandle(thread);
}

bool ThreadItem::suspend()
{
	valid = false;
	if (HANDLE thread = OpenThread(THREAD_ALL_ACCESS, false, thread_id))
	{
		DWORD ret = SuspendThread(thread);
		CloseHandle(thread);
		return ret < 0;
	}
}

bool ThreadItem::resume()
{
	valid = false;
	if (HANDLE thread = OpenThread(THREAD_ALL_ACCESS, false, thread_id))
	{
		DWORD ret;
		while ((ret = ResumeThread(thread)) > 0);
		CloseHandle(thread);
		return ret < 0;
	}
}

bool ThreadItem::terminate()
{
	valid = false;
	if (HANDLE thread = OpenThread(THREAD_ALL_ACCESS, false, thread_id))
	{
		DWORD ret = TerminateThread(thread, NULL);
		CloseHandle(thread);
		return ret == 0;
	}
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