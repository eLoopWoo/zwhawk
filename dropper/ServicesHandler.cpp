#include "stdafx.h"
#include "ServicesHandler.h"


ServicesHandler::ServicesHandler()
{
}

// Load driver to kernel space
bool ServicesHandler::load_kernel_code_scm(wchar_t *driver_name) {
	wchar_t aPath[1024];
	wchar_t aCurrentDirectory[515];

	SC_HANDLE sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!sh) {
		printf("load_kernel_code_scm - OpenSCManager failed (%Iu)\n", GetLastError());
		return false;
	}

	GetCurrentDirectory(512, aCurrentDirectory);
	_snwprintf_s(aPath, 1022, 1024, L"%s\\%s.sys", aCurrentDirectory, driver_name);
	printf("load_kernel_code_scm - loading %ws\n", aPath);

	SC_HANDLE rh = CreateService(sh,
		driver_name,
		driver_name,
		SERVICE_ALL_ACCESS,
		SERVICE_KERNEL_DRIVER,
		SERVICE_SYSTEM_START,
		SERVICE_ERROR_NORMAL,
		aPath,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL);

	if (!rh) {
		if (GetLastError() == ERROR_SERVICE_EXISTS) {
			rh = OpenService(sh, driver_name, SERVICE_ALL_ACCESS);
			if (!rh) {
				printf("load_kernel_code_scm - OpenService failed (%Iu)\n", GetLastError());
				CloseServiceHandle(sh);
				return false;
			}
		}
		else {
			printf("load_kernel_code_scm - CreateService failed (%Iu)\n", GetLastError());
			CloseServiceHandle(sh);
			return false;
		}
	}

	// start the drivers
	if (rh) {
		if (0 == StartService(rh, 0, NULL)) {
			if (ERROR_SERVICE_ALREADY_RUNNING == GetLastError()) {
				printf("load_kernel_code_scm - SERVICE_ALREADY_RUNNING");

			}
			else {
				printf("load_kernel_code_scm - StartService failed (%Iu)\n", GetLastError());
				CloseServiceHandle(sh);
				CloseServiceHandle(rh);
				return false;
			}
		}
		CloseServiceHandle(sh);
		CloseServiceHandle(rh);
	}
	return true;
}

// Unload driver from kernel space
bool ServicesHandler::unload_kernel_code_scm(wchar_t *driver_name) {
	SC_HANDLE sh = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
	if (!sh) {
		printf("unload_kernel_code_scm - OpenSCManager failed (%Iu)\n", GetLastError());
		return false;
	}
	SC_HANDLE rh = OpenService(sh, driver_name, SERVICE_ALL_ACCESS);
	if (!rh) {
		printf("unload_kernel_code_scm - OpenService failed (%Iu)\n", GetLastError());
		CloseServiceHandle(sh);
		return false;
	}
	SERVICE_STATUS_PROCESS ssp;
	DWORD dwBytesNeeded;
	// Make sure the service is not already stopped.
	if (!QueryServiceStatusEx(rh, SC_STATUS_PROCESS_INFO, (LPBYTE)&ssp, sizeof(SERVICE_STATUS_PROCESS), &dwBytesNeeded)) {
		printf("unload_kernel_code_scm - QueryServiceStatusEx failed (%Iu)\n", GetLastError());
		CloseServiceHandle(rh);
		CloseServiceHandle(sh);
		return false;
	}
	if (ssp.dwCurrentState == SERVICE_STOPPED) {
		printf("unload_kernel_code_scm - Service is already stopped.\n");
		CloseServiceHandle(rh);
		CloseServiceHandle(sh);
		return false;
	}
	if (!ControlService(rh, SERVICE_CONTROL_STOP, (LPSERVICE_STATUS)&ssp)) {
		printf("unload_kernel_code_scm - ControlService failed (%Iu)\n", GetLastError());
		CloseServiceHandle(rh);
		CloseServiceHandle(sh);
		return false;
	}
	if (!DeleteService(rh)) {
		printf("unload_kernel_code_scm - DeleteService failed (%Iu)\n", GetLastError());
		CloseServiceHandle(rh);
		CloseServiceHandle(sh);
		return false;
	}
	CloseServiceHandle(rh);
	CloseServiceHandle(sh);
	return true;
}

ServicesHandler::~ServicesHandler()
{
}
