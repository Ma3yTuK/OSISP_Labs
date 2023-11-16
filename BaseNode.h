#pragma once

#include <windows.h>
#include <Windowsx.h>
#include <d2d1.h>
#include <Winuser.h>

#include <list>
#include <memory>

#include <objidl.h>
#include <gdiplus.h>
#include <winternl.h>
#include <CommCtrl.h>
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
 