#pragma once

#include "BaseNode.h"

#include <Windows.h>
#include <set>
#include <map>

class ValueNode;

class KeyNode : public BaseNode
{
public:
	static const std::map<std::wstring, HKEY> PREDEFINED_NAMES;
	static const size_t MAX_NAME_SIZE;

	KeyNode(HWND tree, LPCWSTR name, KeyNode* parent);
	virtual ~KeyNode() override;

	virtual void remove() override;

protected:
	LSTATUS open(PHKEY key);

	KeyNode* parent;

	std::set<BaseNode*> children;

	friend ValueNode;
};