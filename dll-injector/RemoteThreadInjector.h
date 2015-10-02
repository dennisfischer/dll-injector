#pragma once

#include "stdafx.h"

class RemoteThreadInjector : public Injector
{
public:
	RemoteThreadInjector();
	virtual ~RemoteThreadInjector();
private:
	void do_inject(HANDLE hProcess, const std::string cDllPath) override final;
	void do_free() override final;
};
