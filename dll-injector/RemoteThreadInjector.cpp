#include "stdafx.h"

RemoteThreadInjector::RemoteThreadInjector()
{
}

RemoteThreadInjector::~RemoteThreadInjector()
{
}

void RemoteThreadInjector::CallWithRemoteThread(HANDLE hProcess, LPVOID hModule, LPVOID lp_base_address) const
{
	auto hRemoteHandle = CreateRemoteThread(hProcess, nullptr, 0, LPTHREAD_START_ROUTINE(hModule), lp_base_address, 0, nullptr);
	auto error = GetLastError();
	if (error == ERROR_ACCESS_DENIED)
	{
		logError(L"Access was denied by process!", error);
	}
	else if (error != 0)
	{
		logError(L"Unknown error during remote thread creation", error);
	}
	WaitForSingleObject(hRemoteHandle, INFINITE);
	CloseHandle(hRemoteHandle);
}

void RemoteThreadInjector::do_inject(HANDLE hProcess, const std::string cDllPath)
{
	auto hModule = LPVOID(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
	auto lpBaseAdress = VirtualAllocEx(hProcess, nullptr, cDllPath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (!WriteProcessMemory(hProcess, lpBaseAdress, cDllPath.c_str(), cDllPath.size(), nullptr))
	{
		logError(L"Failed writing into process memory", GetLastError());
	}
	else
	{
		CallWithRemoteThread(hProcess, hModule, lpBaseAdress);
	}
	VirtualFreeEx(hProcess, lpBaseAdress, 0, MEM_RELEASE);
}

void RemoteThreadInjector::do_free(HANDLE hProcess)
{
}
