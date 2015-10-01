#pragma once

#include "stdafx.h"

class RemoteThreadInjector : public Injector {
public:
	RemoteThreadInjector();
	virtual ~RemoteThreadInjector();
private:
	void do_inject(HANDLE hProcess, const std::wstring cDllPath) final;
	void do_free() final;
};