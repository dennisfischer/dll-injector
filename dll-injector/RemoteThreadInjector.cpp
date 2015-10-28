#include "stdafx.h"

RemoteThreadInjector::RemoteThreadInjector(const std::string cProcessName, const std::string cDllPath): m_ProcessName(cProcessName), m_DllPath(cDllPath)
{
	m_hProcess = CreateProcessHandleByName(cProcessName);
}

RemoteThreadInjector::~RemoteThreadInjector()
{
}

void RemoteThreadInjector::Release()
{
	if (!CloseHandle(m_hProcess))
	{
		logWarning(L"Couldn't close process handle", GetLastError());
	}
}

HANDLE RemoteThreadInjector::CallWithRemoteThread(LPVOID hModule, LPVOID lp_base_address) const
{
	auto hRemoteHandle = CreateRemoteThread(m_hProcess, nullptr, 0, LPTHREAD_START_ROUTINE(hModule), lp_base_address, 0, nullptr);
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

void RemoteThreadInjector::CallDLLMethod(const std::string& method) const
{
	auto hModule = LoadLibraryW(std::wstring(m_DllPath.begin(), m_DllPath.end()).c_str());
	if (hModule != nullptr)
	{
		auto procAddress = GetProcAddress(hModule, method.c_str());
		typedef void (__stdcall * funct)();
		funct DLLMethod;
		DLLMethod = funct(procAddress);
		DLLMethod();

		FreeLibrary(hModule);
	}
}

void RemoteThreadInjector::do_inject()
{
	auto hModule = PTHREAD_START_ROUTINE(GetProcAddress(GetModuleHandleA("kernel32.dll"), "LoadLibraryA"));
	auto lpBaseAdress = VirtualAllocEx(m_hProcess, nullptr, m_DllPath.size(), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

	if (lpBaseAdress == nullptr)
	{
		logError(L"Failed alllocate memory in target process", GetLastError());
		return;
	}


	if (!WriteProcessMemory(m_hProcess, lpBaseAdress, m_DllPath.c_str(), m_DllPath.size(), nullptr))
	{
		logError(L"Failed writing into process memory", GetLastError());
		return;
	}
	//Inject Dll
	auto hRemoteThread = CallWithRemoteThread(hModule, lpBaseAdress);
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
	VirtualFreeEx(m_hProcess, lpBaseAdress, 0, MEM_RELEASE);
}

void RemoteThreadInjector::do_free()
{
	CallDLLMethod("UnloadDLL");
}
