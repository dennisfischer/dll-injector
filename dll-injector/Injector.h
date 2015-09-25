#pragma once

class Injector {
public:
	void inject();
	void free();
	virtual ~Injector();
	Injector(Injector const &) = delete;
	Injector & operator=(Injector const &) = delete;
protected:
	Injector();
private:
	virtual void do_inject() = 0;
	virtual void do_free() = 0;
};