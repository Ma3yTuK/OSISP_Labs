#include "ValueNode.h"
#include "KeyNode.h"

#include <stdexcept>

const DWORD ValueNode::DEFAULT_TYPE = REG_SZ;
const LPCWSTR ValueNode::DEFAULT_NAME = L"DEFAULT";
const BYTE *ValueNode::DEFAULT_VALUE = (BYTE*)L"DEFAULT";

ValueNode::ValueNode(HWND tree, LPCWSTR name, KeyNode* parent) : BaseNode(tree, name), parent(parent)
{
	for (auto& ptr : parent->children)
	{
		if (ptr->getName() == this->name)
		{
			throw std::invalid_argument("File with such name already exists");
		}
	}
	parent->children.insert(this);

	node.hParent = parent->getNode().item.hItem;
	node.hInsertAfter = TVI_SORT;

	node.item.mask = TVIF_PARAM | TVIF_TEXT;
	node.item.lParam = (LPARAM)this;
	if (this->name.size())
		node.item.pszText = (LPWSTR)this->name.c_str();
	else
		node.item.pszText = (LPWSTR)DEFAULT_NAME;
	node.item.cchTextMax = this->name.size() + 1;

	node.item.hItem = (HTREEITEM)SendMessage(tree, TVM_INSERTITEM, 0, (LPARAM)&node);

	HKEY key;
	parent->open(&key);
	if (RegGetValueW(key, NULL, this->name.c_str(), NULL, NULL, NULL, NULL) == ERROR_FILE_NOT_FOUND)
		RegSetValueExW(key, this->name.c_str(), NULL, DEFAULT_TYPE, DEFAULT_VALUE, sizeof(DEFAULT_VALUE));
	RegCloseKey(key);
}

ValueNode::~ValueNode()
{
	parent->children.erase(this);
	SendMessage(tree, TVM_DELETEITEM, 0, (LPARAM)node.item.hItem);
}
	
void ValueNode::set(DWORD type, const BYTE* data, DWORD dataSize)
{
	HKEY key;
	parent->open(&key);
	RegSetValueExW(key, this->name.c_str(), NULL, type, data, dataSize);
	RegCloseKey(key);
}

void ValueNode::get(DWORD* type, BYTE* data, DWORD* dataSize)
{
	HKEY key;
	parent->open(&key);
	LSTATUS some = RegGetValueW(key, NULL, this->name.c_str(), RRF_RT_ANY, type, data, dataSize);
	LSTATUS some1 = some;
	RegCloseKey(key);
}

void ValueNode::remove()
{
	HKEY key;
	parent->open(&key);
	RegDeleteValueW(key, this->name.c_str());
	RegCloseKey(key);
}