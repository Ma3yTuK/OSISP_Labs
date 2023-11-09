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
	virtual void suspend() override;
	virtual void terminate() override;
	virtual void resume() override;
	virtual bool isSuspended() override { return suspended; }
	virtual LPCWSTR getName() override { return name.c_str(); }
	virtual HANDLE getHandle() override { return thread; }
	virtual ~ThreadItem();

	bool update(_SYSTEM_THREAD_INFORMATION* info);

protected:
	HANDLE thread;
	std::wstring name;
	bool suspended;
};