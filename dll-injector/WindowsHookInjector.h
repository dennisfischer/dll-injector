#pragma once

#include "stdafx.h"

class WindowsHookInjector : public Injector
{
public:
	WindowsHookInjector(const std::string cProcessName, const std::string cDllPath);
	virtual ~WindowsHookInjector();
	void Release() override;
protected:
	std::string m_ProcessName;
	std::string m_DllPath;
	HANDLE m_hProcess;
	HHOOK m_hHook = nullptr;
private:
	void do_inject() override final;
	void do_free() override final;
};
