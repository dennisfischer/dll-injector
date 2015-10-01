#include "stdafx.h"

void logError(std::wstring message, unsigned long code) {
	std::wcout << "ERROR (" << std::to_wstring(code) << "): " << message << std::endl;
}

void logWarning(std::wstring message, unsigned long code) {
	std::wcout << "WARNING (" << std::to_wstring(code) << "): " << message << std::endl;
}

void logInfo(std::wstring message) {
	std::wcout << "INFO: " << message << std::endl;
}