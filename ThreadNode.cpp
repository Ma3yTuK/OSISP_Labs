#include "ThreadNode.h"

#include <strsafe.h>

const size_t ThreadNode::COLUMNT_COUNT = 1;
const size_t ThreadNode::PU_COLUMNS[] = { 1 };
const size_t ThreadNode::STR_SIZE = 20;
LPCWSTR ThreadNode::SUSPEND_VAL = L"Suspended";
LPCWSTR NOT_SUSPEND_VAL = L"Active";

ThreadNode::ThreadNode(_SYSTEM_THREAD_INFORMATION* info, HWND tree, int index, int group) : tree(tree), nodes()
{
	data = ThreadItem(info);

	nodes.reserve(COLUMNT_COUNT);

	LVITEMW node;

	node = LVITEMW();
	node.mask = LVIF_COLUMNS | LVIF_GROUPID | LVIF_TEXT | LVIF_STATE;
	node.iItem = index;
	node.iSubItem = NULL;
	node.state = NULL;
	node.stateMask = NULL;
	node.iGroupId = group;
	node.cColumns = COLUMNT_COUNT;
	node.puColumns = PU_COLUMNS;
	node.pszText = new WCHAR[STR_SIZE];
	node.cchTextMax = STR_SIZE;
	StringCchCopy(node.pszText, node.cchTextMax, data.getName());
	nodes.push_back(node);
	TreeView_InsertItem(tree, &nodes.back());

	node = LVITEMW();
	node.mask = LVIF_TEXT;
	node.iItem = index;
	node.iSubItem = 1;
	node.pszText = new WCHAR[STR_SIZE];
	node.cchTextMax = STR_SIZE;
	if (data.isSuspended())
		StringCchCopy(node.pszText, node.cchTextMax, SUSPEND_VAL);
	else
		StringCchCopy(node.pszText, node.cchTextMax, NOT_SUSPEND_VAL);
	nodes.push_back(node);
	TreeView_InsertItem(tree, &nodes.back());
}

bool ThreadNode::update(_SYSTEM_THREAD_INFORMATION* info, int index)
{
	LVITEMW node = nodes.front();

	bool updated = false;

	if (node.iItem != index)
	{
		node.iItem = index;
		updated = true;
	}

	if (data.update(info))
	{
		StringCchCopy(node.pszText, node.cchTextMax, data.getName());

		if (data.isSuspended())
			StringCchCopy(node.pszText, node.cchTextMax, SUSPEND_VAL);
		else
			StringCchCopy(node.pszText, node.cchTextMax, NOT_SUSPEND_VAL);

		updated = true;
	}

	if (updated)
	{
		ListView_Update(tree, node.iItem);
	}

	return updated;
}

ThreadNode::~ThreadNode() 
{
	for (size_t i = 0; i < COLUMNT_COUNT; ++i)
	{
		delete[] nodes[i].pszText;
		ListView_DeleteItem(tree, nodes[i].iItem);
	}
}