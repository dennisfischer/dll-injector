#pragma once

#define BOOLIFY(x) !!(x)

#define NOP 0x90
class NopHoppingInjector : public Injector
{
public:
	NopHoppingInjector();
	~NopHoppingInjector();
	void Release() override;
private:
	void do_inject() override;
	void do_free() override;
	std::vector<DWORD_PTR> GetInstructionList(const unsigned char* const pBytes, const size_t ulSize, const DWORD_PTR dwOffset, const bool bNopsOnly);
	std::vector<DWORD_PTR> GetNopList(const unsigned char* const pBytes, const size_t ulSize, const DWORD_PTR dwOffset);
	std::vector<std::pair<DWORD_PTR, DWORD_PTR>> FindNops(const unsigned char* const pBytes, const size_t ulSize, const DWORD_PTR dwOffset);
	std::vector<std::vector<std::pair<DWORD_PTR, DWORD_PTR>>> FindNopRanges(const HANDLE hProcess, const std::map<std::wstring, std::vector<std::pair<DWORD_PTR, DWORD_PTR>>>& executableRegions, const size_t ulSize);
	std::vector<DWORD_PTR> SelectRegions(const HANDLE hProcess, const std::vector<std::vector<std::pair<DWORD_PTR, DWORD_PTR>>>& nopRangeList, std::vector<DWORD_PTR>& writeInstructions);
	bool WriteJumps(const HANDLE hProcess, const std::vector<DWORD_PTR>& writeInstructions, const std::vector<DWORD_PTR>& selectedRegions);
	HANDLE WriteCodeToRegions(const HANDLE hProcess, const std::map<std::wstring, std::vector<std::pair<DWORD_PTR, DWORD_PTR>>>& executableRegions, const unsigned char* const pBytes, const size_t ulSize);
	HANDLE LoadProcess(const DWORD dwProcessId);
	std::map<std::wstring, std::vector<std::pair<DWORD_PTR, DWORD_PTR>>> GetExecutableRegions(const HANDLE hProcess, const std::map<std::wstring, std::pair<DWORD_PTR, DWORD_PTR>>& mapModules);
	std::map<std::wstring, std::pair<DWORD_PTR, DWORD_PTR>> GetModules(const DWORD dwProcessId);
};
