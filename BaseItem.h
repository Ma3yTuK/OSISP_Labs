#pragma once

#include <Windows.h>

class BaseItem
{
public:
	virtual void suspend() = 0;
	virtual void terminate() = 0;
	virtual void resume() = 0;
	virtual bool isSuspended() = 0;
	virtual LPCWSTR getName() = 0;
};