#pragma once

#include "stdafx.h"

class RemoteThreadInjector : public Injector
{
public:
	RemoteThreadInjector(const std::string cProcessName, const std::string cDllPath);;
	virtual ~RemoteThreadInjector();
	void Release() override;
	HANDLE CallWithRemoteThread(LPVOID hModule, LPVOID lp_base_address) const;
	void CallDLLMethod(const std::string& method) const;
protected:
	std::string m_ProcessName;
	std::string m_DllPath;
	HANDLE m_hProcess;
private:
	void do_inject() override final;
	void do_free() override final;
};
