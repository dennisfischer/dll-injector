#include "stdafx.h"

IATHookInjector::IATHookInjector(const std::string cProcessName): m_ProcessName(cProcessName)
{}

IATHookInjector::~IATHookInjector()
{
}

void IATHookInjector::Release()
{
}

void IATHookInjector::do_inject()
{
	char* handler =
		"\x55\x31\xdb\xeb\x55\x64\x8b\x7b"
		"\x30\x8b\x7f\x0c\x8b\x7f\x1c\x8b"
		"\x47\x08\x8b\x77\x20\x8b\x3f\x80"
		"\x7e\x0c\x33\x75\xf2\x89\xc7\x03"
		"\x78\x3c\x8b\x57\x78\x01\xc2\x8b"
		"\x7a\x20\x01\xc7\x89\xdd\x8b\x34"
		"\xaf\x01\xc6\x45\x8b\x4c\x24\x04"
		"\x39\x0e\x75\xf2\x8b\x4c\x24\x08"
		"\x39\x4e\x04\x75\xe9\x8b\x7a\x24"
		"\x01\xc7\x66\x8b\x2c\x6f\x8b\x7a"
		"\x1c\x01\xc7\x8b\x7c\xaf\xfc\x01"
		"\xf8\xc3\x68\x4c\x69\x62\x72\x68"
		"\x4c\x6f\x61\x64\xe8\x9c\xff\xff"
		"\xff\x31\xc9\x66\xb9\x33\x32\x51"
		"\x68\x75\x73\x65\x72\x54\xff\xd0"
		"\x50\x68\x72\x6f\x63\x41\x68\x47"
		"\x65\x74\x50\xe8\x7d\xff\xff\xff"
		"\x59\x59\x59\x68\xf0\x86\x17\x04"
		"\xc1\x2c\x24\x04\x68\x61\x67\x65"
		"\x42\x68\x4d\x65\x73\x73\x54\x51"
		"\xff\xd0\x53\x53\x53\x53\xff\xd0"
		"\xb9\x07\x00\x00\x00\x58\xe2\xfd"
		"\x5d\xb8\xef\xbe\xad\xde\xff\xe0";
	auto dwProcessId = GetProcessIdFromProcessName(m_ProcessName);
	HookFunction(dwProcessId, "user32.dll", "GetClipboardData", handler, 0x100);
}

void IATHookInjector::do_free()
{
}

DWORD IATHookInjector::FindRemotePEB(HANDLE hProcess) const
{
	auto hNTDLL = LoadLibraryA("ntdll");
	if (!hNTDLL)
	{
		return 0;
	}
	auto fpNtQueryInformationProcess = GetProcAddress(hNTDLL, "NtQueryInformationProcess");
	if (!fpNtQueryInformationProcess)
	{
		return 0;
	}
	auto ntQueryInformationProcess = NtQueryInformationProcess(fpNtQueryInformationProcess);
	auto pBasicInfo = new PROCESS_BASIC_INFORMATION();
	DWORD dwReturnLength = 0;
	ntQueryInformationProcess(hProcess, 0, pBasicInfo, sizeof(PROCESS_BASIC_INFORMATION), &dwReturnLength);
	return pBasicInfo->PebBaseAddress;
}

PEB* IATHookInjector::ReadRemotePEB(HANDLE hProcess) const
{
	auto dwPEBAddress = FindRemotePEB(hProcess);
	printf("PEB Address: 0x%x\n", dwPEBAddress);
	auto pPEB = new PEB();
	SIZE_T* bytesRead = nullptr;
	auto bSuccess = ReadProcessMemory(hProcess, LPCVOID(dwPEBAddress), pPEB, sizeof(PEB), bytesRead);
	printf("ReadProcMemory %d\n", GetLastError());
	printf("Read: %p\n", bytesRead);

	if (!bSuccess)
	{
		return nullptr;
	}
	return pPEB;
}

