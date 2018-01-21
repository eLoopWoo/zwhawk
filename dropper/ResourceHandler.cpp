#include "stdafx.h"
#include "ResourceHandler.h"


ResourceHandler::ResourceHandler()
{
}

// Encapsulate driver_name in dropper_name
bool ResourceHandler::encapsulation(wchar_t *dropper_name, wchar_t *driver_name) {
	HANDLE hFileSys;
	DWORD dwFileSize, dwBytesRead;
	LPBYTE lpBuffer;
	HANDLE hResourceEXE;
	wchar_t aSysPath[1024];
	wchar_t aCurrentDirectory[515];
	wchar_t aExePath[1024];

	GetCurrentDirectory(512, aCurrentDirectory);

	_snwprintf_s(aSysPath, 1024, 1024, L"%s\\%s.sys", aCurrentDirectory, driver_name);
	_snwprintf_s(aExePath, 1024, 1024, L"%s\\%s", aCurrentDirectory, dropper_name);
	wprintf(L"%s\n", aExePath, dropper_name);
	wprintf(L"%s\n%s\n", aSysPath, driver_name);
	hFileSys = CreateFile(aSysPath, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	// If CreateFile succeeded
	if (INVALID_HANDLE_VALUE == hFileSys) {
		printf("encapsulation - CreateFile failed (%Iu)\n", GetLastError());
		return false;
	}

	dwFileSize = GetFileSize(hFileSys, NULL);
	if (!(dwFileSize)) {
		printf("encapsulation - GetFileSize failed (%Iu)\n", GetLastError());
		CloseHandle(hFileSys);
		return false;
	}
	lpBuffer = new BYTE[dwFileSize];

	if (!(ReadFile(hFileSys, lpBuffer, dwFileSize, &dwBytesRead, NULL))) {
		printf("encapsulation - ReadFile failed (%Iu)\n", GetLastError());
		CloseHandle(hFileSys);
		free(lpBuffer);
		return false;
	}
	CloseHandle(hFileSys);

	hResourceEXE = BeginUpdateResource((LPCTSTR)aExePath, FALSE);
	if (!(hResourceEXE)) {
		printf("encapsulation - BeginUpdateResource failed (%Iu)\n", GetLastError());
		free(lpBuffer);
		return false;
	}

	if (!(UpdateResource(hResourceEXE, RT_RCDATA, MAKEINTRESOURCE(10), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPVOID)lpBuffer, dwFileSize))) {
		printf("encapsulation - UpdateResource failed (%Iu)\n", GetLastError());
		CloseHandle(hResourceEXE);
		free(lpBuffer);
		return false;
	}
	printf("%p\n", hResourceEXE);
	if (!(EndUpdateResource(hResourceEXE, FALSE))) {
		printf("encapsulation - EndUpdateResource failed (%Iu)\n", GetLastError());
		CloseHandle(hResourceEXE);
		free(lpBuffer);
		return false;
	}
	CloseHandle(hResourceEXE);
	free(lpBuffer);
	return true;
}

// Decapsulate from dropper_name resource driver_name
bool ResourceHandler::decapsulation(wchar_t *dropper_name, wchar_t *driver_name) {
	HMODULE hLibrary;
	HRSRC hResource;
	HGLOBAL hResourceLoaded;
	DWORD dwBytesWritten;
	LPBYTE lpBuffer;
	DWORD dwFileSize;
	HANDLE hFileSys;

	wchar_t aExePath[1024];
	wchar_t aSysPath[1024];

	wchar_t aCurrentDirectory[515];


	GetCurrentDirectory(512, aCurrentDirectory);
	_snwprintf_s(aExePath, 1024, 1024, L"%s\\%s", aCurrentDirectory, dropper_name);
	_snwprintf_s(aSysPath, 1024, 1024, L"%s\\%s.sys", aCurrentDirectory, driver_name);

	hLibrary = LoadLibrary(aExePath);
	if (!(hLibrary)) {
		printf("decapsulation - LoadLibrary failed (%Iu)\n", GetLastError());
		return false;
	}
	//printf("%d\n", hLibrary);
	hResource = FindResource(hLibrary, MAKEINTRESOURCE(10), MAKEINTRESOURCE(10));
	if (!(hResource)) {
		printf("decapsulation - FindResource failed (%Iu)\n", GetLastError());
		FreeLibrary(hLibrary);
		return false;
	}

	hResourceLoaded = LoadResource(hLibrary, hResource);
	if (!(hResourceLoaded)) {
		printf("decapsulation - LoadResource failed (%Iu)\n", GetLastError());
		FreeLibrary(hLibrary);
		CloseHandle(hResource);
		return false;
	}


	lpBuffer = (LPBYTE)LockResource(hResourceLoaded);
	if (!(lpBuffer)) {
		printf("decapsulation - LockResource failed (%Iu)\n", GetLastError());
		FreeLibrary(hLibrary);
		CloseHandle(hResource);
		return false;
	}

	dwFileSize = SizeofResource(hLibrary, hResource);

	hFileSys = CreateFile(aSysPath, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (INVALID_HANDLE_VALUE == hFileSys) {
		printf("decapsulation - CreateFile failed (%Iu)\n", GetLastError());
		FreeLibrary(hLibrary);
		CloseHandle(hResource);
		return false;
	}

	if (!(WriteFile(hFileSys, lpBuffer, dwFileSize, &dwBytesWritten, NULL))) {
		printf("decapsulation - WriteFile failed (%Iu)\n", GetLastError());
		FreeLibrary(hLibrary);
		CloseHandle(hResource);
		CloseHandle(hFileSys);
		return false;
	}

	FreeLibrary(hLibrary);
	CloseHandle(hResource);
	CloseHandle(hFileSys);
	return true;
}

bool ResourceHandler::is_exist(wchar_t *dropper_name, wchar_t *driver_name) {
	HMODULE hLibrary;
	HRSRC hResource;
	wchar_t aExePath[1024];
	wchar_t aCurrentDirectory[515];
	GetCurrentDirectory(512, aCurrentDirectory);
	_snwprintf_s(aExePath, 1024, 1024, L"%s\\%s", aCurrentDirectory, dropper_name);
	hLibrary = LoadLibrary(aExePath);
	if (!(hLibrary)) {
		printf("is_exist - LoadLibrary failed (%Iu)\n", GetLastError());
		return false;
	}
	hResource = FindResource(hLibrary, MAKEINTRESOURCE(10), MAKEINTRESOURCE(10));
	if (!(hResource)) {
		printf("is_exist - FindResource failed (%Iu)\n", GetLastError());
		FreeLibrary(hLibrary);
		return false;
	}
	FreeLibrary(hLibrary);
	CloseHandle(hResource);
	return true;
}

ResourceHandler::~ResourceHandler()
{
}
