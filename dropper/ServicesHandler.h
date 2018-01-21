#pragma once
#include <Windows.h>
class ServicesHandler
{
public:
	ServicesHandler();
	~ServicesHandler();
	bool load_kernel_code_scm(wchar_t *driver_name);
	bool unload_kernel_code_scm(wchar_t *driver_name);
};

