#pragma once

class Injector
{
public:
	void inject();
	void free();
	virtual void Release() = 0;
	unsigned long GetProcessIdFromProcessName(const std::string cProcessName) const;
	unsigned long GetMainThreadIdFromProcessHandle(HANDLE hProcess) const;
	virtual ~Injector();
	HANDLE CreateProcessHandle(unsigned long dwProcessId) const;
	HANDLE CreateProcessHandleByName(const std::string cProcessName) const;
	Injector(Injector const&) = delete;
	Injector& operator=(Injector const&) = delete;
protected:
	Injector();
private:
	virtual void do_inject() = 0;
	virtual void do_free() = 0;
};
