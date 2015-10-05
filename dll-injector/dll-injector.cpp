// dll-injector.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
void SetDebugPrivilege();

int main(const int argc, const char* argv[])
{
	if (argc != 3)
	{
		logError(L"Not enough arguments, requiring 2: <target> <dll>!", argc);
		system("PAUSE");
		return 0;
	}

	std::string sTarget(argv[1]);
	std::string sDll(argv[2]);

	logInfo(L"Injecting DLL into " + std::wstring(sTarget.begin(), sTarget.end()));
	//SetDebugPrivilege();
	Injector* iInjector = new RemoteThreadInjector();
	iInjector->inject(sTarget, sDll);
	logInfo(L"Finished injecting");
	system("PAUSE");
	iInjector->free(sTarget, sDll);
	logInfo(L"Ejected!");
	delete iInjector;
	return 0;
}


void SetDebugPrivilege()
{
	auto hProcess = GetCurrentProcess();
	HANDLE hToken = nullptr;

	DWORD lastError = ERROR_SUCCESS;
	if(!OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken) || (lastError = GetLastError()) != ERROR_SUCCESS)
	{
		logError(L"Couldn't open process token", lastError);
	}

	LUID luid;
	if(!LookupPrivilegeValueW(nullptr, SE_DEBUG_NAME, &luid) || (lastError = GetLastError()) != ERROR_SUCCESS)
	{
		logError(L"Couldn't find seDebugPrivilege value", lastError);
	}


	TOKEN_PRIVILEGES priv;
	priv.PrivilegeCount = 1;
	priv.Privileges[0].Luid = luid;
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	if(!AdjustTokenPrivileges(hToken, false, &priv, 0, nullptr, nullptr) || (lastError = GetLastError()) != ERROR_SUCCESS)
	{
		logError(L"Couldn't adjust debug privileges", lastError);
	}
	
	CloseHandle(hToken);
	CloseHandle(hProcess);
}