PLOADED_IMAGE IATHookInjector::ReadRemoteImage(HANDLE hProcess, LPCVOID lpImageBaseAddress) const
{
	auto lpBuffer = new BYTE[BUFFER_SIZE];
	auto bSuccess = ReadProcessMemory(hProcess, lpImageBaseAddress, lpBuffer, BUFFER_SIZE, nullptr);
	if (!bSuccess)
	{
		return nullptr;
	}
	auto pDOSHeader = PIMAGE_DOS_HEADER(lpBuffer);
	auto pImage = new LOADED_IMAGE();
	pImage->FileHeader = PIMAGE_NT_HEADERS(lpBuffer + pDOSHeader->e_lfanew);
	pImage->NumberOfSections = pImage->FileHeader->FileHeader.NumberOfSections;
	pImage->Sections = PIMAGE_SECTION_HEADER(lpBuffer + pDOSHeader->e_lfanew + sizeof(IMAGE_NT_HEADERS32));
	return pImage;
}

PIMAGE_SECTION_HEADER IATHookInjector::FindSectionHeaderByName(PIMAGE_SECTION_HEADER pHeaders, DWORD dwNumberOfSections, char* pName) const
{
	PIMAGE_SECTION_HEADER pHeaderMatch = nullptr;

	for (DWORD i = 0; i < dwNumberOfSections; i++)
	{
		auto pHeader = &pHeaders[i];

		if (!_stricmp(reinterpret_cast<char*>(pHeader->Name), pName))
		{
			pHeaderMatch = pHeader;
			break;
		}
	}

	return pHeaderMatch;
}


PIMAGE_IMPORT_DESCRIPTOR IATHookInjector::ReadRemoteImportDescriptors(HANDLE hProcess, LPCVOID lpImageBaseAddress, PIMAGE_DATA_DIRECTORY pImageDataDirectory) const
{
	auto importDirectory = pImageDataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	auto pImportDescriptors = new IMAGE_IMPORT_DESCRIPTOR[importDirectory.Size / sizeof(IMAGE_IMPORT_DESCRIPTOR)];
	auto bSuccess = ReadProcessMemory(hProcess, LPCVOID(DWORD(lpImageBaseAddress) + importDirectory.VirtualAddress), pImportDescriptors, importDirectory.Size, nullptr);

	if (!bSuccess)
	{
		return nullptr;
	}
	return pImportDescriptors;
}

char* IATHookInjector::ReadRemoteDescriptorName(HANDLE hProcess, LPCVOID lpImageBaseAddress, PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor) const
{
	auto pBuffer = new char[BUFFER_SIZE];
	auto bSuccess = ReadProcessMemory(hProcess, LPCVOID(DWORD(lpImageBaseAddress) + pImageImportDescriptor->Name), pBuffer, BUFFER_SIZE, nullptr);

	if (!bSuccess)
	{
		return nullptr;
	}
	return pBuffer;
}

PIMAGE_THUNK_DATA32 IATHookInjector::ReadRemoteILT(HANDLE hProcess, LPCVOID lpImageBaseAddress, PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor) const
{
	DWORD dwThunkArrayLen = BUFFER_SIZE / sizeof(IMAGE_THUNK_DATA32);
	auto pILT = new IMAGE_THUNK_DATA32[dwThunkArrayLen];
	auto bSuccess = ReadProcessMemory(hProcess, LPCVOID(DWORD(lpImageBaseAddress) + pImageImportDescriptor->OriginalFirstThunk), pILT, BUFFER_SIZE, nullptr);

	if (!bSuccess)
	{
		return nullptr;
	}
	return pILT;
}


PIMAGE_THUNK_DATA32 IATHookInjector::ReadRemoteIAT(HANDLE hProcess, LPCVOID lpImageBaseAddress, PIMAGE_IMPORT_DESCRIPTOR pImageImportDescriptor) const
{
	DWORD dwThunkArrayLen = BUFFER_SIZE / sizeof(IMAGE_THUNK_DATA32);
	auto pIAT = new IMAGE_THUNK_DATA32[dwThunkArrayLen];
	auto bSuccess = ReadProcessMemory(hProcess, LPCVOID(DWORD(lpImageBaseAddress) + pImageImportDescriptor->FirstThunk), pIAT, BUFFER_SIZE, nullptr);

	if (!bSuccess)
	{
		return nullptr;
	}
	return pIAT;
}

