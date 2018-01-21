#pragma once
#include "stdlib.h"
#include <cstring>

// Get const char* and return equivlient wchar_t*
wchar_t* GetWC(const char *c);

// Get const wchar_t* and return equivlient char* 
char* GetChar_Convert(wchar_t *wc);