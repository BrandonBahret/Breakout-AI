#include "LocateWindowHandles.h"
#include <stdio.h>

BOOL CALLBACK EnumWindowProc(HWND hwnd, LPARAM lParam) {
	FindExe *out = (FindExe*)lParam;

	DWORD pid;
	GetWindowThreadProcessId(hwnd, &pid);
	HANDLE han = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, true, pid);

	char *str = (char*)malloc(256);
	GetModuleFileNameExA(han, NULL, str, 256);
	size_t point = strlen(str) - strlen(out->name);
	if (point > 0) {
		char classtype[256];
		char name[256];

		RealGetWindowClassA(hwnd, classtype, 256);
		GetWindowTextA(hwnd, name, 256);

		if (!strcmp(str + point, out->name)) {
			printf("%s |%s | %s\n", str + point, name, classtype);
			if (IsWindowVisible(hwnd)) {
				out->hwnd = hwnd;
				return false;
			}
		}
	}
	return true;
}

HWND GetHWNDFromExecutableName(const char* executableName) {
	FindExe fe = { executableName, NULL, NULL };
	EnumWindows(EnumWindowProc, (LPARAM)&fe);
	return fe.hwnd;
}
