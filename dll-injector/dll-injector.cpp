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
	SetDebugPrivilege();
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
	HANDLE hProcess = GetCurrentProcess(), hToken;
	TOKEN_PRIVILEGES priv;
	LUID luid;

	OpenProcessToken(hProcess, TOKEN_ADJUST_PRIVILEGES, &hToken);
	LookupPrivilegeValueW(nullptr, L"seDebugPrivilege", &luid);

	priv.PrivilegeCount = 1;
	priv.Privileges[0].Luid = luid;
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, false, &priv, 0, nullptr, nullptr);

	CloseHandle(hToken);
	CloseHandle(hProcess);
}
