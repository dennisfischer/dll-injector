#include "stdafx.h"

RemoteThreadInjector::RemoteThreadInjector()
{
}

RemoteThreadInjector::~RemoteThreadInjector()
{
}

HANDLE RemoteThreadInjector::CallWithRemoteThread(HANDLE hProcess, LPVOID hModule, LPVOID lp_base_address) const
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
	return hRemoteHandle;
}

void RemoteThreadInjector::CallDLLMethod(const std::string& cDllPath, const std::string& method) const
{
	auto hModule = LoadLibraryW(std::wstring(cDllPath.begin(), cDllPath.end()).c_str());
	auto procAddress = GetProcAddress(hModule, method.c_str());
	typedef void (__stdcall * icfunc)();
	icfunc DLLMethod;
	DLLMethod = icfunc(procAddress);
	DLLMethod();

	FreeLibrary(hModule);
}

void RemoteThreadInjector::do_inject(HANDLE hProcess, const std::string cDllPath)
{
	auto hModule = PTHREAD_START_ROUTINE(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
	auto lpBaseAdress = VirtualAllocEx(hProcess, nullptr, cDllPath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (lpBaseAdress == nullptr)
	{
		logError(L"Failed alllocate memory in target process", GetLastError());
		return;
	}


	if (!WriteProcessMemory(hProcess, lpBaseAdress, cDllPath.c_str(), cDllPath.size(), nullptr))
	{
		logError(L"Failed writing into process memory", GetLastError());
		return;
	}
	//Inject Dll
	auto hRemoteThread = CallWithRemoteThread(hProcess, hModule, lpBaseAdress);
	if (WaitForSingleObject(hRemoteThread, INFINITE) == WAIT_FAILED)
	{
		logError(L"Failed waiting for remote thread during injection!", GetLastError());
		return;
	}
	DWORD dllBaseAddress = 0;
	GetExitCodeThread(hRemoteThread, &dllBaseAddress);

	if (hRemoteThread != nullptr)
	{
		CloseHandle(hRemoteThread);
	}
	VirtualFreeEx(hProcess, lpBaseAdress, 0, MEM_RELEASE);
}

void RemoteThreadInjector::do_free(HANDLE hProcess, const std::string cDllPath)
{
	CallDLLMethod(cDllPath, "UnloadDLL");
}
