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
	HANDLE process;
	std::wstring name;
	std::deque<ThreadItem> threads;

	ProcessItem(_SYSTEM_PROCESS_INFORMATION* processInfo);
	ProcessItem(const ProcessItem& obj);
	virtual void suspend() override;
	virtual void terminate() override;
	virtual void resume() override;
	virtual bool isSuspended() const override { return suspended; }
	virtual LPCWSTR getName() const override { return name.c_str(); }
	virtual HANDLE getHandle() const override { return process; }
	virtual ~ProcessItem();

	std::deque<ThreadItem>* getThreads() { return &threads; }
	bool update(_SYSTEM_PROCESS_INFORMATION* info);
};