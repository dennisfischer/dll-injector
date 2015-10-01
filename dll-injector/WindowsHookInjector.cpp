#include "stdafx.h"

WindowsHookInjector::WindowsHookInjector()
{
}

WindowsHookInjector::~WindowsHookInjector()
{
}

void WindowsHookInjector::do_inject(HANDLE hProcess, const std::wstring cDllPath) {
	auto hModule = LoadLibrary(cDllPath.c_str());
	if (hModule == nullptr) {
		return logError(L"Failed to load dll for hooking", GetLastError());
	}

	auto hProc = HOOKPROC(GetProcAddress(hModule, "hookProc"));
	if (hProc == nullptr) {
		return logError(L"Hook function not existing in dll module!", GetLastError());
	}

	auto threadId = GetMainThreadIdFromProcessHandle(hProcess);
	if (threadId == 0) {
		return logError(L"Failed to find thread id from process handle.", threadId);
	}

	std::cout << "Thread id is: " << threadId << std::endl;

	hHook = SetWindowsHookEx(WH_GETMESSAGE, hProc, hModule, threadId);
	if (!PostThreadMessageW(threadId, WM_APP + 1, 0, 0)) {
		logError(L"Failed: ", GetLastError());
	}

	if (hHook == nullptr) {
		return logError(L"Failed to install hook", GetLastError());
	}
}

void WindowsHookInjector::do_free() {
	if (hHook != nullptr) {
		UnhookWindowsHookEx(hHook);
	}
}