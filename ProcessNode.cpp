#include "ProcessNode.h"

#include <strsafe.h>

const size_t ProcessNode::COLUMNT_COUNT = 1;
const size_t ProcessNode::PU_COLUMNS[] = { 1 };
const size_t ProcessNode::STR_SIZE = 20;
LPCWSTR ProcessNode::SUSPEND_VAL = L"Suspended";
LPCWSTR ProcessNode::NOT_SUSPEND_VAL = L"Active";

ProcessNode::ProcessNode(_SYSTEM_PROCESS_INFORMATION* info, HWND tree, const BaseNode* parent, const BaseNode* prev) : tree(tree), data(info), threads()
{
	node = TVINSERTSTRUCTW();

	if (parent != NULL)
		node.hParent = parent->getNode().item.hItem;
	if (prev != NULL)
		node.hInsertAfter = prev->getNode().item.hItem;
	else
		node.hInsertAfter = TVI_FIRST;

	node.item.mask = TVIF_PARAM | TVIF_TEXT;
	node.item.lParam = (LPARAM)&data;
	node.item.pszText = new WCHAR[STR_SIZE];
	node.item.cchTextMax = STR_SIZE;
	StringCchCopy(node.item.pszText, node.item.cchTextMax, data.getName());

	node.item.hItem = (HTREEITEM)SendMessage(tree, TVM_INSERTITEM, 0, (LPARAM)&node);

	ThreadNode* after = NULL;

	for (size_t i = 0; i < data.threads.size(); i++)
	{
		threads.emplace(threads.begin() + i, new ThreadNode(data.threads[i], tree, this, after));
		after = threads.back();
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
			return ((int)((DWORD)(arg1->ClientId.UniqueThread) > (DWORD)(arg2->ClientId.UniqueThread)) - (int)((DWORD)(arg1->ClientId.UniqueThread) < (DWORD)(arg2->ClientId.UniqueThread)));
		}
	);

	data.suspended = true;
	ThreadNode* after = NULL;
	size_t i = 0, j = 0;
	while (i < threads.size() && j < info->NumberOfThreads)
	{
		while (i < threads.size() && j < info->NumberOfThreads && (DWORD)((threadsData + j)->ClientId.UniqueThread) < threads[i]->getId())
		{
			if ((threadsData + j)->WaitReason != 5)
				data.suspended = false;
			data.threads.emplace(data.threads.begin() + i, new ThreadItem(threadsData + j));
			threads.emplace(threads.begin() + i, new ThreadNode(data.threads[i], tree, this, after));
			after = threads[i];
			j++;
			i++;
		}
		while (i < threads.size() && j < info->NumberOfThreads && (DWORD)((threadsData + j)->ClientId.UniqueThread) == threads[i]->getId())
		{
			if ((threadsData + j)->WaitReason != 5)
				data.suspended = false;
			data.threads[i]->update(threadsData + j);
			after = threads[i];
			j++;
			i++;
		}
		while (i < threads.size() && j < info->NumberOfThreads && (DWORD)((threadsData + j)->ClientId.UniqueThread) > threads[i]->getId())
		{
			delete threads[i];
			threads.erase(threads.begin() + i);
			delete data.threads[i];
			data.threads.erase(data.threads.begin() + i);
		}
	}

	while (i < threads.size())
	{
		delete threads[i];
		threads.erase(threads.begin() + i);
		delete data.threads[i];
		data.threads.erase(data.threads.begin() + i);
	}

	while (j < info->NumberOfThreads)
	{
		if ((threadsData + j)->WaitReason != 5)
			data.suspended = false;
		data.threads.emplace(data.threads.begin() + i, new ThreadItem(threadsData + j));
		threads.emplace(threads.begin() + i, new ThreadNode(data.threads[i], tree, this, after));
		after = threads.back();
		j++;
		i++;
	}

	data.valid = true;
}

ProcessNode::~ProcessNode()
{
	for (auto& thread : threads)
		delete thread;

	TreeView_DeleteItem(tree, node.item.hItem);
	delete[] (WCHAR*)node.item.pszText;
}