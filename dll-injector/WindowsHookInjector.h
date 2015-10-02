#pragma once

#include "stdafx.h"

class WindowsHookInjector : public Injector
{
public:
	WindowsHookInjector();
	virtual ~WindowsHookInjector();
private:
	void do_inject(HANDLE hProcess, const std::string cDllPath) override final;
	void do_free(HANDLE hProcess) override final;

	HHOOK hHook = nullptr;
};
