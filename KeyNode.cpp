#include "KeyNode.h"
#include "ValueNode.h"

#include <stdexcept>


const std::map<std::wstring, HKEY> KeyNode::PREDEFINED_NAMES = {
	{ L"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT },
	{ L"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG },
	{ L"HKEY_CURRENT_USER", HKEY_CURRENT_USER },
	{ L"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE },
	{ L"HKEY_USERS", HKEY_USERS }
}

const size_t KeyNode::MAX_NAME_SIZE = 256;


KeyNode::KeyNode(HWND tree, LPCWSTR name, KeyNode* parent) : BaseNode(tree, name), parent(parent), children()
{
	if (parent != NULL)
	{
		for (auto& ptr : parent->children)
		{
			if (ptr->getName() == this->name)
			{
				throw std::invalid_argument("File with such name already exists");
			}
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

	HKEY key;
	open(&key);

	LPWSTR childName = new WCHAR[MAX_NAME_SIZE];

	DWORD realSize = MAX_NAME_SIZE;

	for (DWORD i = 0; RegEnumKeyExW(key, i, childName, &realSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i) 
		new KeyNode(tree, childName, this);

	for (DWORD i = 0; RegEnumValueW(key, i, childName, &realSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS; ++i)
		new ValueNode(tree, childName, this);

	delete[] childName;

	RegCloseKey(key);
}

KeyNode::~KeyNode()
{
	while (children.size())
		delete *children.begin();

	if (parent != NULL)
		parent->children.erase(this);
	SendMessage(tree, TVM_DELETEITEM, 0, (LPARAM)&node.item.hItem);
}

void KeyNode::remove()
{
	if (parent != NULL)
	{
		HKEY key;
		parent->open(&key);

		RegDeleteTreeW(key, name.c_str());
		RegDeleteKeyW(key, name.c_str());

		RegCloseKey(key);
	}
}

LSTATUS KeyNode::open(PHKEY key)
{
	if (PREDEFINED_NAMES.find(name) != PREDEFINED_NAMES.end())
	{
		*key = PREDEFINED_NAMES[name];
	}
	else
	{
		HKEY pKey;
		parent->open(&pKey);
		RegCreateKeyExW(pKey, name.c_str(), NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, key, NULL);
		RegCloseKey(pKey);
	}
}