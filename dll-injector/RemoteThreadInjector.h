#pragma once

#include "Injector.h"

class RemoteThreadInjector : public Injector {
public:
	RemoteThreadInjector();
	virtual ~RemoteThreadInjector();
private:
	void do_inject() final;
	void do_free() final;
};