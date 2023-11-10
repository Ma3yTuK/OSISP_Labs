#pragma once

#include <windows.h>

class BaseItem
{
public:
	BaseItem() : valid(true) {}
	bool valid;

	virtual void suspend() = 0;
	virtual void terminate() = 0;
	virtual void resume() = 0;
	virtual bool isSuspended() const = 0;
	virtual LPCWSTR getName() const = 0;
	virtual HANDLE getHandle() const = 0;
	virtual ~BaseItem() = default;

	bool isValid() { return valid; }
	void invalidate() { valid = false; }
};