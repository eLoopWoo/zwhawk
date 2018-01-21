#pragma once
#include <Windows.h>


#define SIOCTL_TYPE 40000

#define IOCTL_BASIC CTL_CODE( SIOCTL_TYPE, 0x800, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_ECHO CTL_CODE( SIOCTL_TYPE, 0x801, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)
#define IOCTL_DKOMPSHIDE CTL_CODE( SIOCTL_TYPE, 0x802, METHOD_BUFFERED, FILE_READ_DATA|FILE_WRITE_DATA)

class DriverHandler
{
public:
	DriverHandler();
	~DriverHandler();

	HANDLE connect_driver(wchar_t *driver_name);
	bool disconnect_driver(HANDLE hDriver);

	HANDLE cli_handle_driver(HANDLE hDriver);
	bool cli_handle_echo(HANDLE hDriver);
	bool cli_handle_helloworld(HANDLE hDriver);
	bool cli_handle_hideprocess(HANDLE hDriver);

	bool handle_echo(HANDLE hDriver, char* buffer);
	bool handle_helloworld(HANDLE hDriver, char* buffer);
	bool handle_hideprocess(HANDLE hDriver, char* buffer);
};

