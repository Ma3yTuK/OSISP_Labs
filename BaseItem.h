#pragma once

#include <windows.h>

class BaseItem
{
public:
	BaseItem() : valid(true) {}

	virtual void suspend() = 0;
	virtual void terminate() = 0;
	virtual void resume() = 0;
	virtual bool isSuspended() = 0;
	virtual LPCWSTR getName() = 0;
	virtual HANDLE getHandle() = 0;
	virtual ~BaseItem() = 0;

	bool isValid() { return valid; }
	void invalidate() { valid = false; }

protected:
	bool valid;
};