#include "stdafx.h"

WindowsHookInjector::~WindowsHookInjector()
{
}


void WindowsHookInjector::Release()
{
	if (!CloseHandle(m_hProcess))
	{
		logWarning(L"Couldn't close process handle", GetLastError());
	}
}

void WindowsHookInjector::do_inject()
{
	auto hModule = LoadLibraryW(std::wstring(m_DllPath.begin(), m_DllPath.end()).c_str());
	if (hModule == nullptr)
	{
		return logError(L"Failed to load dll for hooking", GetLastError());
	}

	auto hProc = HOOKPROC(GetProcAddress(hModule, "hookProc"));
	if (hProc == nullptr)
	{
		return logError(L"Hook function not existing in dll module!", GetLastError());
	}

	auto threadId = GetMainThreadIdFromProcessHandle(m_hProcess);
	if (threadId == 0)
	{
		return logError(L"Failed to find thread id from process handle.", threadId);
	}

	std::cout << "Thread id is: " << threadId << std::endl;

	m_hHook = SetWindowsHookExW(WH_GETMESSAGE, hProc, hModule, threadId);
	if (!PostThreadMessageW(threadId, WM_APP + 1, 0, 0))
	{
		logError(L"Failed: ", GetLastError());
	}

	if (m_hHook == nullptr)
	{
		return logError(L"Failed to install hook", GetLastError());
	}
}

void WindowsHookInjector::do_free()
{
	if (m_hHook != nullptr)
	{
		UnhookWindowsHookEx(m_hHook);
	}
}
