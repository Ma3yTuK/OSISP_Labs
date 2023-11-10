#include "ProcessNode.h"

#include <strsafe.h>

const size_t ProcessNode::COLUMNT_COUNT = 1;
const size_t ProcessNode::PU_COLUMNS[] = { 1 };
const size_t ProcessNode::STR_SIZE = 20;
LPCWSTR ProcessNode::SUSPEND_VAL = L"Suspended";
LPCWSTR ProcessNode::NOT_SUSPEND_VAL = L"Active";

ProcessNode::ProcessNode(_SYSTEM_PROCESS_INFORMATION* info, HWND tree, const BaseNode* parent, const BaseNode* prev) : tree(tree), data(ProcessItem(info))
{
	node = TVINSERTSTRUCTW();

	if (parent != NULL)
		node.hParent = parent->getNode().item.hItem;
	node.hInsertAfter = prev->getNode().item.hItem;

	node.item.mask = TVIF_PARAM | TVIF_TEXT;
	node.item.lParam = (LPARAM)&data;
	node.item.pszText = new WCHAR[STR_SIZE];
	node.item.cchTextMax = STR_SIZE;
	StringCchCopy(node.item.pszText, node.item.cchTextMax, data.getName());

	TreeView_InsertItem(tree, &node);

	ThreadNode* after = NULL;

	for (auto& thread : *data.getThreads())
	{
		threads.emplace_back(&thread, tree, this, after);
		after = &threads.back();
	}
}

void ProcessNode::update(_SYSTEM_PROCESS_INFORMATION* info)
{
	_SYSTEM_THREAD_INFORMATION* threadsData = (_SYSTEM_THREAD_INFORMATION*)(info + 1);

	std::qsort(
		threadsData,
		info->NumberOfThreads,
		sizeof(_SYSTEM_THREAD_INFORMATION),
		[](const void* x, const void* y) {
			const _SYSTEM_THREAD_INFORMATION* arg1 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(x);
			const _SYSTEM_THREAD_INFORMATION* arg2 = static_cast<const _SYSTEM_THREAD_INFORMATION*>(y);
			return ((GetThreadId(arg1->ClientId.UniqueThread) > GetThreadId(arg2->ClientId.UniqueThread)) - (GetThreadId(arg1->ClientId.UniqueThread) < GetThreadId(arg2->ClientId.UniqueThread)));
		}
	);

	data.suspended = true;
	ThreadNode* after = NULL;
	size_t i = 0, j = 0;
	while (i < threads.size() && j < info->NumberOfThreads)
	{
		while (i < threads.size() && j < info->NumberOfThreads && GetThreadId((threadsData + j)->ClientId.UniqueThread) < GetThreadId(threads[i].getHandle()))
		{
			if ((threadsData + j)->WaitReason != 5)
				data.suspended = false;
			data.threads.emplace(data.threads.begin() + i, threadsData + j);
			threads.emplace(threads.begin() + i, &data.threads.back(), tree, this, after);
			after = &threads[i];
			j++;
			i++;
		}
		while (i < threads.size() && j < info->NumberOfThreads && GetThreadId((threadsData + j)->ClientId.UniqueThread) == GetThreadId(threads[i].getHandle()))
		{
			if ((threadsData + j)->WaitReason != 5)
				data.suspended = false;
			data.threads[i].update(threadsData + j);
			after = &threads[i];
			j++;
			i++;
		}
		while (i < threads.size() && j < info->NumberOfThreads && GetThreadId((threadsData + j)->ClientId.UniqueThread) > GetThreadId(threads[i].getHandle()))
		{
			data.threads.erase(data.threads.begin() + i);
			threads.erase(threads.begin() + i);
		}
	}

	while (i < threads.size())
	{
		data.threads.erase(data.threads.begin() + i);
		threads.erase(threads.begin() + i);
	}

	while (j < info->NumberOfThreads)
	{
		if ((threadsData + j)->WaitReason != 5)
			data.suspended = false;
		data.threads.emplace_back(threadsData + j);
		threads.emplace_back(&data.threads.back(), tree, this, after);
		after = &threads.back();
		j++;
	}

	data.valid = true;
}

ProcessNode::~ProcessNode()
{
	delete[] node.item.pszText;
	ListView_DeleteItem(tree, node.item.hItem);
}