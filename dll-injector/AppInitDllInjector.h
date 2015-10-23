#pragma once
#include "stdafx.h"

class AppInitDllInjector : public Injector
{
public:
	explicit AppInitDllInjector(const std::string cDllPath);

	virtual ~AppInitDllInjector();
	void Release() override;

protected:
	std::string m_DllPath;
private:
	void do_inject() override;
	void do_free() override;
};
