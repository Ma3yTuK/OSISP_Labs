#pragma once

#include <commctrl.h>
#include <Windows.h>
#include <string>

class BaseNode
{
public:
	BaseNode(HWND tree, LPCWSTR name) : tree(tree), name(name), node() {}
	virtual ~BaseNode() = default;

	const TVINSERTSTRUCTW& getNode() const { return node; }
	const std::wstring& getName() const { return name; }

	virtual void remove() = 0;

protected:
	HWND tree;

	std::wstring name;

	TVINSERTSTRUCTW node;
}; 
 