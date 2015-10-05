#pragma once

class Injector
{
public:
	void inject(const std::string cProcessName, const std::string cDllPath);
	void free(const std::string cProcessName, const std::string cDllPath);
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
	virtual void do_inject(HANDLE hProcess, const std::string cDllPath) = 0;
	virtual void do_free(HANDLE hProcess, const std::string cDllPath) = 0;
};
