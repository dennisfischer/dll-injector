// dll-injector.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include "stdafx.h"
void SetDebugPrivilege();

int main(const int argc, const char* argv[])
{
	if (argc != 3 && argc != 4)
	{
		logError(L"Not enough arguments, requiring 2: <target> <dll> [<0|default 1|inject, 2|eject>]!", argc);
		system("PAUSE");
		return 0;
	}

	std::string sTarget(argv[1]);
	std::string sDll(argv[2]);

	auto option = 0;
	if(argc == 4)
	{
		option = atoi(argv[3]);

		if(option>2 || option < 0)
		{
			logError(L"Invalid 3rd parameter given.", option);
			system("PAUSE");
			return 0;
		}
	}

	logInfo(L"Injecting DLL into " + std::wstring(sTarget.begin(), sTarget.end()));
	//SetDebugPrivilege();
	Injector* iInjector = new AppInitDllInjector(sDll);

	if (option == 0 || option == 1) {
		iInjector->inject();
		logInfo(L"Finished injecting");
		system("PAUSE");
	}
	if(option == 0 || option == 2)
	{
		iInjector->free();
		logInfo(L"Ejected!");
		system("PAUSE");
	}

	iInjector->Release();
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
