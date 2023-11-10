#pragma once

#include "ThreadNode.h"

#include <commctrl.h>


class ProcessNode : public BaseNode
{
	static const size_t COLUMNT_COUNT;
	static const size_t STR_SIZE;
	static const size_t PU_COLUMNS[];
	static LPCWSTR SUSPEND_VAL;
	static LPCWSTR NOT_SUSPEND_VAL;

public:
	ProcessNode(_SYSTEM_PROCESS_INFORMATION* info, HWND tree, const BaseNode* parent, const BaseNode* after);
	void update(_SYSTEM_PROCESS_INFORMATION* info);
	HANDLE getHandle() { data.getHandle(); }
	~ProcessNode();

protected:
	ProcessItem data;
	std::deque<ThreadNode> threads;
	HWND tree;
};