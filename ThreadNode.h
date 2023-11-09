#pragma once

#include "ProcessItem.h"

#include <commctrl.h>


class ThreadNode
{
	static const size_t COLUMNT_COUNT;
	static const size_t STR_SIZE;
	static const size_t PU_COLUMNS[];
	static LPCWSTR SUSPEND_VAL;
	static LPCWSTR NOT_SUSPEND_VAL;

public:
	ThreadNode(_SYSTEM_THREAD_INFORMATION* info, HWND tree, int index, int group);
	HANDLE getHandle() { data.getHandle(); }
	bool update(_SYSTEM_THREAD_INFORMATION* info, int index);
	~ThreadNode();

protected:
	ThreadItem data;
	std::vector<LVITEMW> nodes;
	HWND tree;
};