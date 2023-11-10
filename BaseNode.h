#pragma once

#include <commctrl.h>


class BaseNode
{
public:
	TVINSERTSTRUCTW getNode() { return node; }
	const TVINSERTSTRUCTW getNode() const { return node; }

protected:
	TVINSERTSTRUCTW node;
}; 
 