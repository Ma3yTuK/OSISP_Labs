#pragma once

#include "ThreadItem.h"

#include <vector>

class ProcessItem : public BaseItem
{
public:
	ProcessItem(_SYSTEM_PROCESS_INFORMATION* processInfo);
	virtual void suspend() override;
	virtual void terminate() override;
	virtual void resume() override;
	virtual bool isSuspended() override { return suspended; }
	virtual LPCWSTR getName() override { return name.c_str(); }
	virtual HANDLE getHandle() override { return process; }
	virtual ~ProcessItem();

	std::vector<ThreadItem>* getThreads() { return &threads; }
	void update(_SYSTEM_PROCESS_INFORMATION* info);

protected:
	HANDLE process;
	std::wstring name;
	bool suspended;

	std::vector<ThreadItem> threads;
};