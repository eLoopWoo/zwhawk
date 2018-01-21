#pragma once
#include <iostream>

class manual
{
public:
	manual();
	~manual();
	static void showHelp();
	static bool showMan(const char* arg);
};

