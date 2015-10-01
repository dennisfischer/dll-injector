#pragma once

class Injector {
public:
	void inject(const std::wstring processName, const std::wstring cDllPath);
	void free();
	unsigned long GetProcessIdFromProcessName(const std::wstring processName);
	unsigned long GetMainThreadIdFromProcessHandle(HANDLE hProcess);
	virtual ~Injector();
	Injector(Injector const &) = delete;
	Injector & operator=(Injector const &) = delete;
protected:
	Injector();
private:
	virtual void do_inject(HANDLE hProcess,const std::wstring cDllPath) = 0;
	virtual void do_free() = 0;
};