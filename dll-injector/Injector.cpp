#include "stdafx.h"

Injector::Injector() {
}
Injector::~Injector() {

}


void Injector::inject(const std::wstring processName, const std::wstring cDllPath) {

	unsigned long dwProcessId = GetProcessIdFromProcessName(processName);
	if (dwProcessId == 0) {
		return logError(L"Requested process isn't running!", GetLastError());
	}

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcessId);
	if (!hProcess) {
		logError(L"Couldn' t receive process handle", GetLastError());
	}

	do_inject(hProcess, cDllPath);

	if (!CloseHandle(hProcess)) {
		logWarning(L"Couldn't close process handle", GetLastError());
	}
}

void Injector::free() {
	do_free();
}

unsigned long Injector::GetProcessIdFromProcessName(const std::wstring ProcName)
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		logError(L"Unable to create toolhelp snapshot!", GetLastError());
		return 0;
	}


	PROCESSENTRY32 pe;
	pe.dwSize = sizeof(PROCESSENTRY32);

	BOOL retVal = Process32First(hSnapShot, &pe);
	while (retVal)
	{
		if (wcsstr(pe.szExeFile, ProcName.c_str()))
		{
			return pe.th32ProcessID;
		}
		retVal = Process32Next(hSnapShot, &pe);
	}
	return 0;
}

unsigned long Injector::GetMainThreadIdFromProcessHandle(HANDLE hProcess) {
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hSnapShot == INVALID_HANDLE_VALUE)
	{
		logError(L"Unable to create toolhelp snapshot!", GetLastError());
		return 0;
	}

	unsigned long pid = GetProcessId(hProcess);

	THREADENTRY32 te;
	te.dwSize = sizeof(THREADENTRY32);

	BOOL retVal = Thread32First(hSnapShot, &te);
	while (retVal) {
		if (te.th32OwnerProcessID == pid) {
			return te.th32ThreadID;
		}
		retVal = Thread32Next(hSnapShot, &te);
	}
	return 0;
}