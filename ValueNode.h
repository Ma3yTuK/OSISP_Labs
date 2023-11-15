#pragma once

#include "BaseNode.h"

class KeyNode;

class ValueNode : public BaseNode
{
	static const DWORD DEFAULT_TYPE;
	static const BYTE DEFAULT_VALUE[];

public:
	ValueNode(HWND tree, LPCWSTR name, KeyNode* parent);
	virtual ~ValueNode() override;

	void set(DWORD type, BYTE* data, DWORD dataSize);
	void get(DWORD* type, BYTE* data, DWORD* dataSize);

	virtual void remove() override;

protected:
	KeyNode* parent;

	friend KeyNode;
};