#pragma once

#include "BaseNode.h"

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

	void initialize(int depth);

	virtual void remove() override;

protected:
	void open(PHKEY key);

	KeyNode* parent;

	std::set<BaseNode*> children;

	friend class ValueNode;
};