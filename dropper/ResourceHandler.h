#pragma once
#include <Windows.h>

class ResourceHandler
{
public:
	ResourceHandler();
	~ResourceHandler();
	bool encapsulation(wchar_t *dropper_name, wchar_t *driver_name);
	bool decapsulation(wchar_t *dropper_name, wchar_t *driver_name);
	bool is_exist(wchar_t *dropper_name, wchar_t *driver_name);
};