PIMAGE_IMPORT_BY_NAME IATHookInjector::ReadRemoteImportByName(HANDLE hProcess, LPCVOID lpImageBaseAddress, PIMAGE_THUNK_DATA32 pImageThunk) const
{
	auto lpImportNameBuffer = new BYTE[BUFFER_SIZE];
	auto bSuccess = ReadProcessMemory(hProcess, LPCVOID(DWORD(lpImageBaseAddress) + pImageThunk->u1.AddressOfData), lpImportNameBuffer,BUFFER_SIZE, nullptr);

	if (!bSuccess)
	{
		return nullptr;
	}

	return PIMAGE_IMPORT_BY_NAME(lpImportNameBuffer);
}

PPEB_LDR_DATA IATHookInjector::ReadRemoteLoaderData(HANDLE hProcess, PPEB pPEB) const
{
	auto pLoaderData = new PEB_LDR_DATA();
	auto bSuccess = ReadProcessMemory(hProcess, pPEB->LoaderData, pLoaderData, sizeof(PEB_LDR_DATA), nullptr);

	if (!bSuccess)
	{
		return nullptr;
	}
	return pLoaderData;
}

PVOID IATHookInjector::FindRemoteImageBase(HANDLE hProcess, PPEB pPEB, char* pModuleName) const
{
	auto pLoaderData = ReadRemoteLoaderData(hProcess, pPEB);
	PVOID firstFLink = pLoaderData->InLoadOrderModuleList.Flink;
	PVOID fLink = pLoaderData->InLoadOrderModuleList.Flink;
	auto pModule = new LDR_MODULE();

	do
	{
		auto bSuccess = ReadProcessMemory(hProcess, fLink, pModule, sizeof(LDR_MODULE), nullptr);

		if (!bSuccess)
		{
			return nullptr;
		}
		auto pwBaseDllName = new WCHAR[pModule->BaseDllName.MaximumLength];

		bSuccess = ReadProcessMemory(hProcess, pModule->BaseDllName.Buffer, pwBaseDllName, pModule->BaseDllName.Length + 2, nullptr);

		if (bSuccess)
		{
			size_t sBaseDllName = pModule->BaseDllName.Length / 2 + 1;
			auto pBaseDllName = new char[sBaseDllName];

			WideCharToMultiByte(CP_ACP, 0, pwBaseDllName, pModule->BaseDllName.Length + 2, pBaseDllName, sBaseDllName, nullptr, nullptr);

			if (!_stricmp(pBaseDllName, pModuleName))
			{
				return pModule->BaseAddress;
			}
		}

		fLink = pModule->InLoadOrderModuleList.Flink;
	}
	while (pModule->InLoadOrderModuleList.Flink != firstFLink);

	return nullptr;
}

BOOL IATHookInjector::PatchDWORD(BYTE* pBuffer, DWORD dwBufferSize, DWORD dwOldValue, DWORD dwNewValue) const
{
	for (auto i = 0; i < int(dwBufferSize - 4); i++)
	{
		if (*PDWORD(pBuffer + i) == dwOldValue)
		{
			memcpy(pBuffer + i, &dwNewValue, 4);

			return TRUE;
		}
	}

	return FALSE;
}

