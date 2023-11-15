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
	virtual bool suspend() override;
	virtual bool terminate() override;
	virtual bool resume() override;
	virtual bool isSuspended() const override { return suspended; }
	virtual LPCWSTR getName() const override { return name.c_str(); }
	virtual DWORD getId() const override { return thread_id; }
	virtual ~ThreadItem();

	bool update(_SYSTEM_THREAD_INFORMATION* info);

	bool operator<(const ThreadItem& obj) const { return thread_id < obj.getId(); }
	bool operator==(const ThreadItem& obj) const { return thread_id == obj.getId(); }

protected:
	size_t thread_id;
	std::wstring name;
	bool suspended;
};