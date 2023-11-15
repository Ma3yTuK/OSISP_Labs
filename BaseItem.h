#pragma once

#include <windows.h>

class BaseItem
{
public:
	BaseItem() : valid(true) {}
	bool valid;

	virtual bool suspend() = 0;
	virtual bool terminate() = 0;
	virtual bool resume() = 0;
	virtual bool isSuspended() const = 0;
	virtual LPCWSTR getName() const = 0;
	virtual DWORD getId() const = 0;
	virtual ~BaseItem() = default;

	bool isValid() { return valid; }
	void invalidate() { valid = false; }
};