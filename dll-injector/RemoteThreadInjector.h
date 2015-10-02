#pragma once

#include "stdafx.h"

class RemoteThreadInjector : public Injector
{
public:
	RemoteThreadInjector();
	virtual ~RemoteThreadInjector();
	void CallWithRemoteThread(HANDLE hProcess, LPVOID hModule, LPVOID lp_base_address) const;
private:
	void do_inject(HANDLE hProcess, const std::string cDllPath) override final;
	void do_free(HANDLE hProcess) override final;
};
