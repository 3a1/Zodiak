#include "includes.hpp"

#include "functions.hpp"

#ifdef UNICODE
#define _tcsicmp wcscmp
#else
#define _tcsicmp _stricmp
#endif


DWORD getModuleAddress(DWORD processID, const char* moduleName)
{
	DWORD_PTR dwModuleBaseAddress = 0;
	DWORD result = 0;

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32))
		{
			do {
				std::string moduleNameString(moduleName);
				std::string moduleNameWideString(moduleNameString.begin(), moduleNameString.end());
				if (_tcsicmp(ModuleEntry32.szModule, moduleNameWideString.c_str()) == 0)
				{
					dwModuleBaseAddress = (DWORD_PTR)ModuleEntry32.modBaseAddr;
					result = dwModuleBaseAddress;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	return result;
}

