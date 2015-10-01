#include "stdafx.h"

RemoteThreadInjector::RemoteThreadInjector()
{
}

RemoteThreadInjector::~RemoteThreadInjector()
{
}

void RemoteThreadInjector::do_inject(HANDLE hProcess, const std::wstring  cDllPath) {
	auto hModule = LPVOID(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
	auto lp_base_address = VirtualAllocEx(hProcess, nullptr, cDllPath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	WriteProcessMemory(hProcess, lp_base_address, cDllPath.c_str(), cDllPath.size(), nullptr);

	auto hRemoteHandle = CreateRemoteThread(hProcess, nullptr, 0, LPTHREAD_START_ROUTINE(hModule), lp_base_address, 0, nullptr);
	if (GetLastError() == ERROR_ACCESS_DENIED) {
		logError(L"Access was denied by process!", GetLastError());
	}
	WaitForSingleObject(hRemoteHandle, INFINITE);
	VirtualFreeEx(hProcess, lp_base_address, 0, MEM_RELEASE);
	CloseHandle(hRemoteHandle);
}

void RemoteThreadInjector::do_free() {

}
