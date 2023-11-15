#pragma once

#include "ProcessItem.h"
#include "BaseNode.h"

#include <commctrl.h>


class ThreadNode : public BaseNode
{
	static const size_t COLUMNT_COUNT;
	static const size_t STR_SIZE;
	static const size_t PU_COLUMNS[];
	static LPCWSTR SUSPEND_VAL;
	static LPCWSTR NOT_SUSPEND_VAL;

public:
	ThreadNode(ThreadItem* data, HWND tree, const BaseNode* parent, const BaseNode* after);
	DWORD getId() { return data->getId(); }
	~ThreadNode();

protected:
	ThreadItem* data;
	HWND tree;
};