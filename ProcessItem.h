#pragma once

#include "ThreadItem.h"

#include <vector>
#include <deque>

class ProcessItem : public BaseItem
{
private:
	static LPCWSTR NAME_PREFIX;

public:
	bool suspended;
	size_t process_id;
	std::wstring name;
	std::deque<ThreadItem*> threads;

	ProcessItem(_SYSTEM_PROCESS_INFORMATION* processInfo);
	virtual bool suspend() override;
	virtual bool terminate() override;
	virtual bool resume() override;
	virtual bool isSuspended() const override { return suspended; }
	virtual LPCWSTR getName() const override { return name.c_str(); }
	virtual DWORD getId() const override { return process_id; }
	virtual ~ProcessItem();

	std::deque<ThreadItem*>* getThreads() { return &threads; }
	bool update(_SYSTEM_PROCESS_INFORMATION* info);
};