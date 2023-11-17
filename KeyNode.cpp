#include "KeyNode.h"
#include "ValueNode.h"

#include <stdexcept>


const std::map<std::wstring, HKEY> KeyNode::PREDEFINED_NAMES = {
	{ L"HKEY_CLASSES_ROOT", HKEY_CLASSES_ROOT },
	{ L"HKEY_CURRENT_CONFIG", HKEY_CURRENT_CONFIG },
	{ L"HKEY_CURRENT_USER", HKEY_CURRENT_USER },
	{ L"HKEY_LOCAL_MACHINE", HKEY_LOCAL_MACHINE },
	{ L"HKEY_USERS", HKEY_USERS }
};

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
		parent->children.insert(this);
		node.hParent = parent->getNode().item.hItem;
	}

	node.hInsertAfter = TVI_SORT;

	node.item.mask = TVIF_PARAM | TVIF_TEXT;
	node.item.lParam = (LPARAM)this;
	node.item.pszText = (LPWSTR)this->name.c_str();
	node.item.cchTextMax = this->name.size() + 1;

	node.item.hItem = (HTREEITEM)SendMessage(tree, TVM_INSERTITEM, 0, (LPARAM)&node);

	HKEY key;
	open(&key);

	LPWSTR childName = new WCHAR[MAX_NAME_SIZE];

	DWORD realSize;

	realSize = MAX_NAME_SIZE;

	if (RegEnumKeyExW(key, NULL, childName, &realSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
		new KeyNode(tree, childName, this);

	realSize = MAX_NAME_SIZE;

	if (RegEnumValueW(key, NULL, childName, &realSize, NULL, NULL, NULL, NULL) == ERROR_SUCCESS)
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
	SendMessage(tree, TVM_DELETEITEM, 0, (LPARAM)node.item.hItem);
}

void KeyNode::initialize(int depth)
{
	if (depth == 0)
	{
		if (children.size())
		{
			while (children.size() != 1)
				delete* children.begin();
		}
		return;
	}

	HKEY key;
	open(&key);

	LPWSTR childName = new WCHAR[MAX_NAME_SIZE];

	DWORD realSize = MAX_NAME_SIZE;

	LSTATUS some, some1;
	
	for (auto& ptr : children)
	{
		KeyNode* childKey = dynamic_cast<KeyNode*>(ptr);
		if (childKey != NULL)
			childKey->initialize(depth - 1);
	}

	for (DWORD i = 0; (some = RegEnumKeyExW(key, i, childName, &realSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS; ++i)
	{
		realSize = MAX_NAME_SIZE;

		bool exists = false;

		for (auto& ptr : children)
		{
			if (ptr->getName() == childName)
			{
				exists = true;
				break;
			}
		}

		if (!exists)
		{
			KeyNode* childKey;
			childKey = new KeyNode(tree, childName, this);
			childKey->initialize(depth - 1);
		}
	}

	some1 = some;

	for (DWORD i = 0; (some = RegEnumValueW(key, i, childName, &realSize, NULL, NULL, NULL, NULL)) == ERROR_SUCCESS; ++i)
	{
		realSize = MAX_NAME_SIZE;

		bool exists = false;

		for (auto& ptr : children)
		{
			if (ptr->getName() == childName)
			{
				exists = true;
				break;
			}
		}

		if (!exists)
		{
			new ValueNode(tree, childName, this);
		}
	}

	some1 = some;

	delete[] childName;

	RegCloseKey(key);
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

void KeyNode::open(PHKEY key)
{
	auto& it1 = PREDEFINED_NAMES.find(name);
	auto& it2 = PREDEFINED_NAMES.end();
	if (PREDEFINED_NAMES.find(name) != PREDEFINED_NAMES.end())
	{
		*key = PREDEFINED_NAMES.at(name);
	}
	else
	{
		HKEY pKey;
		parent->open(&pKey);
		LSTATUS some;
		if ((some = RegCreateKeyExW(pKey, name.c_str(), NULL, NULL, NULL, KEY_ALL_ACCESS, NULL, key, NULL)) == ERROR_ACCESS_DENIED)
			some = RegCreateKeyExW(pKey, name.c_str(), NULL, NULL, NULL, KEY_READ, NULL, key, NULL) == ERROR_ACCESS_DENIED;
		LSTATUS some1 = some;
		RegCloseKey(pKey);
	}
}