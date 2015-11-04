#pragma once
#include "stdafx.h"

class APCInjector : public Injector
{
public:
	APCInjector(const std::string cProcessName);
	~APCInjector();
	void Release() override;
private:
	std::string m_ProcessName;
	HANDLE hProcess = nullptr;
	void do_inject() override;
	void do_free() override;
};