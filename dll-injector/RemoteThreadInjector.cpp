#include "stdafx.h"

RemoteThreadInjector::RemoteThreadInjector()
{
}

RemoteThreadInjector::~RemoteThreadInjector()
{
}

void RemoteThreadInjector::do_inject(HANDLE hProcess, const std::wstring  cDllPath) {
	LPVOID LoadLibAddr = (LPVOID)GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA");
	LPVOID dereercomp = VirtualAllocEx(hProcess, NULL, cDllPath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(hProcess, dereercomp, cDllPath.c_str(), cDllPath.size(), NULL);

	HANDLE asdc = CreateRemoteThread(hProcess, NULL, NULL, (LPTHREAD_START_ROUTINE)LoadLibAddr, dereercomp, 0, NULL);
	if (GetLastError() == ERROR_ACCESS_DENIED) {
		logError(L"Access was denied by process!", GetLastError());
	}
	WaitForSingleObject(asdc, INFINITE);
	VirtualFreeEx(hProcess, dereercomp, 0, MEM_RELEASE);
	CloseHandle(asdc);
}

void RemoteThreadInjector::do_free() {

}
