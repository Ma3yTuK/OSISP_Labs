#pragma once

#include "BaseItem.h"

#include <winternl.h>
#include <string>

class ThreadItem : public BaseItem
{
private:
	static LPCWSTR NAME_PREFIX;

public:
	ThreadItem(_SYSTEM_THREAD_INFORMATION* info);
	ThreadItem(const ThreadItem& obj);
	virtual void suspend() override;
	virtual void terminate() override;
	virtual void resume() override;
	virtual bool isSuspended() const override { return suspended; }
	virtual LPCWSTR getName() const override { return name.c_str(); }
	virtual HANDLE getHandle() const override { return thread; }
	virtual ~ThreadItem();

	bool update(_SYSTEM_THREAD_INFORMATION* info);

	bool operator<(const ThreadItem& obj) const { return GetThreadId(thread) < GetThreadId(obj.getHandle()); }
	bool operator==(const ThreadItem& obj) const { return GetThreadId(thread) < GetThreadId(obj.getHandle()); }

protected:
	HANDLE thread;
	std::wstring name;
	bool suspended;
};