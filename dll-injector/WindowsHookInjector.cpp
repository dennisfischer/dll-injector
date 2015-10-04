#include "stdafx.h"

WindowsHookInjector::WindowsHookInjector()
{
}

WindowsHookInjector::~WindowsHookInjector()
{
}

void WindowsHookInjector::do_inject(HANDLE hProcess, const std::string cDllPath)
{
	auto hModule = LoadLibraryW(std::wstring(cDllPath.begin(), cDllPath.end()).c_str());
	if (hModule == nullptr)
	{
		return logError(L"Failed to load dll for hooking", GetLastError());
	}

	auto hProc = HOOKPROC(GetProcAddress(hModule, "hookProc"));
	if (hProc == nullptr)
	{
		return logError(L"Hook function not existing in dll module!", GetLastError());
	}

	auto threadId = GetMainThreadIdFromProcessHandle(hProcess);
	if (threadId == 0)
	{
		return logError(L"Failed to find thread id from process handle.", threadId);
	}

	std::cout << "Thread id is: " << threadId << std::endl;

	hHook = SetWindowsHookExW(WH_GETMESSAGE, hProc, hModule, threadId);
	if (!PostThreadMessageW(threadId, WM_APP + 1, 0, 0))
	{
		logError(L"Failed: ", GetLastError());
	}

	if (hHook == nullptr)
	{
		return logError(L"Failed to install hook", GetLastError());
	}
}

void WindowsHookInjector::do_free(HANDLE hProcess, const std::string cDllPath)
{
	if (hHook != nullptr)
	{
		UnhookWindowsHookEx(hHook);
	}
}
