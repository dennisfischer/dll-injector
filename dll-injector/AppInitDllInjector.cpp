#include "stdafx.h"


AppInitDllInjector::AppInitDllInjector(const std::string cDllPath): m_DllPath(cDllPath)
{
}

AppInitDllInjector::~AppInitDllInjector()
{
}

void AppInitDllInjector::do_inject()
{
	std::wstring subKey = L"\\Software\\Microsoft\\Windows NT\\CurrentVersion\\Windows";
	HKEY hKey = nullptr;
	RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, KEY_READ, &hKey);


	DWORD dwType = REG_SZ;
	BYTE bData[64];  // 64 bytes long
	DWORD dwDataLen = sizeof(bData);
	RegQueryValueEx(hKey, L"Spooler", nullptr, &dwType, LPBYTE(&bData), &dwDataLen);
	logInfo(L"Value is: " + std::to_string(static_cast<const char*>(bData)));

	RegCloseKey(hKey);
}

void AppInitDllInjector::do_free()
{
	
}

void AppInitDllInjector::Release()
{

}
