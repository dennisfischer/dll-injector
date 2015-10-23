#include "stdafx.h"


AppInitDllInjector::AppInitDllInjector(const std::string cDllPath) : m_DllPath(cDllPath)
{
}

AppInitDllInjector::~AppInitDllInjector()
{
}

void AppInitDllInjector::do_inject()
{
	std::wstring subKey = L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion\\Windows";
	HKEY hKey = nullptr;
	long n = 0;
	if ((n = RegOpenKeyEx(HKEY_LOCAL_MACHINE, subKey.c_str(), 0, KEY_WRITE, &hKey)) != ERROR_SUCCESS)
	{
		wchar_t buf[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(),
		                                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, nullptr);

		logInfo(buf);
		logInfo(std::to_wstring(n));
	}

	DWORD value = 1;

	if ((n = RegSetValueEx(hKey, L"LoadAppInit_DLLs", 0, REG_DWORD, LPBYTE(&value), sizeof(DWORD))) != ERROR_SUCCESS)
	{
		wchar_t buf[256];
		FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, nullptr, GetLastError(),
		                                         MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, nullptr);

		logInfo(buf);
		logInfo(std::to_wstring(n));
	}
	RegFlushKey(hKey);

	RegCloseKey(hKey);
}

void AppInitDllInjector::do_free()
{
}

void AppInitDllInjector::Release()
{
}
