#include "stdafx.h"
#include "RegistryHandler.h"


RegistryHandler::RegistryHandler()
{
}

HKEY RegistryHandler::OpenKey(HKEY hRootKey, wchar_t* strKey){
	HKEY hKey;
	LONG nError = RegOpenKeyEx(hRootKey, strKey, NULL, KEY_ALL_ACCESS, &hKey);

	if (nError == ERROR_FILE_NOT_FOUND){
		printf("Creating registry key: %ws\n",strKey);
		nError = RegCreateKeyEx(hRootKey, strKey, NULL, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &hKey, NULL);
	}

	if (nError)
		printf("Error: %d Could not find or create %ws\n",nError,strKey);

	return hKey;
}

void RegistryHandler::SetVal(HKEY hKey, LPCTSTR lpValue, LPCTSTR data){
	printf("RegistrySetVal %ws\n", data);
	LONG nError = RegSetValueEx(hKey, lpValue, NULL, REG_SZ, (LPBYTE)data, wcslen(data) * 2 + 1);

	if (nError)
		printf("Error: %d Could not set registry value: %ws\n", nError, lpValue);
}

LPCTSTR RegistryHandler::GetVal(HKEY hKey, LPCTSTR lpValue){
	LPCTSTR data;
	DWORD size = sizeof(data);	
	DWORD type = REG_SZ;
	LONG nError = RegQueryValueEx(hKey, lpValue, NULL, &type, (LPBYTE)&data, &size);

	if (nError == ERROR_FILE_NOT_FOUND)
		data = 0; // The value will be created and set to data next time SetVal() is called.
	else if (nError)
		printf("Error: %d Could not get registry value %s\n", nError, (char*)lpValue);

	return data;
}
RegistryHandler::~RegistryHandler()
{
}
