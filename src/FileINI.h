#pragma once
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int FileINI_GetInt(LPCTSTR Section, LPCTSTR Key, int Default);
float FileINI_GetFloat(LPCTSTR Section, LPCTSTR Key, float Default);
LPCTSTR FileINI_GetString(LPCTSTR Section, LPCTSTR Key, LPCTSTR Default);
DWORD FileINI_GetHex(LPCTSTR Section, LPCTSTR Key, DWORD Default);
