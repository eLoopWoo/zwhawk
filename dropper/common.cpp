#include "stdafx.h"
#include "common.h"

// Get const char* and return equivlient wchar_t*
wchar_t* GetWC(const char *c) {
	const size_t cSize = strlen(c) + 1;
	size_t* pReturnValue;
	pReturnValue = 0;
	wchar_t* wc = new wchar_t[cSize];
	// Converts a sequence of multibyte characters to a corresponding sequence
	// of wide characters. 
	mbstowcs_s(pReturnValue, wc, cSize, c, cSize);
	return wc;
}

char* GetChar_Convert(wchar_t *wc) {
	const size_t wcSize = wcslen(wc) + 1;
	size_t* pReturnValue;
	pReturnValue = 0;
	char* c = new char[wcSize];
	wcstombs_s(pReturnValue, c, wcSize, wc, wcSize);
	return c;
}