BOOL IATHookInjector::HookFunction(DWORD dwProcessId, CHAR* pModuleName, CHAR* pFunctionName, PVOID pHandler, DWORD dwHandlerSize) const
{
	//PROCESS_QUERY_LIMITED_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_READ | PROCESS_VM_WRITE
	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, dwProcessId);
	if (!hProcess)
	{
		printf("Error opening process\r\n");
		return FALSE;
	}

	auto dwPEBAddress = FindRemotePEB(hProcess);
	if (!dwPEBAddress)
	{
		printf("Error finding remote PEB\r\n");
		return FALSE;
	}

	auto pPEB = ReadRemotePEB(hProcess);
	if (!pPEB)
	{
		printf("Error reading remote PEB\r\n");
		return FALSE;
	}

	auto pImage = ReadRemoteImage(hProcess, pPEB->ImageBaseAddress);
	if (!pImage)
	{
		printf("Error reading remote image\r\n");
		return FALSE;
	}

	auto pImportDescriptors = ReadRemoteImportDescriptors(hProcess, pPEB->ImageBaseAddress, pImage->FileHeader->OptionalHeader.DataDirectory);
	if (!pImportDescriptors)
	{
		printf("Error reading remote import descriptors\r\n");
		return FALSE;
	}

	for (DWORD i = 0; i < 0x2000; i++)
	{
		auto descriptor = pImportDescriptors[i];
		auto pName = ReadRemoteDescriptorName(hProcess, pPEB->ImageBaseAddress, &descriptor);

		if (!pName)
		{
			printf("Error reading remote descriptor name\r\n");
			return FALSE;
		}

		if (!_stricmp(pName, pModuleName))
		{
			DWORD dwThunkArrayLen = BUFFER_SIZE / sizeof(IMAGE_THUNK_DATA32);

			auto pILT = ReadRemoteILT(hProcess, pPEB->ImageBaseAddress, &descriptor);

			if (!pILT)
			{
				printf("Error reading remote ILT\r\n");
				return FALSE;
			}

			DWORD dwOffset = 0;

			for (dwOffset = 0; dwOffset < dwThunkArrayLen; dwOffset++)
			{
				auto pImportByName = ReadRemoteImportByName(hProcess, pPEB->ImageBaseAddress, &pILT[dwOffset]);
				if (!pImportByName)
				{
					printf("Error reading remote import by name\r\n");
					return FALSE;
				}

				if (!strcmp(static_cast<char*>(pImportByName->Name), pFunctionName))
					break;
			}

			auto pIAT = ReadRemoteIAT(hProcess, pPEB->ImageBaseAddress, &descriptor);
			if (!pIAT)
			{
				printf("Error reading remote IAT\r\n");
				return FALSE;
			}

			auto dwOriginalAddress = pIAT[dwOffset].u1.AddressOfData;
			printf("Original import address: 0x%x\r\n", dwOriginalAddress);


			auto pImportImageBase = FindRemoteImageBase(hProcess, pPEB, pModuleName);

			if (!pImportImageBase)
			{
				printf("Could not find remote image base for %s\r\n", pModuleName);
				return FALSE;
			}

			auto pImportImage = ReadRemoteImage(hProcess, pImportImageBase);

			if (!pImportImage)
			{
				printf("Could not find remote image at 0x%p\r\n", pImportImageBase);
				return FALSE;
			}

			auto pImportTextHeader = FindSectionHeaderByName(pImportImage->Sections, pImportImage->NumberOfSections, ".text");
			if (!pImportTextHeader)
			{
				printf("Could not find section header\r\n");
				return FALSE;
			}

			auto pHandlerBuffer = new BYTE[dwHandlerSize];

			memcpy(pHandlerBuffer, pHandler, dwHandlerSize);

			auto bSuccess = PatchDWORD(pHandlerBuffer, dwHandlerSize, 0xDEADBEEF, dwOriginalAddress);

			if (!bSuccess)
			{
				printf("Error patching import address into handler");
				return FALSE;
			}

			auto dwHandlerAddress = DWORD(pImportImageBase) + pImportTextHeader->VirtualAddress + pImportTextHeader->SizeOfRawData - dwHandlerSize;

			// Write handler to text section
			bSuccess = WriteProcessMemory(hProcess, LPVOID(dwHandlerAddress), pHandlerBuffer, dwHandlerSize, nullptr);
			if (!bSuccess)
			{
				printf("Error writing process memory");
				return FALSE;
			}

			printf("Handler address: 0x%x\r\n", dwHandlerAddress);

			auto pAddress = LPVOID(DWORD(pPEB->ImageBaseAddress) + descriptor.FirstThunk + (dwOffset * sizeof(IMAGE_THUNK_DATA32)));

			// Write IAT
			bSuccess = WriteProcessMemory(hProcess, pAddress, &dwHandlerAddress, 4, nullptr);

			if (!bSuccess)
			{
				printf("Error writing process memory");
				return FALSE;
			}

			return TRUE;
		}
		if (!descriptor.Characteristics)
		{
			return FALSE;
		}
	}

	return FALSE;
}


int IATHookInjector::GetCalcId() const
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
		return 0;

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32))
	{
		CloseHandle(hProcessSnap);
		return 0;
	}

	do
	{
		if (!_wcsicmp(pe32.szExeFile, L"calc.exe"))
		{
			CloseHandle(hProcessSnap);
			return pe32.th32ProcessID;
		}
	}
	while (Process32Next(hProcessSnap, &pe32));

	CloseHandle(hProcessSnap);

	return 0;
}