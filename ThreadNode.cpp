#include "ThreadNode.h"

#include <strsafe.h>

const size_t ThreadNode::COLUMNT_COUNT = 1;
const size_t ThreadNode::PU_COLUMNS[] = { 1 };
const size_t ThreadNode::STR_SIZE = 20;
LPCWSTR ThreadNode::SUSPEND_VAL = L"Suspended";
LPCWSTR ThreadNode::NOT_SUSPEND_VAL = L"Active";

ThreadNode::ThreadNode(ThreadItem* data, HWND tree, const BaseNode* parent, const BaseNode* after) : data(data), tree(tree)
{
	node = TVINSERTSTRUCTW();

	if (parent != NULL)
		node.hParent = parent->getNode().item.hItem;
	if (after != NULL)
		node.hInsertAfter = after->getNode().item.hItem;

	node.item.mask = TVIF_PARAM | TVIF_TEXT;
	node.item.lParam = (LPARAM)data;
	node.item.pszText = new WCHAR[STR_SIZE];
	node.item.cchTextMax = STR_SIZE;
	StringCchCopy(node.item.pszText, node.item.cchTextMax, data->getName());

	TreeView_InsertItem(tree, &node);
}

ThreadNode::~ThreadNode() 
{
	TreeView_DeleteItem(tree, node.item.hItem);
	delete[] node.item.pszText;
}