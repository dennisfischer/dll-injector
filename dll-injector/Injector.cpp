#include "stdafx.h"

Injector::Injector()
{
}

Injector::~Injector()
{
}


HANDLE Injector::CreateProcessHandle(unsigned long dwProcessId) const
{
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcessId);
	if (!hProcess)
	{
		logError(L"Couldn' t receive process handle", GetLastError());
		return nullptr;
	}
	return hProcess;
}

HANDLE Injector::CreateProcessHandleByName(const std::string cProcessName) const
{
	auto dwProcessId = GetProcessIdFromProcessName(cProcessName);
	if (dwProcessId == 0)
	{
		logError(L"Requested process isn't running!", GetLastError());
		return nullptr;
	}

	return CreateProcessHandle(dwProcessId);
}

void Injector::inject(const std::string cProcessName, const std::string cDllPath)
{
	auto hProcess = CreateProcessHandleByName(cProcessName);
	if (hProcess == nullptr)
	{
		return;
	}

	do_inject(hProcess, cDllPath);

	if (!CloseHandle(hProcess))
	{
		logWarning(L"Couldn't close process handle", GetLastError());
	}
}

void Injector::free(const std::string cProcessName, const std::string cDllPath)
{
	auto hProcess = CreateProcessHandleByName(cProcessName);
	if (hProcess == nullptr)
	{
		return;
	}

	do_free(hProcess, cDllPath);

	if (!CloseHandle(hProcess))
	{
		logWarning(L"Couldn't close process handle", GetLastError());
	}
}

unsigned long Injector::GetProcessIdFromProcessName(const std::string cProcessName) const
{
	auto hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		logError(L"Unable to create toolhelp snapshot!", GetLastError());
		return 0;
	}

	PROCESSENTRY32W pe;
	pe.dwSize = sizeof(PROCESSENTRY32W);

	auto retVal = Process32FirstW(hSnapShot, &pe);
	while (retVal)
	{
		if (wcsstr(pe.szExeFile, std::wstring(cProcessName.begin(), cProcessName.end()).c_str()))
		{
			logInfo(L"Found process with pid: " + std::to_wstring(pe.th32ProcessID));

			return pe.th32ProcessID;
		}
		retVal = Process32NextW(hSnapShot, &pe);
	}
	return 0;
}

unsigned long Injector::GetMainThreadIdFromProcessHandle(HANDLE hProcess) const
{
	auto hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		logError(L"Unable to create toolhelp snapshot!", GetLastError());
		return 0;
	}

	auto pid = GetProcessId(hProcess);

	THREADENTRY32 te;
	te.dwSize = sizeof(THREADENTRY32);

	auto retVal = Thread32First(hSnapShot, &te);
	while (retVal)
	{
		if (te.th32OwnerProcessID == pid)
		{
			return te.th32ThreadID;
		}
		retVal = Thread32Next(hSnapShot, &te);
	}
	return 0;
}
