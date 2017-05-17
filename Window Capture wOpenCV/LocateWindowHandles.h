#pragma once
#include <Windows.h>
#include <Psapi.h>

struct FindExe {
	const char *name;
	const char *className;
	HWND hwnd;
};

//Pass by reference a "FindExe" object to EnumWindows, will then fill it's hwnd 
//variable with the HWND pointing to that executable name.
BOOL CALLBACK EnumWindowProc(HWND hwnd, LPARAM lParam);

HWND GetHWNDFromExecutableName(const char* executableName);