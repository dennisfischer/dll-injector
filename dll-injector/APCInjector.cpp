#include "stdafx.h"

APCInjector::APCInjector(const std::string cProcessName) : m_ProcessName(cProcessName)
{
	hProcess = CreateProcessHandleByName(cProcessName);
}

APCInjector::~APCInjector()

{
}

void APCInjector::Release()
{
}


DWORD WINAPI APCRoutine(DWORD APCParam)
{
	printf("[Thread %4ld] Inside APC routine with argument (%ld)\n",
		GetCurrentThreadId(), APCParam);
	return 0;
}

void APCInjector::do_inject()
{
	HANDLE hThread = OpenThread(THREAD_ALL_ACCESS, false, GetMainThreadIdFromProcessHandle(hProcess));
	QueueUserAPC((PAPCFUNC)APCRoutine, hThread, ULONG_PTR(0));
}

void APCInjector::do_free()
{
}