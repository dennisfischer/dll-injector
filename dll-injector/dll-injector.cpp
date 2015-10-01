// dll-injector.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
void SetDebugPrivilege();

int main()
{
	std::wstring target = L"chrome.exe";

	logInfo(std::wstring(L"Injecting DLL into ") + target);
	//SetDebugPrivilege();
	Injector* iInjector = new WindowsHookInjector();
	iInjector->inject(target, L"C:/Users/denni/Documents/Visual Studio 2015/Projects/dll-injector-sample/x64/Debug/dll-injector-sample.dll");
	logInfo(L"Finished injecting");
	system("PAUSE");
	iInjector->free();
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
	LookupPrivilegeValue(nullptr, L"seDebugPrivilege", &luid);

	priv.PrivilegeCount = 1;
	priv.Privileges[0].Luid = luid;
	priv.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	AdjustTokenPrivileges(hToken, false, &priv, 0, nullptr, nullptr);

	CloseHandle(hToken);
	CloseHandle(hProcess);
}
