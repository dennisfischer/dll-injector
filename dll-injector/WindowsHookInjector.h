#pragma once

#include "stdafx.h"

class WindowsHookInjector : public Injector {
public:
	WindowsHookInjector();
	virtual ~WindowsHookInjector();
private:
	void do_inject(HANDLE hProcess, const std::wstring cDllPath) final;
	void do_free() final;

	HHOOK hHook;
};