#pragma once

#include "stdafx.h"

class RemoteThreadInjector : public Injector
{
public:
	RemoteThreadInjector();
	virtual ~RemoteThreadInjector();
	HANDLE CallWithRemoteThread(HANDLE hProcess, LPVOID hModule, LPVOID lp_base_address) const;
	void CallDLLMethod(const std::string& cDllPath, const std::string& method) const;
private:
	void do_inject(HANDLE hProcess, const std::string cDllPath) override final;
	void do_free(HANDLE hProcess, const std::string cDllPath) override final;
};
