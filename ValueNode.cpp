#include "ValueNode.h"
#include "KeyNode.h"

#include <stdexcept>

const DWORD ValueNode::DEFAULT_TYPE = REG_SZ;
const BYTE ValueNode::DEFAULT_VALUE[] = L"DEFAULT";

ValueNode::ValueNode(HWND tree, LPCWSTR name, KeyNode* parent) : BaseNode(tree, name), parent(parent)
{
	for (auto& ptr : parent->children)
	{
		if (ptr->getName() == this->name)
		{
			throw std::invalid_argument("File with such name already exists");
		}
	}

	if (parent != NULL)
		node.hParent = parent->getNode().item.hItem;
	node.hInsertAfter = TVI_SORT;

	node.item.mask = TVIF_PARAM | TVIF_TEXT;
	node.item.lParam = (LPARAM)this;
	node.item.pszText = this->name.c_str();
	node.item.cchTextMax = this->name.size();

	node.item.hItem = (HTREEITEM)SendMessage(tree, TVM_INSERTITEM, 0, (LPARAM)&node);

	parent->children.insert(this);

	HKEY key;
	parent->open(&key);
	if (RegGetValueW(key, NULL, this->name.c_str(), NULL, NULL, NULL, NULL) == ERROR_FILE_NOT_FOUND)
		RegSetValueExW(key, this->name.c_str(), NULL, DEFAULT_TYPE, DEFAULT_VALUE, sizeof(DEFAULT_VALUE));
	RegCloseKey(key);
}

ValueNode::~ValueNode()
{
	parent->children.erase(this);
	SendMessage(tree, TVM_DELETEITEM, 0, (LPARAM)&node.item.hItem);
}
	
void ValueNode::set(DWORD type, BYTE* data, DWORD dataSize)
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
	RegGetValueW(key, NULL, this->name.c_str(), NULL, type, data, dataSize);
	RegCloseKey(key);
}

void ValueNode::remove()
{
	HKEY key;
	parent->open(&key);
	RegDeleteValueW(key, this->name.c_str());
	RegCloseKey(key);
}