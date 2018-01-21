#pragma once
#include <Windows.h>
class RegistryHandler
{
public:
	RegistryHandler();
	~RegistryHandler();
	HKEY OpenKey(HKEY hRootKey, wchar_t* strKey);
	void SetVal(HKEY hKey, LPCTSTR lpValue, LPCTSTR data);
	LPCTSTR GetVal(HKEY hKey, LPCTSTR lpValue);
};

